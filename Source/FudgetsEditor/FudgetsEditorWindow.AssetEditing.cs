using FlaxEditor.GUI;
using FlaxEditor;
using System;
using System.Collections.Generic;
using FlaxEngine;
using Fudgets;
using FlaxEditor.GUI.Tree;
using FlaxEditor.SceneGraph;
using FlaxEditor.CustomEditors;
using FlaxEditor.GUI.ContextMenu;
using FlaxEditor.Scripting;
using FlaxEngine.GUI;

namespace FudgetsEditor;

public partial class FudgetsEditorWindow
{
    // Toolstrip and asset editing members
    private Undo _undo;
    private ToolStripButton _saveButton;
    private ToolStripButton _toolStripUndo;
    private ToolStripButton _toolStripRedo;

    /// <summary>
    /// Gets the undo system used by this window for changes tracking.
    /// </summary>
    public Undo Undo => _undo;

    private void SetupToolstrip()
    {
        var inputOptions = Editor.Options.Options.Input;

        // Toolstrip
        _saveButton = (ToolStripButton)_toolstrip.AddButton(Editor.Icons.Save64, Save).LinkTooltip("Save");
        _saveButton.Enabled = false;
        _toolstrip.AddSeparator();
        _toolStripUndo = (ToolStripButton)_toolstrip.AddButton(Editor.Icons.Undo64, _undo.PerformUndo).LinkTooltip($"Undo ({inputOptions.Undo})");
        _toolStripRedo = (ToolStripButton)_toolstrip.AddButton(Editor.Icons.Redo64, _undo.PerformRedo).LinkTooltip($"Redo ({inputOptions.Redo})");

        // TODO: Maybe reload after scripts change to load new types?
        /*ScriptsBuilder.ScriptsReloadBegin += OnScriptsReloadBegin;
        ScriptsBuilder.ScriptsReloadEnd += OnScriptsReloadEnd;*/

        // Setup input actions
        InputActions.Add(options => options.Undo, () =>
        {
            _undo.PerformUndo();
            Focus();
        });
        InputActions.Add(options => options.Redo, () =>
        {
            _undo.PerformRedo();
            Focus();
        });

        // TODO: Implement copy/cut/etc operations.
        //InputActions.Add(options => options.Cut, Cut);
        //InputActions.Add(options => options.Copy, Copy);
        //InputActions.Add(options => options.Paste, Paste);
        //InputActions.Add(options => options.Duplicate, Duplicate);
        InputActions.Add(options => options.Delete, Delete);
        InputActions.Add(options => options.Rename, Rename);
    }

    private void ControlsValueEditorUpdate()
    {
        OnAnyChange();
    }

    /// <summary>
    /// Deletes a selected control.
    /// </summary>
    /// <param name="control">The target control.</param>
    public void DeleteControl(FudgetControl control)
    {
        TreeNode controlNode = FindNodeByControl(control);

        // Ensure the node is deselected.
        ValueContainer selection = _fudgetControlsEditor.Selection;
        selection.Remove(control);
        _fudgetControlsEditor.Deselect();
        _fudgetControlsEditor.Select(selection);

        List<TreeNode> nodesSelection = _tree.Selection;
        nodesSelection.Remove(controlNode);
        _tree.Deselect();
        _tree.Select(nodesSelection);

        controlNode.Parent = null;
        control.Parent = null;

        controlNode.DisposeChildren();
        controlNode.Dispose();

        OnRebuildTree();
        OnAnyChange();
        Focus();
    }

    private void Delete()
    {
        var actions = new List<IUndoAction>();
        foreach (object selected in _fudgetControlsEditor.Selection)
        {
            FudgetControl control = (FudgetControl)selected;
            actions.Add(new FudgetDeleteAction(this, control));
        }

        MultiUndoAction multi = new MultiUndoAction(actions);
        multi.Do();
        Undo.AddAction(multi);
    }

    private void Rename()
    {
        // TODO: Rename all at once, instead of indiviudally.
        // Also this behavior is probably terrible
        foreach (TreeNode selected in _tree.Selection)
        {
            ItemNode node = (ItemNode)selected;
            node.StartRenaming();
        }

        OnAnyChange();
        Focus();
    }

