using FlaxEditor.GUI;
using FlaxEditor;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FlaxEditor.Options;
using FlaxEngine;
using System.Xml.Linq;
using Fudgets;
using FlaxEditor.GUI.Tree;
using FlaxEditor.SceneGraph;
using FlaxEditor.CustomEditors;
using System.Runtime.CompilerServices;
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
        RefreshNames();
        MarkAsEdited();
    }

    /// <summary>
    /// Saves the associated FudgetJsonAsset.
    /// </summary>
    public override void Save()
    {
        _asset.Save();
        ClearEditedFlag();
    }

    protected void DeleteControl(FudgetControl control)
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

        FlaxEngine.Object.Destroy(control);
        controlNode.DisposeChildren();
        controlNode.Dispose();

        Focus();
    }

    private void Delete()
    {
        foreach (object selected in _fudgetControlsEditor.Selection)
        {
            FudgetControl control = (FudgetControl)selected;
            DeleteControl(control);
        }

        MarkAsEdited();
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

        MarkAsEdited();
        Focus();
    }

    private void ChangeControlParent(FudgetControl control, FudgetContainer newParent)
    {
        TreeNode controlNode = FindNodeByControl(control);
        TreeNode newParentNode = FindNodeByControl(newParent);

        controlNode.Parent = newParentNode;
        control.Parent = newParent;
        OnRebuildTree();
    }

    public void Select(List<SceneGraphNode> nodes)
    {
    }

    private void OnRebuildTree()
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

    private class FudgetTree : Tree
    {
        public Action<Float2> BaseTreeRightClick;
        private bool _rightClickDown = false;
        private ContextMenu _menu = null;
        private ContextMenuChildMenu _newMenu = null;

        public FudgetTree() : base()
        {
            RightClick += TreeNodeRightClick;
            BaseTreeRightClick += OnBaseTreeRightClick;
        }

        public void Deinitialize()
        {
            RightClick -= TreeNodeRightClick;
            BaseTreeRightClick -= OnBaseTreeRightClick;
        }

        public override bool OnMouseDown(Float2 location, MouseButton button)
        {
            if (button == MouseButton.Right)
                _rightClickDown = true;

            return base.OnMouseDown(location, button);
        }

        public override void OnMouseLeave()
        {
            base.OnMouseLeave();
            _rightClickDown = false;
        }

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
            menu.AddButton("Delete", (node as ItemNode).Delete);
            menu.Show(node, location);
        }

        private void OnBaseTreeRightClick(Float2 location)
        {
            var menu = GetOrSetContextMenu();
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

        private void OnNewControl(FudgetsEditorWindow window, FudgetControl control)
        {
            window.MarkAsEdited();
            window.OnRebuildTree();
            Deselect();
            Select(window.FindNodeByControl(control));
        }

        private void ContextMenuNewClicked(ContextMenuButton button)
        {
            ScriptType type = (ScriptType)button.Tag;
            FudgetControl control = (FudgetControl)type.CreateInstance();
            if (control == null)
                return;

            control.Name = button.Text;
            FudgetsEditorWindow window = (FudgetsEditorWindow)Tag;
            if (Selection.Count <= 0)
            {
                control.Parent = window.RootObject;
                OnNewControl(window, control);
                return;
            }

            FudgetControl selected = (FudgetControl)Selection[0].Tag;
            if (selected == null || selected is not FudgetContainer container)
            {
                control.Parent = window.RootObject;
                OnNewControl(window, control);
                return;
            }

            control.Parent = container;
            OnNewControl(window, control);
        }
    }

    private class ItemNode : TreeNode
    {
        public FudgetControl Control => (FudgetControl)Tag;

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
                    Delete();
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
            window.MarkAsEdited();
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
                ((FudgetsEditorWindow)ParentTree.Tag).MarkAsEdited();
            Control.Name = popup.Text;
            Text = Control.Name;
        }
    }
}
