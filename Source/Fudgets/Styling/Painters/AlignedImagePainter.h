#pragma once

#include "PartPainters.h"


// TODO: these alignments might be unified somewhere if there are multiple

/// <summary>
/// Horizontal image alignment for FudgetAlignedImagePainter
/// </summary>
API_ENUM()
enum class FudgetImageHorzAlign
{
    /// <summary>
    /// Image is aligned to the left
    /// </summary>
    Left,
    /// <summary>
    /// Image is aligned to the right
    /// </summary>
    Right,
    /// <summary>
    /// Image is aligned to be at the center horizontally
    /// </summary>
    Center,
    /// <summary>
    /// Stretch image to fill the horizontal space
    /// </summary>
    Stretch
};

/// <summary>
/// Vertical image alignment for FudgetAlignedImagePainter
/// </summary>
API_ENUM()
enum class FudgetImageVertAlign
{
    /// <summary>
    /// Image is aligned to the top
    /// </summary>
    Top,
    /// <summary>
    /// Image is aligned to the bottom
    /// </summary>
    Bottom,
    /// <summary>
    /// Image is aligned to be at the center vertically
    /// </summary>
    Center,
    /// <summary>
    /// Stretch image to fill the vertical space
    /// </summary>
    Stretch
};

API_STRUCT(Attributes = "HideInEditor")
struct FUDGETS_API FudgetAlignedImagePainterMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetAlignedImagePainterMapping);

    API_FIELD() int Image = 0;
    API_FIELD() int Padding = 0;
    API_FIELD() int Offset = 0;
    API_FIELD() int Tint = 0;

    API_FIELD() int HorzAlign = 0;
    API_FIELD() int VertAlign = 0;
};

/// <summary>
/// Painter for controls that have a background and a frame around a field. For text editors
/// list boxes, drop down menus.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetAlignedImagePainter : public FudgetStatePainter
{
    using Base = FudgetStatePainter;
    DECLARE_SCRIPTING_TYPE(FudgetAlignedImagePainter);
public:
    using Mapping = FudgetAlignedImagePainterMapping;

    /// <inheritdoc />
    void Initialize(FudgetControl *control, /*FudgetStyle *style_override,*/ const Variant &mapping) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, uint64 states) override;
private:
    AssetReference<TextureBase> _image;
    FudgetPadding _padding;
    Float2 _offset;
    FudgetDrawColors _tint;

    FudgetImageHorzAlign _horz_align;
    FudgetImageVertAlign _vert_align;

};