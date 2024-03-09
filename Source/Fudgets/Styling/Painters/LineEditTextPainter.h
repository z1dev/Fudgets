#pragma once

#include "PartPainters.h"


API_ENUM()
enum class FudgetLineEditTextPainterIds
{
    First = 10000,

    SelectionDraw = First,
    FocusedSelectionDraw,
    DisabledSelectionDraw,
    TextColor,
    DisabledTextColor,
    SelectedTextColor,
    FocusedSelectedTextColor,
    DisabledSelectedTextColor,
    Font,
};

API_STRUCT(Attributes = "HideInEditor")
struct FUDGETS_API FudgetLineEditTextPainterResources
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetLineEditTextPainterResources);

    API_FIELD() int StateOrderIndex = -1;

    API_FIELD() int SelectionDraw = 0;
    API_FIELD() int FocusedSelectionDraw = 0;
    API_FIELD() int DisabledSelectionDraw = 0;
    API_FIELD() int TextColor = 0;
    API_FIELD() int DisabledTextColor = 0;
    API_FIELD() int SelectedTextColor = 0;
    API_FIELD() int FocusedSelectedTextColor = 0;
    API_FIELD() int DisabledSelectedTextColor = 0;
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
    using ResourceMapping = FudgetLineEditTextPainterResources;

    /// <inheritdoc />
    void Initialize(FudgetControl *control, FudgetStyle *style_override, const Variant &mapping) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, 
        FudgetVisualControlState state, const FudgetSingleLineTextOptions &text_options) override;

    /// <inheritdoc />
    Float2 Measure(FudgetControl *control, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state,
        const FudgetSingleLineTextOptions &text_options) override;

    /// <inheritdoc />
    float GetKerning(Char a, Char b, float scale) const override;

    /// <inheritdoc />
    int HitTest(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range,
        FudgetVisualControlState state, const FudgetSingleLineTextOptions &text_options, const Float2 &point) override;

    /// <inheritdoc />
    float GetFontHeight() const override;

private:
    FudgetDrawable *_sel_area;
    FudgetDrawable *_focused_sel_area;
    FudgetDrawable *_disabled_sel_area;

    Color _text_color;
    Color _disabled_text_color;
    Color _selected_text_color;
    Color _focused_selected_text_color;
    Color _disabled_selected_text_color;

    FudgetFont _font;

    FudgetStateOrder *_state_order;
};