    private void ChangeControlParent(FudgetControl control, FudgetContainer newParent)
    {
        TreeNode controlNode = FindNodeByControl(control);
        TreeNode newParentNode = FindNodeByControl(newParent);

        controlNode.Parent = newParentNode;
        control.Parent = newParent;
        OnRebuildTree();
        OnAnyChange();
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
    /// <param name="nodes"></param>
    public void Select(List<SceneGraphNode> nodes)
    {
    }

    /// <summary>
    /// Rebuilds the fudget display tree after a change is made to the hierarchy.
    /// </summary>
    public void OnRebuildTree()
    {
        List<FudgetControl> oldControlSelection = _selectedControls;

        _tree.Deselect();
        List<object> expandedControls = GatherExpanded(_tree);
        _tree.RemoveChildren();

        List<TreeNode> newExpandedNodes = new List<TreeNode>();
        List<TreeNode> newSelectedNodes = new List<TreeNode>();
        BuildForControl(RootObject, _tree, expandedControls, ref newExpandedNodes, oldControlSelection, ref newSelectedNodes);

        foreach (TreeNode node in newExpandedNodes)
        {
            node.Expand(true);
        }

        _tree.Select(newSelectedNodes);
        Focus();
    }

    private void SelectedControlChanged(List<TreeNode> before, List<TreeNode> after)
    {
        List<FudgetControl> selectedNodes = new List<FudgetControl>();
        foreach (TreeNode node in after)
        {
            if (node.Tag == null)
                continue;

            if (node.Tag is not FudgetControl control)
                continue;
            selectedNodes.Add(control);
        }
        _selectedControls = selectedNodes;

        _fudgetControlsEditor.Deselect();
        _fudgetControlsEditor.Select(_selectedControls);
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
    public void OnAnyChange()
    {
        RefreshNames();
        MarkAsEdited();
        _saveButton.Enabled = true;

        ForceLayout();
    }

    /// <summary>
    /// Saves the associated FudgetJsonAsset.
    /// </summary>
    public override void Save()
    {
        _asset.Save();
        ClearEditedFlag();
        _saveButton.Enabled = false;
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
    public class FudgetTree : Tree
    {
        /// <summary>
        /// TODO: docs
        /// </summary>
        public Action<Float2> BaseTreeRightClick;
        private bool _rightClickDown = false;
        private ContextMenu _menu = null;
        private ContextMenuChildMenu _newMenu = null;

        /// <summary>
        /// TODO: docs
        /// </summary>
        public FudgetTree() : base()
        {
            RightClick += TreeNodeRightClick;
            BaseTreeRightClick += OnBaseTreeRightClick;
        }

        /// <summary>
        /// TODO: docs
        /// </summary>
        public void Deinitialize()
        {
            RightClick -= TreeNodeRightClick;
            BaseTreeRightClick -= OnBaseTreeRightClick;
        }

        /// <inheritdoc />
        public override bool OnMouseDown(Float2 location, MouseButton button)
        {
            if (button == MouseButton.Right)
                _rightClickDown = true;

            return base.OnMouseDown(location, button);
        }

        /// <inheritdoc />
        public override void OnMouseLeave()
        {
            base.OnMouseLeave();
            _rightClickDown = false;
        }

        /// <inheritdoc />
        public override bool OnMouseUp(Float2 location, MouseButton button)
        {
            if (button == MouseButton.Right && _rightClickDown)
            {
                _rightClickDown = false;
                if (BaseTreeRightClick != null)
                {
                    BaseTreeRightClick(location);
                }
            }

            return base.OnMouseUp(location, button);
        }

        private ContextMenu GetOrSetContextMenu()
        {
            if (_menu == null || !_menu.IsOpened)
                _menu = new ContextMenu();

            return _menu;
        }

        private void TreeNodeRightClick(TreeNode node, Float2 location)
        {
            var menu = GetOrSetContextMenu();
            menu.Tag = node;
            menu.AddButton("Rename", (node as ItemNode).StartRenaming);
            menu.Show(node, location);
        }

        private void OnBaseTreeRightClick(Float2 location)
        {
            var menu = GetOrSetContextMenu();
            FudgetsEditorWindow window = (FudgetsEditorWindow)Tag;
            menu.AddButton("Delete", window.Delete);

            _newMenu = menu.GetOrAddChildMenu("New");
            ContextMenu newMenu = _newMenu.ContextMenu;

            ScriptType controlType = new ScriptType(typeof(FudgetControl));
            var allTypes = Editor.Instance.CodeEditing.All.Get();
            foreach (var type in allTypes)
            {
                if (controlType.IsAssignableFrom(type))
                {
                    ContextMenuButton button = newMenu.AddButton(FlaxEditor.Utilities.Utils.GetPropertyNameUI(type.Name));
                    button.Tag = type;
                    button.ButtonClicked += ContextMenuNewClicked;
                }
            }

            menu.Show(this, location);
        }

        /// <summary>
        /// TODO: Docs
        /// </summary>
        /// <param name="window"></param>
        /// <param name="control"></param>
        /// <param name="oldParent"></param>
        /// <param name="oldIndex"></param>
        public void OnNewControl(FudgetsEditorWindow window, FudgetControl control, FudgetContainer oldParent, int oldIndex)
        {
            FudgetControl selected = Selection.Count > 0 ? (FudgetControl)Selection[0].Tag : null;
            if (oldParent != null)
            {
                oldParent.AddChild(control, oldIndex);
            }
            else if (selected == null || selected is not FudgetContainer container)
            {
                window.RootObject.AddChild(control, oldIndex);   
            }
            else
            {
                container.AddChild(control, oldIndex);
            }

            window.OnAnyChange();
            window.OnRebuildTree();
            Deselect();
            Select(window.FindNodeByControl(control));
        }

        private void ContextMenuNewClicked(ContextMenuButton button)
        {
            ScriptType type = (ScriptType)button.Tag;
            FudgetsEditorWindow window = (FudgetsEditorWindow)Tag;
            var action = new FudgetNewControlAction(window, this, type, button.Text);
            window.Undo.AddAction(action);
            action.Do();
        }
    }

    private class TreePanel : Panel
    {
        private bool _rightClickDown = false;

        /// <inheritdoc />
        public override bool OnMouseDown(Float2 location, MouseButton button)
        {
            if (button == MouseButton.Right)
                _rightClickDown = true;

            return base.OnMouseDown(location, button);
        }

        /// <inheritdoc />
        public override void OnMouseLeave()
        {
            base.OnMouseLeave();
            _rightClickDown = false;
        }

        /// <inheritdoc />
        public override bool OnMouseUp(Float2 location, MouseButton button)
        {
            if (button == MouseButton.Right && _rightClickDown)
            {
                _rightClickDown = false;
                FudgetTree tree = (FudgetTree)Tag;
                if (tree.BaseTreeRightClick != null)
                {
                    tree.BaseTreeRightClick(location);
                }
            }

            return base.OnMouseUp(location, button);
        }
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
    public class ItemNode : TreeNode
    {
        /// <summary>
        /// TODO: docs
        /// </summary>
        public FudgetControl Control => (FudgetControl)Tag;

        /// <summary>
        /// TODO: docs
        /// </summary>
        public ItemNode(FudgetControl control)
        : base(false)
        {
            Text = control.Name;
            Tag = control;
        }

        /// <inheritdoc />
        protected override bool OnMouseDoubleClickHeader(ref Float2 location, MouseButton button)
        {
            StartRenaming();
            return true;
        }

        /// <inheritdoc />
        public override bool OnKeyDown(KeyboardKeys key)
        {
            if (base.OnKeyDown(key))
                return true;
            switch (key)
            {
                case KeyboardKeys.F2:
                    StartRenaming();
                    return true;
                case KeyboardKeys.Delete:
                    FudgetsEditorWindow window = (FudgetsEditorWindow)ParentTree.Tag;
                    window.Delete();
                    return true;
            }
            return false;
        }

        /// <summary>
        /// Deletes the node (and its associated control).
        /// </summary>
        public void Delete()
        {
            Tree tree = ParentTree;
            FudgetsEditorWindow window = (FudgetsEditorWindow)tree.Tag;
            window.DeleteControl(Control);
            window.OnAnyChange();
        }

        /// <summary>
        /// Shows the rename popup for the item.
        /// </summary>
        public void StartRenaming()
        {
            // Start renaming the control.
            var dialog = RenamePopup.Show(this, HeaderRect, Control.Name, false);
            dialog.Tag = Tag;
            dialog.Renamed += OnRenamed;
        }

        private void OnRenamed(RenamePopup popup)
        {
            if (Control.Name != popup.Text)
            {
                FudgetsEditorWindow window = (FudgetsEditorWindow)ParentTree.Tag;
                var action = new FudgetRenameAction(window, Control, popup.Text);
                window.Undo.AddAction(action);
                action.Do();
            }
        }
    }
}

// Undo Actions

/// <summary>
/// TODO: docs
/// </summary>
public class FudgetDeleteAction : IUndoAction
{
    private FudgetControl _deletionTarget;
    private FudgetContainer _oldParent;
    private FudgetsEditorWindow _window;
    private int _oldIndex = -1;
    private bool _done = false;

    /// <summary>
    /// TODO: docs
    /// </summary>
    public FudgetDeleteAction(FudgetsEditorWindow window, FudgetControl deletionTarget)
    {
        _window = window;
        _deletionTarget = deletionTarget;
        _oldParent = null;
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
    public string ActionString => "Delete control";

    /// <summary>
    /// TODO: docs
    /// </summary>
    public void Dispose()
    {
        _deletionTarget = null;
        _oldParent = null;
        _window = null;

        if (_done)
        {
            FlaxEngine.Object.Destroy(_deletionTarget);
        }
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
    public void Do()
    {
        _oldParent = _deletionTarget.Parent;
        _oldIndex = _deletionTarget.IndexInParent;
        _window.DeleteControl(_deletionTarget);
        _done = true;

        _window.OnAnyChange();
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
    public void Undo()
    {
        _oldParent.AddChild(_deletionTarget, _oldIndex);
        _oldParent = null;
        _window.OnRebuildTree();
        _window.FindNodeByControl(_deletionTarget).Select();
        _done = false;

        _window.OnAnyChange();
    }
}

/// <summary>
/// TODO: docs
/// </summary>
public class FudgetRenameAction : IUndoAction
{
    private FudgetsEditorWindow _window;
    private FudgetControl _renameTarget;
    private string _oldName;
    private string _newName;

    /// <summary>
    /// TODO: docs
    /// </summary>
    public FudgetRenameAction(FudgetsEditorWindow window, FudgetControl renameTarget, string newName)
    {
        _window = window;
        _renameTarget = renameTarget;
        _newName = newName;
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
    public string ActionString => "Rename control";

    /// <summary>
    /// TODO: docs
    /// </summary>
    public void Dispose()
    {
        _window = null;
        _renameTarget = null;
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
    public void Do()
    {
        _oldName = _renameTarget.Name;
        _renameTarget.Name = _newName;
        _window.RefreshNames();

        _window.OnAnyChange();
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
    public void Undo()
    {
        _renameTarget.Name = _oldName;
        _window.RefreshNames();

        _window.OnAnyChange();
    }
}

/// <summary>
/// TODO: docs
/// </summary>
public class FudgetNewControlAction : IUndoAction
{
    private FudgetsEditorWindow _window;
    private FudgetsEditorWindow.FudgetTree _tree;
    private ScriptType _newControlType;
    private FudgetControl _createdControl;
    private FudgetContainer _oldParent;
    private string _newName;
    private int _oldIndex;

    /// <summary>
    /// TODO: docs
    /// </summary>
    public FudgetNewControlAction(FudgetsEditorWindow window, FudgetsEditorWindow.FudgetTree tree, ScriptType newControlType, string newName)
    {
        _window = window;
        _tree = tree;
        _newControlType = newControlType;
        _newName = newName;
        _oldIndex = -1;
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
    public string ActionString => "Add new control";

    /// <summary>
    /// TODO: docs
    /// </summary>
    public void Dispose()
    {
        _window = null;
        _createdControl = null;
        _oldParent = null;
        _tree = null;
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
    public void Do()
    {
        _createdControl = (FudgetControl)_newControlType.CreateInstance();
        if (_createdControl == null)
            return;

        _createdControl.Name = _newName;
        _tree.OnNewControl(_window, _createdControl, _oldParent, _oldIndex);
        _oldParent = _createdControl.Parent;
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
    public void Undo()
    {
        _oldIndex = _createdControl.IndexInParent;
        _window.DeleteControl(_createdControl);

        _window.OnAnyChange();
        FlaxEngine.Object.Destroy(_createdControl);
        _createdControl = null;
    }
}