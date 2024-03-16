#pragma once

#include "../MarginStructs.h"

#include "Engine/Render2D/SpriteAtlas.h"
#include "Engine/Core/Math/Color.h"
#include "Engine/Core/Math/Vector4.h"
#include "Engine/Render2D/FontAsset.h"
#include "Engine/Render2D/TextLayoutOptions.h"
#include "Engine/Core/Types/Variant.h"

struct FudgetDrawArea;

API_STRUCT()
struct FUDGETS_API FudgetPartPainterMapping
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetPartPainterMapping);

	/// <summary>
	/// Full type name of the painter that can be created for control part drawing.
	/// </summary>
	API_FIELD() StringAnsi PainterType;

	/// <summary>
	/// Must be one of the Fudget***PainterMapping structs that correspond to the painter named in PainterType.
	/// </summary>
	API_FIELD() Variant Mapping;
};


/// <summary>
/// Handle to sprite atlas slot with a single sprite texture.
/// </summary>
API_STRUCT() struct FUDGETS_API FudgetSpriteHandle
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetSpriteHandle);

	/// <summary>
	/// The parent atlas.
	/// </summary>
	API_FIELD() AssetReference<SpriteAtlas> Atlas;

	/// <summary>
	/// The atlas sprites array index.
	/// </summary>
	API_FIELD() int32 Index;

	/// <summary>
	/// Initializes a new instance of the <see cref="SpriteHandle"/> struct.
	/// </summary>
	FudgetSpriteHandle()
	{
		Index = -1;
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="SpriteHandle"/> struct.
	/// </summary>
	/// <param name="atlas">The sprite atlas.</param>
	/// <param name="index">The sprite slot index.</param>
	FudgetSpriteHandle(const SpriteHandle &original)
		: Atlas(original.Atlas)
	{
		Index = original.Index;
	}

	/// <summary>
	/// Returns a sprite handle from this wrapper
	/// </summary>
	SpriteHandle ToHandle() const
	{
		return SpriteHandle(Atlas, Index);
	}
};


/// <summary>
/// Used in FudgetDrawArea when it is initialized. Use one of the constructors instead.
/// </summary>
API_ENUM(Attributes = "Flags")
enum class FudgetFillType : uint8
{
	/// <summary>
	/// No drawing. Only here because Flax needs an empty constructor.
	/// </summary>
	None = 0,

	/// <summary>
	/// Don't use point drawing
	/// </summary>
	Normal = 1 << 0,
	/// <summary>
	/// Use point drawing
	/// </summary>
	Point = 1 << 1,
	/// <summary>
	/// Draw over rectangle with 9slicing. Only valid if texture is stretched or shrinking in any direction
	/// </summary>
	Sliced = 1 << 2,
};
DECLARE_ENUM_OPERATORS(FudgetFillType);

/// <summary>
/// Image alignment options
/// </summary>
API_ENUM(Attributes = "Flags")
enum class FudgetImageAlignment
{
	// Align image texture or sprite at the center of the rectangle. This value is here for convenience because it is 0.
	Center = 0,

	/// <summary>
	/// Align texture or sprite to the left. When no horizontal alignment is set, texture will be
	/// centered horizontally. If both left and right are set, texture is stretched horizontally.
	/// </summary>
	LeftAlign = 1 << 0,
	/// <summary>
	/// Align texture or sprite to the right. When no horizontal alignment is set, texture will be
	/// centered horizontally. If both left and right are set, texture is stretched horizontally.
	/// </summary>
	RightAlign = 1 << 1,
	/// <summary>
	/// Stretch texture or sprite horizontally if there is more available space than the texture size.
	/// This won't make the texture shrink in itself if there isn't enough space.
	/// </summary>
	StretchHorz = LeftAlign | RightAlign,
	/// <summary>
	/// Align texture or sprite to the top. When no vertical alignment is set, texture will be
	/// centered vertically. If both top and bottom are set, texture is stretched vertically.
	/// </summary>
	TopAlign = 1 << 2,
	/// <summary>
	/// Align texture or sprite to the bottom. When no vertical alignment is set, texture will be
	/// centered vertically. If both top and bottom are set, texture is stretched vertically.
	/// </summary>
	BottomAlign = 1 << 3,
	/// <summary>
	/// Stretch texture or sprite vertically if there is more available space than the texture size.
	/// This won't make the texture shrink in itself if there isn't enough space.
	/// </summary>
	StretchVert = TopAlign | BottomAlign,

