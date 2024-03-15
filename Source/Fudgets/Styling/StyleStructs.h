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
/// Type for drawing a FudgetDrawArea. It determines how the area is filled, whether with color or texture, or only as a border.
/// </summary>
API_ENUM(Attributes = "Flags")
enum class FudgetFillType : uint8
{
	/// <summary>
	/// None
	/// </summary>
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

/// <summary>
/// Values determining a frame's placement and exact size
/// </summary>
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
	FudgetDrawArea() : AreaType(FudgetFillType::None), Tint(Color::White), TextureOffset(0.0f), TextureScale(1.0f), Texture(nullptr), SpriteHandle(), Borders(-1.f)
	{
	}

	/// <summary>
	/// Initializes draw area to fill a rectangle with the specified color
	/// </summary>
	/// <param name="color">Color used for filling the rectangle</param>
	FudgetDrawArea(Color color) : AreaType(FudgetFillType::Color), Tint(color), TextureOffset(0.0f), TextureScale(1.0f), Texture(nullptr), SpriteHandle(), Borders(-1.f)
	{
	}

	/// <summary>
	/// Initializes draw area to draw a border in a rectangle with the specified color and border width.
	/// /// </summary>
	/// <param name="borders">The width of borders on each side</param>
	/// <param name="color">Color of the border</param>
	/// <param name="border_type">Where to draw the border</param>
	FudgetDrawArea(FudgetBorder borders, Color color, FudgetFrameType border_type) : AreaType(FudgetFillType::Color |
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
	FudgetDrawArea(TextureBase *texture, bool stretch = false, bool point_tex = false, Float2 tex_offset = Float2(0.f), Float2 tex_scale = Float2(-1.0f), const Color &tint = Color::White) :
		AreaType((stretch ? FudgetFillType::Stretch : FudgetFillType::None) | (point_tex ? FudgetFillType::Point : FudgetFillType::None)),
		Tint(tint), Texture(texture), TextureOffset(tex_offset), TextureScale(tex_scale), SpriteHandle(), Borders(-1.f)
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
	FudgetDrawArea(TextureBase *texture, FudgetBorder border_widths, FudgetFrameType border_type, bool stretch = false, bool point_tex = false, Float2 tex_offset = Float2(0.f), Float2 tex_scale = Float2(-1.0f), const Color &tint = Color::White) :
		AreaType((stretch ? FudgetFillType::Stretch : FudgetFillType::None) | (point_tex ? FudgetFillType::Point : FudgetFillType::None) |
		(border_type == FudgetFrameType::Inside ? FudgetFillType::BorderInside : border_type == FudgetFrameType::Outside ? FudgetFillType::BorderOutside :
		FudgetFillType::BorderCentered)), Tint(tint), TextureOffset(tex_offset), TextureScale(tex_scale), Texture(texture), SpriteHandle(), Borders(border_widths)
	{
	}

	/// <summary>
	/// Initializes draw area to fill a rectangle with a texture using 9-slicing.
	/// </summary>
	/// <param name="texture">Texture to draw inside the border</param>
	/// <param name="borders_9p">Fixed sized not stretched borders of the texture</param>
	/// <param name="point_tex">Draw texture with point rendering</param>
	/// <param name="tex_offset">Offset of the texture when not drawing stretched</param>
	/// <param name="tint">Color to multiply the texture with</param>
	FudgetDrawArea(TextureBase *texture, FudgetPadding borders_9p, bool point_tex = false, Float2 tex_offset = Float2(0.f), Float2 tex_scale = Float2(-1.0f), const Color &tint = Color::White) :
		AreaType(FudgetFillType::Sliced | (point_tex ? FudgetFillType::Point : FudgetFillType::None)),
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
	FudgetDrawArea(const SpriteHandle &sprite_handle, bool stretch = false, bool point_sprite = false, Float2 tex_offset = Float2(0.f), Float2 tex_scale = Float2(-1.0f), const Color &tint = Color::White) :
		AreaType((stretch ? FudgetFillType::Stretch : FudgetFillType::None) | (point_sprite ? FudgetFillType::Point : FudgetFillType::None)),
		Tint(tint), TextureOffset(tex_offset), TextureScale(tex_scale), Texture(nullptr), SpriteHandle(sprite_handle), Borders(-1.f)
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
	FudgetDrawArea(const SpriteHandle &sprite_handle, FudgetPadding border_widths, FudgetFrameType border_type, bool stretch = false, bool point_sprite = false, Float2 tex_offset = Float2(0.f), Float2 tex_scale = Float2(-1.0f), const Color &tint = Color::White) :
		AreaType((stretch ? FudgetFillType::Stretch : FudgetFillType::None) | (point_sprite ? FudgetFillType::Point : FudgetFillType::None) |
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
	FudgetDrawArea(const SpriteHandle &sprite_handle, FudgetPadding borders_9p, bool point_sprite, Float2 tex_offset = Float2(0.f), Float2 tex_scale = Float2(-1.0f), const Color &tint = Color::White) :
		AreaType(FudgetFillType::Sliced | (point_sprite ? FudgetFillType::Point : FudgetFillType::None)),
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
	API_FIELD() FudgetSpriteHandle SpriteHandle;

	/// <summary>
	/// Set when the area is for a 9-slicing image or it is for drawing a border.
	/// </summary>
	API_FIELD() FudgetBorder Borders;
};


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
