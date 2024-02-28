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
    /// Initializes tokens used by the styles
    /// </summary>
    API_FUNCTION() static void InitializeTokens();
    /// <summary>
    /// Frame Painter token
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken FramePainterToken;
    /// <summary>
    /// Frame Style token
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken FrameStyleToken;
    /// <summary>
    /// Text Painter token
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken TextPainterToken;
    /// <summary>
    /// Text Style token
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken TextStyleToken;

    /// <summary>
    /// Caret Draw token, color, draw area or drawable
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken CaretDrawToken;
    /// <summary>
    /// Caret blink time token, time of visible and invisible caret
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken CaretBlinkTimeToken;
    /// <summary>
    /// Caret width token, the size of the caret
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken CaretWidthToken;
    /// <summary>
    /// Caret scroll count token, how many characters to scroll sideways in view when caret moves out of bounds
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken CaretScrollCountToken;

    /// <inheritdoc />
    void OnInitialize() override;

    /// <inheritdoc />
    Float2 GetLayoutHintSize() const override;

    /// <inheritdoc />
    Float2 GetLayoutMinSize() const override;

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

    ///// <inheritdoc />
    //void OnFocusChanged(bool focused, FudgetControl *other) override;

    ///// <inheritdoc />
    //void OnVirtuallyEnabledChanged() override;

    /// <inheritdoc />
    void OnMouseMove(Float2 pos, Float2 global_pos) override;

    /// <inheritdoc />
    FudgetInputResult OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click) override;

    /// <inheritdoc />
    bool OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button) override;
protected:
    /// <inheritdoc />
    FudgetControlFlag GetInitFlags() const override;

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
    FudgetPadding GetInnerPadding() const;

    void ScrollToPos();
    void FixScrollPos();

    void Process(const StringView &value);

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