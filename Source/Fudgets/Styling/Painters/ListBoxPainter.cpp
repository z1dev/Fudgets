#include "ListBoxPainter.h"
#include "LineEditTextPainter.h"

#include "../../Control.h"
#include "../../Controls/ListControl.h"



FudgetListBoxItemPainter::FudgetListBoxItemPainter(const SpawnParams &params) : Base(params), _text_painter(nullptr)
{
}

FudgetListBoxItemPainter::~FudgetListBoxItemPainter()
{
}

void FudgetListBoxItemPainter::Initialize(FudgetControl *control, const Variant &mapping)
{
    if (control == nullptr)
        return;

    const ResourceMapping *ptrres = mapping.AsStructure<ResourceMapping>();
    ResourceMapping res;
    if (ptrres != nullptr)
        res = *ptrres;

    //FudgetStyle *text_style = nullptr;
    //if (!GetMappedStyle(control, style_override, (int)FudgetListBoxItemPainterIds::TextStyle, res.TextStyle, text_style))
    //    text_style = nullptr;

    _text_painter = control->CreateStylePainter<FudgetSingleLineTextPainter>(_text_painter, res.TextPainter);
}

void FudgetListBoxItemPainter::Draw(FudgetControl *control, const Rectangle &bounds, Int2 offset, int item_index, IListDataProvider *data, FudgetVisualControlState states)
{
    if (_text_painter == nullptr || data == nullptr || item_index < 0 || item_index >= data->GetCount())
        return;

    String text = data->GetText(item_index);

    // TODO: make the range argument a pointer so it doesn't have to be passed for every painter function.

    FudgetTextRange full_range;
    full_range.StartIndex = 0;
    full_range.EndIndex = text.Length();

    FudgetSingleLineTextOptions opt;
    _text_painter->Draw(control, bounds, text, full_range, states, opt);
}

Int2 FudgetListBoxItemPainter::Measure(FudgetControl *control, int item_index, IListDataProvider *data, FudgetVisualControlState state)
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

