#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Math/Vector4.h"
#include "Engine/Core/Math/Rectangle.h"

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

	FudgetPadding(const FudgetPadding &other) : Left(other.Left), Right(other.Right), Top(other.Top), Bottom(other.Bottom)
	{
	}

	FudgetPadding(FudgetPadding &&other) noexcept : Left(other.Left), Right(other.Right), Top(other.Top), Bottom(other.Bottom)
	{
	}

	FudgetPadding& operator=(const FudgetPadding &other)
	{
		Left = other.Left;
		Right = other.Right;
		Top = other.Top;
		Bottom = other.Bottom;
		return *this;
	}

	FudgetPadding& operator=(FudgetPadding &&other) noexcept
	{
		Left = other.Left;
		Right = other.Right;
		Top = other.Top;
		Bottom = other.Bottom;
		return *this;
	}

	/// <summary>
	/// Converts padding to a Float4 with left, right, top, bottom values in this order.
	/// </summary>
	Float4 AsFloat4() const { return Float4(Left, Right, Top, Bottom); }

	/// <summary>
	/// Returns a rectangle that results in padding the passed value
	/// </summary>
	/// <param name="rect">Rectangle to pad</param>
	Rectangle Padded(const Rectangle &rect) const
	{
		return Rectangle(rect.Location + Float2(Left, Top), rect.Size - Float2(Width(), Height()) );
	}

	/// <summary>
	/// Left and right padding added together.
	/// </summary>
	FORCE_INLINE float Width() const { return Left + Right; }
	/// <summary>
	/// Top and bottom padding added together.
	/// </summary>
	FORCE_INLINE float Height() const { return Top + Bottom; }

	/// <summary>
	/// Horizontal and vertical padding as a size
	/// </summary>
	FORCE_INLINE Float2 Size() const { return Float2(Left + Right, Top + Bottom);  }

	/// <summary>
	/// Padding to the upper left
	/// </summary>
	FORCE_INLINE Float2 TopLeft() const { return Float2(Left, Top);  }
	/// <summary>
	/// Padding to the upper left
	/// </summary>
	FORCE_INLINE Float2 UpperLeft() const { return Float2(Left, Top); }
	/// <summary>
	/// Padding to the lower right
	/// </summary>
	FORCE_INLINE Float2 BottomRight() const { return Float2(Right, Bottom); }
	/// <summary>
	/// Padding to the lower right
	/// </summary>
	FORCE_INLINE Float2 LowerRight() const { return Float2(Right, Bottom); }

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


