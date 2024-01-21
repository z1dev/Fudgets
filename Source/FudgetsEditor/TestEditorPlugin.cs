using System;
using System.Collections.Generic;
using FlaxEngine;
using FlaxEditor;

namespace FudgetsEditor;

/// <summary>
/// TestEditorPlugin Script.
/// </summary>
public class TestEditorPlugin : EditorPlugin
{
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
                Debug.Log("What the");//
            }
        };
    }
}