	/// <summary>
	/// Stretch texture or sprite both horizontally and vertically if there is more available space
	/// than the texture size.
	/// This won't make the texture shrink in itself if there isn't enough space.
	/// </summary>
	Stretch = StretchHorz | StretchVert,

	/// <summary>
	/// Shrink texture horizontally if the draw area is smaller. This is independent of stretching.
	/// </summary>
	ShrinkHorz = 1 << 4,

	/// <summary>
	/// Shrink texture vertically if the draw area is smaller. This is independent of stretching.
	/// </summary>
	ShrinkVert = 1 << 5,

	/// <summary>
	/// Shrink texture both horizontally and vertically if the draw area is smaller. This is
	/// independent of stretching.
	/// </summary>
	Shrink = ShrinkHorz | ShrinkVert,

	/// <summary>
	/// Shrink and stretch texture horizontally as needed to fit in the available space.
	/// </summary>
	FitHorz = StretchHorz | ShrinkHorz,
	/// <summary>
	/// Shrink and stretch texture vertically as needed to fit in the available space.
	/// </summary>
	FitVert = StretchVert | ShrinkVert,

	/// <summary>
	/// Shrink and stretch texture both horizontally and vertically as needed to fit in the available space.
	/// </summary>
	Fit = FitHorz | FitVert,

	/// <summary>
	/// Fill the drawn rectangle with the texture or sprite, tiled over the area. It can be combined with
	/// the other alignment flags to tile the texture or sprite over the calculated drawing bounds instead of
	/// shrinking or stretching.
	/// </summary>
	Tiled = 1 << 6,
};
DECLARE_ENUM_OPERATORS(FudgetImageAlignment);

