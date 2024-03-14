#pragma once

#include "PartPainters.h"
#include "../StateOrderBuilder.h"


API_STRUCT()
struct FUDGETS_API FudgetFieldMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetFieldMapping);

    template<typename S, typename D, typename P, typename T>
    FudgetFieldMapping(S state, D drawable, P padding, T  tint) : State((uint64)state), Drawable((int)drawable), Padding((int)padding), Tint((int)tint) {}
    FudgetFieldMapping() {}

    API_FIELD() uint64 State = 0;

    API_FIELD() int Drawable = 0;
    API_FIELD() int Padding = 0;
    API_FIELD() int Tint = 0;
};

template<>
struct TIsPODType<FudgetFieldMapping>
{
    enum { Value = true };
};

API_STRUCT()
struct FUDGETS_API FudgetFramedFieldLayer
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetFramedFieldLayer);

    API_FIELD() Array<FudgetFieldMapping> Mappings;
};

API_STRUCT()
struct FUDGETS_API FudgetFramedFieldPainterMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetFramedFieldPainterMapping);

    API_FIELD() Array<FudgetFramedFieldLayer> Layers;

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
    using Mapping = FudgetFramedFieldPainterMapping;

    /// <inheritdoc />
    void Initialize(FudgetControl *control, const Variant &mapping) override;

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
    struct DrawMapping
    {
        uint64 _state;

        FudgetDrawable *_drawable;
        Color _tint;
        FudgetPadding _padding;
    };
    Array<Array<DrawMapping>> _layers;

    FudgetPadding _visual_padding;
    FudgetPadding _content_padding;
};