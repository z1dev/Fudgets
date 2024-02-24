using FlaxEngine;

namespace Fudgets
{

    public partial struct FudgetLayoutSizeCache
    {
        /// <summary>
        /// Initializes the structure to be valid with only the measured space set. Every other value at zero.
        /// </summary>
        /// <param name="space">Space used for measurement</param>
        public FudgetLayoutSizeCache(Float2 space)
        {
            IsValid = true;
            Space = space;
            Size = Float2.Zero;
            Min = Float2.Zero;
            Max = Float2.Zero;
            SizeFromSpace = false;
        }

        /// <summary>
        /// Initializes the structure to be valid with all the values
        /// </summary>
        /// <param name="space">Space used for measurement</param>
        /// <param name="size">Requested size</param>
        /// <param name="min">Requested minimum size</param>
        /// <param name="max">Requested maximum size</param>
        /// <param name="sizeFromSpace">Control calculates its size based on the space it gets</param>
        public FudgetLayoutSizeCache(Float2 space, Float2 size, Float2 min, Float2 max, bool sizeFromSpace)
        {
            IsValid = true;
            Space = space;
            Size = size;
            Min = min;
            Max = max;
            SizeFromSpace = sizeFromSpace;
        }

    }

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
