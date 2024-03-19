using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Fudgets
{
    public partial class FudgetDrawableBuilder
    {
        /// <summary>
        /// Adds a resource id to the drawable's list of instructions. If the id refers to a resource that can be drawn
        /// by the drawable, it'll be used for drawing. 
        /// </summary>
        /// <typeparam name="T">An enum type with int backing used as the id</typeparam>
        /// <param name="id">Id of a resource in a theme</param>
        public unsafe static void AddResource<T>(T id) where T : unmanaged, Enum
        {
            AddResource(*(int*)(&id));
        }
    }
}
