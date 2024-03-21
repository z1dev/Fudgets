#pragma once

#include "PartPainters.h"


API_STRUCT()
struct FUDGETS_API FudgetDrawablePainterMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetDrawablePainterMapping);

    API_FIELD() int Drawable = 0;
    API_FIELD() int Margin = 0;
    API_FIELD() int Tint = 0;

    API_FIELD() int Padding = 0;
};

/// <summary>
/// Painter for controls that have a background and a frame around a field. For text editors
/// list boxes, drop down menus.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetDrawablePainter : public FudgetStatePainter
{
    using Base = FudgetStatePainter;
    DECLARE_SCRIPTING_TYPE(FudgetDrawablePainter);
public:
    using Mapping = FudgetDrawablePainterMapping;

    /// <inheritdoc />
    void Initialize(FudgetControl *control, const Variant &mapping) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, uint64 states) override;

    /// <summary>
    /// Padding inside the rectangle of the painted frame.
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetPadding GetPadding() const { return _padding; }
private:

    FudgetDrawable *_drawable;
    FudgetPadding _margin;
    FudgetDrawColors _tint;

    FudgetPadding _padding;
};