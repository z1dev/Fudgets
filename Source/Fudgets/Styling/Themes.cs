using FlaxEngine;
using System.Collections.Generic;
using System.Runtime.InteropServices.Marshalling;

namespace Fudgets
{

    /// <inheritdoc />

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

#if FLAX_EDITOR
            if (!RuntimeUse)
                editorRegisteredStyles.Add(style);
            else
#endif
            registeredStyles.Add(style);
            return style;
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