/// <summary>
/// Settings for filling a rectangular area with texture, tiling, stretching, aligning or 9slicing it. Can be used in a
/// drawable or drawn by a control with DrawArea.
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetDrawArea
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetDrawArea)

	/// <summary>
	/// Necessary empty constructor. Draws nothing.
	/// </summary>
	FudgetDrawArea() : FillType(FudgetFillType::None), ImageAlignment(FudgetImageAlignment::Fit), Tint(Color::White), Offset(0.0f), Scale(1.0f), Texture(nullptr), SpriteHandle(), Slices(-1)
	{
	}

	/// <summary>
	/// Initializes the draw area to fill a rectangle with a texture
	/// </summary>
	/// <param name="texture">Texture to draw inside a rectangle</param>
	/// <param name="alignment">Placement and stretching options to draw the texture inside the rectangle</param>
	/// <param name="point_draw">Draw texture with point rendering</param>
	/// <param name="tint">Color to multiply the texture with</param>
	/// <param name="tex_scale">Scale of the texture when not drawing stretched</param>
	/// <param name="tex_offset">Offset of the texture when not drawing stretched</param>
	FudgetDrawArea(TextureBase *texture, bool point_draw = false, FudgetImageAlignment alignment = FudgetImageAlignment::Fit, const Color &tint = Color::White, Float2 tex_scale = Float2(-1.0f), Float2 tex_offset = Float2(0.f)) :
		FillType(point_draw ? FudgetFillType::Point : FudgetFillType::Normal), ImageAlignment(alignment), Tint(tint), Texture(texture), Offset(tex_offset), Scale(tex_scale), SpriteHandle(), Slices(-1)
	{
	}

	/// <summary>
	/// Initializes the draw area to fill a rectangle with a texture using 9-slicing.
	/// </summary>
	/// <param name="texture">Texture to draw inside a rectangle</param>
	/// <param name="alignment">Placement and stretching options to draw the texture inside the rectangle</param>
	/// <param name="slices_9p">The division size on each side for 9-slicing</param>
	/// <param name="point_draw">Draw texture with point rendering</param>
	/// <param name="tex_scale">Scale of the texture when not drawing stretched</param>
	/// <param name="tex_offset">Offset of the texture when not drawing stretched</param>
	/// <param name="tint">Color to multiply the texture with</param>
	FudgetDrawArea(TextureBase *texture, FudgetPadding slices_9p, bool point_draw = false, FudgetImageAlignment alignment = FudgetImageAlignment::Fit, const Color &tint = Color::White, Float2 tex_scale = Float2(-1.0f), Float2 tex_offset = Float2(0.f)) :
		FillType(FudgetFillType::Sliced | (point_draw ? FudgetFillType::Point : FudgetFillType::Normal)), ImageAlignment(alignment), Tint(tint), Texture(texture), Offset(tex_offset), Scale(tex_scale), SpriteHandle(), Slices(slices_9p)
	{
	}

	/// <summary>
	/// Initializes the draw area to fill a rectangle with a sprite
	/// </summary>
	/// <param name="sprite_handle">Sprite to draw inside a rectangle</param>
	/// <param name="alignment">Placement and stretching options to draw the sprite inside the rectangle</param>
	/// <param name="point_draw">Draw sprite with point rendering</param>
	/// <param name="tex_scale">Scale of the sprite when not drawing stretched</param>
	/// <param name="tex_offset">Offset of the sprite when not drawing stretched</param>
	/// <param name="tint">Color to multiply the sprite with</param>
	FudgetDrawArea(const SpriteHandle &sprite_handle, bool point_draw = false, FudgetImageAlignment alignment = FudgetImageAlignment::Fit, const Color &tint = Color::White, Float2 tex_scale = Float2(-1.0f), Float2 tex_offset = Float2(0.f)) :
		FillType(point_draw ? FudgetFillType::Point : FudgetFillType::Normal), ImageAlignment(alignment), Tint(tint), Texture(nullptr), Offset(tex_offset), Scale(tex_scale), SpriteHandle(sprite_handle), Slices(-1)
	{
	}

	/// <summary>
	/// Initializes the draw area to fill a rectangle with a sprite using 9-slicing.
	/// </summary>
	/// <param name="sprite_handle">Sprite to draw inside a rectangle</param>
	/// <param name="alignment">Placement and stretching options to draw the sprite inside the rectangle</param>
	/// <param name="slices_9p">The division size on each side for 9-slicing</param>
	/// <param name="point_draw">Draw sprite with point rendering</param>
	/// <param name="tex_scale">Scale of the sprite when not drawing stretched</param>
	/// <param name="tex_offset">Offset of the sprite when not drawing stretched</param>
	/// <param name="tint">Color to multiply the sprite with</param>
	FudgetDrawArea(const SpriteHandle &sprite_handle, FudgetPadding slices_9p, bool point_draw = false, FudgetImageAlignment alignment = FudgetImageAlignment::Fit, const Color &tint = Color::White, Float2 tex_scale = Float2(-1.0f), Float2 tex_offset = Float2(0.f)) :
		FillType(FudgetFillType::Sliced | (point_draw ? FudgetFillType::Point : FudgetFillType::Normal)), ImageAlignment(alignment), Tint(tint), Texture(nullptr), Offset(tex_offset), Scale(tex_scale), SpriteHandle(sprite_handle), Slices(slices_9p)
	{
	}

	FudgetDrawArea(const FudgetDrawArea &other)
	{
		Tint = other.Tint;
		Offset = other.Offset;
		Scale = other.Scale;
		FillType = other.FillType;
		ImageAlignment = other.ImageAlignment;
		Texture = other.Texture;
		SpriteHandle = other.SpriteHandle;
		Slices = other.Slices;
	}

	FudgetDrawArea(FudgetDrawArea &&other) noexcept
	{
		Tint = other.Tint;
		Offset = other.Offset;
		Scale = other.Scale;
		FillType = other.FillType;
		ImageAlignment = other.ImageAlignment;
		Texture = other.Texture;
		other.Texture = nullptr;
		SpriteHandle = other.SpriteHandle;
		Slices = other.Slices;
	}

	FudgetDrawArea& operator=(const FudgetDrawArea &other)
	{
		Tint = other.Tint;
		Offset = other.Offset;
		Scale = other.Scale;
		FillType = other.FillType;
		ImageAlignment = other.ImageAlignment;
		Texture = other.Texture;
		SpriteHandle = other.SpriteHandle;
		Slices = other.Slices;
		return *this;
	}

	FudgetDrawArea& operator=(FudgetDrawArea &&other) noexcept
	{
		Tint = other.Tint;
		Offset = other.Offset;
		Scale = other.Scale;
		FillType = other.FillType;
		ImageAlignment = other.ImageAlignment;
		Texture = other.Texture;
		other.Texture = nullptr;
		SpriteHandle = other.SpriteHandle;
		Slices = other.Slices;
		return *this;
	}

	/// <summary>
	/// Determines how the drawn area is filled.
	/// </summary>
	API_FIELD() FudgetFillType FillType;
	/// <summary>
	/// Alignment option that determines how the texture or sprite is drawn inside the drawing rectangle.
	/// </summary>
	API_FIELD() FudgetImageAlignment ImageAlignment;

	/// <summary>
	/// Flat color to fill the area
	/// </summary>
	API_FIELD() Color Tint;

	/// <summary>
	/// Used for tiled sprite and texture drawing, to offset the tiles left or right 
	/// Offset is applied after scaling.
	/// </summary>
	API_FIELD() Float2 Offset;

	/// <summary>
	/// Used for tiled sprite and texture drawing, to scale the size of the drawn texture for tiling.
	/// Offset is applied after scaling.
	/// </summary>
	API_FIELD() Float2 Scale;

	/// <summary>
	/// Texture to draw in the area
	/// </summary>
	API_FIELD() AssetReference<TextureBase> Texture;

	/// <summary>
	/// Sprite information to draw in the area
	/// </summary>
	API_FIELD() FudgetSpriteHandle SpriteHandle;

	/// <summary>
	/// Set when the area is for a 9-slicing texture.
	/// </summary>
	API_FIELD() FudgetPadding Slices;
};



