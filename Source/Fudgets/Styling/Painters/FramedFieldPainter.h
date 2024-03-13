#pragma once

#include "PartPainters.h"
#include "../StateOrderBuilder.h"


API_STRUCT()
struct FUDGETS_API FudgetFramedField
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetFramedField);

    template<typename S, typename D, typename P, typename T>
    FudgetFramedField(S state, D drawable, P padding, T  tint) : State((int)state), Drawable((int)drawable), Padding((int)padding), Tint((int)tint) {}
    FudgetFramedField() {}

    API_FIELD() int State = 0;

    API_FIELD() int Drawable = 0;
    API_FIELD() int Padding = 0;
    API_FIELD() int Tint = 0;
};


template<>
struct TIsPODType<FudgetFramedField>
{
    enum { Value = true };
};

API_STRUCT()
struct FUDGETS_API FudgetFramedFieldLayer
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetFramedFieldLayer);

    API_FIELD() Array<FudgetFramedField> Fields;
};

API_STRUCT()
struct FUDGETS_API FudgetFramedFieldPainterResources
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetFramedFieldPainterResources);

    API_FIELD() int StateOrderIndex = -1;

    API_FIELD() Array<FudgetFramedFieldLayer> Layers;

    /*API_FIELD() int FieldBackground = 0;
    API_FIELD() int HoveredFieldBackground = 0;
    API_FIELD() int PressedFieldBackground = 0;
    API_FIELD() int DownFieldBackground = 0;
    API_FIELD() int DisabledFieldBackground = 0;
    API_FIELD() int FocusedFieldBackground = 0;
    API_FIELD() int SelectedFieldBackground = 0;

    API_FIELD() int FieldTint = 0;
    API_FIELD() int HoveredFieldTint = 0;
    API_FIELD() int PressedFieldTint = 0;
    API_FIELD() int DownFieldTint = 0;
    API_FIELD() int DisabledFieldTint = 0;
    API_FIELD() int FocusedFieldTint = 0;
    API_FIELD() int SelectedFieldTint = 0;

    API_FIELD() int FieldPadding = 0;
    API_FIELD() int HoveredFieldPadding = 0;
    API_FIELD() int PressedFieldPadding = 0;
    API_FIELD() int DownFieldPadding = 0;
    API_FIELD() int DisabledFieldPadding = 0;
    API_FIELD() int FocusedFieldPadding = 0;
    API_FIELD() int SelectedFieldPadding = 0;

    API_FIELD() int FrameDraw = 0;
    API_FIELD() int HoveredFrameDraw = 0;
    API_FIELD() int PressedFrameDraw = 0;
    API_FIELD() int DownFrameDraw = 0;
    API_FIELD() int DisabledFrameDraw = 0;
    API_FIELD() int FocusedFrameDraw = 0;
    API_FIELD() int SelectedFrameDraw = 0;

    API_FIELD() int FrameTint = 0;
    API_FIELD() int HoveredFrameTint = 0;
    API_FIELD() int PressedFrameTint = 0;
    API_FIELD() int DownFrameTint = 0;
    API_FIELD() int DisabledFrameTint = 0;
    API_FIELD() int FocusedFrameTint = 0;
    API_FIELD() int SelectedFrameTint = 0;

    API_FIELD() int FramePadding = 0;
    API_FIELD() int HoveredFramePadding = 0;
    API_FIELD() int PressedFramePadding = 0;
    API_FIELD() int DownFramePadding = 0;
    API_FIELD() int DisabledFramePadding = 0;
    API_FIELD() int FocusedFramePadding = 0;
    API_FIELD() int SelectedFramePadding = 0;*/

    API_FIELD() int VisualPadding = 0;
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
    void Initialize(FudgetControl *control, /*FudgetStyle *style_override,*/ const Variant &mapping) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState states) override;

    /// <summary>
    /// Padding inside the rectangle of the painted frame.
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetPadding GetVisualPadding() const { return _visual_padding; }

    /// <summary>
    /// Padding inside the rectangle of the painted frame that includes extra padding for contents.
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetPadding GetContentPadding() const { return _content_padding; }
private:
    struct DrawField
    {
        int _state;

        FudgetDrawable *_drawable;
        Color _tint;
        FudgetPadding _padding;
    };

    Array<Array<DrawField>> _layers;

    FudgetPadding _visual_padding;
    FudgetPadding _content_padding;

    FudgetStateOrder *_state_order;
};