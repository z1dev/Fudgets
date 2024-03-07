﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Fudgets
{
    public partial class FudgetStyle
    {
        /// <summary>
        /// Creates a new style that inherits all its values from this one, named as the template parameter class' full name, or
        /// null if the name is already taken.
        /// </summary>
        /// <typeparam name="T">Class to use its full name as the name of the new style</typeparam>
        /// <returns>The created style or null if style with a clashing name exists</returns>
        public FudgetStyle CreateInheritedStyle<T>() where T: class
        {
            return InheritStyleInternal(typeof(T).FullName, IsOwnedStyle);
        }


        /// <summary>
        /// Looks up an enum resource associated with an id in this style or a parent style using the theme, and sets the result
        /// value to it on success.
        /// </summary>
        /// <typeparam name="T">Type of the enum</typeparam>
        /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
        /// <param name="id">Id that might be associated with the value in this style.</param>
        /// <param name="result">Receives the resource's value if it is found.</param>
        /// <returns>Whether the id was associated with a value of the requested type</returns>
        public bool GetEnumResource<T>(FudgetTheme theme, int id, out T result) where T : Enum
        {
            if (GetResourceValue(theme, id, true, out var res))
            {
                if (res.GetType() == typeof(T))
                {
                    result = (T)res;
                    return true;
                }
            }
            result = default;
            return false;
        }

        /// <summary>
        /// Looks up an enum resource associated with an id in this style or a parent style using the theme, and sets the result
        /// value to it on success.
        /// This version of the function checks multiple ids until one matches.
        /// </summary>
        /// <typeparam name="T">Type of the enum</typeparam>
        /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
        /// <param name="ids">Ids that might be associated with the value in this style.</param>
        /// <param name="result">Receives the resource's value if it is found.</param>
        /// <returns>Whether the token was associated with a value of the requested type</returns>
        public bool GetEnumResource<T>(FudgetTheme theme, int[] ids, out T result) where T : Enum
        {
            foreach (var id in ids)
            {
                if (GetEnumResource<T>(theme, id, out result))
                    return true;
            }
            result = default;
            return false;
        }

        /// <summary>
        /// Sets a resource id override that will be used to retrieve a value from a theme. Calling GetResourceValue with the
        /// id will fetch the resource id override, and will use that to look for the value in the theme. 
        /// </summary>
        /// <typeparam name="T">Enum type with int backing used as the enum id</typeparam>
        /// <param name="enum_value">Id to be used to retrieve a value later. Usually this is the id of a resource in a theme</param>
        /// <param name="resource_id">The resource id that overrides the original</param>
        public unsafe void SetResourceOverride<T>(T enum_value, int resource_id) where T: unmanaged, Enum
        {
            SetResourceOverride(*(int*)(&enum_value), resource_id);
        }

        /// <summary>
        /// Sets a resource id override that will be used to retrieve a value from a theme. Calling GetResourceValue with the
        /// id will fetch the resource id override, and will use that to look for the value in the theme. 
        /// </summary>
        /// <typeparam name="T">Enum type with int backing used as the enum id</typeparam>
        /// <typeparam name="Y">Enum type with int backing used as the resource id</typeparam>
        /// <param name="enum_value">Id to be used to retrieve a value later. Usually this is the id of a resource in a theme</param>
        /// <param name="resource_id">The resource id that overrides the original</param>
        public unsafe void SetResourceOverride<T,Y>(T enum_value, Y resource_id) where T : unmanaged, Enum where Y : unmanaged, Enum
        {
            SetResourceOverride(*(int*)(&enum_value), *(int*)(&resource_id));
        }

        /// <summary>
        /// Sets value as the override for the passed id. Calling GetResourceValue will return this value directly, unless
        /// an inherited style is queried which has a different override for it. 
        /// </summary>
        /// <typeparam name="T">Enum type with int backing used as the enum id</typeparam>
        /// <param name="enum_value">Id to be used to retrieve a value later. Usually this is the id of a resource in a theme</param>
        /// <param name="value">The value to set for the id</param>
        public unsafe void SetValueOverride<T>(T enum_value, object value) where T : unmanaged, Enum
        {
            SetValueOverride(*(int*)(&enum_value), value);
        }
    }
}
