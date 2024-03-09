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


API_ENUM()
enum class FudgetAlignedImagePainterIds
{
    First = 8000,

    Image = First,
    HoveredImage,
    PressedImage,
    DownImage,
    FocusedImage,
    DisabledImage,

    ImageTint,
    HoveredImageTint,
    PressedImageTint,
    DownImageTint,
    FocusedImageTint,
    DisabledImageTint,

    ImageOffset,
    HoveredImageOffset,
    PressedImageOffset,
    DownImageOffset,
    FocusedImageOffset,
    DisabledImageOffset,

    ImagePadding,
    HoveredImagePadding,
    PressedImagePadding,
    DownImagePadding,
    FocusedImagePadding,
    DisabledImagePadding,

    HorzAlign,
    VertAlign,
};

API_STRUCT(Attributes = "HideInEditor")
struct FUDGETS_API FudgetAlignedImagePainterResources
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetAlignedImagePainterResources);

    API_FIELD() int StateOrderIndex = -1;

    API_FIELD() int Image = 0;
    API_FIELD() int HoveredImage = 0;
    API_FIELD() int PressedImage = 0;
    API_FIELD() int DownImage = 0;
    API_FIELD() int FocusedImage = 0;
    API_FIELD() int DisabledImage = 0;

    API_FIELD() int ImageTint = 0;
    API_FIELD() int HoveredImageTint = 0;
    API_FIELD() int PressedImageTint = 0;
    API_FIELD() int DownImageTint = 0;
    API_FIELD() int FocusedImageTint = 0;
    API_FIELD() int DisabledImageTint = 0;

    API_FIELD() int ImageOffset = 0;
    API_FIELD() int HoveredImageOffset = 0;
    API_FIELD() int PressedImageOffset = 0;
    API_FIELD() int DownImageOffset = 0;
    API_FIELD() int FocusedImageOffset = 0;
    API_FIELD() int DisabledImageOffset = 0;

    API_FIELD() int ImagePadding = 0;
    API_FIELD() int HoveredImagePadding = 0;
    API_FIELD() int PressedImagePadding = 0;
    API_FIELD() int DownImagePadding = 0;
    API_FIELD() int FocusedImagePadding = 0;
    API_FIELD() int DisabledImagePadding = 0;

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
    using ResourceMapping = FudgetAlignedImagePainterResources;

    /// <inheritdoc />
    void Initialize(FudgetControl *control, FudgetStyle *style_override, const Variant &mapping) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState states) override;
private:
    AssetReference<TextureBase> _image;
    AssetReference<TextureBase> _hovered_image;
    AssetReference<TextureBase> _pressed_image;
    AssetReference<TextureBase> _down_image;
    AssetReference<TextureBase> _focused_image;
    AssetReference<TextureBase> _disabled_image;

    Color _image_tint;
    Color _hovered_image_tint;
    Color _pressed_image_tint;
    Color _down_image_tint;
    Color _focused_image_tint;
    Color _disabled_image_tint;

    Float2 _image_offset;
    Float2 _hovered_image_offset;
    Float2 _pressed_image_offset;
    Float2 _down_image_offset;
    Float2 _focused_image_offset;
    Float2 _disabled_image_offset;

    FudgetPadding _image_padding;
    FudgetPadding _hovered_image_padding;
    FudgetPadding _pressed_image_padding;
    FudgetPadding _down_image_padding;
    FudgetPadding _focused_image_padding;
    FudgetPadding _disabled_image_padding;

    FudgetImageHorzAlign _horz_align;
    FudgetImageVertAlign _vert_align;

    FudgetStateOrder *_state_order;
};