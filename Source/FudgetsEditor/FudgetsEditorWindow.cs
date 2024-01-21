using FlaxEditor;
using FlaxEditor.Windows;
using FlaxEngine;
using FlaxEngine.GUI;
using Fudgets;
using static FlaxEditor.Surface.Archetypes.Animation;

namespace FudgetsEditor;

/// <summary>
/// Fudget editing window.
/// </summary>

// TODO: Make this an asset editor window, and create an asset for Fudgets.
public class FudgetsEditorWindow : EditorWindow
{
    // TODO: Figure out how to (and if you can) simply store a reference to a FudgetGUIRoot
    private Fudget _fudget;
    private GPUTexture _texture = null;
    private RenderTask _renderTask = null;
    private Editor _editor;

    // TODO: Figure out how to properly resize the GPU Texture and whatnot during render time
    private Vector2 _resolution = new Vector2(1920, 1080);

    public FudgetsEditorWindow(Editor editor, bool hideOnClose, ScrollBars scrollBars) : base(editor, hideOnClose, scrollBars)
    {
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
            Object.Destroy(_texture);
            _texture = null;
        }

        if (_renderTask != null)
        {
            Object.Destroy(_renderTask);
            _renderTask = null;
        }

        Image image = AddChild<Image>();
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

    // Temporary
    private void ConfigureFudget()
    {
        FudgetGUIRoot root = _fudget.GUI;
        FudgetFilledBox box = new FudgetFilledBox();
        box.Color = Color.Blue;
        FudgetFilledBox box2 = new FudgetFilledBox();
        box2.Color = Color.Green;

        box.HintSize = new Float2(9999, 9999);
        box2.HintSize = new Float2(9999, 9999);

        root.CreateLayout<FudgetListLayout>();
        root.AddChild(box);
        root.AddChild(box2);
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
            Object.Destroy(_texture);
            _texture = null;
        }

        if (_renderTask != null)
        {
            Object.Destroy(_renderTask);
            _renderTask = null;
        }
    }
}