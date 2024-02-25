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
using System.IO;
using FlaxEditor.GUI.Drag;
using static FudgetsEditor.FudgetsEditorWindow;
using System.Linq;

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

        if (actions.Count > 0)
        {
            MultiUndoAction multi = new MultiUndoAction(actions);
            multi.Do();
            Undo.AddAction(multi);
        }
    }

    private void Rename()
    {
        // TODO: Rename all at once, instead of indiviudally.
        // Also this behavior is probably terrible
        foreach (TreeNode selected in _tree.Selection)
        {
            if (selected.Tag == null)
                continue;
            FudgetControlNode node = (FudgetControlNode)selected;
            node.StartRenaming();
        }

        OnAnyChange();
        Focus();
    }

    /// <summary>
    /// Calculates the drag and drop index of the moving control.
    /// </summary>
    /// <param name="above">If the control is to be placed above the pivot.</param>
    /// <param name="pivotControl">The control which is to have a control placed above or below it.</param>
    /// <param name="movingControl">The control which is to be moved.</param>
    /// <returns>The calculated index, or -1 if something goes wrong.</returns>
    public int FindNewIndex(bool above, FudgetControl pivotControl, FudgetControl movingControl)
    {
        List<FudgetControl> controls = RecurseAndGetSubset(_tree, (_) => { return true; }).ConvertAll((node) => node.Control);
        int pivotIndex = controls.IndexOf(pivotControl);
        int movingIndex = controls.IndexOf(movingControl);

        bool movingFromAbove = pivotIndex > movingIndex;
        int offset = above == movingFromAbove ? -1 : 0;
        offset *= !above ? -1 : 1;

        if (pivotControl.Parent != null)
        {
            int newIndex = pivotControl.IndexInParent + offset;
            newIndex = Mathf.Min(Mathf.Max(newIndex, 0), pivotControl.Parent.ChildCount - 1);

            return newIndex;
        }

        return -1;
    }

    /// <summary>
    /// Changes a control's parent.
    /// </summary>
    /// <param name="control">The control to move.</param>
    /// <param name="newParent">The new parent of the control.</param>
    public void ChangeControlParent(FudgetControl control, FudgetContainer newParent)
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

        _tree.DeselectSilent();
        List<object> expandedControls = GatherExpanded(_tree);
        _tree.RemoveChildren();

        List<TreeNode> newExpandedNodes = new List<TreeNode>();
        List<TreeNode> newSelectedNodes = new List<TreeNode>();

        FudgetControlNode rootNode = new FudgetControlNode(RootObject);
        TreeNode localNode = _tree.AddChild(rootNode);
        localNode.Text = Path.GetFileNameWithoutExtension(_asset != null ? _asset.Path : "Fudget UI");
        localNode.Tag = null;
        newExpandedNodes.Add(localNode);

        BuildForControl(RootObject, rootNode, expandedControls, ref newExpandedNodes, oldControlSelection, ref newSelectedNodes);

        foreach (TreeNode node in newExpandedNodes)
        {
            node.Expand(true);
        }

        _tree.SetSelectionSilent(newSelectedNodes);
        Focus();
    }

    private void SelectedControlChanged(List<TreeNode> before, List<TreeNode> after)
    {
        SetSelection(after);
        Undo.AddAction(new FudgetSelectionChangeAction(this, before, after));
    }

    private void UpdatePropertiesSelection(List<TreeNode> newSelection)
    {
        List<FudgetControl> selectedNodes = new List<FudgetControl>();
        foreach (TreeNode node in newSelection)
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
    public void SetSelection(List<TreeNode> newSelection)
    {
        _tree.SetSelectionSilent(newSelection);
        UpdatePropertiesSelection(newSelection);
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
        public FudgetTree() : base(true)
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

        /// <summary>
        /// Silently (no events fired) sets a new selection. This is used to safely undo without infinite recursion from events.
        /// </summary>
        /// <param name="selection"></param>
        public void SetSelectionSilent(List<TreeNode> selection)
        {
            List<TreeNode> newSelection = new List<TreeNode>();
            newSelection.AddRange(selection);

            Selection.Clear();
            Selection.AddRange(newSelection);

            for (int i = 0; i < Selection.Count; i++)
            {
                Selection[i].ExpandAllParents();
            }
        }

        /// <summary>
        /// Silently (no events fired) clears current select. This is used to safely undo.
        /// </summary>
        public void DeselectSilent()
        {
            Selection.Clear();
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
            menu.AddButton("Rename", (node as FudgetControlNode).StartRenaming);
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
                object[] attrs = type.GetAttributes(false);
                bool hide = false;
                foreach (var attr in attrs)
                    if (attr is HideInEditorAttribute)
                        hide = true;

                if (controlType.IsAssignableFrom(type) &&
                    !type.IsAbstract &&
                    !hide)
                {
                    string name = type.Name;
                    Debug.Log(name);
                    if (name.StartsWith("Fudget"))
                        name = name.Remove(0, 6);

                    ContextMenuButton button = newMenu.AddButton(FlaxEditor.Utilities.Utils.GetPropertyNameUI(name));
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
    public class FudgetControlNode : TreeNode
    {
        private DragFudgets _dragFudgets;
        private DragHandlers _dragHandlers;

        /// <summary>
        /// TODO: docs
        /// </summary>
        public FudgetControl Control => (FudgetControl)Tag;

        /// <summary>
        /// TODO: docs
        /// </summary>
        public FudgetControlNode(FudgetControl control)
        : base(false)
        {
            Text = control.Name;
            Tag = control;
        }

        /// <inheritdoc />
        protected override bool OnMouseDoubleClickHeader(ref Float2 location, MouseButton button)
        {
            if (Tag == null)
                return false;

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

        private bool ValidateDragFudget(FudgetControlNode controlNode)
        {
            /*// Reject dragging actors not linked to scene (eg. from prefab) or in the opposite way
            var thisHasScene = ActorNode.ParentScene != null;
            var otherHasScene = actorNode.ParentScene != null;
            if (thisHasScene != otherHasScene)
                return false;

            // Reject dragging actors between prefab windows (different roots)
            if (!thisHasScene && ActorNode.Root != actorNode.Root)
                return false;

            // Reject dragging parents and itself
            return actorNode.Actor != null && actorNode != ActorNode && actorNode.Find(Actor) == null;*/

            // TODO: Proper validation?
            return controlNode.Tag != null && controlNode != this;
        }

        /// <inheritdoc />
        protected override DragDropEffect OnDragEnterHeader(DragData data)
        {
            // Check if cannot edit scene or there is no scene loaded (handle case for actors in prefab editor)
            /*if (_actorNode?.ParentScene != null)
            {
                if (!Editor.Instance.StateMachine.CurrentState.CanEditScene || !Level.IsAnySceneLoaded)
                    return DragDropEffect.None;
            }
            else
            {
                if (!Editor.Instance.StateMachine.CurrentState.CanEditContent)
                    return DragDropEffect.None;
            }*/

            if (_dragHandlers == null)
                _dragHandlers = new DragHandlers();

            // Check if drop actors
            if (_dragFudgets == null)
            {
                _dragFudgets = new DragFudgets(ValidateDragFudget, ParentTree.Tag as FudgetsEditorWindow);
                _dragHandlers.Add(_dragFudgets);
            }
            if (_dragFudgets.OnDragEnter(data))
                return _dragFudgets.Effect;

            return DragDropEffect.None;
        }

        /// <inheritdoc />
        protected override DragDropEffect OnDragMoveHeader(DragData data)
        {
            return _dragHandlers.Effect;
        }

        /// <inheritdoc />
        protected override void OnDragLeaveHeader()
        {
            _dragHandlers.OnDragLeave();
        }

        /// <inheritdoc />
        protected override DragDropEffect OnDragDropHeader(DragData data)
        {
            var result = DragDropEffect.None;
            FudgetsEditorWindow window = ParentTree.Tag as FudgetsEditorWindow;

            FudgetControl myControl = Tag as FudgetControl;
            FudgetContainer newParent;
            int newOrder = -1;

            // Check if has no control (rules out the Root Control)
            if (myControl == null)
            {
                return result;
            }

            newParent = null;
            if (myControl is FudgetContainer container)
                newParent = container;

            // Use drag positioning to change target parent and index
            if (DragOverMode == DragItemPositioning.Above || DragOverMode == DragItemPositioning.Below)
            {
                newParent = myControl.Parent;
            }

            if (newParent == null)
            {
                throw new InvalidOperationException("Missing parent control.");
            }

            // Drag actors
            if (_dragFudgets != null && _dragFudgets.HasValidDrag)
            {
                var singleObject = _dragFudgets.Objects.Count == 1;
                if (singleObject)
                {
                    var targetControl = _dragFudgets.Objects[0].Tag as FudgetControl;
                    if (myControl.Parent != null)
                    {
                        if (DragOverMode == DragItemPositioning.Above)
                        {
                            newOrder = window.FindNewIndex(true, myControl, targetControl);
                        }
                        else if (DragOverMode == DragItemPositioning.Below)
                        {
                            newOrder = window.FindNewIndex(false, myControl, targetControl);
                        }
                    }

                    var action = new FudgetReorderControlAction(window, newParent, targetControl, newOrder);
                    window.Undo.AddAction(action);
                    action.Do();
                }
                else
                {
                    var targetControls = _dragFudgets.Objects.ConvertAll(x => x.Tag as FudgetControl);
                    var actions = new List<IUndoAction>();

                    foreach (var control in targetControls)
                    {
                        if (myControl.Parent != null)
                        {
                            if (DragOverMode == DragItemPositioning.Above)
                            {
                                newOrder = window.FindNewIndex(true, myControl, control);
                            }
                            else if (DragOverMode == DragItemPositioning.Below)
                            {
                                newOrder = window.FindNewIndex(false, myControl, control);
                            }
                        }

                        actions.Add(new FudgetReorderControlAction(window, newParent, control, newOrder));
                    }

                    if (actions.Count > 0)
                    {
                        MultiUndoAction multi = new MultiUndoAction(actions);
                        multi.Do();
                        window.Undo.AddAction(multi);
                    }
                }

                result = DragDropEffect.Move;
            }

            // Clear cache
            _dragHandlers.OnDragDrop(null);

            return result;
        }

        /// <inheritdoc />
        protected override void DoDragDrop()
        {
            DragData data;
            var tree = ParentTree;

            // Check if this node is selected
            if (tree.Selection.Contains(this))
            {
                // Get selected controls
                var controls = new List<FudgetControlNode>();
                for (var i = 0; i < tree.Selection.Count; i++)
                {
                    var e = tree.Selection[i];

                    // Skip if parent is already selected to keep correct parenting
                    if (tree.Selection.Contains(e.Parent))
                        continue;

                    if (e is FudgetControlNode node /*&& node.CanDrag*/)
                        controls.Add(node);
                }
                data = DragFudgets.GetDragData(controls);
            }
            else
            {
                data = DragFudgets.GetDragData(this);
            }

            // Start drag operation
            DoDragDrop(data);
        }
    }
}

/// <summary>
/// Controls references collection drag handler.
/// </summary>
public sealed class DragFudgets : DragFudgets<DragEventArgs>
{
    /// <summary>
    /// Initializes a new instance of the <see cref="DragFudgets"/> class.
    /// </summary>
    /// <param name="validateFunction">The validation function</param>
    /// <param name="window">TODO: docs</param>
    public DragFudgets(Func<FudgetControlNode, bool> validateFunction, FudgetsEditorWindow window)
    : base(validateFunction, window)
    {
    }
}

/// <summary>
/// Helper class for handling <see cref="FudgetControlNode"/> drag and drop.
/// </summary>
/// <seealso cref="FudgetControl" />
/// <seealso cref="FudgetControlNode" />
public class DragFudgets<U> : DragHelper<FudgetControlNode, U> where U : DragEventArgs
{
    /// <summary>
    /// The default prefix for drag data used for <see cref="FudgetControlNode"/>.
    /// </summary>
    public const string DragPrefix = "Fudget Control";

    /// <summary>
    /// TODO: docs
    /// </summary>
    public FudgetsEditorWindow _window;

    /// <summary>
    /// Creates a new DragHelper
    /// </summary>
    /// <param name="validateFunction">The validation function</param>
    /// <param name="window">TODO: docs</param>
    public DragFudgets(Func<FudgetControlNode, bool> validateFunction, FudgetsEditorWindow window)
    : base(validateFunction)
    {
        _window = window;
    }

    /// <summary>
    /// Gets the drag data.
    /// </summary>
    /// <param name="control">The actor.</param>
    /// <returns>The data.</returns>
    public DragData ToDragData(FudgetControl control) => GetDragData(control);

    /// <inheritdoc/>
    public override DragData ToDragData(FudgetControlNode item) => GetDragData(item);

    /// <inheritdoc/>
    public override DragData ToDragData(IEnumerable<FudgetControlNode> items) => GetDragData(items);

    /// <summary>
    /// Gets the drag data.
    /// </summary>
    /// <param name="control">The actor.</param>
    /// <returns>The data.</returns>
    public static DragData GetDragData(FudgetControl control)
    {
        if (control == null)
            throw new ArgumentNullException();

        return new DragDataText(DragPrefix + control.ID.ToString("N"));
    }

    /// <summary>
    /// Gets the drag data.
    /// </summary>
    /// <param name="item">The item.</param>
    /// <returns>The data.</returns>
    public static DragData GetDragData(FudgetControlNode item)
    {
        if (item == null)
            throw new ArgumentNullException();

        object value = item.Tag;
        if (value == null || value is not FudgetControl control)
            throw new ArgumentNullException();

        return new DragDataText(DragPrefix + control.ID.ToString("N"));
    }

    /// <summary>
    /// Gets the drag data.
    /// </summary>
    /// <param name="items">The items.</param>
    /// <returns>The data.</returns>
    public static DragData GetDragData(IEnumerable<FudgetControlNode> items)
    {
        if (items == null)
            throw new ArgumentNullException();

        string text = DragPrefix;
        foreach (var item in items)
        {
            object value = item.Tag;
            if (value == null || value is not FudgetControl control)
                continue;

            text += control.ID.ToString("N") + '\n';
        }

        return new DragDataText(text);
    }

    /// <inheritdoc/>
    public override IEnumerable<FudgetControlNode> FromDragData(DragData data)
    {
        if (data is DragDataText dataText)
        {
            string text = dataText.Text;
            if (dataText.Text.StartsWith(DragPrefix))
            {
                // Remove prefix and parse split names
                var ids = dataText.Text.Remove(0, DragPrefix.Length).Split('\n');
                var nodes = new List<FudgetControlNode>();
                foreach (string id in ids)
                {
                    if (Guid.TryParse(id, out Guid guid))
                    {
                        FudgetControl control = FlaxEngine.Object.Find<FudgetControl>(ref guid);
                        nodes.Add(_window.FindNodeByControl(control));
                    }
                }

                return nodes;
            }
        }

        return new FudgetControlNode[0];
    }
}

// Undo Actions

/// <summary>
/// Delete control undo action for fudget controls.
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
        _window.SelectNodeByControl(_deletionTarget);
        _done = false;

        _window.OnAnyChange();
    }
}

/// <summary>
/// Rename control undo action for fudget controls.
/// </summary>
public class FudgetRenameAction : IUndoAction
{
    private FudgetsEditorWindow _window;
    private FudgetControl _renameTarget;
    private string _oldName;
    private string _newName;

    /// <inheritdoc />
    public FudgetRenameAction(FudgetsEditorWindow window, FudgetControl renameTarget, string newName)
    {
        _window = window;
        _renameTarget = renameTarget;
        _newName = newName;
    }

    /// <inheritdoc />
    public string ActionString => "Rename control";

    /// <inheritdoc />
    public void Dispose()
    {
        _window = null;
        _renameTarget = null;
    }

    /// <inheritdoc />
    public void Do()
    {
        _oldName = _renameTarget.Name;
        _renameTarget.Name = _newName;
        _window.RefreshNames();

        _window.OnAnyChange();
    }

    /// <inheritdoc />
    public void Undo()
    {
        _renameTarget.Name = _oldName;
        _window.RefreshNames();

        _window.OnAnyChange();
    }
}

/// <summary>
/// New control added undo action for fudget controls.
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

    /// <inheritdoc />
    public FudgetNewControlAction(FudgetsEditorWindow window, FudgetsEditorWindow.FudgetTree tree, ScriptType newControlType, string newName)
    {
        _window = window;
        _tree = tree;
        _newControlType = newControlType;
        _newName = newName;
        _oldIndex = -1;
    }

    /// <inheritdoc />
    public string ActionString => "Add new control";

    /// <inheritdoc />
    public void Dispose()
    {
        _window = null;
        _createdControl = null;
        _oldParent = null;
        _tree = null;
    }

    /// <inheritdoc />
    public void Do()
    {
        _createdControl = (FudgetControl)_newControlType.CreateInstance();
        if (_createdControl == null)
            return;

        _createdControl.Name = _newName;
        _tree.OnNewControl(_window, _createdControl, _oldParent, _oldIndex);
        _oldParent = _createdControl.Parent;
    }

    /// <inheritdoc />
    public void Undo()
    {
        _oldIndex = _createdControl.IndexInParent;
        _window.DeleteControl(_createdControl);

        _window.OnAnyChange();
        FlaxEngine.Object.Destroy(_createdControl);
        _createdControl = null;
    }
}

/// <summary>
/// Selection changed undo action for fudget controls.
/// </summary>
public class FudgetSelectionChangeAction : IUndoAction
{
    private List<TreeNode> _before;
    private List<TreeNode> _after;
    private FudgetsEditorWindow _window;

    /// <inheritdoc />
    public FudgetSelectionChangeAction(FudgetsEditorWindow window, List<TreeNode> before, List<TreeNode> after)
    {
        _window = window;
        _before = new List<TreeNode>();
        _before.AddRange(before);

        _after = new List<TreeNode>();
        _after.AddRange(after);
    }

    /// <inheritdoc />
    public string ActionString => "Change selected control";

    /// <inheritdoc />
    public void Dispose()
    {
        _window = null;
        _before = null;
        _after = null;
    }

    /// <inheritdoc />
    public void Do()
    {
        _window.SetSelection(_after);
    }

    /// <inheritdoc />
    public void Undo()
    {
        _window.SetSelection(_before);
    }
}

/// <summary>
/// Control reordered undo action for fudget controls.
/// </summary>
public class FudgetReorderControlAction : IUndoAction
{
    private int _newOrder;
    private FudgetControl _movingControl;
    private FudgetContainer _newParent;
    private FudgetsEditorWindow _window;
    private FudgetContainer _oldParent;
    private int _oldOrder;

    /// <inheritdoc />
    public FudgetReorderControlAction(FudgetsEditorWindow window, FudgetContainer newParent, FudgetControl movingControl, int newOrder)
    {
        _window = window;
        _newOrder = newOrder;
        _movingControl = movingControl;
        _newParent = newParent;
    }

    /// <inheritdoc />
    public string ActionString => "Reorder control";

    /// <inheritdoc />
    public void Dispose()
    {
        _movingControl = null;
        _newParent = null;
        _window = null;
    }

    /// <inheritdoc />
    public void Do()
    {
        _oldOrder = _movingControl.IndexInParent;
        _oldParent = _movingControl.Parent;

        _window.ChangeControlParent(_movingControl, _newParent);
        _newParent.MoveChildToIndex(_movingControl.IndexInParent, _newOrder);
        _window.OnRebuildTree();
    }

    /// <inheritdoc />
    public void Undo()
    {
        if (_oldParent == null)
            return; // Controls shouldn't have no parent.

        _window.ChangeControlParent(_movingControl, _oldParent);
        _oldParent.MoveChildToIndex(_movingControl.IndexInParent, _oldOrder);
        _window.OnRebuildTree();
    }
}