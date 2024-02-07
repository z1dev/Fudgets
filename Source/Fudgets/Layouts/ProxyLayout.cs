using FlaxEngine;

namespace Fudgets
{
    public partial class FudgetProxyLayout
    {
        /// <summary>
        /// Directly sets a control's position and size on the owner container.
        /// </summary>
        /// <param name="index">The index of the control</param>
        /// <param name="pos">The calculated position of the control</param>
        /// <param name="size">The calculated size of the control</param>
        public new void SetControlDimensions(int index, Float2 pos, Float2 size)
        {
            base.SetControlDimensions(index, pos, size);
        }
    }
}