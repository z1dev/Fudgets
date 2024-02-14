#pragma once

#include "TextBoxBase.h"
#include "../Styling/PartPainters.h"

API_CLASS()
class FudgetLineEdit : public FudgetTextBoxBase
{
    using Base = FudgetTextBoxBase;
    DECLARE_SCRIPTING_TYPE(FudgetLineEdit);
public:
    API_PROPERTY() static FudgetToken GetClassToken();
    API_PROPERTY() static FudgetToken GetFramePainterToken();
    API_PROPERTY() static FudgetToken GetFrameStyleToken();
    API_PROPERTY() static FudgetToken GetTextPainterToken();
    API_PROPERTY() static FudgetToken GetTextStyleToken();

    API_PROPERTY() static FudgetToken GetCaretDrawToken();
    API_PROPERTY() static FudgetToken GetCaretBlinkTimeToken();
    API_PROPERTY() static FudgetToken GetCaretWidthToken();

    /// <inheritdoc />
    void OnInitialize() override;

    /// <inheritdoc />
    String GetText() const override { return _text; }
    /// <inheritdoc />
    int GetTextLength() const override { return _text.Length(); }
    /// <inheritdoc />
    void SetText(const String &value) override;

    /// <inheritdoc />
    void OnDraw() override;

    /// <inheritdoc />
    void OnSizeChanged() override;

    /// <inheritdoc />
    int CharIndexAt(Float2 pos) override;

    /// <inheritdoc />
    void DoPosChanged(int old_caret_pos, int old_sel_pos) override;

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
    bool IsWhitespace(int index) override;

    /// <inheritdoc />
    void InsertCharacter(int index, Char ch) override;

    /// <inheritdoc />
    void DeleteCharacters(int start_index, int end_index) override;

    /// <inheritdoc />
    void ReplaceCharacters(int start_index, int end_index, const String &with) override;

    /// <inheritdoc />
    void ReplaceCharacters(int start_index, int end_index, Char ch) override;
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

    FudgetPadding GetInnerPadding() const;

    void ScrollToPos();
    void FixScrollPos();

    String Process(const String &value) const;

    FudgetPainterStateHelper _draw_state;
    FudgetFramedFieldPainter *_frame_painter;
    FudgetPainterTextDrawOptions _text_options;
    FudgetTextPainter *_text_painter;

    float _blink_passed;
    int _character_scroll_count;

    FudgetDrawArea _caret_draw;
    float _caret_blink_time;
    float _caret_width;

    float _scroll_pos;
    String _text;
};