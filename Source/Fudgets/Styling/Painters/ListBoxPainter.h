#pragma once

#include "PartPainters.h"

/// <summary>
/// Mapping for FudgetListBoxItemPainter. Mapping is used to tell a part painter what Ids to look up
/// in its owner control's style.
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetListBoxItemPainterMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetListBoxItemPainterMapping);

    /// <summary>
    /// The id for a background drawable.
    /// </summary>
    API_FIELD() int BgDraw = 0;
    /// <summary>
    /// The id for a tint color or FudgetDrawColors to multiply every pixel of the
    /// background drawable when drawing.
    /// </summary>
    API_FIELD() int BgTint = 0;
    /// <summary>
    /// The id for a textpainter's painter mapping.
    /// </summary>
    API_FIELD() int TextPainter = 0;
};

/// <summary>
/// Painter for individual list items in a listbox.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetListBoxItemPainter : public FudgetListItemPainter
{
    using Base = FudgetListItemPainter;
    DECLARE_SCRIPTING_TYPE(FudgetListBoxItemPainter);
public:
    using Mapping = FudgetListBoxItemPainterMapping;

    ~FudgetListBoxItemPainter();

    /// <inheritdoc />
    void Initialize(FudgetControl *control, const Variant &mapping) override;
    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, Int2 offset, int item_index, IFudgetDataProvider *data, uint64 states) override;
    /// <inheritdoc />
    Int2 Measure(FudgetControl *control, int item_index, IFudgetDataProvider *data, uint64 state) override;
private:
    FudgetDrawable *_bg_draw;
    FudgetDrawColors _bg_tint;

    FudgetSingleLineTextPainter *_text_painter;
};
