using System;
using System.Collections.Generic;

namespace Fudgets
{

    /// <inheritdoc />
    partial class FudgetTheme
    {
        /// <summary>
        /// Sets or creates a resource for an integer id. The resource can be of any type. Its use only depends on the
        /// styles and controls that can use it.
        /// Warning: This function accepts any enum value, but only works with int backed enums
        /// </summary>
        /// <typeparam name="T">Enum type with int backing used as the id</typeparam>
        /// <param name="enum_value">Unique id of the resource</param>
        /// <param name="resource">The value of the resource</param>
        public unsafe void SetResource<T>(T enum_value, object resource) where T: unmanaged, Enum
        {
            SetResource(*(int*)(&enum_value), resource);
        }

        /// <summary>
        /// Gets a resource for an integer id. The resource can be of any type. Its use only depends on the
        /// styles and controls that can use it.
        /// Warning: This function accepts any enum value, but only works with int backed enums
        /// </summary>
        /// <typeparam name="T">Enum type with int backing used as the id</typeparam>
        /// <param name="enum_value">Unique id of the resource</param>
        /// <param name="resource">The value of the resource</param>
        public unsafe bool GetResource<T>(T enum_value, out object resource) where T : unmanaged, Enum
        {
            return GetResource(*(int*)(&enum_value), out resource);
        }
    }

    /// <inheritdoc />
    partial class FudgetThemes
    {
#if FLAX_EDITOR
        private static List<FudgetStyle> editorRegisteredStyles = [];
        private static bool editorInitialized = false;
#endif
        private static List<FudgetStyle> registeredStyles = [];
        private static bool initialized = false;

        /// <summary>
        /// Creates a style with the given name. The function fails if another style with the same name
        /// already exists.
        /// </summary>
        /// <param name="style_name">Name of the new style. Must be unique</param>
        /// <returns>The created style or null</returns>
        public static FudgetStyle CreateStyle(string style_name)
        {
            if (style_name.Length == 0)
                return null;
            FudgetStyle style = Internal_CreateStyle(style_name);
            if (style == null)
                return null;

#if FLAX_EDITOR
            if (!RuntimeUse)
                editorRegisteredStyles.Add(style);
            else
#endif
            registeredStyles.Add(style);
            return style;
        }

        /// <summary>
        /// Returns a style based on a class' full name, creating it if it doesn't exist. The style can be accessed with GetStyle later
        /// with the same name. Styles with a name matching a control's or painter's full class name will be used for that control or
        /// painter by default.
        /// </summary>
        /// <param name="style_name">Name of class to get the style for</param>
        /// <returns>A style with the same name as the full name of the generic class</returns>
        public static FudgetStyle CreateOrGetStyle(string style_name)
        {
            if (style_name.Length == 0)
                return null;

            FudgetStyle result = CreateStyle(style_name);
            if (result != null)
                return result;

            return GetStyle(style_name);
        }


        /// <summary>
        /// Returns a style based on a class' full name, creating it if it doesn't exist. The style can be accessed with GetStyle later
        /// with the same name. Styles with a name matching a control's or painter's full class name will be used for that control or
        /// painter by default.
        /// </summary>
        /// <typeparam name="T">Name of class to get the style for</typeparam>
        /// <returns>A style with the same name as the full name of the generic class</returns>
        public static FudgetStyle CreateOrGetStyle<T>() where T : class
        {
            return CreateOrGetStyle(typeof(T).FullName);
        }

        /// <summary>
        /// Call when the plugin or editor plugin starts, and call UninitializeManaged on their Deinitialize.
        /// </summary>
        /// <param name="in_game">Whether called by the game plugin instead of the editor plugin</param>
        public static void InitializeManaged(bool in_game)
        {
#if FLAX_EDITOR
            if (!in_game)
            {
                if (editorInitialized)
                    return;
                editorInitialized = true;
            }
#endif
            if (in_game)
            {
                if (initialized)
                    return;
                initialized = true;
            }
        }

        /// <summary>
        /// Removes references to managed objects registered with the global themes database.
        /// </summary>
        /// <param name="in_game">Whether called by the game plugin instead of the editor plugin</param>
        public static void UninitializeManaged(bool in_game)
        {
#if FLAX_EDITOR
            if (!in_game)
            {
                if (!editorInitialized)
                    return;
                editorRegisteredStyles.Clear();
                editorInitialized = false;
            }
#endif
            if (in_game)
            {
                if (!initialized)
                    return;
                registeredStyles.Clear();
                initialized = false;
            }
        }
    }

}