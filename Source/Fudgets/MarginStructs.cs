
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
    /// Left and right padding.
    /// </summary>
    public readonly float Width => Left + Right;
    /// <summary>
    /// Top and bottom padding.
    /// </summary>
    public readonly float Height => Top + Bottom;
    /// <summary>
    /// Converts padding to a Float4 with left, right, top, bottom values in this order.
    /// </summary>
    public readonly Float4 AsFloat4 => new Float4(Left, Right, Top, Bottom);
}