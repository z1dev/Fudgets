#pragma once

#include "PartPainters.h"


API_STRUCT(Attributes = "HideInEditor")
struct FUDGETS_API FudgetListBoxItemPainterMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetListBoxItemPainterMapping);

    API_FIELD() int TextPainter = 0;
};

API_CLASS()
class FUDGETS_API FudgetListBoxItemPainter : public FudgetListItemPainter
{
    using Base = FudgetListItemPainter;
    DECLARE_SCRIPTING_TYPE(FudgetListBoxItemPainter);
public:
    using Mapping = FudgetListBoxItemPainterMapping;

    ~FudgetListBoxItemPainter();

    /// <inheritdoc />
    void Initialize(FudgetControl *control, /*FudgetStyle *style_override,*/ const Variant &mapping) override;
    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, Int2 offset, int item_index, IListDataProvider *data, FudgetVisualControlState states) override;
    /// <inheritdoc />
    Int2 Measure(FudgetControl *control, int item_index, IListDataProvider *data, FudgetVisualControlState state) override;
private:
    FudgetSingleLineTextPainter *_text_painter;
};
