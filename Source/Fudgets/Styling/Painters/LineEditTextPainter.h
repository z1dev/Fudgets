#pragma once

#include "PartPainters.h"



API_STRUCT()
struct FUDGETS_API FudgetTextFieldMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetTextFieldMapping);

    template<typename S, typename SB, typename ST, typename TC, typename STC>
    FudgetTextFieldMapping(S state, SB sel_bg_draw, ST sel_bg_tint, TC text_color, STC sel_text_color) : State((uint64)state), SelectionDraw((int)sel_bg_draw), SelectionTint((int)sel_bg_tint), TextColor((int)text_color), SelectedTextColor((int)sel_text_color) {}
    FudgetTextFieldMapping() {}

    API_FIELD() uint64 State = 0;

    API_FIELD() int SelectionDraw = 0;
    API_FIELD() int SelectionTint = 0;
    API_FIELD() int TextColor = 0;
    API_FIELD() int SelectedTextColor = 0;
};
template<>
struct TIsPODType<FudgetTextFieldMapping>
{
    enum { Value = true };
};

API_STRUCT(Attributes = "HideInEditor")
struct FUDGETS_API FudgetTextPainterMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetTextPainterMapping);

    API_FIELD() Array<FudgetTextFieldMapping> Mappings;

    API_FIELD() int Font = 0;
};

/// <summary>
/// Painter of unformatted text drawn with a single font, that can have spans which should be drawn
/// as selectable
/// </summary>
API_CLASS()
class FUDGETS_API FudgetLineEditTextPainter : public FudgetSingleLineTextPainter
{
    using Base = FudgetSingleLineTextPainter;
    DECLARE_SCRIPTING_TYPE(FudgetLineEditTextPainter);
public:
    using Mapping = FudgetTextPainterMapping;

    /// <inheritdoc />
    void Initialize(FudgetControl *control, /*FudgetStyle *style_override,*/ const Variant &mapping) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, 
        FudgetVisualControlState state, const FudgetSingleLineTextOptions &text_options) override;

    /// <inheritdoc />
    Int2 Measure(FudgetControl *control, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state,
        const FudgetSingleLineTextOptions &text_options) override;

    /// <inheritdoc />
    int GetKerning(Char a, Char b, float scale) const override;

    /// <inheritdoc />
    int HitTest(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range,
        FudgetVisualControlState state, const FudgetSingleLineTextOptions &text_options, const Int2 &point) override;

    /// <inheritdoc />
    int GetFontHeight() const override;

private:
    struct DrawMapping
    {
        uint64 _state;

        FudgetDrawable *_sel_draw;
        Color _sel_draw_tint;
        Color _text_color;
        Color _sel_text_color;
    };

    Array<DrawMapping> _mappings;

    FudgetFont _font;
};