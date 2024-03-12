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
        /// The mapping_id is looked up in the control's styles, unless style_override is not null.
        /// If mapping_style is provided, it'll be used as the primary style to look up the resources for painting. Otherwise the control's styles are used.
        /// If default_mapping is not null and no mapping was found by the mapping_id, the default_mapping is used to provide the painter type name and mapping data.
        /// </summary>
        /// <typeparam name="T">Base type for the painter. The created painter must be the base type or a type derived from it.</typeparam>
        /// <param name="current">The painter that was created before, possibly on the same line. It's only needed to make sure it's freed before a
        /// new painter is created.</param>
        /// <param name="mapping_id">Id of the painter mapping in the control's style.</param>
        /// <param name="mapping_style">An optional style to be used to look up the resources in the painter for painting. if null, the control's
        /// styles are used instead</param>
        /// <param name="default_mapping">An optional mapping to be used to create and initialize the new painter if nothing was found for mapping_id.</param>
        /// <param name="style_override">Id of the painter mapping in the control's style.</param>
        /// <returns>A part painter created based on mapping_id or the default_mapping.</returns>
        public T CreateStylePainter<T>(T current, int mapping_id/*, FudgetStyle mapping_style = null, FudgetPartPainterMapping? default_mapping = null, FudgetStyle style_override = null*/) where T : FudgetPartPainter
        {
            FudgetPartPainterMapping painter_data = default;
            FudgetPartPainter painter = null;

            if (current != null)
                UnregisterStylePainterInternal(current);

            //bool valid = false;
            //if (style_override != null)
            //    valid = FudgetStyle.GetPainterMappingResource(style_override, ActiveTheme, mapping_id, false, out painter_data);
            //if (!valid)
                bool valid = GetStylePainterMapping(mapping_id, out painter_data);

            if (!valid)
                return null;

            //if (valid || default_mapping != null)
            //{
            //    if (valid)
            painter = FudgetThemes.CreatePainter(painter_data.PainterType);
            //    else if (!valid)
            //        painter = FudgetThemes.CreatePainter(default_mapping?.PainterType);
            //}

            if (painter != null && painter is not T)
            {
                Destroy(painter);
                painter = null;
            }
            T result = painter as T;
            if (result != null)
                RegisterStylePainterInternal(result);

            if (result != null)
                result.Initialize(this, painter_data.ResourceMapping /*mapping_style, valid ? painter_data.ResourceMapping : default_mapping?.ResourceMapping*/);

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
