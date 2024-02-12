#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Math/Vector4.h"
#include "Engine/Core/Math/Rectangle.h"


/// <summary>
/// Padding inside any element that contains others, like a slot in a layout or a drawn border in a style.
/// </summary>
API_STRUCT(NoDefault)
struct FUDGETS_API FudgetPadding
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetPadding)

	FORCE_INLINE FudgetPadding() : Left(0.0f), Top(0.0f), Right(0.0f), Bottom(0.0f)
	{
	}

	FORCE_INLINE FudgetPadding(float left, float top, float right, float bottom) : Left(left), Top(top), Right(right), Bottom(bottom)
	{
	}

	FORCE_INLINE FudgetPadding(float horizontal, float vertical) : Left(horizontal), Top(vertical), Right(horizontal), Bottom(vertical)
	{
	}

	FORCE_INLINE FudgetPadding(float padding) : Left(padding), Top(padding), Right(padding), Bottom(padding)
	{
	}

	FORCE_INLINE FudgetPadding(const FudgetPadding &other) : Left(other.Left), Top(other.Top), Right(other.Right), Bottom(other.Bottom)
	{
	}

	FORCE_INLINE FudgetPadding(FudgetPadding &&other) noexcept : Left(other.Left), Top(other.Top), Right(other.Right), Bottom(other.Bottom)
	{
	}

	FORCE_INLINE FudgetPadding& operator=(const FudgetPadding &other)
	{
		Left = other.Left;
		Top = other.Top;
		Right = other.Right;
		Bottom = other.Bottom;
		return *this;
	}

	FORCE_INLINE FudgetPadding& operator=(FudgetPadding &&other) noexcept
	{
		Left = other.Left;
		Top = other.Top;
		Right = other.Right;
		Bottom = other.Bottom;
		return *this;
	}

	/// <summary>
	/// Converts padding to a Float4 with left, right, top, bottom values in this order, which is the order used in some
	/// built-in functions that use Float4 for sides.
	/// </summary>
	FORCE_INLINE Float4 AsFloat4() const { return Float4(Left, Right, Top, Bottom); }

	/// <summary>
	/// Returns a rectangle that results in padding the passed value
	/// </summary>
	/// <param name="rect">Rectangle to pad</param>
	FORCE_INLINE Rectangle Padded(const Rectangle &rect) const
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
	/// Returns a padding with each side individually set to the bigger value.
	/// </summary>
	FORCE_INLINE static FudgetPadding Max(const FudgetPadding &a, float value)
	{
		return FudgetPadding(Math::Max(a.Left, value), Math::Max(a.Top, value), Math::Max(a.Right, value), Math::Max(a.Bottom, value));
	}

	/// <summary>
	/// Returns a padding with each side individually set to the bigger value.
	/// </summary>
	FORCE_INLINE static FudgetPadding Max(const FudgetPadding &a, const FudgetPadding &b)
	{
		return FudgetPadding(Math::Max(a.Left, b.Left), Math::Max(a.Top, b.Top), Math::Max(a.Right, b.Right), Math::Max(a.Bottom, b.Bottom));
	}

	/// <summary>
	/// Returns a padding with each side individually set to the lower value.
	/// </summary>
	FORCE_INLINE static FudgetPadding Min(const FudgetPadding &a, float value)
	{
		return FudgetPadding(Math::Min(a.Left, value), Math::Min(a.Top, value), Math::Min(a.Right, value), Math::Min(a.Bottom, value));
	}

	/// <summary>
	/// Returns a padding with each side individually set to the lower value.
	/// </summary>
	FORCE_INLINE static FudgetPadding Min(const FudgetPadding &a, const FudgetPadding &b)
	{
		return FudgetPadding(Math::Min(a.Left, b.Left), Math::Min(a.Top, b.Top), Math::Min(a.Right, b.Right), Math::Min(a.Bottom, b.Bottom));
	}

	/// <summary>
	/// Padding to the left
	/// </summary>
	API_FIELD() float Left;
	/// <summary>
	/// Padding to the top
	/// </summary>
	API_FIELD() float Top;
	/// <summary>
	/// Padding to the right
	/// </summary>
	API_FIELD() float Right;
	/// <summary>
	/// Padding to the bottom
	/// </summary>
	API_FIELD() float Bottom;
};


