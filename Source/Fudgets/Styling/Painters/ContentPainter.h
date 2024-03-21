#pragma once

#include "PartPainters.h"

/// <summary>
/// Mapping for FudgetContentPainter. Mapping is used to tell a part painter what Ids to look up
/// in its owner control's style.
/// </summary>
API_STRUCT(Attributes = "HideInEditor")
struct FUDGETS_API FudgetContentPainterMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetContentPainterMapping);

    /// <summary>
    /// The drawable to draw.
    /// </summary>
    API_FIELD() int Drawable = 0;
    /// <summary>
    /// Margin around contents inside the bounding rectangle.
    /// </summary>
    API_FIELD() int Margin = 0;
    /// <summary>
    /// Offset of the drawable relative to its calculated position
    /// </summary>
    API_FIELD() int Offset = 0;
    /// <summary>
    /// A color to multiply every pixel of the drawable when drawing.
    /// </summary>
    API_FIELD() int Tint = 0;
};

/// <summary>
/// Painter that draws a drawable aligned in a bounding rectangle. The drawable's size within the drawing rectangle
/// is either determined by the largest texture in the drawn state or the size read from the theme. If no size is
/// set in the theme, each state must contain a texture to be considered for drawing.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetContentPainter : public FudgetStatePainter
{
    using Base = FudgetStatePainter;
    DECLARE_SCRIPTING_TYPE(FudgetContentPainter);
public:
    using Mapping = FudgetContentPainterMapping;

    /// <inheritdoc />
    void Initialize(FudgetControl *control, const Variant &mapping) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, uint64 states) override;
private:
    FudgetDrawable *_drawable;
    FudgetPadding _margin;
    Float2 _offset;
    FudgetDrawColors _tint;
};