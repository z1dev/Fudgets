﻿
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
    public float Width => Left + Right;
    /// <summary>
    /// Top and bottom padding.
    /// </summary>
    public float Height => Top + Bottom;
}