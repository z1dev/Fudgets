#include "FramedFieldPainter.h"
#include "../../Control.h"
#include "../DrawableBuilder.h"
#include "../PartPainterIds.h"

// FudgetFrameFieldPainter


FudgetFramedFieldPainter::FudgetFramedFieldPainter(const SpawnParams &params) : Base(params),
    /*_field_bg(nullptr), _hovered_field_bg(nullptr), _pressed_field_bg(nullptr),
    _down_field_bg(nullptr), _disabled_field_bg(nullptr), _focused_field_bg(nullptr), _selected_field_bg(nullptr),

    _field_tint(Color::White), _hovered_field_tint(Color::White), _pressed_field_tint(Color::White),
    _down_field_tint(Color::White), _disabled_field_tint(Color::White), _focused_field_tint(Color::White), _selected_field_tint(Color::White),

    _frame_area(nullptr), _hovered_frame_area(nullptr), _pressed_frame_area(nullptr),
    _down_frame_area(nullptr), _disabled_frame_area(nullptr), _focused_frame_area(nullptr), _selected_frame_area(nullptr),

    _frame_tint(Color::White), _hovered_frame_tint(Color::White), _pressed_frame_tint(Color::White),
    _down_frame_tint(Color::White), _disabled_frame_tint(Color::White), _focused_frame_tint(Color::White), _selected_frame_tint(Color::White),*/

    _state_order(nullptr)
{
}

void FudgetFramedFieldPainter::Initialize(FudgetControl *control, const Variant &mapping)
{
    if (control == nullptr)
        return;

    const ResourceMapping *ptrres = mapping.AsStructure<ResourceMapping>();
    ResourceMapping res;
    if (ptrres != nullptr)
        res = *ptrres;

    GetMappedStateOrder(res.StateOrderIndex, _state_order);
    FudgetDrawable *empty = nullptr;
    HashSet<int> states;

    for (const FudgetFramedFieldLayer &layer : res.Layers)
    {
        Array<DrawField> fields;
        for (const FudgetFramedField &field : layer.Fields)
        {
            DrawField new_field;
            new_field._state = field.State;

            // Duplicate states are not allowed
            if (states.Contains(field.State))
                continue;
            states.Add(field.State);

            if (!CreateMappedDrawable(control, field.Drawable, new_field._drawable))
            {
                if (empty == nullptr)
                    empty = FudgetDrawable::CreateEmpty(this);
                new_field._drawable = empty;
            }
            if (!GetMappedPadding(control, field.Padding, new_field._padding))
                new_field._padding = FudgetPadding(0);
            if (!GetMappedColor(control, field.Tint, new_field._tint))
                new_field._tint = Color::White;
            fields.Add(new_field);
        }
        _layers.Add(fields);
        states.Clear();
    }

    if (!GetMappedPadding(control, res.VisualPadding, _visual_padding))
        _visual_padding = FudgetPadding(0);
    if (!GetMappedPadding(control, res.ContentPadding, _content_padding))
        _content_padding = FudgetPadding(0);
}

void FudgetFramedFieldPainter::Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState states)
{
    uint64 matched_state = _state_order != nullptr ? _state_order->GetMatchingState((uint64)states) : 0;

    for (auto &layer : _layers)
    {
        bool failed = false;
        for (auto &field : layer)
        {
            if (field._state == 0 || (!failed && (field._state & matched_state) != 0))
            {
                if (field._drawable != nullptr && !field._drawable->IsEmpty())
                {
                    control->DrawDrawable(field._drawable, field._padding.Padded(bounds), field._tint);
                    break;
                }
                else
                {
                    if (field._state == 0)
                        break;

                    failed = true;
                }
            }
        }
    }
}

