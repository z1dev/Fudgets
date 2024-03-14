#pragma once

#include "PartPainters.h"


API_STRUCT()
struct FUDGETS_API FudgetListItemMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetListItemMapping);

    template<typename S, typename B, typename BT>
    FudgetListItemMapping(S state, B sel_bg_draw, BT sel_tint) : State((uint64)state), BgDraw((int)sel_bg_draw), BgTint((int)sel_tint) {}
    FudgetListItemMapping() {}

    API_FIELD() uint64 State = 0;

    API_FIELD() int BgDraw = 0;
    API_FIELD() int BgTint = 0;
};

template<>
struct TIsPODType<FudgetListItemMapping>
{
    enum { Value = true };
};

API_STRUCT()
struct FUDGETS_API FudgetListBoxItemPainterMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetListBoxItemPainterMapping);

    API_FIELD() Array<FudgetListItemMapping> Mappings;

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
    void Initialize(FudgetControl *control, const Variant &mapping) override;
    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, Int2 offset, int item_index, IListDataProvider *data, FudgetVisualControlState states) override;
    /// <inheritdoc />
    Int2 Measure(FudgetControl *control, int item_index, IListDataProvider *data, FudgetVisualControlState state) override;
private:
    FudgetSingleLineTextPainter *_text_painter;

    struct DrawMapping
    {
        uint64 _state;

        FudgetDrawable *_bg_draw;
        Color _bg_tint;
    };

    Array<DrawMapping> _mappings;
};
