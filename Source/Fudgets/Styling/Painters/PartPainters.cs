using FlaxEngine.Utilities;
using System;
using System.Numerics;


namespace Fudgets;



public partial class FudgetPartPainter
{
    /// <summary>
    /// Initializes a FudgetPartPainterMapping structure, setting the PainterType value to the full name of the generic type.
    /// The type has to be a derived class of FudgetPartPainter. The resourceMapping value is stored as the mapping in the
    /// structure. It must be the exact type required by the specific painter's initialization which is usually called the
    /// same as the painter and ending with "Resources".
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="resourceMapping"></param>
    /// <returns></returns>
    public static FudgetPartPainterMapping InitializeMapping<T>(object resourceMapping) where T : FudgetPartPainter, new()
    {
        FudgetPartPainterMapping result;
        result.PainterType = typeof(T).FullName;
        result.ResourceMapping = resourceMapping;
        return result;
    }

    /// <summary>
    /// Checks style_override then the control's styles for an enum resource. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <typeparam name="E">The type of the enum, which must match with the resource.</typeparam>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type.</returns>
    public bool GetMappedEnum<E>(FudgetControl control, FudgetStyle style_override, int painter_id, int mapping_id, out E result) where E: Enum
    {
        FudgetTheme theme = control.ActiveTheme;
        result = default;
        return control.GetStyleEnum<E>(mapping_id, out result);
    }

    /// <summary>
    /// Shortcut to check if the flags in states contains the flags from to_check.
    /// </summary>
    /// <typeparam name="T">A type that supports binary operators like &amp;</typeparam>
    /// <param name="states">Value to check for flags</param>
    /// <param name="to_check">Flags to look for in the value</param>
    /// <returns>Whether the states flags contain the to_check flag(s) or not</returns>
    public bool HasState<T>(T states, T to_check) where T : IBinaryNumber<T>
    {
        return (states & to_check) == to_check;
    }
}
