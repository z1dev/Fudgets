#pragma once

#include "../Control.h"

API_CLASS()
class FUDGETS_API FudgetTextBoxBase : public FudgetControl
{
    using Base = FudgetControl;
    DECLARE_SCRIPTING_TYPE(FudgetTextBoxBase);
public:
    /// <summary>
    /// Text displayed in the text box.
    /// </summary>
    API_PROPERTY() virtual String GetText() const { return TEXT(""); }
    /// <summary>
    /// Text displayed in the text box.
    /// </summary>
    API_PROPERTY() virtual int GetTextLength() const { return 0; }

    /// <summary>
    /// Text displayed in the text box.
    /// </summary>
    API_PROPERTY() virtual void SetText(const String &value) { }

    /// <summary>
    /// Position in the text where new characters will be inserted or used as the starting point of deletion.
    /// If the text box has selection, the caret will be at the movable end of the selected area.
    /// Changing the caret position will deselect the selected text.
    /// </summary>
    API_PROPERTY() FORCE_INLINE int GetCaretPos() const { return _caret_pos; }
    /// <summary>
    /// Position in the text where new characters will be inserted or used as the starting point of deletion.
    /// If the text box has selection, the caret will be at the movable end of the selected area.
    /// Changing the caret position will deselect the selected text.
    /// </summary>
    API_PROPERTY() void SetCaretPos(int value);

    /// <summary>
    /// The starting position or stationary end of the text selection. If there is no selection in the text box, this is the same as
    /// the current caret position. Setting this value to zero or positive number will move the caret and deselect the selected text.
    /// </summary>
    API_PROPERTY() FORCE_INLINE int GetSelStart() const { return _sel_pos < 0 ? _caret_pos : _sel_pos; }
    /// <summary>
    /// The starting position or stationary end of the text selection. If there is no selection in the text box, this is the same as
    /// the current caret position. Setting this value to zero or positive number will move the caret and deselect the selected text.
    /// </summary>
    API_PROPERTY() void SetSelStart(int value);
    /// <summary>
    /// The end position of the text selection. This is always the caret position. Setting this value will change the caret position
    /// and update the selection to be between the text selection's starting position and the caret position.
    /// </summary>
    API_PROPERTY() FORCE_INLINE int GetSelEnd() const { return _caret_pos; }
    /// <summary>
    /// The end position of the text selection. This is always the caret position. Setting this value will change the caret position
    /// and update the selection to be between the text selection's starting position and the caret position.
    /// </summary>
    API_PROPERTY() void SetSelEnd(int value);
    /// <summary>
    /// Number of characters between the text selection's starting position to the caret position. This value is negative
    /// if the caret position is in front of the selection's starting position. Changing this value will change the selection and
    /// move the caret relative to the text selection's starting position. Call SetSelection to change both the selection's start
    /// and the caret position at the same time.
    /// </summary>
    API_PROPERTY() FORCE_INLINE int GetSelLength() const { return _sel_pos < 0 ? 0 : _caret_pos - _sel_pos; }
    /// <summary>
    /// Number of characters between the text selection's starting position to the caret position. This value is negative
    /// if the caret position is in front of the selection's starting position. Changing this value will change the selection and
    /// move the caret relative to the text selection's starting position. Call SetSelection to change both the selection's start
    /// and the caret position at the same time.
    /// </summary>    
    API_PROPERTY() void SetSelLength(int value);

    /// <summary>
    /// Changes the current text selection in the text box and places the caret at the end of the selection. The selectionLength
    /// can be negative, to place the caret at the front of the selected text. If selectionStart is outside the text's range, it
    /// will be clamped, and selectionLength is updated to match it.
    /// </summary>
    /// <param name="selectionStart">The starting position or stationary end of the selection</param>
    /// <param name="selectionLength">The number of characters to select or the distance of the end of the selection from selectionStart</param>
    API_FUNCTION() void SetSelection(int selectionStart, int selectionLength);