/// <summary>
/// Used in FudgetDrawBorder when it is initialized. Use one of the constructors instead.
/// </summary>
API_ENUM(Attributes = "Flags")
enum class FudgetBorderType : uint8
{
	/// <summary>
	/// No drawing. Only here because Flax needs an empty constructor.
	/// </summary>
	None = 0,

	// The fill color or pattern, only one of these is valid

	/// <summary>
	/// Border is single color. If not set, it's either a texture or a sprite handle, depending on which member is set.
	/// </summary>
	Color = 1 << 0,

	/// <summary>
	/// Border is drawn using a texture or sprite, whichever is set.
	/// </summary>
	Texture = 1 << 1,


	/// <summary>
	/// Use point drawing for texture or sprite
	/// </summary>
	Point = 1 << 2,
	/// <summary>
	/// Stretch the texture or sprite over the drawing rectangle. Not specified means tiled texture.
	/// </summary>
	Stretch = 1 << 3,
};
DECLARE_ENUM_OPERATORS(FudgetBorderType);

/// <summary>
/// Values determining a frame's placement and size
/// </summary>
API_ENUM()
enum class FudgetBorderPlacement : uint8
{
	/// <summary>
	/// Draw a border inside a rectangle, its outer edges touching the drawing area.
	/// </summary>
	Inside = 1 << 0,
	/// <summary>
	/// Draw a border outside a rectangle, its inner edges touching the drawing area.
	/// </summary>
	Outside = 1 << 1,
	/// <summary>
	/// Draw a border with its lines centered on the a rectangle, the edges center are on the drawing area's edges.
	/// </summary>
	Centered = Inside | Outside,
};
DECLARE_ENUM_OPERATORS(FudgetBorderPlacement);


