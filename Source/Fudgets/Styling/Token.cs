using System;
using System.Collections.Generic;
using FlaxEngine;

namespace Fudgets;

/// <summary>
/// Token Script.
/// </summary>
public partial struct FudgetToken
{
    /// <summary>
    /// Checks for token equality. Two tokens are equal if they hold the same int value.
    /// </summary>
    public static bool operator ==(FudgetToken left, FudgetToken right) => left.Token == right.Token;

    /// <summary>
    /// Checks for token inequality. Two tokens are not equal if they don't hold the same int value.
    /// </summary>
    public static bool operator !=(FudgetToken left, FudgetToken right) => left.Token != right.Token;

    /// <summary>
    /// Greater than operator for the int value in the token.
    /// </summary>
    public static bool operator >(FudgetToken left, FudgetToken right) => left.Token > right.Token;

    /// <summary>
    /// Less than operator for the int value in the token.
    /// </summary>
    public static bool operator <(FudgetToken left, FudgetToken right) => left.Token < right.Token;

    /// <summary>
    /// Greater or equal operator for the int value in the token.
    /// </summary>
    public static bool operator >=(FudgetToken left, FudgetToken right) => left.Token >= right.Token;

    /// <summary>
    /// Less or equal operator for the int value in the token.
    /// </summary>
    public static bool operator <=(FudgetToken left, FudgetToken right) => left.Token <= right.Token;


    bool IsValid()
    {
        return Token != -1;
    }
}
