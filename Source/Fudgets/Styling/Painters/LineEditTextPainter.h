#pragma once

#include "PartPainters.h"


API_STRUCT(Attributes = "HideInEditor")
struct FUDGETS_API FudgetTextPainterMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetTextPainterMapping);

    API_FIELD() int BackgroundDraw = 0;
    API_FIELD() int BackgroundTint = 0;
    API_FIELD() int TextColor = 0;

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
    void Initialize(FudgetControl *control, const Variant &mapping) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, 
        uint64 state, const FudgetSingleLineTextOptions &text_options) override;

    /// <inheritdoc />
    Int2 Measure(FudgetControl *control, const StringView &text, const FudgetTextRange &range, uint64 state,
        const FudgetSingleLineTextOptions &text_options) override;

    /// <inheritdoc />
    int GetKerning(Char a, Char b, float scale) const override;

    /// <inheritdoc />
    int HitTest(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range,
        uint64 state, const FudgetSingleLineTextOptions &text_options, const Int2 &point) override;

    /// <inheritdoc />
    int GetFontHeight() const override;

private:
    FudgetDrawable *_draw;
    FudgetDrawColors _draw_tint;
    FudgetDrawColors _text_color;

    FudgetFont _font;
};