/// <summary>
/// Settings for drawing a rectangular border with a color or texture, tiling, stretching, offsetting the texture as set. Can be used in a drawable
/// or drawn by a control with DrawBorder..
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetDrawBorder
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetDrawBorder);

	/// <summary>
	/// Necessary empty constructor. No drawing.
	/// </summary>
	FudgetDrawBorder() : BorderType(FudgetBorderType::None), BorderPlacement(FudgetBorderPlacement::Centered), Tint(Color::White), Offset(0.0f), Scale(1.0f), Texture(nullptr), SpriteHandle(), Borders(-1.f)
	{
	}


	/// <summary>
	/// Initializes the border draw object to draw a border with a single color and specified border edge sizes.
	/// /// </summary>
	/// <param name="color">Color of the border</param>
	/// <param name="borders">The width of border edges on each side</param>
	/// <param name="placement">Where to draw the border on a rectangle's edges</param>
	FudgetDrawBorder(Color color, FudgetBorder borders, FudgetBorderPlacement placement) : BorderType(FudgetBorderType::Color), BorderPlacement(placement), Tint(color), Offset(0.0f), Scale(1.0f), Texture(nullptr), SpriteHandle(), Borders(borders)
	{
	}

	/// <summary>
	/// Initializes the border draw object to draw a border with a texture
	/// </summary>
	/// <param name="texture">Texture to fill inside the borders</param>
	/// <param name="borders">The width of border edges on each side</param>
	/// <param name="placement">Where to draw the border on a rectangle's edges</param>
	/// <param name="stretch">Stretching the texture instead of tiling</param>
	/// <param name="point_draw">Draw texture with point rendering</param>
	/// <param name="tint">Color to multiply the texture with</param>
	/// <param name="tex_offset">Offset of the texture when not drawing stretched</param>
	/// <param name="tex_scale">Scale of the texture when not drawing stretched</param>
	FudgetDrawBorder(TextureBase *texture, FudgetBorder borders, FudgetBorderPlacement placement, bool stretch = false, bool point_draw = false, const Color &tint = Color::White, Float2 tex_offset = Float2(0.f), Float2 tex_scale = Float2(-1.0f)) :
		BorderType(FudgetBorderType::Texture | (stretch ? FudgetBorderType::Stretch : FudgetBorderType::None) | (point_draw ? FudgetBorderType::Point : FudgetBorderType::None)),
		BorderPlacement(placement), Tint(tint), Offset(tex_offset), Scale(tex_scale), Texture(texture), SpriteHandle(), Borders(borders)
	{
	}


	/// <summary>
	/// Initializes the border draw object to draw a border with a sprite
	/// </summary>
	/// <param name="sprite_handle">Sprite to fill inside the borders</param>
	/// <param name="borders">The width of border edges on each side</param>
	/// <param name="placement">Where to draw the border on a rectangle's edges</param>
	/// <param name="stretch">Stretching the sprite instead of tiling</param>
	/// <param name="point_draw">Draw sprite with point rendering</param>
	/// <param name="tint">Color to multiply the sprite with</param>
	/// <param name="tex_offset">Offset of the sprite when not drawing stretched</param>
	/// <param name="tex_scale">Scale of the sprite when not drawing stretched</param>
	FudgetDrawBorder(const SpriteHandle &sprite_handle, FudgetBorder borders, FudgetBorderPlacement placement, bool stretch = false, bool point_draw = false, const Color &tint = Color::White, Float2 tex_offset = Float2(0.f), Float2 tex_scale = Float2(-1.0f)) :
		BorderType(FudgetBorderType::Texture | (stretch ? FudgetBorderType::Stretch : FudgetBorderType::None) | (point_draw ? FudgetBorderType::Point : FudgetBorderType::None)),
		BorderPlacement(placement), Tint(tint), Offset(tex_offset), Scale(tex_scale), Texture(nullptr), SpriteHandle(sprite_handle), Borders(borders)
	{
	}

	FudgetDrawBorder(const FudgetDrawBorder &other)
	{
		Tint = other.Tint;
		Offset = other.Offset;
		Scale = other.Scale;
		BorderType = other.BorderType;
		BorderPlacement = other.BorderPlacement;
		Texture = other.Texture;
		SpriteHandle = other.SpriteHandle;
		Borders = other.Borders;
	}

	FudgetDrawBorder(FudgetDrawBorder &&other) noexcept
	{
		Tint = other.Tint;
		Offset = other.Offset;
		Scale = other.Scale;
		BorderType = other.BorderType;
		BorderPlacement = other.BorderPlacement;
		Texture = other.Texture;
		other.Texture = nullptr;
		SpriteHandle = other.SpriteHandle;
		Borders = other.Borders;
	}

	FudgetDrawBorder& operator=(const FudgetDrawBorder &other)
	{
		Tint = other.Tint;
		Offset = other.Offset;
		Scale = other.Scale;
		BorderType = other.BorderType;
		BorderPlacement = other.BorderPlacement;
		Texture = other.Texture;
		SpriteHandle = other.SpriteHandle;
		Borders = other.Borders;
		return *this;
	}

	FudgetDrawBorder& operator=(FudgetDrawBorder &&other) noexcept
	{
		Tint = other.Tint;
		Offset = other.Offset;
		Scale = other.Scale;
		BorderType = other.BorderType;
		BorderPlacement = other.BorderPlacement;
		Texture = other.Texture;
		other.Texture = nullptr;
		SpriteHandle = other.SpriteHandle;
		Borders = other.Borders;
		return *this;
	}

	/// <summary>
	/// Which of the fields is used, and in what way
	/// </summary>
	API_FIELD() FudgetBorderType BorderType;

	/// <summary>
	/// Where the border is drawn on the drawing rectangle edges.
	/// </summary>
	API_FIELD() FudgetBorderPlacement BorderPlacement;

	/// <summary>
	/// Flat color to fill the area
	/// </summary>
	API_FIELD() Color Tint;

	/// <summary>
	/// Used for tiled sprite and texture drawing, to offset the tiles left or right 
	/// Offset is applied after scaling.
	/// </summary>
	API_FIELD() Float2 Offset;

	/// <summary>
	/// Used for tiled sprite and texture drawing, to scale the size of the drawn texture for tiling.
	/// Offset is applied after scaling.
	/// </summary>
	API_FIELD() Float2 Scale;

	/// <summary>
	/// Texture to draw in the area
	/// </summary>
	API_FIELD() AssetReference<TextureBase> Texture;

	/// <summary>
	/// Sprite information to draw in the area
	/// </summary>
	API_FIELD() FudgetSpriteHandle SpriteHandle;

	/// <summary>
	/// Set when the area is for a 9-slicing image or it is for drawing a border.
	/// </summary>
	API_FIELD() FudgetBorder Borders;
};

