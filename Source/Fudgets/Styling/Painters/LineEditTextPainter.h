#pragma once

#include "PartPainters.h"


/// <summary>
/// Mapping for FudgetLineEditTextPainter and FudgetTextBoxPainter. Mapping is used to tell a part painter
/// what Ids to look up in its owner control's style.
/// </summary>
API_STRUCT(Attributes = "HideInEditor")
struct FUDGETS_API FudgetTextPainterMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetTextPainterMapping);

    /// <summary>
    /// The id for a background drawable. The drawable could for example have a background for the
    /// selected or hovered state.
    /// </summary>
    API_FIELD() int BackgroundDraw = 0;
    /// <summary>
    /// The id for a background tint color to multiply all pixels with when drawing the background. The
    /// drawable could for example have a background for the selected or hovered state.
    /// </summary>
    API_FIELD() int BackgroundTint = 0;
    /// <summary>
    /// The id for the color of the text to draw. This can be a FudgetDrawColors to have different color
    /// when the text is selected.
    /// </summary>
    API_FIELD() int TextColor = 0;
    /// <summary>
    /// The id for the font to use for drawing.
    /// </summary>
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