#pragma once

#include "PartPainters.h"


API_STRUCT()
struct FUDGETS_API FudgetDrawablePainterMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetDrawablePainterMapping);

    API_FIELD() int Drawable = 0;
    API_FIELD() int Padding = 0;
    API_FIELD() int Tint = 0;

    API_FIELD() int VisualPadding = 0;
    API_FIELD() int ContentPadding = 0;
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
    API_PROPERTY() FORCE_INLINE FudgetPadding GetVisualPadding() const { return _visual_padding; }

    /// <summary>
    /// Padding inside the rectangle of the painted frame that includes extra padding for contents.
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetPadding GetContentPadding() const { return _content_padding; }
private:

    FudgetDrawable *_drawable;
    FudgetPadding _padding;
    FudgetDrawColors _tint;

    FudgetPadding _visual_padding;
    FudgetPadding _content_padding;
};