/// <summary>
/// Resource with settings for font creation. Set it as resource in a theme to get a font asset for style drawing.
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetFontSettings
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetFontSettings)

	/// <summary>
	/// Default empty constructor that sets invalid settings
	/// </summary>
	FudgetFontSettings() : FontId(-1), Size(12.f), Bold(false), Italics(false)
	{}

	/// <summary>
	/// Initializer for every member of the struct
	/// </summary>
	FudgetFontSettings(int fontId, float size, bool bold = false, bool italics = false) : FontId(fontId), Size(size), Bold(bold), Italics(italics)
	{
	}

	FudgetFontSettings(const FudgetFontSettings &other)
	{
		FontId = other.FontId;
		Size = other.Size;
		Bold = other.Bold;
		Italics = other.Italics;
	}

	FudgetFontSettings& operator=(const FudgetFontSettings &other)
	{
		FontId = other.FontId;
		Size = other.Size;
		Bold = other.Bold;
		Italics = other.Italics;
		return *this;
	}

	FudgetFontSettings(FudgetFontSettings &&other) noexcept
	{
		FontId = other.FontId;
		other.FontId = -1;
		Size = other.Size;
		Bold = other.Bold;
		Italics = other.Italics;
	}

	FudgetFontSettings& operator=(FudgetFontSettings &&other) noexcept
	{
		FontId = other.FontId;
		other.FontId = -1;
		Size = other.Size;
		Bold = other.Bold;
		Italics = other.Italics;
		return *this;
	}

	/// <summary>
	/// Id for the font asset in FudgetThemes.
	/// </summary>
	API_FIELD() int FontId;
	/// <summary>
	/// Size to display the font at.
	/// </summary>
	API_FIELD() float Size;
	/// <summary>
	/// Write text with bold font.
	/// </summary>
	API_FIELD() bool Bold;
	/// <summary>
	/// Write text with italics font.
	/// </summary>
	API_FIELD() bool Italics;
};

