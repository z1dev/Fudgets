using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Fudgets
{
    public partial class FudgetControl
    {
        public T CreateStylePainter<T>(FudgetToken token) where T : FudgetPartPainter
        {
            FudgetStyle style = ActiveStyle;
            if (style == null)
                return null;

            T painter = style.CreatePainter<T>(ActiveTheme, token);

            if (painter != null)
                RegisterStylePainterInternal(painter);

            return painter;
        }
    }
}
