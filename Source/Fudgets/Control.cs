using System;
using System.Transactions;


namespace Fudgets
{
    public partial class FudgetControl
    {

        internal string StyleName
        {
            get => GetStyleName();
            set => SetStyleName(value);
        }


        /// <summary>
        /// Constructs a painter object based on a painter mapping identified by mapping_id and initializes it with the mapping. Mappings contain the name of
        /// the painter's type to be created and the mapping of painter ids to the ids of a style. That is, when the painter tries to look up a resource by an
        /// id, another id will be used to get the resource.
        /// The mapping_id is looked up in the control's styles.
        /// When a new painter is created in place of an existing one, the existing painter can be passed as the first argument to this function. Alternatively
        /// if the painter is no longer needed, it should be deleted with DeleteStylePainter. Painters not deleted this way will crash the game. All painters
        /// created for the control are automatically deleted when the control is deleted.
        /// </summary>
        /// <typeparam name="T">Base type for the painter. The new painter must be this type or a type derived from it.</typeparam>
        /// <param name="current">The painter that was created before, possibly on the same line. It's only needed to make sure it's freed before a
        /// new painter is created.</param>
        /// <param name="mapping_id">Id of the painter mapping in the control's style.</param>
        /// <returns>The part painter that was created or null if no painter mapping matches the mapping id or the template argument.</returns>
        public T CreateStylePainter<T>(T current, int mapping_id) where T : FudgetPartPainter
        {
            FudgetPartPainterMapping painter_data = default;
            FudgetPartPainter painter = null;

            if (current != null)
                Destroy(current);

            bool valid = GetStylePainterMapping(mapping_id, out painter_data);

            if (!valid)
                return null;

            painter = FudgetThemes.CreatePainter(painter_data.PainterType);

            if (painter != null && painter is not T)
            {
                Destroy(painter);
                painter = null;
            }
            T result = painter as T;
            if (result != null)
                RegisterStylePainterInternal(result, ref painter_data);

            return result;
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
            if (TryGetStyleEnumInner<T>(Style, id, false, out result))
                return true;
            return TryGetStyleEnumInner<T>(ClassStyle, id, true, out result);
        }


        private bool TryGetStyleEnumInner<T>(FudgetStyle style, int id, bool check_theme, out T result) where T : Enum
        {
            if (style == null)
            {
                result = default;
                return false;
            }
            return FudgetStyle.GetEnumResource<T>(style, ActiveTheme, id, check_theme, out result);
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
