#pragma once

#include "ListControl.h"


API_CLASS ()
class FUDGETS_API FudgetStringListProvider : public ScriptingObject, public IListDataProvider
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE(FudgetStringListProvider);
public:
    /// <inheritdoc />
    int GetCount() const override { return _items.Count(); }

    /// <inheritdoc />
    Variant GetData(int index) override { return _items[index]; }

    /// <summary>
    /// Use SetText.
    /// </summary>
    void SetData(int index, Variant value) override { ; }

    /// <inheritdoc />
    String GetText(int index) override { return _items[index]; }

    /// <inheritdoc />
    void SetText(int index, const StringView &value) override;

    /// <summary>
    /// Ignored.
    /// </summary>
    int GetIntData(int index) override { return 0; }

    /// <summary>
    /// Ignored.
    /// </summary>
    void SetIntData(int index, int value) override { ; }

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
};


API_CLASS()
class FUDGETS_API FudgetListBoxItemPainter : public FudgetListItemPainter
{
    using Base = FudgetListItemPainter;
    DECLARE_SCRIPTING_TYPE(FudgetListBoxItemPainter);
public:
    /// <summary>
    /// Text Painter token
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken TextPainterToken;
    /// <summary>
    /// Text Style token
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken TextStyleToken;

    /// <summary>
    /// Creates the default style for drawing, filling the resources to use that will be looked up in the current theme.
    /// </summary>
    API_FUNCTION() static void CreateStyle();

    ~FudgetListBoxItemPainter();

    /// <inheritdoc />
    void Initialize(FudgetTheme *theme, FudgetStyle *style) override;
    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, Float2 offset, int item_index, IListDataProvider *data, FudgetVisualControlState state) override;
    /// <inheritdoc />
    Float2 Measure(FudgetControl *control, int item_index, IListDataProvider *data, FudgetVisualControlState state) override;
private:
    FudgetSingleLineTextPainter *_text_painter;
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

    /// <summary>
    /// Initializes tokens used by the styles
    /// </summary>
    API_FUNCTION() static void InitializeTokens();
    /// <summary>
    /// Frame Painter token
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken FramePainterToken;
    /// <summary>
    /// Frame Style token
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken FrameStyleToken;
    /// <summary>
    /// Frame Painter token
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken ItemPainterToken;
    /// <summary>
    /// Frame Style token
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken ItemStyleToken;

    /// <inheritdoc />
    void OnInitialize() override;

    /// <inheritdoc />
    void OnDraw() override;

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
    API_PROPERTY() virtual Float2 GetDefaultItemSize() const { return _default_size; }
    /// <summary>
    /// In list controls with fixed sized items, this is the size of the item to draw. If the item
    /// size is not fixed, this is the size used for unprocessed items while the item sizes haven't been
    /// set or are being calculated. In this case, the closer this size is to the default, the smoother
    /// the processing will appear.
    /// </summary>
    /// <param name="value">Item dimensions to set</param>
    API_PROPERTY() virtual void SetDefaultItemSize(Float2 value);

    /// <summary>
    /// Gets the size of an item in the list control at the given data index. For controls with fixed sized
    /// items this is always the same as the default item size. If the item size is not fixed, this will still
    /// return the default item size if the item at index hasn't yet been processed.
    /// </summary>
    /// <param name="index">The index of the data item to measure</param>
    /// <returns>Item dimensions at index</returns>
    API_FUNCTION() virtual Float2 GetItemSize(int index);

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
private:
    FudgetPadding GetInnerPadding() const;

    FudgetFramedFieldPainter *_frame_painter;
    FudgetListItemPainter *_item_painter;

    FudgetStringListProvider *_data;
    bool _owned_data;

    int _current;
    Float2 _scroll_pos;
    bool _fixed_item_size;

    Float2 _default_size;

    Array<float> _item_heights;
    int _size_processed;
};

