#pragma once

#include "TextBoxBase.h"
#include "../Styling/PartPainters.h"

/// <summary>
/// Single line input box for unformatted basic text
/// </summary>
API_CLASS()
class FudgetLineEdit : public FudgetTextBoxBase
{
    using Base = FudgetTextBoxBase;
    DECLARE_SCRIPTING_TYPE(FudgetLineEdit);
public:
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetClassToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetFramePainterToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetFrameStyleToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetTextPainterToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetTextStyleToken();

    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetCaretDrawToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetCaretBlinkTimeToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetCaretWidthToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetCaretScrollCountToken();

    /// <inheritdoc />
    void OnInitialize() override;

    /// <inheritdoc />
    StringView GetText() const override { return _text; }

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
protected:
    /// <inheritdoc />
    FudgetControlFlags GetInitFlags() const override;

    /// <inheritdoc />
    void SetTextInternal(const StringView &value) override;

    /// <inheritdoc />
    bool IsWhitespace(int index) const override;

    /// <inheritdoc />
    void InsertCharacter(int index, Char ch) override;

    /// <inheritdoc />
    void DeleteCharacters(int start_index, int end_index) override;

    /// <inheritdoc />
    void ReplaceCharacters(int start_index, int end_index, const StringView &with) override;

    /// <inheritdoc />
    void ReplaceCharacters(int start_index, int end_index, Char ch) override;

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

    FudgetPadding GetInnerPadding() const;

    void ScrollToPos();
    void FixScrollPos();

    void Process(const StringView &value);

    FudgetPainterStateHelper _draw_state;
    FudgetFramedFieldPainter *_frame_painter;
    FudgetSingleLineTextPainter *_text_painter;

    float _blink_passed;
    int _character_scroll_count;

    FudgetDrawArea _caret_draw;
    float _caret_blink_time;
    float _caret_width;

    float _scroll_pos;
    String _text;
};