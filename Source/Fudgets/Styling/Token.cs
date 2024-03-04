using System;
using System.Collections.Generic;
using System.Numerics;
using FlaxEngine;

namespace Fudgets
{
    ///// <summary>
    ///// Token Script.
    ///// </summary>
    //public partial struct FudgetToken
    //{
    //    /// <summary>
    //    /// Checks for token equality. Two tokens are equal if they hold the same int value.
    //    /// </summary>
    //    public static bool operator ==(FudgetToken left, FudgetToken right) => left.Token == right.Token;

    //    /// <summary>
    //    /// Checks for token inequality. Two tokens are not equal if they don't hold the same int value.
    //    /// </summary>
    //    public static bool operator !=(FudgetToken left, FudgetToken right) => left.Token != right.Token;

    //    /// <summary>
    //    /// Greater than operator for the int value in the token.
    //    /// </summary>
    //    public static bool operator >(FudgetToken left, FudgetToken right) => left.Token > right.Token;

    //    /// <summary>
    //    /// Less than operator for the int value in the token.
    //    /// </summary>
    //    public static bool operator <(FudgetToken left, FudgetToken right) => left.Token < right.Token;

    //    /// <summary>
    //    /// Greater or equal operator for the int value in the token.
    //    /// </summary>
    //    public static bool operator >=(FudgetToken left, FudgetToken right) => left.Token >= right.Token;

    //    /// <summary>
    //    /// Less or equal operator for the int value in the token.
    //    /// </summary>
    //    public static bool operator <=(FudgetToken left, FudgetToken right) => left.Token <= right.Token;

    //    /// <summary>
    //    /// Constructs a token from an integer.
    //    /// </summary>
    //    /// <param name="value">The integer that sets the token's value</param>
    //    public static implicit operator FudgetToken(int value) => new FudgetToken(value);

    //    /// <summary>
    //    /// Constructs a token from an integer.
    //    /// </summary>
    //    /// <param name="value">The integer that sets the token's value</param>
    //    public FudgetToken(int value)
    //    {
    //        Token = value;
    //    }

    //    /// <summary>
    //    /// Returns whether the value of the token is registered with the themes
    //    /// </summary>
    //    public readonly bool IsValid => Token != -1;

    //    /// <summary>
    //    /// A token that is null. Null tokens should be used where a valid token is required, but it must indicate
    //    /// that it represents nothing. For example a style resource must be valid. Setting it to invalid would mean
    //    /// that the style doesn't override a value. Instead, null might be used to hide a value of an inherited style.
    //    /// </summary>
    //    public readonly bool IsNull => Token == -2;
    //}
}
