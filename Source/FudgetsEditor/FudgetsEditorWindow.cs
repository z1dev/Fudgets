using FlaxEditor;
using FlaxEditor.Content;
using FlaxEditor.Content.Import;
using FlaxEditor.CustomEditors;
using FlaxEditor.Gizmo;
using FlaxEditor.GUI;
using FlaxEditor.GUI.ContextMenu;
using FlaxEditor.GUI.Tree;
using FlaxEditor.Options;
using FlaxEditor.SceneGraph;
using FlaxEditor.Viewport;
using FlaxEditor.Windows;
using FlaxEditor.Windows.Assets;
using FlaxEngine;
using FlaxEngine.GUI;
using FlaxEngine.Json;
using Fudgets;
using System;
using System.Collections.Generic;
using System.IO;
using System.Xml.Linq;

namespace FudgetsEditor;

/// <summary>
/// Fudget editing window.
/// </summary>

// TODO: Make this an asset editor window, and create an asset for Fudgets.
public partial class FudgetsEditorWindow : AssetEditorWindowBase<FudgetJsonAsset>, IPresenterOwner
{
    // Fudget Data
    private FudgetJsonAsset _asset;
    private FudgetJsonAssetItem _assetItem;

    /// <summary>
    /// TODO: docs
    /// </summary>
    public FudgetGUIRoot RootObject = null;

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
    private FudgetTree _tree;

    // TODO: Figure out how to properly resize the GPU Texture and whatnot during render time
    private Vector2 _resolution = new Vector2(1920, 1080);
    private CustomEditorPresenter _fudgetControlsEditor;

    private List<FudgetControl> _selectedControls = null;

    /// <summary>
    /// Empty.
    /// </summary>
    public EditorViewport PresenterViewport => null;

    /// <summary>
    /// TODO: docs
    /// </summary>
    public FudgetsEditorWindow(Editor editor, AssetItem item) : base(editor, item)
    {
        _assetItem = (FudgetJsonAssetItem)item;
        _asset = Content.Load<FudgetJsonAsset>(_assetItem.ID);
        RootObject = _asset.WidgetData;
        _selectedControls = new List<FudgetControl>();

        _editor = editor;
        _undo = new Undo();
        RootObject.HintSize = _resolution;

        // TODO: Figure out how to get update events the FudgetGUIRoot so it can update its layouting when things get moved.
        // Also figure out how edit-time UI should work. Do we skip rendering/layouting in the scene if someone decides to edit during play mode? Or do we make that impossible?
        //_resolution = Screen.Size;

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
        _panel.Offsets = new Margin(0, 0, _toolstrip.Bottom, 0);
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
        _tree = _editorTreePanel.AddChild<FudgetTree>();
        _tree.AnchorPreset = AnchorPresets.StretchAll;
        _tree.SelectedChanged += SelectedControlChanged;
        _tree.Tag = this;
        OnRebuildTree();

        _fudgetControlsEditor = new CustomEditorPresenter(_undo, "No control selected.", this);
        _fudgetControlsEditor.Features = FeatureFlags.None;
        _fudgetControlsEditor.Modified += ControlsValueEditorUpdate;
        _fudgetControlsEditor.Panel.Parent = _editorPropertiesPanel;

        FudgetRenderImage image = _imagePanel.AddChild<FudgetRenderImage>();
        image.AnchorPreset = AnchorPresets.StretchAll;
        image.Tag = this;

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

        SetupToolstrip();
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
    protected override void OnClose()
    {
        base.OnClose();
        _tree.Deinitialize();
    }

    private void RefreshNames()
    {
        List<ItemNode> nodes = RecurseAndGetSubset(_tree, (node) => { return true; });
        foreach (TreeNode node in nodes)
        {
            if (node.Tag is not FudgetControl control)
            {
                continue;
            }

            node.Text = control.Name;
        }
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

    private void BuildForControl(FudgetContainer container, ContainerControl currentTreeNode, List<object> expanded, ref List<TreeNode> newExpandedNodes, List<FudgetControl> selected, ref List<TreeNode> newSelectedNodes)
    {
        for (int i = 0; i < container.ChildCount; i++)
        {
            FudgetControl control = container.ChildAt(i);
            ItemNode newNode = new ItemNode(control);
            TreeNode node = currentTreeNode.AddChild(newNode);
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
        RootObject.Draw();
        Render2D.End();
    }

    /// <summary>
    /// TODO: docs
    /// </summary>
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
}