    /// <summary>
    /// Called when the caret or selection position changes. If the change was due to an edit action that changed the
    /// text as well, DoTextEdited is called instead.
    /// </summary>
    /// <param name="old_caret_pos">Caret position before the change</param>
    /// <param name="old_sel_pos">Selection position before the change</param>
    API_FUNCTION() virtual void DoPosChanged(int old_caret_pos, int old_sel_pos) { }

    /// <summary>
    /// Called when the text changed because of an edit action, like typing a character or pasting text.
    /// </summary>
    /// <param name="old_caret_pos">Caret position before the change</param>
    /// <param name="old_sel_pos">Selection position before the change</param>
    API_FUNCTION() virtual void DoTextEdited(int old_caret_pos, int old_sel_pos) { }

    API_FUNCTION() virtual int CharIndexAt(Float2 pos) { return 0; };

    /// <inheritdoc />
    FudgetInputResult OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click) override;

    /// <inheritdoc />
    bool OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button) override;

    /// <inheritdoc />
    void OnMouseMove(Float2 pos, Float2 global_pos) override;

    /// <inheritdoc />
    bool WantsNavigationKey(KeyboardKeys key) override;

    /// <inheritdoc />
    FudgetInputResult OnCharInput(Char ch) override;

    /// <inheritdoc />
    FudgetInputResult OnKeyDown(KeyboardKeys key) override;

    /// <inheritdoc />
    bool OnKeyUp(KeyboardKeys key) override;

    /// <summary>
    /// Deletes the selected part of the edited text
    /// </summary>
    API_FUNCTION() void DeleteSelected();

    /// <summary>
    /// Replaces the selected part of the edited text with a string. If no text is selected, inserts the text at
    /// the caret's position
    /// </summary>
    /// <param name="with">The replacement string</param>
    API_FUNCTION() void ReplaceSelected(const String &with);

    /// <summary>
    /// Replaces the selected part of the edited text with a character. If no text is selected, inserts the character at
    /// the caret's position
    /// </summary>
    /// <param name="ch">The replacement character</param>
    API_FUNCTION() void ReplaceSelected(Char ch);

protected:
    /// <inheritdoc />
    FudgetControlFlags GetInitFlags() const override;

    /// <summary>
    /// Checks if a character in the text is a whitespace character.
    /// </summary>
    /// <param name="index">Character index</param>
    /// <returns>Whether character at index is whitespace or not</returns>
    API_FUNCTION() virtual bool IsWhitespace(int index) { return false; }

    /// <summary>
    /// Inserts a single character into the editor's text.
    /// </summary>
    /// <param name="index">Character index</param>
    /// <param name="ch">The character to add</param>
    API_FUNCTION() virtual void InsertCharacter(int index, Char ch) {}

    /// <summary>
    /// Removes characters from the text starting at an index, and before the ending index.
    /// </summary>
    /// <param name="start_index">Index of first character to remove</param>
    /// <param name="end_index">Index after last character to remove</param>
    API_FUNCTION() virtual void DeleteCharacters(int start_index, int end_index) {}

    /// <summary>
    /// Replaces characters in the text starting at an index before the ending index.
    /// </summary>
    /// <param name="start_index">Index of first character to replace</param>
    /// <param name="end_index">Index after last character to replace</param>
    /// <param name="with">Replacement string</param>
    API_FUNCTION() virtual void ReplaceCharacters(int start_index, int end_index, const String &with) {}

    /// <summary>
    /// Replaces characters in the text starting at an index before the ending index.
    /// </summary>
    /// <param name="start_index">Index of first character to replace</param>
    /// <param name="end_index">Index after last character to replace</param>
    /// <param name="ch">Replacement character</param>
    API_FUNCTION() virtual void ReplaceCharacters(int start_index, int end_index, Char ch) {}

private:
    void SetCaretPosInner(int value);

    int _caret_pos;
    int _sel_pos;

    bool _key_selecting;
    bool _mouse_selecting;

    // Ctrl key is pressed, move between words when going left and right
    bool _word_skip;
};


