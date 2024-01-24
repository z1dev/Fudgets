using System;
using System.Collections.Generic;
using FlaxEngine;
using FlaxEditor;
using FlaxEditor.Content;
using static FlaxEditor.GUI.Docking.DockHintWindow;

namespace FudgetsEditor;

/// <summary>
/// TestEditorPlugin Script.
/// </summary>
public class TestEditorPlugin : EditorPlugin
{
    private FudgetJsonAssetProxy _proxy;

    // TODO: Make this an asset editor to avoid this whole hacky BS
    public override void InitializeEditor()
    {
        base.InitializeEditor();
        Editor.UI.MenuTools.ContextMenu.AddButton("Test Fudgets Window").Clicked += () =>
        {
            FudgetsEditorWindow win = new FudgetsEditorWindow(Editor, false, FlaxEngine.GUI.ScrollBars.None);
            win?.Show();

            if (win == null)
            {
                Debug.Log("What the");
            }
        };

        _proxy = new FudgetJsonAssetProxy();
        Editor.ContentDatabase.AddProxy(_proxy, true);
    }

    /// <inheritdoc />
    public override void DeinitializeEditor()
    {
        Editor.ContentDatabase.RemoveProxy(_proxy, true);
        _proxy = null;

        base.DeinitializeEditor();
    }
}
