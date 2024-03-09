#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Math/Vector4.h"
#include "Engine/Core/Math/Rectangle.h"
#include "Engine/Core/ISerializable.h"


/// <summary>
/// Padding inside any element that contains others, like a slot in a layout. Each side is represented by an int.
/// </summary>
API_STRUCT(NoDefault)
struct FUDGETS_API FudgetPadding : public ISerializable
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetPadding)
	API_AUTO_SERIALIZATION();

	FORCE_INLINE FudgetPadding() : Left(0), Top(0), Right(0), Bottom(0)
	{
	}

	FORCE_INLINE FudgetPadding(int left, int top, int right, int bottom) : Left(left), Top(top), Right(right), Bottom(bottom)
	{
	}

	FORCE_INLINE FudgetPadding(int horizontal, int vertical) : Left(horizontal), Top(vertical), Right(horizontal), Bottom(vertical)
	{
	}

	FORCE_INLINE FudgetPadding(int padding) : Left(padding), Top(padding), Right(padding), Bottom(padding)
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

	FORCE_INLINE bool operator==(const FudgetPadding &other) const
	{
		return Left == other.Left &&
			Top == other.Top &&
			Right == other.Right &&
			Bottom == other.Bottom;
	}

	FORCE_INLINE bool operator!=(const FudgetPadding &other) const
	{
		return Left != other.Left ||
			Top != other.Top ||
			Right != other.Right ||
			Bottom != other.Bottom;
	}

	/// <summary>
	/// Converts padding to a Int4 with left, right, top, bottom values in this order, which is the order used in some
	/// built-in functions that use Int4 for sides.
	/// </summary>
	FORCE_INLINE Int4 AsInt4() const { return Int4(Left, Right, Top, Bottom); }

	/// <summary>
	/// Returns a rectangle that results in padding the passed value
	/// </summary>
	/// <param name="rect">Rectangle to pad</param>
	FORCE_INLINE Rectangle Padded(const Rectangle &rect) const
	{
		return Rectangle(rect.Location + Float2(float(Left), float(Top)), rect.Size - Float2(float(Width()), float(Height())));
	}

	/// <summary>
	/// Left and right padding added together.
	/// </summary>
	FORCE_INLINE int Width() const { return Left + Right; }
	/// <summary>
	/// Top and bottom padding added together.
	/// </summary>
	FORCE_INLINE int Height() const { return Top + Bottom; }

	/// <summary>
	/// Horizontal and vertical padding as a size
	/// </summary>
	FORCE_INLINE Int2 Size() const { return Int2(Left + Right, Top + Bottom);  }

	/// <summary>
	/// Padding to the upper left
	/// </summary>
	FORCE_INLINE Int2 TopLeft() const { return Int2(Left, Top);  }
	/// <summary>
	/// Padding to the upper left
	/// </summary>
	FORCE_INLINE Int2 UpperLeft() const { return Int2(Left, Top); }
	/// <summary>
	/// Padding to the lower right
	/// </summary>
	FORCE_INLINE Int2 BottomRight() const { return Int2(Right, Bottom); }
	/// <summary>
	/// Padding to the lower right
	/// </summary>
	FORCE_INLINE Int2 LowerRight() const { return Int2(Right, Bottom); }

	/// <summary>
	/// Returns a padding with each side individually set to the bigger value.
	/// </summary>
	FORCE_INLINE static FudgetPadding Max(const FudgetPadding &a, int value)
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
	FORCE_INLINE static FudgetPadding Min(const FudgetPadding &a, int value)
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
	API_FIELD() int Left;
	/// <summary>
	/// Padding to the top
	/// </summary>
	API_FIELD() int Top;
	/// <summary>
	/// Padding to the right
	/// </summary>
	API_FIELD() int Right;
	/// <summary>
	/// Padding to the bottom
	/// </summary>
	API_FIELD() int Bottom;
};

template<>
struct TIsPODType<FudgetPadding>
{
	enum { Value = true };
};


