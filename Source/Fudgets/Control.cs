using System;


namespace Fudgets
{
    public partial class FudgetControl
    {
        /// <summary>
        /// Constructs a painter object based on a type and the style of the control.
        /// </summary>
        /// <typeparam name="T">Base type of the painter</typeparam>
        /// <param name="painter_id">Id of the painter in the control's style</param>
        /// <returns>The created painter, or null if the id is not matching a painter or the painter is not derived from the template argument</returns>
        public T CreateStylePainter<T>(int painter_id) where T : FudgetPartPainter, new()
        {
            return CreateStylePainter<T,T>(painter_id);
        }

        /// <summary>
        /// Constructs a painter object based on a type and the style of the control.
        /// </summary>
        /// <typeparam name="T">Base type of the painter</typeparam>
        /// <typeparam name="DEF">Type of the painter if the id does not correspond to a painter's type name in the theme</typeparam>
        /// <param name="painter_id">Id of the painter in the control's style</param>
        /// <returns>The created painter, or null if the id is not matching a painter or the painter is not derived from the template argument</returns>
        public T CreateStylePainter<T, DEF>(int painter_id) where T : FudgetPartPainter where DEF : FudgetPartPainter, new()
        {
            FudgetStyle style = ActiveStyle;
            T painter = null;
            if (style != null)
                painter = style.CreatePainter<T>(ActiveTheme, painter_id);

            if (typeof(T).IsAssignableFrom(typeof(DEF)))
                painter = new DEF() as T;

            if (painter != null)
                RegisterStylePainterInternal(painter);

            return painter;
        }

        /// <summary>
        /// Returns an enum value for the control based on a theme id.
        /// The resulting value depends on both the active style and the theme currently set for this control.
        /// </summary>
        /// <typeparam name="T">The enum type</typeparam>
        /// <param name="id">Id associated with the enum in the active style</param>
        /// <param name="result">Variable that receives the result</param>
        /// <returns>Whether a valid value was found for the id</returns>
        public bool GetStyleEnum<T>(int id, out T result) where T: Enum
        {
            var style = ActiveStyle;
            if (style == null)
            {
                result = default;
                return false;
            }
            return style.GetEnumResource<T>(ActiveTheme, id, out result);
        }

        /// <summary>
        /// Returns an enum value for the control based on a theme id.
        /// The resulting value depends on both the active style and the theme currently set for this control.
        /// This version of the function accepts an array, and returns the value for the first id found.
        /// </summary>
        /// <typeparam name="T">The enum type</typeparam>
        /// <param name="ids">An array of ids that are checked in order</param>
        /// <param name="result">Variable that receives the result</param>
        /// <returns>Whether a valid value was found for one of the ids</returns>
        public bool GetStyleEnum<T>(int[] ids, out T result) where T : Enum
        {
            foreach (var id in ids)
            {
                if (GetStyleEnum<T>(id, out result))
                    return true;
            }
            result = default;
            return false;
        }
    }
}
