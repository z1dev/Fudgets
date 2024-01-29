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

	FudgetPadding(float left, float right, float top, float bottom) : Left(left), Right(right), Top(top), Bottom(bottom)
	{
	}

	FudgetPadding(float padding) : Left(padding), Right(padding), Top(padding), Bottom(padding)
	{
	}

	float Width() const { return Left + Right; }
	float Height() const { return Top + Bottom; }

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
enum class FudgetFillAreaType : uint8
{
	Color,
	Texture,
	TextureStretch,
	TexturePoint,
	TexturePointStretch,
	Texture9,
	Texture9Point,
	Sprite,
	SpriteStretch,
	SpritePoint,
	SpritePointStretch,
	Sprite9,
	Sprite9Point,
};

API_STRUCT()
struct FUDGETS_API FudgetFillAreaSettings
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetFillAreaSettings)

	FudgetFillAreaSettings() : AreaType(FudgetFillAreaType::Color), Color(Color::White), Texture(nullptr), SpriteHandle(), Borders9P(-1.0f)
	{
	}

	FudgetFillAreaSettings(Color color) : AreaType(FudgetFillAreaType::Color), Color(color), Texture(nullptr), SpriteHandle(), Borders9P(-1.0f)
	{
	}

	FudgetFillAreaSettings(TextureBase *texture, bool stretch = false, bool point_tex = false, const Color &color = Color::White) :
		AreaType(point_tex && stretch ? FudgetFillAreaType::TexturePointStretch : point_tex ? FudgetFillAreaType::TexturePoint :
			stretch ? FudgetFillAreaType::TextureStretch : FudgetFillAreaType::Texture), Color(color), Texture(texture), SpriteHandle(), Borders9P(-1.0f)
	{
	}

	FudgetFillAreaSettings(TextureBase *texture, Float4 borders_9p, bool point_tex = false, const Color &color = Color::White) :
		AreaType(point_tex ? FudgetFillAreaType::Texture9Point : FudgetFillAreaType::Texture9),
		Color(color), Texture(texture), SpriteHandle(), Borders9P(borders_9p)
	{
	}

	FudgetFillAreaSettings(const SpriteHandle &sprite_handle, bool stretch = false, bool point_sprite = false, const Color &color = Color::White) :
		AreaType(point_sprite && stretch ? FudgetFillAreaType::SpritePointStretch : point_sprite ? FudgetFillAreaType::SpritePoint :
		stretch ? FudgetFillAreaType::SpriteStretch : FudgetFillAreaType::Sprite), Color(color), Texture(nullptr), SpriteHandle(sprite_handle), Borders9P(-1.0f)
	{
	}

	FudgetFillAreaSettings(const SpriteHandle &sprite_handle, Float4 borders_9p, bool point_sprite, const Color &color = Color::White) :
		AreaType(point_sprite ? FudgetFillAreaType::Sprite9Point : FudgetFillAreaType::Sprite9),
		Color(color), Texture(nullptr), SpriteHandle(sprite_handle), Borders9P(borders_9p)
	{
	}

	FudgetFillAreaSettings(const FudgetFillAreaSettings &other)
	{
		Color = other.Color;
		AreaType = other.AreaType;
		Texture = other.Texture;
		SpriteHandle = other.SpriteHandle;
		Borders9P = other.Borders9P;
	}

	FudgetFillAreaSettings(FudgetFillAreaSettings &&other) noexcept
	{
		Color = other.Color;
		AreaType = other.AreaType;
		Texture = other.Texture;
		other.Texture = nullptr;
		SpriteHandle = other.SpriteHandle;
		Borders9P = other.Borders9P;
	}

	FudgetFillAreaSettings& operator=(const FudgetFillAreaSettings &other)
	{
		Color = other.Color;
		AreaType = other.AreaType;
		Texture = other.Texture;
		SpriteHandle = other.SpriteHandle;
		Borders9P = other.Borders9P;
		return *this;
	}

	FudgetFillAreaSettings& operator=(FudgetFillAreaSettings &&other) noexcept
	{
		Color = other.Color;
		AreaType = other.AreaType;
		Texture = other.Texture;
		other.Texture = nullptr;
		SpriteHandle = other.SpriteHandle;
		Borders9P = other.Borders9P;
		return *this;
	}

	// Which of the fields is used, and in what way
	FudgetFillAreaType AreaType;

	// Flat color to fill the area
	Color Color;

	// Texture to draw in the area
	AssetReference<TextureBase> Texture;

	//// Sprite information to draw in the area
	SpriteHandle SpriteHandle;

	// Set when using 9-slicing images.
	Float4 Borders9P;
};