#pragma once

#include "TextBoxBase.h"
#include "TextBoxPainter.h"


/// <summary>
/// Multiline input box for unformatted basic text. Text can span over multiple lines either
/// when lines are too long and word wrapping is enabled, or if there are newline characters
/// in the text.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetTextBox : public FudgetTextBoxBase
{
    using Base = FudgetTextBoxBase;
    DECLARE_SCRIPTING_TYPE(FudgetTextBox);
public:
    API_PROPERTY() static FudgetToken GetClassToken();
    API_PROPERTY() static FudgetToken GetFramePainterToken();
    API_PROPERTY() static FudgetToken GetFrameStyleToken();
    API_PROPERTY() static FudgetToken GetTextPainterToken();
    API_PROPERTY() static FudgetToken GetTextStyleToken();

    API_PROPERTY() static FudgetToken GetCaretDrawToken();
    API_PROPERTY() static FudgetToken GetCaretBlinkTimeToken();
    API_PROPERTY() static FudgetToken GetCaretWidthToken();
    API_PROPERTY() static FudgetToken GetCaretScrollCountToken();
    API_PROPERTY() static FudgetToken GetSnapTopLineToken();

    /// <inheritdoc />
    void OnInitialize() override;

    /// <inheritdoc />
    String GetText() const override { return _text; }

    /// <inheritdoc />
    int GetTextLength() const override { return _text.Length(); }

    /// <inheritdoc />
    void OnDraw() override;

    /// <inheritdoc />
    void OnSizeChanged() override;

    /// <inheritdoc />
    int CharIndexAt(Float2 pos) override;

    /// <inheritdoc />
    void DoPositionChanged(int old_caret_pos, int old_sel_pos) override;

    /// <inheritdoc />
    void DoTextEdited(int old_caret_pos, int old_sel_pos) override;

    /// <inheritdoc />
    void OnUpdate(float delta_time) override;

    /// <inheritdoc />
    void OnFocusChanged(bool focused, FudgetControl *other) override;

    /// <inheritdoc />
    void OnVirtuallyEnabledChanged() override;

    /// <inheritdoc />
    void OnMouseMove(Float2 pos, Float2 global_pos) override;

    /// <inheritdoc />
    FudgetInputResult OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click) override;

    /// <inheritdoc />
    bool OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button) override;

    /// <summary>
    /// Whether the lines of a text should wrap to the new line if they are too long.
    /// </summary>
    API_PROPERTY() bool GetLineWrap() const { return _line_wrap; }
    /// <summary>
    /// Whether the lines of a text should wrap to the new line if they are too long.
    /// </summary>
    API_PROPERTY() void SetLineWrap(bool value);

    /// <summary>
    /// Method used to wrap long lines to the next line
    /// </summary>
    API_PROPERTY() FudgetLineWrapMode GetLineWrapMode() const { return _wrap_mode; }

    /// <summary>
    /// Method used to wrap long lines to the next line
    /// </summary>
    API_PROPERTY() void SetLineWrapMode(FudgetLineWrapMode value);

protected:
    /// <inheritdoc />
    FudgetControlFlags GetInitFlags() const override;

    /// <inheritdoc />
    void SetTextInternal(const String &value) override;

    /// <inheritdoc />
    bool IsWhitespace(int index) const override;

    /// <inheritdoc />
    void InsertCharacter(int index, Char ch) override;

    /// <inheritdoc />
    void DeleteCharacters(int start_index, int end_index) override;

    /// <inheritdoc />
    void ReplaceCharacters(int start_index, int end_index, const String &with) override;

    /// <inheritdoc />
    void ReplaceCharacters(int start_index, int end_index, Char ch) override;

    /// <inheritdoc />
    int GetCaretPosUp() override;

    /// <inheritdoc />
    int GetCaretPosDown() override;

    /// <inheritdoc />
    int GetCaretPosPageUp() override;

    /// <inheritdoc />
    int GetCaretPosPageDown() override;

    /// <inheritdoc />
    FudgetTextBoxFlags GetTextBoxInitFlags() const override;
private:
    static void InitializeTokens();

    static FudgetToken ClassToken;
    static FudgetToken FramePainterToken;
    static FudgetToken FrameStyleToken;
    static FudgetToken TextPainterToken;
    static FudgetToken TextStyleToken;

    static FudgetToken CaretDrawToken;
    static FudgetToken CaretBlinkTimeToken;
    static FudgetToken CaretWidthToken;
    static FudgetToken CaretScrollCountToken;

    static FudgetToken SnapTopLineToken;

    FudgetPadding GetInnerPadding() const;

    void ScrollToPos();
    void FixScrollPos();

    void Process();

    FudgetPainterStateHelper _draw_state;
    FudgetFramedFieldPainter *_frame_painter;
    FudgetMultiLineTextPainter *_text_painter;
    FudgetMultilineTextMeasurements _text_measurements;

    float _blink_passed;
    int _character_scroll_count;

    // Make the line snap to the top, letting partial lines show on the bottom when scrolling down.
    bool _snap_top_line;

    FudgetDrawArea _caret_draw;
    float _caret_blink_time;
    float _caret_width;

    // Rightmost X position of the caret when moving with the up and down arrows. This is changed to -1 when
    // the caret position changes by a method that should reset it.
    float _caret_updown_x;

    Float2 _scroll_pos;
    String _text;

    bool _line_wrap;
    FudgetLineWrapMode _wrap_mode;

    bool _draw_ellipsis;
};