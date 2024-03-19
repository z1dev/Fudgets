#pragma once

#include "ListControl.h"
#include "ScrollBar.h"

#include <vector>

class FudgetDrawablePainter;
class FudgetListItemPainter;


API_CLASS ()
class FUDGETS_API FudgetStringListProvider : public ScriptingObject, public IFudgetDataProvider
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE(FudgetStringListProvider);
public:
    ~FudgetStringListProvider();

    /// <inheritdoc />
    void BeginChange() override { _consumers->BeginChange(); }

    /// <inheritdoc />
    void EndChange() override { _consumers->EndChange(); }

    /// <inheritdoc />
    void BeginDataReset() override { _consumers->BeginDataReset(); }

    /// <inheritdoc />
    void EndDataReset() override { _consumers->EndDataReset(); }

    /// <inheritdoc />
    int GetCount() const override { return _items.Count(); }

    /// <inheritdoc />
    void Clear() override;

    /// <inheritdoc />
    Variant GetValue(int index) override { return _items[index]; }

    /// <summary>
    /// Use SetText.
    /// </summary>
    void SetValue(int index, Variant value) override { ; }

    /// <inheritdoc />
    String GetText(int index) override { return _items[index]; }

    /// <inheritdoc />
    void SetText(int index, const StringView &value) override;

    /// <summary>
    /// Ignored.
    /// </summary>
    int GetInt(int index) override { return 0; }

    /// <summary>
    /// Ignored.
    /// </summary>
    void SetInt(int index, int value) override { ; }

    /// <inheritdoc />
    void RegisterDataConsumer(IFudgetDataConsumer *consumer) override { _consumers->RegisterDataConsumer(consumer); }

    /// <inheritdoc />
    void UnregisterDataConsumer(IFudgetDataConsumer *consumer) override { _consumers->UnregisterDataConsumer(consumer); }

    /// <summary>
    /// Adds the string to the end of the list.
    /// </summary>
    /// <param name="value">String to add</param>
    /// <returns>Index of the newly added item or -1 if nothing was added</returns>
    API_FUNCTION() int AddItem(const StringView &value);
    /// <summary>
    /// Inserts the string at the specified position in the list.
    /// </summary>
    /// <param name="index">Position to insert. It will be clamped between 0 and count</param>
    /// <param name="value">String to insert</param>
    /// <returns>Index of newly inserted item or -1 if nothing was inserted</returns>
    API_FUNCTION() int InsertItem(int index, const StringView &value);
    /// <summary>
    /// Removes a string at index from the list
    /// </summary>
    /// <param name="index">Position of item to remove. If this is out of range nothing will be removed.</param>
    API_FUNCTION() void DeleteItem(int index);

    /// <summary>
    /// Whether the list data can hold items with the same value.
    /// </summary>
    API_PROPERTY() bool GetAllowDuplicates() const { return _allow_duplicates; }
    /// <summary>
    /// Set whether the list data can hold items with the same value. If set to true,t he items will be filtered
    /// and only the first occurrence of each value will be kept.
    /// </summary>
    API_PROPERTY() void SetAllowDuplicates(bool value);
protected:
    API_FUNCTION() bool IsDuplicate(const StringView &value) const;
private:
    Array<String> _items;
    bool _allow_duplicates;

    FudgetDataConsumerRegistry *_consumers;
};

