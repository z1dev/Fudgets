﻿using FlaxEditor;
using FlaxEditor.CustomEditors;
using FlaxEditor.GUI.Tree;
using FlaxEditor.SceneGraph;
using FlaxEditor.Viewport;
using FlaxEditor.Windows;
using FlaxEngine;
using FlaxEngine.GUI;
using Fudgets;
using System;
using System.Collections.Generic;

namespace FudgetsEditor;

/// <summary>
/// Fudget editing window.
/// </summary>

// TODO: Make this an asset editor window, and create an asset for Fudgets.
public class FudgetsEditorWindow : EditorWindow, IPresenterOwner
{
    // TODO: Figure out how to (and if you can) simply store a reference to a FudgetGUIRoot
    private Fudget _fudget;
    private GPUTexture _texture = null;
    private RenderTask _renderTask = null;
    private Editor _editor;

    // Window controls
    private SplitPanel _panel;
    private Panel _imagePanel;
    private Panel _editorRootPanel;
    private SplitPanel _editorPanel;

    // Editor window controls
    private Panel _editorTreePanel;
    private Panel _editorPropertiesPanel;
    private Tree _tree;

    // TODO: Figure out how to properly resize the GPU Texture and whatnot during render time
    private Vector2 _resolution = new Vector2(1920, 1080);
    private CustomEditorPresenter _fudgetControlsEditor;

    private List<FudgetControl> _selectedControls = null;

    public EditorViewport PresenterViewport => null;

    public FudgetsEditorWindow(Editor editor, bool hideOnClose, ScrollBars scrollBars) : base(editor, hideOnClose, scrollBars)
    {
        _selectedControls = new List<FudgetControl>();

        _editor = editor;
        _resolution = Screen.Size;

        // Temporary hack
        // TODO: Replace this with proper deserialization of a FudgetGUIRoot
        _fudget = Level.FindActor<Fudget>();
        if (_fudget == null)
        {
            return;
        }
        
        // TODO: Figure out how to get update events the FudgetGUIRoot so it can update its layouting when things get moved.
        // Also figure out how edit-time UI should work. Do we skip rendering/layouting in the scene if someone decides to edit during play mode? Or do we make that impossible?
        ConfigureFudget();

        if (_texture != null)
        {
            FlaxEngine.Object.Destroy(_texture);
            _texture = null;
        }

        if (_renderTask != null)
        {
            FlaxEngine.Object.Destroy(_renderTask);
            _renderTask = null;
        }

        // Main Panel
        _panel = AddChild(new SplitPanel());
        _panel.AnchorPreset = AnchorPresets.StretchAll;
        _panel.SplitterValue = 0.7f;
        _imagePanel = _panel.Panel1;
        _editorRootPanel = _panel.Panel2;

        // Main Split Panels
        _imagePanel.AnchorPreset = AnchorPresets.StretchAll;
        _editorRootPanel.AnchorPreset = AnchorPresets.StretchAll;

        // Editor Panel
        _editorPanel = _editorRootPanel.AddChild(new SplitPanel(orientation: Orientation.Vertical));
        _editorPanel.AnchorPreset = AnchorPresets.StretchAll;
        _editorPanel.SplitterValue = 0.7f;
        _editorTreePanel = _editorPanel.Panel1;
        _editorPropertiesPanel = _editorPanel.Panel2;

        // Editor Split Panels
        _tree = _editorTreePanel.AddChild<Tree>();
        _tree.AnchorPreset = AnchorPresets.StretchAll;
        _tree.SelectedChanged += SelectedControlChanged;
        OnRebuildTree();

        _fudgetControlsEditor = new CustomEditorPresenter(null, "No control selected.", this); // No undo for knowledge editing
        _fudgetControlsEditor.Features = FeatureFlags.None;
        _fudgetControlsEditor.Modified += ControlsEditorChanged;
        _fudgetControlsEditor.Panel.Parent = _editorPropertiesPanel;

        Image image = _imagePanel.AddChild<Image>();
        image.AnchorPreset = AnchorPresets.StretchAll;

        GPUTextureBrush brush = new GPUTextureBrush();

        // Init Fudget Rendering
        _texture = new GPUTexture();
        var desc = GPUTextureDescription.New2D(
            (int)_resolution.X,
            (int)_resolution.Y,
            PixelFormat.R8G8B8A8_UNorm);
        _texture.Init(ref desc);

        _renderTask = new RenderTask();
        _renderTask.Render += RenderWindow;
        _renderTask.Enabled = true;
        
        brush.Texture = _texture;

        image.Color = Color.White;
        image.KeepAspectRatio = false;
        image.Brush = brush;
    }

    private void ControlsEditorChanged()
    {
        RefreshNames();
    }

    private void ChangeControlParent(FudgetControl control, FudgetContainer newParent)
    {
        TreeNode controlNode = FindNodeByControl(control);
        TreeNode newParentNode = FindNodeByControl(newParent);

        controlNode.Parent = newParentNode;
        control.Parent = newParent;
    }

    private void DeleteControl(FudgetControl control)
    {
        TreeNode controlNode = FindNodeByControl(control);
        controlNode.Parent = null;
        control.Parent = null;

        List<TreeNode> nodes = RecurseAndGetSubset(controlNode, (node) => { return true; });
        foreach (TreeNode node in nodes)
        {
            if (node.Tag != null)
            {
                if (node.Tag is not FudgetControl tagControl)
                {
                    continue;
                }

                FlaxEngine.Object.Destroy(tagControl);
            }
        }

        FlaxEngine.Object.Destroy(control);
        controlNode.DisposeChildren();
        controlNode.Dispose();
    }

