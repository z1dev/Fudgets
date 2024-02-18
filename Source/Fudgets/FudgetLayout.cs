namespace Fudgets
{
    public partial class FudgetLayout
    {
        /// <summary>
        /// Gets the slot of a child control at an index in its container
        /// </summary>
        /// <param name="index">The control's index in its parent</param>
        /// <returns>The slot holding layout specific data for the control</returns>
        public FudgetLayoutSlot GetSlotInternal(int index)
        {
            return GetSlot(index);
        }
    }
}