/// <summary>
/// Border widths of any element. Each side is represented by a float.
/// </summary>
API_STRUCT(NoDefault)
struct FUDGETS_API FudgetBorder : public ISerializable
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetBorder)
	API_AUTO_SERIALIZATION();

	FORCE_INLINE FudgetBorder() : Left(0), Top(0), Right(0), Bottom(0)
	{
	}

	FORCE_INLINE FudgetBorder(float left, float top, float right, float bottom) : Left(left), Top(top), Right(right), Bottom(bottom)
	{
	}

	FORCE_INLINE FudgetBorder(float horizontal, float vertical) : Left(horizontal), Top(vertical), Right(horizontal), Bottom(vertical)
	{
	}

	FORCE_INLINE FudgetBorder(float padding) : Left(padding), Top(padding), Right(padding), Bottom(padding)
	{
	}

	FORCE_INLINE FudgetBorder(const FudgetBorder &other) : Left(other.Left), Top(other.Top), Right(other.Right), Bottom(other.Bottom)
	{
	}

	FORCE_INLINE FudgetBorder(FudgetBorder &&other) noexcept : Left(other.Left), Top(other.Top), Right(other.Right), Bottom(other.Bottom)
	{
	}

	FORCE_INLINE FudgetBorder(const FudgetPadding &other) : Left((float)other.Left), Top((float)other.Top), Right((float)other.Right), Bottom((float)other.Bottom)
	{
	}

	FORCE_INLINE FudgetBorder& operator=(const FudgetBorder &other)
	{
		Left = other.Left;
		Top = other.Top;
		Right = other.Right;
		Bottom = other.Bottom;
		return *this;
	}

	FORCE_INLINE FudgetBorder& operator=(FudgetBorder &&other) noexcept
	{
		Left = other.Left;
		Top = other.Top;
		Right = other.Right;
		Bottom = other.Bottom;
		return *this;
	}

	FORCE_INLINE FudgetBorder& operator=(const FudgetPadding &other)
	{
		Left = (float)other.Left;
		Top = (float)other.Top;
		Right = (float)other.Right;
		Bottom = (float)other.Bottom;
		return *this;
	}

	FORCE_INLINE bool operator==(const FudgetBorder &other) const
	{
		return Left == other.Left &&
			Top == other.Top &&
			Right == other.Right &&
			Bottom == other.Bottom;
	}

	FORCE_INLINE bool operator!=(const FudgetBorder &other) const
	{
		return Left != other.Left ||
			Top != other.Top ||
			Right != other.Right ||
			Bottom != other.Bottom;
	}

	/// <summary>
	/// Converts padding to a Float4 with left, right, top, bottom values in this order, which is the order used in some
	/// built-in functions that use Float4 for sides.
	/// </summary>
	FORCE_INLINE Float4 AsFloat4() const { return Float4(Left, Right, Top, Bottom); }

	/// <summary>
	/// Converts padding to a FudgetPadding.
	/// </summary>
	FORCE_INLINE FudgetPadding AsPadding() const { return FudgetPadding((int)Left, (int)Top, (int)Right, (int)Bottom); }

	///// <summary>
	///// Returns a rectangle that results in padding the passed value
	///// </summary>
	///// <param name="rect">Rectangle to pad</param>
	//FORCE_INLINE Rectangle Padded(const Rectangle &rect) const
	//{
	//	return Rectangle(rect.Location + Float2(float(Left), float(Top)), rect.Size - Float2(float(Width()), float(Height())));
	//}

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
	FORCE_INLINE Float2 Size() const { return Float2(Left + Right, Top + Bottom); }

	/// <summary>
	/// Border to the upper left
	/// </summary>
	FORCE_INLINE Float2 TopLeft() const { return Float2(Left, Top); }
	/// <summary>
	/// Border to the upper left
	/// </summary>
	FORCE_INLINE Float2 UpperLeft() const { return Float2(Left, Top); }
	/// <summary>
	/// Border to the lower right
	/// </summary>
	FORCE_INLINE Float2 BottomRight() const { return Float2(Right, Bottom); }
	/// <summary>
	/// Border to the lower right
	/// </summary>
	FORCE_INLINE Float2 LowerRight() const { return Float2(Right, Bottom); }

	/// <summary>
	/// Returns a padding with each side individually set to the bigger value.
	/// </summary>
	FORCE_INLINE static FudgetBorder Max(const FudgetBorder &a, float value)
	{
		return FudgetBorder(Math::Max(a.Left, value), Math::Max(a.Top, value), Math::Max(a.Right, value), Math::Max(a.Bottom, value));
	}

	/// <summary>
	/// Returns a padding with each side individually set to the bigger value.
	/// </summary>
	FORCE_INLINE static FudgetBorder Max(const FudgetBorder &a, const FudgetBorder &b)
	{
		return FudgetBorder(Math::Max(a.Left, b.Left), Math::Max(a.Top, b.Top), Math::Max(a.Right, b.Right), Math::Max(a.Bottom, b.Bottom));
	}

	/// <summary>
	/// Returns a padding with each side individually set to the lower value.
	/// </summary>
	FORCE_INLINE static FudgetBorder Min(const FudgetBorder &a, float value)
	{
		return FudgetBorder(Math::Min(a.Left, value), Math::Min(a.Top, value), Math::Min(a.Right, value), Math::Min(a.Bottom, value));
	}

	/// <summary>
	/// Returns a padding with each side individually set to the lower value.
	/// </summary>
	FORCE_INLINE static FudgetBorder Min(const FudgetBorder &a, const FudgetBorder &b)
	{
		return FudgetBorder(Math::Min(a.Left, b.Left), Math::Min(a.Top, b.Top), Math::Min(a.Right, b.Right), Math::Min(a.Bottom, b.Bottom));
	}

	/// <summary>
	/// Border to the left
	/// </summary>
	API_FIELD() float Left;
	/// <summary>
	/// Border to the top
	/// </summary>
	API_FIELD() float Top;
	/// <summary>
	/// Border to the right
	/// </summary>
	API_FIELD() float Right;
	/// <summary>
	/// Border to the bottom
	/// </summary>
	API_FIELD() float Bottom;
};

template<>
struct TIsPODType<FudgetBorder>
{
	enum { Value = true };
};


