using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Fudgets
{
    public partial class FudgetStyle
    {
        /// <summary>
        /// Creates a painter object associated with a token in the theme. The theme must hold a token as a
        /// resource for this token, which was created from the full name of the painter. The type passed as
        /// the type parameter can be a base class that the to-be-created painter is derived from, or the
        /// result will be null.
        /// </summary>
        /// <typeparam name="T">The base type of the painter to be created</typeparam>
        /// <param name="theme">Theme holding the painter's name token for the passed token</param>
        /// <param name="painter_token">The token to look up in the theme</param>
        /// <returns>The newly created painter object, or null if the token doesn't refer to a valid painter type derived from the requested type</returns>
        public T CreatePainter<T>(FudgetTheme theme, FudgetToken painter_token) where T : FudgetPartPainter
        {
            if (painter_token == FudgetToken.Invalid)
                return null;

            if (!GetTokenResource(theme, painter_token, out var token))
                return null;
            if (token == FudgetToken.Invalid)
                return null;

            FudgetPartPainter painter = FudgetThemes.CreatePainter(token);
            if (painter == null)
                return null;
            if (painter is not T)
            {
                Destroy(painter);
                return null;
            }

            return painter as T;
        }

        /// <summary>
        /// Looks up an enum resource associated with a token in this style or a parent style using the theme, and sets the result
        /// value to it on success.
        /// </summary>
        /// <typeparam name="T">Type of the enum</typeparam>
        /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
        /// <param name="token">Token that might be associated with the value in this style.</param>
        /// <param name="result">Receives the resource's value if it is found.</param>
        /// <returns>Whether the token was associated with a value of the requested type</returns>
        public bool GetEnumResource<T>(FudgetTheme theme, FudgetToken token, out T result) where T : Enum
        {

            if (GetResourceValue(theme, token, out var res))
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
        /// Looks up an enum resource associated with a token in this style or a parent style using the theme, and sets the result
        /// value to it on success.
        /// This version of the function checks multiple tokens until one matches.
        /// </summary>
        /// <typeparam name="T">Type of the enum</typeparam>
        /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
        /// <param name="tokens">Token that might be associated with the value in this style.</param>
        /// <param name="result">Receives the resource's value if it is found.</param>
        /// <returns>Whether the token was associated with a value of the requested type</returns>
        public bool GetEnumResource<T>(FudgetTheme theme, FudgetToken[] tokens, out T result) where T : Enum
        {
            foreach (var item in tokens)
            {
                if (GetEnumResource<T>(theme, item, out result))
                    return true;
            }
            result = default;
            return false;
        }
    }
}
