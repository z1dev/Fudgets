#include "FramedFieldPainter.h"
#include "../../Control.h"
#include "../DrawableBuilder.h"
#include "../PartPainterIds.h"

// FudgetFrameFieldPainter


FudgetFramedFieldPainter::FudgetFramedFieldPainter(const SpawnParams &params) : Base(params)
{
}

void FudgetFramedFieldPainter::Initialize(FudgetControl *control, const Variant &mapping)
{
    Mapping res = *mapping.AsStructure<Mapping>();

    for (const FudgetFramedFieldLayer &layer : res.Layers)
    {
        Array<DrawMapping> mappings;
        HashSet<uint64> states;
        for (const FudgetFieldMapping &mapping : layer.Mappings)
        {
            DrawMapping new_mapping;

            // Duplicate states are not allowed
            if (states.Contains(mapping.State))
                continue;
            states.Add(mapping.State);
            new_mapping._state = mapping.State;

            if (!CreateMappedDrawable(control, mapping.Drawable, new_mapping._drawable))
                new_mapping._drawable = FudgetDrawable::Empty;
            if (!GetMappedPadding(control, mapping.Padding, new_mapping._padding))
                new_mapping._padding = FudgetPadding(0);
            if (!GetMappedColor(control, mapping.Tint, new_mapping._tint))
                new_mapping._tint = Color::White;
            mappings.Add(new_mapping);
        }
        _layers.Add(mappings);
    }

    if (!GetMappedPadding(control, res.VisualPadding, _visual_padding))
        _visual_padding = FudgetPadding(0);
    if (!GetMappedPadding(control, res.ContentPadding, _content_padding))
        _content_padding = FudgetPadding(0);
}

void FudgetFramedFieldPainter::Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState states)
{
    uint64 matched_state = GetMatchingState(states);

    for (auto &layer : _layers)
    {
        bool failed = false;
        for (auto &mapping : layer)
        {
            if (mapping._state == 0 || (!failed && matched_state != 0 && (mapping._state & matched_state) == matched_state))
            {
                if (mapping._drawable != nullptr && !mapping._drawable->IsEmpty())
                {
                    control->DrawDrawable(mapping._drawable, mapping._padding.Padded(bounds), mapping._tint);
                    break;
                }
                else
                {
                    if (mapping._state == 0)
                        break;

                    failed = true;
                }
            }
        }
    }
}

