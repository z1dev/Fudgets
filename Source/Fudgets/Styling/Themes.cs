using FlaxEngine;
using System.Collections.Generic;

namespace Fudgets
{
    /// <inheritdoc />
    partial class FudgetThemes
    {
        private static List<FudgetElementPainter> registeredPainters = [];
        private static List<FudgetStyle> registeredStyles = [];

        /// <summary>
        /// Creates a new element painter with a token that styles and other painters can use. This token needs to be unique to
        /// painters. It can be registered with any theme via their PainterIds dictionaries.
        /// This is equivalent to creating the painter directly with a constructor and keeping a reference to it.
        /// </summary>
        /// <param name="name">Name for the token of the painter</param>
        /// <returns>Whether the element painter was successfully created</returns>
        public static bool CreateElementPainter<T>(string name) where T : FudgetElementPainter, new()
        {
            return CreateElementPainter<T>(RegisterToken(name));
        }

        /// <summary>
        /// Creates a new element painter with a token that styles and other painters can use. This token needs to be unique to
        /// painters. It can be registered with any theme via their PainterIds dictionaries.
        /// This is equivalent to creating the painter directly with a constructor and keeping a reference to it.
        /// </summary>
        /// <param name="painter_token">Token of the painter</param>
        /// <returns>Whether the element painter was successfully created</returns>
        public static bool CreateElementPainter<T>(FudgetToken painter_token) where T : FudgetElementPainter, new()
        {
            if (typeof(T).IsAbstract || GetElementPainter(painter_token) != null)
                return false;

            T painter = new T();
            RegisterElementPainter(painter_token, painter);
            registeredPainters.Add(painter);
            return true;
        }

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
            registeredPainters.Clear();
            registeredStyles.Clear();
        }
    }

}