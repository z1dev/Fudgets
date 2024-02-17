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
    }
}
