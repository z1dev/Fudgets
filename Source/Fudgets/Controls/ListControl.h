#pragma once

#include "../Container.h"

/// <summary>
/// Base class for controls that show a flat list of items. In derived classes, the items can be laid
/// out in a grid, a list or any other way provided by the list control.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetListControl : public FudgetContainer
{
    using Base = FudgetContainer;
    DECLARE_SCRIPTING_TYPE(FudgetListControl);
public:
    /// <summary>
    /// Gets the index of the nearest item at a point relative to the control's top left coordinates.
    /// </summary>
    /// <param name="point">Position in the content bounds</param>
    /// <returns>Index of the closest item</returns>
    API_FUNCTION() virtual int ItemIndexAt(Float2 point) { return -1; }

    /// <summary>
    /// Checks if the item at index is selected and returns it.
    /// </summary>
    /// <param name="item_index">Index of the item in the list starting at 0</param>
    /// <returns>Whether the item is selected or not</returns>
    API_FUNCTION() virtual bool IsItemSelected(int item_index) const { return false; }

    /// <summary>
    /// Gets the size of an item at index, calculating or measuring it if necessary. This can be faster
    /// than calling GetItemRect and checking its size, if the item's location is not needed.
    /// </summary>
    /// <param name="item_index">Index of the item in the list</param>
    /// <returns>Item size of the item at the index</returns>
    API_FUNCTION() virtual Int2 GetItemSize(int item_index) { return Int2::Zero; }

    /// <summary>
    /// Calculates the rectangle of an item at index relative to the control's top left coordinates.
    /// </summary>
    /// <param name="item_index">Index of the item in the list starting at 0</param>
    /// <returns>The bounding rectangle of the item inside the control's bounds</returns>
    API_FUNCTION() virtual Rectangle GetItemRect(int item_index) { return Rectangle::Empty; }
};
