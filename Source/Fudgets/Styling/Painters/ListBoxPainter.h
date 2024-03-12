#pragma once

#include "PartPainters.h"


//API_ENUM()
//enum class FudgetListBoxItemPainterIds
//{
//    First = 4000,
//
//    TextPainter = First,
//    TextStyle,
//
//    SelectionDraw,
//    DisabledSelectionDraw,
//    TextColor,
//    DisabledTextColor,
//    SelectedTextColor,
//    DisabledSelectedTextColor,
//
//    Font,
//};

API_STRUCT(Attributes = "HideInEditor")
struct FUDGETS_API FudgetListBoxItemPainterResources
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetListBoxItemPainterResources);

    API_FIELD() int TextPainter = 0;
    API_FIELD() int TextStyle = 0;

    API_FIELD() int SelectionDraw = 0;
    API_FIELD() int DisabledSelectionDraw = 0;
    API_FIELD() int TextColor = 0;
    API_FIELD() int DisabledTextColor = 0;
    API_FIELD() int SelectedTextColor = 0;
    API_FIELD() int DisabledSelectedTextColor = 0;

    API_FIELD() int Font = 0;
};



API_CLASS()
class FUDGETS_API FudgetListBoxItemPainter : public FudgetListItemPainter
{
    using Base = FudgetListItemPainter;
    DECLARE_SCRIPTING_TYPE(FudgetListBoxItemPainter);
public:
    using ResourceMapping = FudgetListBoxItemPainterResources;

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
