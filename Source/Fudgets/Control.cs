using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Fudgets
{
    public partial class FudgetControl
    {
        /// <summary>
        /// Constructs a painter object based on a type and the style of the control
        /// </summary>
        /// <typeparam name="T">Base type of the painter</typeparam>
        /// <param name="token">Associated token in the control's style</param>
        /// <param name="style_token">Token in the control's style that refers to a style to be used with the painter</param>
        /// <returns>The created painter, or null if the token is not matching a painter or the painter is not derived from the template argument</returns>
        public T CreateStylePainter<T>(FudgetToken token, FudgetToken style_token) where T : FudgetPartPainter
        {
            FudgetStyle style = ActiveStyle;
            if (style == null)
                return null;

            T painter = style.CreatePainter<T>(ActiveTheme, token);

            if (painter != null)
                RegisterStylePainterInternal(painter, style_token);

            return painter;
        }

        /// <summary>
        /// Returns an enum value for the control based on a theme token.
        /// The resulting value depends on both the active style and the theme currently set for this control.
        /// </summary>
        /// <typeparam name="T">The enum type</typeparam>
        /// <param name="token">Token associated with the int in the active style</param>
        /// <param name="result">Variable that receives the result</param>
        /// <returns>Whether a valid value was found for the token</returns>
        public bool GetStyleEnum<T>(FudgetToken token, out T result) where T: Enum
        {
            var style = ActiveStyle;
            if (style == null)
            {
                result = default;
                return false;
            }
            return style.GetEnumResource<T>(ActiveTheme, token, out result);
        }

        /// <summary>
        /// Returns an enum value for the control based on a theme token.
        /// The resulting value depends on both the active style and the theme currently set for this control.
        /// This version of the function accepts an array, and returns the value for the first token found.
        /// </summary>
        /// <typeparam name="T">The enum type</typeparam>
        /// <param name="tokens">An array of tokens that are checked in order</param>
        /// <param name="result">Variable that receives the result</param>
        /// <returns>Whether a valid value was found for a token</returns>
        public bool GetStyleEnum<T>(FudgetToken[] tokens, out T result) where T : Enum
        {
            foreach (var item in tokens)
            {
                if (GetStyleEnum<T>(item, out result))
                    return true;
            }
            result = default;
            return false;
        }
    }
}
