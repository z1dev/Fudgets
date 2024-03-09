using FlaxEngine;

namespace Fudgets;

public partial struct FudgetPadding
{
    /// <summary>
    /// Initializes a new padding structure and sets its sizes
    /// </summary>
    public FudgetPadding(int left, int top, int right, int bottom)
    {
        Left = left;
        Top = top;
        Right = right;
        Bottom = bottom;
    }

    /// <summary>
    /// Initializes a new padding structure and sets its sizes
    /// </summary>
    /// <param name="horizontal">Set for left and right</param>
    /// <param name="vertical">Set for top and bottom</param>
    public FudgetPadding(int horizontal, int vertical)
    {
        Left = horizontal;
        Top = vertical;
        Right = horizontal;
        Bottom = vertical;
    }

    /// <summary>
    /// Initializes a new padding structure and sets its sizes
    /// </summary>
    /// <param name="value">Same value for each side</param>
    public FudgetPadding(int value)
    {
        Left = value;
        Top = value;
        Right = value;
        Bottom = value;
    }

    /// <summary>
    /// Returns a rectangle that results in padding the passed value
    /// </summary>
    /// <param name="rect">Rectangle to pad</param>
    public Rectangle Padded(in Rectangle rect)
    {
        return new Rectangle(rect.Location + new Float2(Left, Top), rect.Size - new Float2(Width, Height) );
    }

    /// <summary>
    /// Returns a padding with each side individually set to the bigger value.
    /// </summary>
    static FudgetPadding Max(in FudgetPadding a, int value)
    {
        return new FudgetPadding(Mathf.Max(a.Left, value), Mathf.Max(a.Top, value), Mathf.Max(a.Right, value), Mathf.Max(a.Bottom, value));
    }

    /// <summary>
    /// Returns a padding with each side individually set to the bigger value.
    /// </summary>
    static FudgetPadding Max(in FudgetPadding a, in FudgetPadding b)
    {
        return new FudgetPadding(Mathf.Max(a.Left, b.Left), Mathf.Max(a.Top, b.Top), Mathf.Max(a.Right, b.Right), Mathf.Max(a.Bottom, b.Bottom));
    }

    /// <summary>
	/// Returns a padding with each side individually set to the lower value.
	/// </summary>
    static FudgetPadding Min(in FudgetPadding a, int value)
    {
        return new FudgetPadding(Mathf.Min(a.Left, value), Mathf.Min(a.Top, value), Mathf.Min(a.Right, value), Mathf.Min(a.Bottom, value));
    }

    /// <summary>
	/// Returns a padding with each side individually set to the lower value.
	/// </summary>
    static FudgetPadding Min(in FudgetPadding a, in FudgetPadding b)
    {
        return new FudgetPadding(Mathf.Min(a.Left, b.Left), Mathf.Min(a.Top, b.Top), Mathf.Min(a.Right, b.Right), Mathf.Min(a.Bottom, b.Bottom));
    }

    /// <summary>
    /// Left and right padding added together.
    /// </summary>
    public readonly int Width => Left + Right;
    /// <summary>
    /// Top and bottom padding added together.
    /// </summary>
    public readonly int Height => Top + Bottom;

    /// <summary>
    /// Horizontal and vertical padding as a size
    /// </summary>
    public readonly Int2 Size => new Int2(Left + Right, Top + Bottom);

    /// <summary>
    /// Padding to the upper left
    /// </summary>
    public readonly Int2 TopLeft => new Int2(Left, Top);
    /// <summary>
    /// Padding to the upper left
    /// </summary>
    public readonly Int2 UpperLeft => new Int2(Left, Top);

    /// <summary>
    /// Padding to the lower right
    /// </summary>
    public readonly Int2 LowerRight => new Int2(Right, Bottom);
    /// <summary>
    /// Padding to the lower right
    /// </summary>
    public readonly Int2 BottomRight => new Int2(Right, Bottom);

    /// <summary>
    /// Converts padding to a Int4 with left, right, top, bottom values in this order.
    /// </summary>
    public readonly Int4 AsInt4 => new Int4(Left, Right, Top, Bottom);

    /// <summary>
    /// Converts padding to a border
    /// </summary>
    public readonly FudgetBorder AsBorder => new FudgetBorder((float)Left, (float)Top, (float)Right, (float)Bottom);

    /// <summary>
    /// Sum of the corresponding sides' padding.
    /// </summary>
    public static FudgetPadding operator+(FudgetPadding a, FudgetPadding b) => new FudgetPadding(a.Left + b.Left, a.Top + b.Top, a.Right + b.Right, a.Bottom + b.Bottom);

    /// <summary>
    /// Compares two padding structures for equality of each side
    /// </summary>
    public static bool operator==(FudgetPadding a, FudgetPadding b) => Mathf.NearEqual(a.Left, b.Left) && Mathf.NearEqual(a.Right, b.Right) && Mathf.NearEqual(a.Top, b.Top) && Mathf.NearEqual(a.Bottom, b.Bottom);

    /// <summary>
    /// Compares two padding structures for unequality of any side
    /// </summary>
    public static bool operator!=(FudgetPadding a, FudgetPadding b) => !Mathf.NearEqual(a.Left, b.Left) || !Mathf.NearEqual(a.Right, b.Right) || !Mathf.NearEqual(a.Top, b.Top) || !Mathf.NearEqual(a.Bottom, b.Bottom);
}