/// <summary>
/// A font object with its creation settings
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetFont
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetFont);

	FudgetFont() : Font(nullptr), Settings()
	{}

	FudgetFont(const FudgetFont &other)
	{
		Font = other.Font;
		Settings = other.Settings;
	}

	FudgetFont& operator=(const FudgetFont &other)
	{
		Font = other.Font;
		Settings = other.Settings;
		return *this;
	}

	FudgetFont(FudgetFont &&other) noexcept
	{
		Font = other.Font;
		Settings = std::move(other.Settings);
		other.Font = nullptr;
	}

	FudgetFont& operator=(FudgetFont &&other) noexcept
	{
		Font = other.Font;
		Settings = std::move(other.Settings);
		other.Font = nullptr;
		return *this;
	}

	/// <summary>
	/// A font object created from the Settings
	/// </summary>
	API_FIELD() Font *Font;
	
	/// <summary>
	/// Settings that were used to generate the font
	/// </summary>
	API_FIELD() FudgetFontSettings Settings;
};


/// <summary>
/// Settings for drawing text of a control
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetTextDrawSettings
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetTextDrawSettings)

	FudgetTextDrawSettings() : FudgetTextDrawSettings(0, Color::White, TextAlignment::Near, TextAlignment::Center, 0, -1)
	{
	}

	FudgetTextDrawSettings(Color color, TextAlignment horz_align = TextAlignment::Near, TextAlignment vert_align = TextAlignment::Center, FudgetPadding padding = FudgetPadding(0), int material_id = -1) :
		FudgetTextDrawSettings(0, color, horz_align, vert_align, padding, material_id)
	{

	}

	FudgetTextDrawSettings(FudgetPadding padding, TextAlignment horz_align = TextAlignment::Near, TextAlignment vert_align = TextAlignment::Center, int material_id = -1) :
		FudgetTextDrawSettings(0, Color::White, horz_align, vert_align, padding, material_id)
	{

	}

	FudgetTextDrawSettings(Int2 offset, Color color = Color::White, TextAlignment horz_align = TextAlignment::Near, TextAlignment vert_align = TextAlignment::Center, FudgetPadding padding = FudgetPadding(0), int material_id = -1) :
		Offset(offset), Color(color), HorizontalAlignment(horz_align), VerticalAlignment(vert_align), Padding(padding), MaterialId(material_id)
	{

	}

	FudgetTextDrawSettings(const FudgetTextDrawSettings &other)
	{
		Offset = other.Offset;
		Color = other.Color;
		MaterialId = other.MaterialId;
		HorizontalAlignment = other.HorizontalAlignment;
		VerticalAlignment = other.VerticalAlignment;
		Padding = other.Padding;
	}

	FudgetTextDrawSettings(FudgetTextDrawSettings &&other) noexcept
	{
		Offset = other.Offset;
		Color = other.Color;
		MaterialId = other.MaterialId;
		HorizontalAlignment = other.HorizontalAlignment;
		VerticalAlignment = other.VerticalAlignment;
		Padding = other.Padding;
	}

	FudgetTextDrawSettings& operator=(const FudgetTextDrawSettings &other)
	{
		Offset = other.Offset;
		Color = other.Color;
		MaterialId = other.MaterialId;
		HorizontalAlignment = other.HorizontalAlignment;
		VerticalAlignment = other.VerticalAlignment;
		Padding = other.Padding;
		return *this;
	}
	FudgetTextDrawSettings& operator=(FudgetTextDrawSettings &&other) noexcept
	{
		Offset = other.Offset;
		Color = other.Color;
		MaterialId = other.MaterialId;
		HorizontalAlignment = other.HorizontalAlignment;
		VerticalAlignment = other.VerticalAlignment;
		Padding = other.Padding;
		return *this;
	}

	/// <summary>
	/// Offset of the text bounds when drawing.
	/// </summary>
	API_FIELD() Int2 Offset;
	/// <summary>
	/// Color of the text to draw
	/// </summary>
	API_FIELD() Color Color;
	/// <summary>
	/// How to align the text horizontally within the bounds of the font drawing. Not applicable when not drawing in bounds.
	/// </summary>
	API_FIELD() TextAlignment HorizontalAlignment;
	/// <summary>
	/// How to align the text vertically within the bounds of the font drawing. Not applicable when not drawing in bounds.
	/// </summary>
	API_FIELD() TextAlignment VerticalAlignment;
	/// <summary>
	/// Padding to use on the font drawing bounds. Not applicable when not drawing in bounds.
	/// </summary>
	API_FIELD() FudgetPadding Padding;
	/// <summary>
	/// Material on the text. Must be a material made for UI
	/// </summary>
	API_FIELD() int MaterialId;
};


