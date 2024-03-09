#include "FramedFieldPainter.h"
#include "../../Control.h"
#include "../DrawableBuilder.h"

// FudgetFrameFieldPainter


FudgetFramedFieldPainter::FudgetFramedFieldPainter(const SpawnParams &params) : Base(params),
    _field_bg(nullptr), _hovered_field_bg(nullptr), _pressed_field_bg(nullptr), _down_field_bg(nullptr), _focused_field_bg(nullptr),
    _disabled_field_bg(nullptr), _frame_area(nullptr), _hovered_frame_area(nullptr), _pressed_frame_area(nullptr),
    _down_frame_area(nullptr), _focused_frame_area(nullptr), _disabled_frame_area(nullptr), _state_order(nullptr)
{
}

void FudgetFramedFieldPainter::Initialize(FudgetControl *control, FudgetStyle *style_override, const Variant &mapping)
{
    if (control == nullptr)
        return;

    const ResourceMapping *ptrres = mapping.AsStructure<ResourceMapping>();
    ResourceMapping res;
    if (ptrres != nullptr)
        res = *ptrres;

    GetMappedStateOrder(res.StateOrderIndex, _state_order);

    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::FieldBackground, res.FieldBackground, _field_bg))
        _field_bg = FudgetDrawable::FromColor(Color::White);
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::HoveredFieldBackground, res.HoveredFieldBackground, _hovered_field_bg))
        _hovered_field_bg = _field_bg;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::PressedFieldBackground, res.PressedFieldBackground, _pressed_field_bg))
        _pressed_field_bg = _field_bg;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::DownFieldBackground, res.DownFieldBackground, _down_field_bg))
        _down_field_bg = _pressed_field_bg;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::FocusedFieldBackground, res.FocusedFieldBackground, _focused_field_bg))
        _focused_field_bg = _field_bg;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::DisabledFieldBackground, res.DisabledFieldBackground, _disabled_field_bg))
        _disabled_field_bg = _field_bg;

    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::FieldPadding, res.FieldPadding, _field_padding))
        _field_padding = FudgetPadding(0);
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::HoveredFieldPadding, res.HoveredFieldPadding, _hovered_field_padding))
        _hovered_field_padding = _field_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::PressedFieldPadding, res.PressedFieldPadding, _pressed_field_padding))
        _pressed_field_padding = _field_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::DownFieldPadding, res.DownFieldPadding, _down_field_padding))
        _down_field_padding = _pressed_field_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::FocusedFieldPadding, res.FocusedFieldPadding, _focused_field_padding))
        _focused_field_padding = _field_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::DisabledFieldPadding, res.DisabledFieldPadding, _disabled_field_padding))
        _disabled_field_padding = _field_padding;

    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::FrameDraw, res.FrameDraw, _frame_area))
        _frame_area = FudgetDrawable::FromDrawArea(FudgetDrawArea(FudgetPadding::Max(_field_padding, 1), Color::Gray, FudgetFrameType::Inside));
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::HoveredFrameDraw, res.HoveredFrameDraw, _hovered_frame_area))
        _hovered_frame_area = _frame_area;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::PressedFrameDraw, res.PressedFrameDraw, _pressed_frame_area))
        _pressed_frame_area = _frame_area;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::DownFrameDraw, res.DownFrameDraw, _down_frame_area))
        _down_frame_area = _pressed_frame_area;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::FocusedFrameDraw, res.FocusedFrameDraw, _focused_frame_area))
        _focused_frame_area = _frame_area;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::DisabledFrameDraw, res.DisabledFrameDraw, _disabled_frame_area))
        _disabled_frame_area = _frame_area;

    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::FramePadding, res.FramePadding, _frame_padding))
        _frame_padding = FudgetPadding(0);
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::HoveredFramePadding, res.HoveredFramePadding, _hovered_frame_padding))
        _hovered_frame_padding = _frame_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::PressedFramePadding, res.PressedFramePadding, _pressed_frame_padding))
        _pressed_frame_padding = _frame_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::DownFramePadding, res.DownFramePadding, _down_frame_padding))
        _down_frame_padding = _pressed_frame_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::FocusedFramePadding, res.FocusedFramePadding, _focused_frame_padding))
        _focused_frame_padding = _frame_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::DisabledFramePadding, res.DisabledFramePadding, _disabled_frame_padding))
        _disabled_frame_padding = _frame_padding;

    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::ContentPadding, res.ContentPadding, _inner_padding))
        _inner_padding = FudgetPadding(0);
}

void FudgetFramedFieldPainter::Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState states)
{
    uint64 matched_state = _state_order != nullptr ? _state_order->GetMatchingState((uint64)states) : 0;

    FudgetDrawable *area = DrawDisabled(matched_state) ? _disabled_field_bg :
        DrawDown(matched_state) ? _down_field_bg :
        DrawPressed(matched_state) ? _pressed_field_bg :
        DrawHovered(matched_state) ? _hovered_field_bg :
        DrawFocused(matched_state) ? _focused_field_bg :
        _field_bg;
    FudgetPadding field_padding = DrawDisabled(matched_state) ? _disabled_field_padding :
        DrawDown(matched_state) ? _down_field_padding :
        DrawPressed(matched_state) ? _pressed_field_padding :
        DrawHovered(matched_state) ? _hovered_field_padding :
        DrawFocused(matched_state) ? _focused_field_padding :
        _field_padding;

    if (area != nullptr)
        control->DrawDrawable(area, field_padding.Padded(bounds));

    FudgetDrawable *frame = DrawDisabled(matched_state) ? _disabled_frame_area :
        DrawDown(matched_state) ? _down_frame_area :
        DrawPressed(matched_state) ? _pressed_frame_area :
        DrawHovered(matched_state) ? _hovered_frame_area :
        DrawFocused(matched_state) ? _focused_frame_area :
        _frame_area;
    FudgetPadding frame_padding = DrawDisabled(matched_state) ? _disabled_frame_padding :
        DrawDown(matched_state) ? _down_frame_padding :
        DrawPressed(matched_state) ? _pressed_frame_padding :
        DrawHovered(matched_state) ? _hovered_frame_padding :
        DrawFocused(matched_state) ? _focused_frame_padding :
        _frame_padding;

    if (frame != nullptr)
        control->DrawDrawable(frame, frame_padding.Padded(bounds));
}

