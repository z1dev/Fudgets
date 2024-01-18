using FlaxEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
                return null;

            var child = New<T>();
            AddChild(child);
            return (T)child;
        }

        /// <summary>
        /// Creates a layout and adds it to the container as the layout to manage the child controls positioning and size
        /// </summary>
        /// <typeparam name="T">The type of the layout to create. Must derive from FudgetLayout</typeparam>
        /// <returns>The newly created and added layout</returns>
        public T CreateLayout<T>() where T : FudgetLayout
        {
            if (typeof(T).IsAbstract)
                return null;

            var child = New<T>(); //Activator.CreateInstance<T>();
            AddLayoutInternal(child);
            return (T)child;
        }


        /*
    template<typename T>
    FORCE_INLINE T* CreateChild()
    {
        T* child = New<T>();
        AddChild(child);
        return child;
    }

    template<typename T>
    FORCE_INLINE T* CreateLayout()
    {
        T* layout = New<T>();
        _layout = layout;
        layout->SetOwner(this);
        return layout;
    }         */
    }
}
