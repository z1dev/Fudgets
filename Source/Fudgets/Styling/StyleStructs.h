#pragma once

#include "Token.h"
#include "../MarginStructs.h"

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Render2D/SpriteAtlas.h"
#include "Engine/Core/Math/Color.h"
#include "Engine/Core/Math/Vector4.h"
#include "Engine/Render2D/FontAsset.h"

API_ENUM(Attributes = "Flags")
enum class FudgetFillType : uint8
{
	None = 0,

	// The fill color or pattern, only one of these is valid

	/// <summary>
	/// Fill or border with single color. If not set, it depends on whether a texture or a sprite handle is set in the draw area.
	/// </summary>
	Color = 1 << 0,

	// Drawing filled area or border, only one of these is valid. It's fill if no border is set.

	/// <summary>
	/// Fill rectangle
	/// </summary>
	Fill = 0,
	/// <summary>
	/// Draw border inside rectangle
	/// </summary>
	BorderInside = 1 << 1,
	/// <summary>
	/// Draw border outside rectangle
	/// </summary>
	BorderOutside = 1 << 2,
	/// <summary>
	/// Draw border centered on rectangle
	/// </summary>
	BorderCentered = BorderInside | BorderOutside,

	// Point, stretch, 9-slice. Only used with texture or sprite.

	/// <summary>
	/// Use point drawing
	/// </summary>
	Point = 1 << 3,
	/// <summary>
	/// Stretch texture or style over the rectangle. Not specified means tiled
	/// </summary>
	Stretch = 1 << 4,
	/// <summary>
	/// Draw over rectangle with 9slicing. Only used for Fill
	/// </summary>
	Sliced = 1 << 5

};
DECLARE_ENUM_OPERATORS(FudgetFillType);

API_ENUM()
enum class FudgetFrameType
{
	/// <summary>
	/// Draw a frame with width inside a rectangle
	/// </summary>
	Inside,
	/// <summary>
	/// Draw a frame with width outside a rectangle like an outline
	/// </summary>
	Outside,
	/// <summary>
	/// Draw a frame with width, borders centered on the rectangle.
	/// </summary>
	Centered
};

