using System;
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

            if (GUIRoot == this)
            {
                Debug.LogError("Adding layout directly to the root is not supported. Add a child container or FudgetAssetRoot and set its hint size to the size of this root.");
                return null;
            }

            var layout = New<T>(); //Activator.CreateInstance<T>();
            SetLayoutInternal(layout);
            return (T)layout;
        }
    }

    partial class FudgetGUIRoot
    {
        /// <inheritdoc />
        [Obsolete("Adding layout directly to the root is not supported. Add a child container or FudgetAssetRoot and set its hint size to the size of this root.", true)]
        public override T CreateLayout<T>()
        {
            return base.CreateLayout<T>();
        }
    }
}
