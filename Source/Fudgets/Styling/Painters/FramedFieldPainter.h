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
    FieldDrawPadding,
    HoveredFieldDrawPadding,
    PressedFieldDrawPadding,
    DownFieldDrawPadding,
    DisabledFieldDrawPadding,
    FocusedFieldDrawPadding,

    FrameDraw,
    HoveredFrameDraw,
    PressedFrameDraw,
    DownFrameDraw,
    FocusedFrameDraw,
    DisabledFrameDraw,
    FrameDrawPadding,
    HoveredFrameDrawPadding,
    PressedFrameDrawPadding,
    DownFrameDrawPadding,
    FocusedFrameDrawPadding,
    DisabledFrameDrawPadding,

    Padding,
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
    API_FIELD() int FieldDrawPadding = 0;
    API_FIELD() int HoveredFieldDrawPadding = 0;
    API_FIELD() int PressedFieldDrawPadding = 0;
    API_FIELD() int DownFieldDrawPadding = 0;
    API_FIELD() int DisabledFieldDrawPadding = 0;
    API_FIELD() int FocusedFieldDrawPadding = 0;

    API_FIELD() int FrameDraw = 0;
    API_FIELD() int HoveredFrameDraw = 0;
    API_FIELD() int PressedFrameDraw = 0;
    API_FIELD() int DownFrameDraw = 0;
    API_FIELD() int FocusedFrameDraw = 0;
    API_FIELD() int DisabledFrameDraw = 0;
    API_FIELD() int FrameDrawPadding = 0;
    API_FIELD() int HoveredFrameDrawPadding = 0;
    API_FIELD() int PressedFrameDrawPadding = 0;
    API_FIELD() int DownFrameDrawPadding = 0;
    API_FIELD() int FocusedFrameDrawPadding = 0;
    API_FIELD() int DisabledFrameDrawPadding = 0;

    API_FIELD() int Padding = 0;
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
    /// Padding inside the rectangle of the painted frame.
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetPadding GetPadding() const { return _padding; }

    /// <summary>
    /// Padding inside the rectangle of the painted frame that includes extra padding for contents.
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetPadding GetContentPadding() const { return _content_padding; }
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

    FudgetPadding _padding;
    FudgetPadding _content_padding;

    FudgetStateOrder *_state_order;
};