/// <summary>
/// Settings for filling a background or drawing a border with a color or texture with or without 9 slicing. Use with a control's DrawArea function.
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetDrawArea
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetDrawArea)

	/// <summary>
	/// Necessary empty constructor. For filling area with white color
	/// </summary>
	FudgetDrawArea() : AreaType(FudgetFillType::Color), Tint(Color::White), TextureOffset(0.0f), TextureScale(1.0f), Texture(nullptr), SpriteHandle(), Borders(-1.0f)
	{
	}

	/// <summary>
	/// Initializes draw area to fill a rectangle with the specified color
	/// </summary>
	/// <param name="color">Color used for filling the rectangle</param>
	FudgetDrawArea(Color color) : AreaType(FudgetFillType::Color), Tint(color), TextureOffset(0.0f), TextureScale(1.0f), Texture(nullptr), SpriteHandle(), Borders(-1.0f)
	{
	}

	/// <summary>
	/// Initializes draw area to draw a border in a rectangle with the specified color and border width.
	/// /// </summary>
	/// <param name="borders">The width of borders on each side</param>
	/// <param name="color">Color of the border</param>
	/// <param name="border_type">Where to draw the border</param>
	FudgetDrawArea(FudgetPadding borders, Color color, FudgetFrameType border_type) : AreaType(FudgetFillType::Color |
		(border_type == FudgetFrameType::Inside ? FudgetFillType::BorderInside : border_type == FudgetFrameType::Outside ? FudgetFillType::BorderOutside :
		FudgetFillType::BorderCentered)), Tint(color), TextureOffset(0.0f), TextureScale(1.0f), Texture(nullptr), SpriteHandle(), Borders(borders)
	{
	}

	/// <summary>
	/// Initializes draw area to fill a rectangle with a texture
	/// </summary>
	/// <param name="texture">Texture to draw in a rectangle</param>
	/// <param name="stretch">Stretching the texture instead of tiling</param>
	/// <param name="point_tex">Draw texture with point rendering</param>
	/// <param name="tex_offset">Offset of the texture when not drawing stretched</param>
	/// <param name="tint">Color to multiply the texture with</param>
	FudgetDrawArea(TextureBase *texture, bool stretch = false, bool point_tex = false, Float2 tex_offset = Float2(0.0f), Float2 tex_scale = Float2(-1.0f), const Color &tint = Color::White) :
		AreaType(FudgetFillType::Texture | (stretch ? FudgetFillType::Stretch : FudgetFillType::None) | (point_tex ? FudgetFillType::Point : FudgetFillType::None)),
		Tint(tint), Texture(texture), TextureOffset(tex_offset), TextureScale(tex_scale), SpriteHandle(), Borders(-1.0f)
	{
	}

	/// <summary>
	/// Initializes draw area to fill a border with a texture
	/// </summary>
	/// <param name="texture">Texture to fill inside the borders</param>
	/// <param name="border_widths">Width of the border</param>
	/// <param name="border_type">Where to draw the border</param>
	/// <param name="stretch">Stretching the texture instead of tiling</param>
	/// <param name="point_tex">Draw texture with point rendering</param>
	/// <param name="tex_offset">Offset of the texture when not drawing stretched</param>
	/// <param name="tint">Color to multiply the texture with</param>
	FudgetDrawArea(TextureBase *texture, FudgetPadding border_widths, FudgetFrameType border_type, bool stretch = false, bool point_tex = false, Float2 tex_offset = Float2(0.0f), Float2 tex_scale = Float2(-1.0f), const Color &tint = Color::White) :
		AreaType(FudgetFillType::Texture | (stretch ? FudgetFillType::Stretch : FudgetFillType::None) | (point_tex ? FudgetFillType::Point : FudgetFillType::None) |
		(border_type == FudgetFrameType::Inside ? FudgetFillType::BorderInside : border_type == FudgetFrameType::Outside ? FudgetFillType::BorderOutside :
		FudgetFillType::BorderCentered)), Tint(tint), TextureOffset(tex_offset), TextureScale(tex_scale), Texture(texture), SpriteHandle(), Borders(border_widths)
	{
	}

	/// <summary>
	/// Initializes draw area to fill a rectangle with a texture using 9-slicing.
	/// </summary>
	/// <param name="texture">Texture to draw inside the border</param>
	/// <param name="borders_9p">Fixed sized not stretched borders around the texture</param>
	/// <param name="point_tex">Draw texture with point rendering</param>
	/// <param name="tex_offset">Offset of the texture when not drawing stretched</param>
	/// <param name="tint">Color to multiply the texture with</param>
	FudgetDrawArea(TextureBase *texture, FudgetPadding borders_9p, bool point_tex = false, Float2 tex_offset = Float2(0.0f), Float2 tex_scale = Float2(-1.0f), const Color &tint = Color::White) :
		AreaType(FudgetFillType::Texture | FudgetFillType::Sliced | (point_tex ? FudgetFillType::Point : FudgetFillType::None)),
		Tint(tint), TextureOffset(tex_offset), TextureScale(tex_scale), Texture(texture), SpriteHandle(), Borders(borders_9p)
	{
	}

	/// <summary>
	/// Initializes draw area to fill a rectangle with a sprite
	/// </summary>
	/// <param name="sprite_handle">Sprite handle for sprite to draw in a rectangle</param>
	/// <param name="stretch">Stretching the sprite instead of tiling</param>
	/// <param name="point_sprite">Draw sprite with point rendering</param>
	/// <param name="tex_offset">Offset of the sprite when not drawing stretched</param>
	/// <param name="tint">Color to multiply the sprite with</param>
	FudgetDrawArea(const SpriteHandle &sprite_handle, bool stretch = false, bool point_sprite = false, Float2 tex_offset = Float2(0.0f), Float2 tex_scale = Float2(-1.0f), const Color &tint = Color::White) :
		AreaType(FudgetFillType::Sprite | (stretch ? FudgetFillType::Stretch : FudgetFillType::None) | (point_sprite ? FudgetFillType::Point : FudgetFillType::None)),
		Tint(tint), TextureOffset(tex_offset), TextureScale(tex_scale), Texture(nullptr), SpriteHandle(sprite_handle), Borders(-1.0f)
	{
	}

	/// <summary>
	/// Initializes draw area to fill a border with a sprite
	/// </summary>
	/// <param name="sprite_handle">Sprite handle for sprite to fill inside the borders</param>
	/// <param name="border_widths">Width of the border</param>
	/// <param name="border_type">Where to draw the border</param>
	/// <param name="stretch">Stretching the sprite instead of tiling</param>
	/// <param name="point_sprite">Draw sprite with point rendering</param>
	/// <param name="tex_offset">Offset of the sprite when not drawing stretched</param>
	/// <param name="tint">Color to multiply the sprite with</param>
	FudgetDrawArea(const SpriteHandle &sprite_handle, FudgetPadding border_widths, FudgetFrameType border_type, bool stretch = false, bool point_sprite = false, Float2 tex_offset = Float2(0.0f), Float2 tex_scale = Float2(-1.0f), const Color &tint = Color::White) :
		AreaType(FudgetFillType::Sprite | (stretch ? FudgetFillType::Stretch : FudgetFillType::None) | (point_sprite ? FudgetFillType::Point : FudgetFillType::None) |
		(border_type == FudgetFrameType::Inside ? FudgetFillType::BorderInside : border_type == FudgetFrameType::Outside ? FudgetFillType::BorderOutside :
		FudgetFillType::BorderCentered)), Tint(tint), TextureOffset(tex_offset), TextureScale(tex_scale), Texture(nullptr), SpriteHandle(sprite_handle), Borders(border_widths)
	{
	}

	/// <summary>
	/// Initializes draw area to fill a rectangle with a sprite using 9-slicing.
	/// </summary>
	/// <param name="sprite_handle">Sprite to draw inside the border</param>
	/// <param name="borders_9p">Fixed sized not stretched borders around the sprite</param>
	/// <param name="point_sprite">Draw sprite with point rendering</param>
	/// <param name="tex_offset">Offset of the sprite when not drawing stretched</param>
	/// <param name="tint">Color to multiply the sprite with</param>
	FudgetDrawArea(const SpriteHandle &sprite_handle, FudgetPadding borders_9p, bool point_sprite, Float2 tex_offset = Float2(0.0f), Float2 tex_scale = Float2(-1.0f), const Color &tint = Color::White) :
		AreaType(FudgetFillType::Sprite | FudgetFillType::Sliced | (point_sprite ? FudgetFillType::Point : FudgetFillType::None)),
		Tint(tint), TextureOffset(tex_offset), TextureScale(tex_scale), Texture(nullptr), SpriteHandle(sprite_handle), Borders(borders_9p)
	{
	}

	FudgetDrawArea(const FudgetDrawArea &other)
	{
		Tint = other.Tint;
		TextureOffset = other.TextureOffset;
		TextureScale = other.TextureScale;
		AreaType = other.AreaType;
		Texture = other.Texture;
		SpriteHandle = other.SpriteHandle;
		Borders = other.Borders;
	}

	FudgetDrawArea(FudgetDrawArea &&other) noexcept
	{
		Tint = other.Tint;
		TextureOffset = other.TextureOffset;
		TextureScale = other.TextureScale;
		AreaType = other.AreaType;
		Texture = other.Texture;
		other.Texture = nullptr;
		SpriteHandle = other.SpriteHandle;
		Borders = other.Borders;
	}

	FudgetDrawArea& operator=(const FudgetDrawArea &other)
	{
		Tint = other.Tint;
		TextureOffset = other.TextureOffset;
		TextureScale = other.TextureScale;
		AreaType = other.AreaType;
		Texture = other.Texture;
		SpriteHandle = other.SpriteHandle;
		Borders = other.Borders;
		return *this;
	}

	FudgetDrawArea& operator=(FudgetDrawArea &&other) noexcept
	{
		Tint = other.Tint;
		TextureOffset = other.TextureOffset;
		TextureScale = other.TextureScale;
		AreaType = other.AreaType;
		Texture = other.Texture;
		other.Texture = nullptr;
		SpriteHandle = other.SpriteHandle;
		Borders = other.Borders;
		return *this;
	}

	/// <summary>
	/// Which of the fields is used, and in what way
	/// </summary>
	API_FIELD() FudgetFillType AreaType;

	/// <summary>
	/// Flat color to fill the area
	/// </summary>
	API_FIELD() Color Tint;

	/// <summary>
	/// Used for tiled sprite and texture drawing, to offset the tiles left or right 
	/// Offset is applied after scaling.
	/// </summary>
	API_FIELD() Float2 TextureOffset;

	/// <summary>
	/// Used for tiled sprite and texture drawing, to scale the size of the drawn texture for tiling.
	/// Offset is applied after scaling.
	/// </summary>
	API_FIELD() Float2 TextureScale;

	/// <summary>
	/// Texture to draw in the area
	/// </summary>
	API_FIELD() AssetReference<TextureBase> Texture;

	/// <summary>
	/// Sprite information to draw in the area
	/// </summary>
	API_FIELD() SpriteHandle SpriteHandle;

	/// <summary>
	/// Set when using 9-slicing images or drawing border.
	/// </summary>
	API_FIELD() FudgetPadding Borders;
};




API_STRUCT()
struct FUDGETS_API FudgetFontSettings
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetFontSettings)

	FudgetFontSettings() : FontToken(FudgetToken::Invalid), Size(12.f), Bold(false), Italics(false)
	{}

	FudgetFontSettings(FudgetToken fontToken, float size, bool bold = false, bool italics = false) : FontToken(fontToken), Size(size), Bold(bold), Italics(italics)
	{
	}

	/// <summary>
	/// Token for the font asset in FudgetThemes.
	/// </summary>
	API_FIELD() FudgetToken FontToken;
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

API_STRUCT()
struct FUDGETS_API FudgetFont
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetFont);

	/// <summary>
	/// A font object created from the Settings
	/// </summary>
	API_FIELD() Font *Font;
	
	/// <summary>
	/// Settings that were used to generate the font
	/// </summary>
	API_FIELD() FudgetFontSettings Settings;
};