/// <summary>
/// A list of states and their associated colors. Can be used directly as a resource in a theme or converted
/// to a drawable. The conversion is implicit and happens when the style fetches a resource that needs
/// to be a drawable.
/// As a state and color list, part painters can use this for picking a color for a state by the same
/// rules that drawables are picked. The first color for the first state that fully matches the drawing
/// requested state is used. Latter states are ignored. Always add the 0 (normal) state last to make
/// sure the other states are considered first.
/// It's recommended to create by the FudgetDrawableBuilder. When created manually, make sure there are no
/// duplicate states and that there are equal number of states as colors.
/// 
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetDrawColors
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetDrawColors);

	/// <summary>
	/// States that are checked when drawing. The matching state's index will indicate for the part painter
	/// which color to use. There must be equal number of items in _states and _colors.
	/// </summary>
	API_FIELD() Array<uint64> _states;
	/// <summary>
	/// Colors used for drawing. There must be equal number of items in _states and _colors.
	/// </summary>
	API_FIELD() Array<Color> _colors;

	/// <summary>
	/// Returns the index of a state within the colors list that matches the passed state flags. The index of the first
	/// state that fully matches is returned. The 0 state in the colors list matches any combination of state flags.
	/// </summary>
	/// <param name="states">State flags to match</param>
	/// <returns>Index to the first matching state in the colors list or -1 on failure</returns>
	int FindMatchingState(uint64 states) const
	{
		for (int ix = 0, siz = _states.Count(); ix < siz; ++ix)
		{
			uint64 state = _states[ix];
			if ((states & state) == state)
				return ix;
		}
		return -1;
	}

	/// <summary>
	/// Finds a color for the state matching the passed state flags and returns it if there was a match. Otherwise returns white. The first
	/// state that fully matches is used. The 0 state in the colors list matches any combination of state flags.
	/// Use FindMatchingState instead to check the success of the state matching.
	/// </summary>
	/// <param name="states">State flags to match</param>
	/// <returns>Color matching the states flags or a white color if no match was found.</returns>
	Color FindMatchingColor(uint64 states) const
	{
		int ix = FindMatchingState(states);
		if (ix < 0)
			return Color::White;
		return _colors[ix];
	}
};
