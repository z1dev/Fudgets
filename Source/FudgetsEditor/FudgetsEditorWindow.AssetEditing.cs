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
        _undo = new Undo();
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

    /// <summary>
    /// Saves the associated FudgetJsonAsset.
    /// </summary>
    public override void Save()
    {
        _asset.Save();
        ClearEditedFlag();
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
    }
}
