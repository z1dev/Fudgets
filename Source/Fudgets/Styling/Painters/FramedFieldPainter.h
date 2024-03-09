#pragma once

#include "PartPainters.h"
#include "../StateOrderBuilder.h"


API_ENUM()
enum class FudgetFramedFieldPainterIds
{
    First = 9000,

    FieldBackground = First,
    HoveredFieldBackground,
    PressedFieldBackground,
    DownFieldBackground,
    DisabledFieldBackground,
    FocusedFieldBackground,
    FieldPadding,
    HoveredFieldPadding,
    PressedFieldPadding,
    DownFieldPadding,
    DisabledFieldPadding,
    FocusedFieldPadding,

    FrameDraw,
    HoveredFrameDraw,
    PressedFrameDraw,
    DownFrameDraw,
    FocusedFrameDraw,
    DisabledFrameDraw,
    FramePadding,
    HoveredFramePadding,
    PressedFramePadding,
    DownFramePadding,
    FocusedFramePadding,
    DisabledFramePadding,

    ContentPadding,
};

API_STRUCT(Attributes = "HideInEditor")
struct FUDGETS_API FudgetFramedFieldPainterResources
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetFramedFieldPainterResources);

    API_FIELD() int StateOrderIndex = -1;

    API_FIELD() int FieldBackground = 0;
    API_FIELD() int HoveredFieldBackground = 0;
    API_FIELD() int PressedFieldBackground = 0;
    API_FIELD() int DownFieldBackground = 0;
    API_FIELD() int DisabledFieldBackground = 0;
    API_FIELD() int FocusedFieldBackground = 0;
    API_FIELD() int FieldPadding = 0;
    API_FIELD() int HoveredFieldPadding = 0;
    API_FIELD() int PressedFieldPadding = 0;
    API_FIELD() int DownFieldPadding = 0;
    API_FIELD() int DisabledFieldPadding = 0;
    API_FIELD() int FocusedFieldPadding = 0;

    API_FIELD() int FrameDraw = 0;
    API_FIELD() int HoveredFrameDraw = 0;
    API_FIELD() int PressedFrameDraw = 0;
    API_FIELD() int DownFrameDraw = 0;
    API_FIELD() int FocusedFrameDraw = 0;
    API_FIELD() int DisabledFrameDraw = 0;
    API_FIELD() int FramePadding = 0;
    API_FIELD() int HoveredFramePadding = 0;
    API_FIELD() int PressedFramePadding = 0;
    API_FIELD() int DownFramePadding = 0;
    API_FIELD() int FocusedFramePadding = 0;
    API_FIELD() int DisabledFramePadding = 0;

    API_FIELD() int ContentPadding = 0;
};

/// <summary>
/// Painter for controls that have a background and a frame around a field. For text editors
/// list boxes, drop down menus.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetFramedFieldPainter : public FudgetStatePainter
{
    using Base = FudgetStatePainter;
    DECLARE_SCRIPTING_TYPE(FudgetFramedFieldPainter);
public:
    using ResourceMapping = FudgetFramedFieldPainterResources;

    /// <inheritdoc />
    void Initialize(FudgetControl *control, FudgetStyle *style_override, const Variant &mapping) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState states) override;


    /// <summary>
    /// Padding inside the rectangle of the painted frame that restricts the size of the contents inside the frame
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetPadding GetContentPadding() const { return _inner_padding; }
private:
    FudgetDrawable *_field_bg;
    FudgetDrawable *_hovered_field_bg;
    FudgetDrawable *_pressed_field_bg;
    FudgetDrawable *_down_field_bg;
    FudgetDrawable *_focused_field_bg;
    FudgetDrawable *_disabled_field_bg;
    FudgetPadding _field_padding;
    FudgetPadding _hovered_field_padding;
    FudgetPadding _pressed_field_padding;
    FudgetPadding _down_field_padding;
    FudgetPadding _focused_field_padding;
    FudgetPadding _disabled_field_padding;

    FudgetDrawable *_frame_area;
    FudgetDrawable *_hovered_frame_area;
    FudgetDrawable *_pressed_frame_area;
    FudgetDrawable *_down_frame_area;
    FudgetDrawable *_focused_frame_area;
    FudgetDrawable *_disabled_frame_area;
    FudgetPadding _frame_padding;
    FudgetPadding _hovered_frame_padding;
    FudgetPadding _pressed_frame_padding;
    FudgetPadding _down_frame_padding;
    FudgetPadding _focused_frame_padding;
    FudgetPadding _disabled_frame_padding;

    FudgetPadding _inner_padding;

    FudgetStateOrder *_state_order;
};