
using FlaxEngine;

namespace Fudgets;

public partial struct FudgetPadding
{
    /// <summary>
    /// Initializes a new padding structure and sets its sizes
    /// </summary>
    public FudgetPadding(float left, float top, float right, float bottom)
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
    public FudgetPadding(float horizontal, float vertical)
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
    public FudgetPadding(float value)
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
    /// Padding to the upper left
    /// </summary>
    public readonly Float2 TopLeft => new Float2(Left, Top);
    /// <summary>
    /// Padding to the upper left
    /// </summary>
    public readonly Float2 UpperLeft => new Float2(Left, Top);

    /// <summary>
    /// Padding to the lower right
    /// </summary>
    public readonly Float2 LowerRight => new Float2(Right, Bottom);
    /// <summary>
    /// Padding to the lower right
    /// </summary>
    public readonly Float2 BottomRight => new Float2(Right, Bottom);

    /// <summary>
    /// Converts padding to a Float4 with left, right, top, bottom values in this order.
    /// </summary>
    public readonly Float4 AsFloat4 => new Float4(Left, Right, Top, Bottom);

    /// <summary>
    /// Sum of the corresponding sides' padding.
    /// </summary>
    public static FudgetPadding operator+(FudgetPadding a, FudgetPadding b) => new FudgetPadding(a.Left + b.Left, a.Top + b.Top, a.Right + b.Right, a.Bottom + b.Bottom);
}