public partial struct FudgetBorder
{
    /// <summary>
    /// Initializes a new padding structure and sets its sizes
    /// </summary>
    public FudgetBorder(float left, float top, float right, float bottom)
    {
        Left = left;
        Top = top;
        Right = right;
        Bottom = bottom;
    }

    /// <summary>
    /// Initializes a new padding structure and sets its sizes
    /// </summary>
    /// <param name="horizontal">Set for left and right</param>
    /// <param name="vertical">Set for top and bottom</param>
    public FudgetBorder(float horizontal, float vertical)
    {
        Left = horizontal;
        Top = vertical;
        Right = horizontal;
        Bottom = vertical;
    }

    /// <summary>
    /// Initializes a new padding structure and sets its sizes
    /// </summary>
    /// <param name="value">Same value for each side</param>
    public FudgetBorder(float value)
    {
        Left = value;
        Top = value;
        Right = value;
        Bottom = value;
    }

    /// <summary>
    /// Returns a rectangle that results in padding the passed value
    /// </summary>
    /// <param name="rect">Rectangle to pad</param>
    public Rectangle Padded(in Rectangle rect)
    {
        return new Rectangle(rect.Location + new Float2(Left, Top), rect.Size - new Float2(Width, Height));
    }

    /// <summary>
    /// Returns a padding with each side individually set to the bigger value.
    /// </summary>
    static FudgetBorder Max(in FudgetBorder a, float value)
    {
        return new FudgetBorder(Mathf.Max(a.Left, value), Mathf.Max(a.Top, value), Mathf.Max(a.Right, value), Mathf.Max(a.Bottom, value));
    }

    /// <summary>
    /// Returns a padding with each side individually set to the bigger value.
    /// </summary>
    static FudgetBorder Max(in FudgetBorder a, in FudgetBorder b)
    {
        return new FudgetBorder(Mathf.Max(a.Left, b.Left), Mathf.Max(a.Top, b.Top), Mathf.Max(a.Right, b.Right), Mathf.Max(a.Bottom, b.Bottom));
    }

    /// <summary>
	/// Returns a padding with each side individually set to the lower value.
	/// </summary>
    static FudgetBorder Min(in FudgetBorder a, float value)
    {
        return new FudgetBorder(Mathf.Min(a.Left, value), Mathf.Min(a.Top, value), Mathf.Min(a.Right, value), Mathf.Min(a.Bottom, value));
    }

    /// <summary>
	/// Returns a padding with each side individually set to the lower value.
	/// </summary>
    static FudgetBorder Min(in FudgetBorder a, in FudgetBorder b)
    {
        return new FudgetBorder(Mathf.Min(a.Left, b.Left), Mathf.Min(a.Top, b.Top), Mathf.Min(a.Right, b.Right), Mathf.Min(a.Bottom, b.Bottom));
    }

    /// <summary>
    /// Left and right padding added together.
    /// </summary>
    public readonly float Width => Left + Right;
    /// <summary>
    /// Top and bottom padding added together.
    /// </summary>
    public readonly float Height => Top + Bottom;

    /// <summary>
    /// Horizontal and vertical padding as a size
    /// </summary>
    public readonly Float2 Size => new Float2(Left + Right, Top + Bottom);

    /// <summary>
    /// Border to the upper left
    /// </summary>
    public readonly Float2 TopLeft => new Float2(Left, Top);
    /// <summary>
    /// Border to the upper left
    /// </summary>
    public readonly Float2 UpperLeft => new Float2(Left, Top);

    /// <summary>
    /// Border to the lower right
    /// </summary>
    public readonly Float2 LowerRight => new Float2(Right, Bottom);
    /// <summary>
    /// Border to the lower right
    /// </summary>
    public readonly Float2 BottomRight => new Float2(Right, Bottom);

    /// <summary>
    /// Converts padding to a Float4 with left, right, top, bottom values in this order.
    /// </summary>
    public readonly Float4 AsFloat4 => new Float4(Left, Right, Top, Bottom);

    /// <summary>
    /// Converts the borders to FudgetPadding.
    /// </summary>
    public readonly FudgetPadding AsPadding => new FudgetPadding((int)Left, (int)Top, (int)Right, (int)Bottom);

    /// <summary>
    /// Sum of the corresponding sides' padding.
    /// </summary>
    public static FudgetBorder operator +(FudgetBorder a, FudgetBorder b) => new FudgetBorder(a.Left + b.Left, a.Top + b.Top, a.Right + b.Right, a.Bottom + b.Bottom);

    /// <summary>
    /// Compares two padding structures for equality of each side
    /// </summary>
    public static bool operator ==(FudgetBorder a, FudgetBorder b) => Mathf.NearEqual(a.Left, b.Left) && Mathf.NearEqual(a.Right, b.Right) && Mathf.NearEqual(a.Top, b.Top) && Mathf.NearEqual(a.Bottom, b.Bottom);

    /// <summary>
    /// Compares two padding structures for unequality of any side
    /// </summary>
    public static bool operator !=(FudgetBorder a, FudgetBorder b) => !Mathf.NearEqual(a.Left, b.Left) || !Mathf.NearEqual(a.Right, b.Right) || !Mathf.NearEqual(a.Top, b.Top) || !Mathf.NearEqual(a.Bottom, b.Bottom);
}
