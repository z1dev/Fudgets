using FlaxEngine;
using System.Collections.Generic;

namespace Fudgets
{
    /// <inheritdoc />
    partial class FudgetThemes
    {
        private static List<FudgetStyle> registeredStyles = [];

        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        public static FudgetStyle CreateStyle(string name)
        {
            return CreateStyle(RegisterToken(name));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="style_token"></param>
        /// <returns></returns>
        public static FudgetStyle CreateStyle(FudgetToken style_token)
        {
            if (GetStyle(style_token) != null)
                return null;
            FudgetStyle style = Internal_CreateStyle(ref style_token);

            registeredStyles.Add(style);
            return style;
        }

        /// <summary>
        /// Removes references to managed objects registered with the global themes database.
        /// </summary>
        public static void UninitializeManaged()
        {
            registeredStyles.Clear();
        }
    }

}