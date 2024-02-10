using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Fudgets
{
    public partial class FudgetStyle
    {
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
    }
}
