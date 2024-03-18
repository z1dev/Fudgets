#include "ListBoxPainter.h"
#include "LineEditTextPainter.h"
#include "../DrawableBuilder.h"

#include "../../Control.h"
#include "../../Controls/ListControl.h"



FudgetListBoxItemPainter::FudgetListBoxItemPainter(const SpawnParams &params) : Base(params), _bg_draw(nullptr), _text_painter(nullptr)
{
}

FudgetListBoxItemPainter::~FudgetListBoxItemPainter()
{
}

void FudgetListBoxItemPainter::Initialize(FudgetControl *control, const Variant &mapping)
{
    Mapping res = *mapping.AsStructure<Mapping>();

    if (!CreateMappedDrawable(control, res.BgDraw, _bg_draw))
        _bg_draw = FudgetDrawable::Empty;
    if (!GetMappedDrawColors(control, res.BgTint, _bg_tint))
        _bg_tint = FudgetDrawColors();

    _text_painter = control->CreateStylePainter<FudgetSingleLineTextPainter>(_text_painter, res.TextPainter);
}

void FudgetListBoxItemPainter::Draw(FudgetControl *control, const Rectangle &bounds, Int2 offset, int item_index, IFudgetDataProvider *data, uint64 states)
{
    if (_text_painter == nullptr || data == nullptr || item_index < 0 || item_index >= data->GetCount())
        return;

    if (!_bg_draw->IsEmpty())
        control->DrawDrawable(_bg_draw, _bg_draw->FindMatchingState(states), bounds, _bg_tint.FindMatchingColor(states));

    String text = data->GetText(item_index);

    FudgetTextRange full_range;
    full_range.StartIndex = 0;
    full_range.EndIndex = text.Length();

    FudgetSingleLineTextOptions opt;
    _text_painter->Draw(control, bounds, text, full_range, states, opt);
}

Int2 FudgetListBoxItemPainter::Measure(FudgetControl *control, int item_index, IFudgetDataProvider *data, uint64 state)
{
    if (_text_painter == nullptr || data == nullptr || item_index < 0 || item_index >= data->GetCount())
        return Int2::Zero;

    String text = data->GetText(item_index);

    FudgetTextRange full_range;
    full_range.StartIndex = 0;
    full_range.EndIndex = text.Length();

    FudgetSingleLineTextOptions opt;
    return _text_painter->Measure(control, data->GetText(item_index), full_range, state, opt);
}

