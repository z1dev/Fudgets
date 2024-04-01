#pragma once

#include <vector>
#include "Engine/Scripting/ScriptingObject.h"


// Structure for a single selection block. The block has a start index and a length.
struct FudgetSelectionBlock
{
    // Index of first selected item in the block.
    int _start;
    // Index after last selected item in the block.
    int _end;
};

/// <summary>
/// Helper class that keeps track of selected items in controls.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetItemSelection : public ScriptingObject
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE(FudgetItemSelection);
public:
    API_PROPERTY() int Count() const { return _count; }

    /// <summary>
    /// Gets the number of items in the related item collection.
    /// </summary>
    API_PROPERTY() int GetSize() const { return _size; }
    /// <summary>
    /// Sets the number of items in the related item collection. Changing the size to a smaller value than
    /// the current size ensures that there is no selection data above the new maximum index.
    /// </summary>
    /// <param name="value">The new number of items.</param>
    API_PROPERTY() void SetSize(int value);

    /// <summary>
    /// Returns whether there is anything selected.
    /// </summary>
    API_FUNCTION() bool HasSelection() const { return !_blocks.empty(); }

    /// <summary>
    /// Clears the selection.
    /// </summary>
    API_FUNCTION() FORCE_INLINE void DeselectAll() { Clear(); }

    /// <summary>
    /// Clears the selection.
    /// </summary>
    API_FUNCTION() void Clear();

    /// <summary>
    /// Changes the selected state of one or more items.
    /// </summary>
    /// <param name="index">Index of the first item to select or unselect.</param>
    /// <param name="count">Number of items to select or unselect.</param>
    /// <param name="select">Whether to select or unselect the items.</param>
    API_FUNCTION() void SetSelected(int index, int count, bool select);
    /// <summary>
    /// Checks the selection state of an item at the passed index.
    /// </summary>
    /// <param name="index">Index of the item.</param>
    /// <returns>Whether the item is selected.</returns>
    API_FUNCTION() bool IsSelected(int index) const;

    /// <summary>
    /// Modifies the selection when items were inserted in the related item collection. The newly inserted
    /// items will be in an unselected state. The size of the selection is also increased.
    /// </summary>
    /// <param name="index">Indes of the first inserted item.</param>
    /// <param name="count">Number of items inserted.</param>
    API_FUNCTION() void ItemsInserted(int index, int count);
    /// <summary>
    /// Modifies the selection when items were removed in the related item collection. The removed items
    /// selection data is erased, and items at higher indexes are kept at their new index. The size of
    /// the selection is also decreased.
    /// </summary>
    /// <param name="index">Indes of the first removed item.</param>
    /// <param name="count">Number of items removed.</param>
    API_FUNCTION() void ItemsRemoved(int index, int count);
private:
    int _size;
    int _count;
    std::vector<FudgetSelectionBlock> _blocks;
};