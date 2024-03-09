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

void FudgetListBoxItemPainter::Initialize(FudgetControl *control, FudgetStyle *style_override, const Variant &mapping)
{
    if (control == nullptr)
        return;

    const ResourceMapping *ptrres = mapping.AsStructure<ResourceMapping>();
    ResourceMapping res;
    if (ptrres != nullptr)
        res = *ptrres;

    FudgetStyle *text_style = nullptr;
    if (!GetMappedStyle(control, style_override, (int)FudgetListBoxItemPainterIds::TextStyle, res.TextStyle, text_style))
        text_style = nullptr;

    _text_painter = control->CreateStylePainter<FudgetSingleLineTextPainter>(_text_painter, res.TextPainter, text_style, nullptr, style_override);

    if (_text_painter == nullptr)
    {
        FudgetLineEditTextPainter::ResourceMapping text_res;
        text_res.StateOrderIndex = FudgetThemes::FOCUSED_HOVERED_STATE_ORDER_INDEX;
        text_res.SelectionDraw = res.SelectionDraw;
        text_res.FocusedSelectionDraw = res.SelectionDraw;
        text_res.DisabledSelectionDraw = res.DisabledSelectionDraw;
        text_res.TextColor = res.TextColor;
        text_res.DisabledTextColor = res.DisabledTextColor;
        text_res.SelectedTextColor = res.SelectedTextColor;
        text_res.FocusedSelectedTextColor = res.SelectedTextColor;
        text_res.DisabledSelectedTextColor = res.DisabledSelectedTextColor;
        text_res.Font = res.Font;
        FudgetPartPainterMapping def_text_mapping = FudgetPartPainter::InitializeMapping<FudgetLineEditTextPainter>(text_res);
        _text_painter = control->CreateStylePainter<FudgetSingleLineTextPainter>(_text_painter, (int)FudgetListBoxItemPainterIds::TextPainter, text_style, &def_text_mapping, style_override);
    }
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

