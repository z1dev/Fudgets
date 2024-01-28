#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Render2D/SpriteAtlas.h"
#include "Engine/Core/Math/Color.h"
#include "Engine/Core/Math/Vector4.h"

/// <summary>
/// Padding inside any element that contains others, like a slot in a layout or a drawn border in a style.
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetPadding
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetPadding)

	FudgetPadding() : Left(0.0f), Right(0.0f), Top(0.0f), Bottom(0.0f)
	{
	}

	/// <summary>
	/// Padding to the left
	/// </summary>
	API_FIELD() float Left;
	/// <summary>
	/// Padding to the right
	/// </summary>
	API_FIELD() float Right;
	/// <summary>
	/// Padding to the top
	/// </summary>
	API_FIELD() float Top;
	/// <summary>
	/// Padding to the bottom
	/// </summary>
	API_FIELD() float Bottom;
};


API_ENUM()
enum class FudgetRectType
{
	Color,
	Texture,
	TextureP,
	Texture9,
	Texture9P,
	Sprite,
	SpriteP,
	Sprite9,
	Sprite9P,
};

API_STRUCT()
struct FUDGETS_API FudgetFillAreaSettings
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetFillAreaSettings)

	FudgetFillAreaSettings() : RectType(FudgetRectType::Color), Color(Color::White), Texture(nullptr), SpriteHandle(), Borders9P(-1.0f)
	{
	}

	FudgetFillAreaSettings(Color color) : RectType(FudgetRectType::Color), Color(color), Texture(nullptr), SpriteHandle(), Borders9P(-1.0f)
	{
	}

	FudgetFillAreaSettings(TextureBase *texture, bool point_tex = false) :
		RectType(point_tex ? FudgetRectType::TextureP : FudgetRectType::Texture), Color(), Texture(texture), SpriteHandle(), Borders9P(-1.0f)
	{
	}

	FudgetFillAreaSettings(TextureBase *texture, Float4 borders_9p, bool point_tex = false) :
		RectType(point_tex ? FudgetRectType::Texture9P : FudgetRectType::Texture9),
		Color(), Texture(texture), SpriteHandle(), Borders9P(borders_9p)
	{
	}

	FudgetFillAreaSettings(const SpriteHandle &sprite_handle, bool point_sprite) :
		RectType(point_sprite ? FudgetRectType::SpriteP : FudgetRectType::Sprite), Color(), Texture(nullptr), SpriteHandle(sprite_handle), Borders9P(-1.0f)
	{
	}

	FudgetFillAreaSettings(const SpriteHandle &sprite_handle, Float4 borders_9p, bool point_sprite) :
		RectType(point_sprite ? FudgetRectType::Sprite9P : FudgetRectType::Sprite9),
		Color(), Texture(nullptr), SpriteHandle(sprite_handle), Borders9P(borders_9p)
	{
	}

	FudgetFillAreaSettings& operator=(const FudgetFillAreaSettings &other)
	{
		RectType = other.RectType;
		Texture = other.Texture;
		SpriteHandle = other.SpriteHandle;
		Borders9P = other.Borders9P;
		return *this;
	}

	FudgetFillAreaSettings& operator=(FudgetFillAreaSettings &&other)
	{
		RectType = other.RectType;
		Texture = other.Texture;
		SpriteHandle = other.SpriteHandle;
		Borders9P = other.Borders9P;
		return *this;
	}

	// Which of the fields is used, and in what way
	FudgetRectType RectType;

	//This could be a union?

	// Flat color to fill the area
	Color Color;
	// Texture to draw in the area
	AssetReference<TextureBase> Texture;

	//// Sprite information to draw in the area
	//API_FIELD() SpriteAtlas *SpriteAtlas;
	//API_FIELD() int SpriteIndex;
	SpriteHandle SpriteHandle;

	// Set when using 9-slicing images.
	Float4 Borders9P;
};