    private List<TreeNode> RecurseAndGetSubset(ContainerControl current, Func<TreeNode, bool> validator)
    {
        List<TreeNode> subset = new List<TreeNode>();
        foreach (Control child in current.Children)
        {
            if (child is not TreeNode node)
            {
                continue;
            }

            if (validator(node))
            {
                subset.Add(node);
            }

            subset.AddRange(RecurseAndGetSubset(node, validator));
        }

        return subset;
    }

    private void RefreshNames()
    {
        List<TreeNode> nodes = RecurseAndGetSubset(_tree, (node) => { return true; });
        foreach (TreeNode node in nodes)
        {
            if (node.Tag is not FudgetControl control)
            {
                continue;
            }

            node.Text = control.Name;
        }
    }

    private TreeNode FindNodeByControl(FudgetControl control)
    {
        List<TreeNode> subset = RecurseAndGetSubset(_tree, (node) =>
        {
            if (node.Tag == (object) control)
            {
                return true;
            }

            return false;
        });

        return subset.Count >= 1 ? subset[0] : null;
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

    // Temporary
    private void ConfigureFudget()
    {
        FudgetGUIRoot root = _fudget.GUI;
        FudgetFilledBox box = new FudgetFilledBox();
        box.Color = Color.Blue;
        FudgetFilledBox box2 = new FudgetFilledBox();
        box2.Color = Color.Green;

        FudgetContainer containerTest = new FudgetContainer();
        FudgetFilledBox box3 = new FudgetFilledBox();
        box3.Color = Color.DarkRed;
        FudgetFilledBox box4 = new FudgetFilledBox();
        box4.Color = Color.Beige;

        box.HintSize = new Float2(9999, 9999);
        box2.HintSize = new Float2(9999, 9999);
        box3.HintSize = new Float2(9999, 9999);
        box4.HintSize = new Float2(9999, 9999);
        containerTest.HintSize = new Float2(9999, 9999);

        containerTest.CreateLayout<FudgetListLayout>();
        containerTest.AddChild(box3);
        containerTest.AddChild(box4);

        box.Name = "Box";
        box2.Name = "Box 2";
        box3.Name = "Box 3";
        box4.Name = "Box 4";
        containerTest.Name = "Container.";

        root.CreateLayout<FudgetListLayout>();
        root.AddChild(box);
        root.AddChild(box2);
        root.AddChild(containerTest);
    }

    private List<object> GatherExpanded(ContainerControl current)
    {
        List<object> output = new List<object>();
        foreach (Control child in current.Children)
        {
            if (child is not TreeNode node)
            {
                continue;
            }

            if (node.IsExpanded)
            {
                output.Add(node.Tag);
            }

            output.AddRange(GatherExpanded(node));
        }

        return output;
    }

    private void OnRebuildTree()
    {
        List<FudgetControl> oldControlSelection = _selectedControls;

        _tree.Deselect();
        List<object> expandedControls = GatherExpanded(_tree);
        _tree.RemoveChildren();

        List<TreeNode> newExpandedNodes = new List<TreeNode>();
        List<TreeNode> newSelectedNodes = new List<TreeNode>();
        BuildForControl(_fudget.GUI, _tree, expandedControls, ref newExpandedNodes, oldControlSelection, ref newSelectedNodes);

        foreach (TreeNode node in newExpandedNodes)
        {
            node.Expand(true);
        }

        _tree.Select(newSelectedNodes);
    }

    private void BuildForControl(FudgetContainer container, ContainerControl currentTreeNode, List<object> expanded, ref List<TreeNode> newExpandedNodes, List<FudgetControl> selected, ref List<TreeNode> newSelectedNodes)
    {
        for (int i = 0; i < container.ChildCount; i++)
        {
            FudgetControl control = container.ChildAt(i);
            TreeNode node = currentTreeNode.AddChild<TreeNode>();
            node.Text = control.Name;
            node.Tag = control;

            if (selected.Contains(control))
            {
                newSelectedNodes.Add(node);
            }

            if (control is FudgetContainer con)
            {
                if (expanded.Contains(control))
                {
                    newExpandedNodes.Add(node);
                }

                BuildForControl(con, node, expanded, ref newExpandedNodes, selected, ref newSelectedNodes);
            }
        }
    }

    private void RenderWindow(RenderTask task, GPUContext context)
    {
        // TODO: Figure out weird behavior, like why does the Fudget also render in the editor/game view when I use the rendering method here?
        // TODO: Test if moving controls around still draws as expected and layouts properly (this requires feeding update events to the FudgetGUIRoot)
        Render2D.Begin(context, _texture);
        _fudget.GUI.Draw();
        Render2D.End();
    }

    public override void OnDestroy()
    {
        base.OnDestroy();
        if (_texture != null)
        {
            FlaxEngine.Object.Destroy(_texture);
            _texture = null;
        }

        if (_renderTask != null)
        {
            FlaxEngine.Object.Destroy(_renderTask);
            _renderTask = null;
        }
    }

    public void Select(List<SceneGraphNode> nodes)
    {
    }
}