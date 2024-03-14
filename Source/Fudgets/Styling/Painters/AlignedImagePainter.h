#pragma once

#include "PartPainters.h"
#include "../StateOrderBuilder.h"


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


API_STRUCT()
struct FUDGETS_API FudgetAlignedImageMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetAlignedImageMapping);

    template<typename S, typename I, typename P, typename O, typename T>
    FudgetAlignedImageMapping(S state, I image, P padding, O offset, T tint) : State((uint64)state), Image((int)image), Padding((int)padding), Offset((int)offset), Tint((int)tint) {}
    FudgetAlignedImageMapping() {}

    API_FIELD() uint64 State = 0;

    API_FIELD() int Image = 0;
    API_FIELD() int Padding = 0;
    API_FIELD() int Offset = 0;
    API_FIELD() int Tint = 0;
};

template<>
struct TIsPODType<FudgetAlignedImageMapping>
{
    enum { Value = true };
};

API_STRUCT(Attributes = "HideInEditor")
struct FUDGETS_API FudgetAlignedImagePainterMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetAlignedImagePainterMapping);

    API_FIELD() Array<FudgetAlignedImageMapping> Mappings;

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
    void Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState states) override;
private:
    struct DrawMapping
    {
        uint64 _state;

        AssetReference<TextureBase> _image;
        Color _tint;
        Float2 _offset;
        FudgetPadding _padding;
    };

    Array<DrawMapping> _mappings;

    FudgetImageHorzAlign _horz_align;
    FudgetImageVertAlign _vert_align;

};