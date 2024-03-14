#include "ListBoxPainter.h"
#include "LineEditTextPainter.h"
#include "../DrawableBuilder.h"
#include "../StateOrderBuilder.h"

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
    Mapping res = *mapping.AsStructure<Mapping>();

    Array<DrawMapping> mappings;
    HashSet<uint64> states;
    for (const FudgetListItemMapping &mapping : res.Mappings)
    {
        DrawMapping new_mapping;

        // Duplicate states are not allowed
        if (states.Contains(mapping.State))
            continue;
        states.Add(mapping.State);
        new_mapping._state = mapping.State;

        if (!CreateMappedDrawable(control, mapping.BgDraw, new_mapping._bg_draw))
            new_mapping._bg_draw = FudgetDrawable::Empty;
        if (!GetMappedColor(control, mapping.BgTint, new_mapping._bg_tint))
            new_mapping._bg_tint = Color::White;

        mappings.Add(new_mapping);
    }
    _mappings.Add(mappings);

    _text_painter = control->CreateStylePainter<FudgetSingleLineTextPainter>(_text_painter, res.TextPainter);
}

void FudgetListBoxItemPainter::Draw(FudgetControl *control, const Rectangle &bounds, Int2 offset, int item_index, IListDataProvider *data, FudgetVisualControlState states)
{
    if (_text_painter == nullptr || data == nullptr || item_index < 0 || item_index >= data->GetCount())
        return;

    uint64 matched_state = GetMatchingState(states);

    FudgetDrawable *bg_draw = FudgetDrawable::Empty;
    Color bg_tint = Color::White;


    bool failed = false;
    for (auto &mapping : _mappings)
    {
        if (mapping._state == 0 || (!failed && matched_state != 0 && (mapping._state & matched_state) == matched_state))
        {
            if (mapping._bg_draw != nullptr && !mapping._bg_draw->IsEmpty())
            {
                bg_draw = mapping._bg_draw;
                bg_tint = mapping._bg_tint;
            }
            else
            {
                if (mapping._state == 0)
                    break;

                failed = true;
            }
        }
    }

    if (!bg_draw->IsEmpty())
    {
        control->DrawDrawable(bg_draw, bounds, bg_tint);
    }

    String text = data->GetText(item_index);

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