/// <summary>
/// Standard list box class that shows a list of text, with items one below the other, sized to fill the
/// width of the content area of the control.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetListBox : public FudgetListControl
{
    using Base = FudgetListControl;
    DECLARE_SCRIPTING_TYPE(FudgetListBox);
public:
    ~FudgetListBox();

    /// <inheritdoc />
    void OnInitialize() override;

    /// <inheritdoc />
    void OnStyleInitialize() override;

    /// <inheritdoc />
    void OnDraw() override;

    /// <inheritdoc />
    FudgetInputResult OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click) override;
    /// <inheritdoc />
    bool OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button) override;
    /// <inheritdoc />
    void OnMouseMove(Float2 pos, Float2 global_pos) override;
    /// <inheritdoc />
    void OnMouseLeave() override;
    /// <inheritdoc />
    bool WantsNavigationKey(KeyboardKeys key) override;
    /// <inheritdoc />
    FudgetInputResult OnKeyDown(KeyboardKeys key) override;

    /// <summary>
    /// Gets the data provider currently set for this list control.
    /// </summary>
    /// <returns></returns>
    API_PROPERTY() FudgetStringListProvider* GetDataProvider() const { return _data; }

    /// <summary>
    /// Sets a data provider for this list control, replacing the default or previously set provider.
    /// </summary>
    /// <param name="value">The new data provider</param>
    API_PROPERTY() void SetDataProvider(FudgetStringListProvider *value);

    /// <summary>
    /// In list controls with fixed sized items, this is the size of the item to draw. If the item
    /// size is not fixed, this is the size used for unprocessed items while the item sizes haven't been
    /// set or are being calculated. In this case, the closer this size is to the default, the smoother
    /// the processing will appear.
    /// </summary>
    /// <returns>Item dimensions in the list control</returns>
    API_PROPERTY() virtual Int2 GetDefaultItemSize() const { return _default_size; }
    /// <summary>
    /// In list controls with fixed sized items, this is the size of the item to draw. If the item
    /// size is not fixed, this is the size used for unprocessed items while the item sizes haven't been
    /// set or are being calculated. In this case, the closer this size is to the default, the smoother
    /// the processing will appear.
    /// </summary>
    /// <param name="value">Item dimensions to set</param>
    API_PROPERTY() virtual void SetDefaultItemSize(Int2 value);

    /// <summary>
    /// Whether items in the list control all have the same size. If so, use the default item size to get or
    /// set that size.
    /// </summary>
    API_PROPERTY() virtual bool GetItemsHaveFixedSize() const { return _fixed_item_size; }
    /// <summary>
    /// Set whether items in the list control all have the same size. If so, use the default item size to get or
    /// set that size.
    /// </summary>
    /// <param name="value">Whether items should all be the same size</param>
    API_PROPERTY() virtual void SetItemsHaveFixedSize(bool value);

    /// <summary>
    /// Gets the size of an item at index, calculating or measuring it if necessary. For controls with fixed sized
    /// items this is always the same as the default item size. This can be faster than calling GetItemRect and
    /// checking its size, if the item's location is not needed.
    /// </summary>
    /// <param name="item_index">Index of the item in the list</param>
    /// <returns>Item size of the item at the index</returns>
    Int2 GetItemSize(int item_index) override;
    /// <inheritdoc />
    int ItemIndexAt(Float2 point) override;
    /// <inheritdoc />
    bool IsItemSelected(int item_index) const override;
    /// <inheritdoc />
    Rectangle GetItemRect(int item_index) override;

    /// <summary>
    /// Causes recalculation of the size required by all the items in the listbox when layouting is next
    /// requested or drawing.
    /// </summary>
    /// <returns></returns>
    API_FUNCTION() void MarkExtentsDirty() { _dirty_extents = true; }
protected:

    /// <inheritdoc />
    void RequestLayout() override;

    /// <inheritdoc />
    void DataChangeBegin() override;
    /// <inheritdoc />
    void DataChangeEnd(bool changed) override;
    /// <inheritdoc />
    void DataToBeReset() override;
    /// <inheritdoc />
    void DataReset() override;
    /// <inheritdoc />
    void DataToBeCleared() override;
    /// <inheritdoc />
    void DataCleared() override;
    /// <inheritdoc />
    void DataToBeUpdated(int index) override;
    /// <inheritdoc />
    void DataUpdated(int index) override;
    /// <inheritdoc />
    void DataToBeAdded(int count) override;
    /// <inheritdoc />
    void DataAdded(int count) override;
    /// <inheritdoc />
    void DataToBeRemoved(int index, int count) override;
    /// <inheritdoc />
    void DataRemoved(int index, int count) override;
    /// <inheritdoc />
    void DataToBeInserted(int index, int count) override;
    /// <inheritdoc />
    void DataInserted(int index, int count) override;

    /// <inheritdoc />
    FudgetControlFlag GetInitFlags() const override;
private:
    FudgetPadding GetFramePadding() const;
    FudgetPadding GetContentPadding() const;
    Rectangle GetScrollBarBounds() const;

    void EnsureDefaultSize();
    void RecalculateListExtents();

    FudgetScrollBarComponent *_v_scrollbar;

    FudgetDrawablePainter *_frame_painter;
    FudgetListItemPainter *_item_painter;

    FudgetStringListProvider *_data;
    bool _owned_data;

    // Index of the focused item which was last selected or moved to. The focused item is always visible.
    int _focus_index;

    Int2 _scroll_pos;
    // Index of the first visible item at the top.
    int _top_item;
    // Position of the first visible item at the top, relative to the virtual origin of the contents.
    Int2 _top_item_pos;
    // Scrolling keeps the top item fully in view.
    bool _snap_top_item;

    // Whether every item has the same size
    bool _fixed_item_size;
    // The default size of items in the list. Setting _fixed_item_size to true makes every item have the default size.
    // Otherwise the items will be treated like having this size before they are measured.
    Int2 _default_size;

    // A dirty flag when the contents changed and their size needs to be calculated to be able to show a scrollbar or limit scrolling.
    bool _dirty_extents;

    // Dimensions of the displayed list with all items included. When item sizes are not fully calculated, this is an
    // approximated value.
    Int2 _list_extents;

    // Height of each item in the list when _fixed_item_size is false. This list is not always complete if the items
    // are measured in small blocks. Not measured items have a value of -1 in this list. Check _size_processed to tell
    // the number of fully measured items.
    std::vector<int> _item_heights;
    // Number of list box items with their sizes measured. Only used if _fixed_item_size is false and the items are
    // measured in small blocks.
    int _size_processed;

    // Index of item currently under the mouse pointer.
    int _hovered_index;
};

