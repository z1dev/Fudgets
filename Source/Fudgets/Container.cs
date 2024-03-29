﻿using System;
using FlaxEngine;

namespace Fudgets
{
    partial class FudgetContainer
    {
        /// <summary>
        /// Creates a control and adds it to the container as child. Controls belonging to a container are drawn by
        /// the container and their events are also called by the container.
        /// </summary>
        /// <typeparam name="T">The type of the control to create. Must derive from FudgetControl</typeparam>
        /// <returns>The newly created and added control</returns>
        public T CreateChild<T>() where T : FudgetControl
        {
            if (typeof(T).IsAbstract)
            {
                Debug.LogError("Cannot create child control from an abstract type.");
                return null;
            }

            var child = New<T>();
            AddChild(child);
            return (T)child;
        }

        /// <summary>
        /// Creates a control and adds it to the container as child. Controls belonging to a container are drawn by
        /// the container and their events are also called by the container.
        /// </summary>
        /// <typeparam name="T">The type of the control to create. Must derive from FudgetControl</typeparam>
        /// <param name="style_name">Name of style to use for this control, mainly for compound controls to create their child controls
        /// with a style. This is different from setting a new style, to allow overriding and resetting it. After the control is
        /// created with the style name, this becomes its default style.</param>
        /// <returns>The newly created and added control</returns>
        public T CreateChild<T>(string style_name) where T : FudgetControl
        {
            if (typeof(T).IsAbstract)
            {
                Debug.LogError("Cannot create child control from an abstract type.");
                return null;
            }

            var child = New<T>();
            child.StyleName = style_name;
            AddChild(child);
            return (T)child;
        }

        /// <summary>
        /// Creates a layout and adds it to the container as the layout to manage the child controls positioning and size
        /// </summary>
        /// <typeparam name="T">The type of the layout to create. Must derive from FudgetLayout</typeparam>
        /// <returns>The newly created and added layout</returns>
        public virtual T CreateLayout<T>() where T : FudgetLayout
        {
            if (typeof(T).IsAbstract)
            {
                Debug.LogError("Cannot create layout from an abstract type.");
                return null;
            }

            var layout = New<T>(); //Activator.CreateInstance<T>();
            var old_layout = SetLayout(layout);
            if (old_layout != null)
                Destroy(old_layout);
            return (T)layout;
        }
    }
}
