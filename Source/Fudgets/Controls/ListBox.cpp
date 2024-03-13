#include "ListBox.h"
#include "../Styling/Painters/ListBoxPainter.h"
#include "../Styling/Painters/FramedFieldPainter.h"
#include "../Styling/PartPainterIds.h"

FudgetStringListProvider::FudgetStringListProvider(const SpawnParams &params) : Base(params), _allow_duplicates(false)
{

}

void FudgetStringListProvider::SetText(int index, const StringView &value)
{
    if (!_allow_duplicates)
        if (IsDuplicate(value))
            return;
    _items[index] = value;
}

int FudgetStringListProvider::AddItem(const StringView &value)
{
    if (IsDuplicate(value))
        return -1;

    _items.Add(value);
    return _items.Count() - 1;
}

int FudgetStringListProvider::InsertItem(int index, const StringView &value)
{
    if (IsDuplicate(value))
        return -1;

    int cnt = _items.Count();
    index = Math::Clamp(index, 0, cnt);
    if (index == cnt)
        return AddItem(value);

    _items.Insert(index, value);
    return index;
}

void FudgetStringListProvider::DeleteItem(int index)
{
    if (index < 0 || index >= _items.Count())
        return;
    _items.RemoveAtKeepOrder(index);
}

void FudgetStringListProvider::SetAllowDuplicates(bool value)
{
    if (_allow_duplicates == value)
        return;
    _allow_duplicates = value;

    bool changed = false;
    if (!_allow_duplicates)
    {
        int first_duplicate = -1;

        HashSet<String> found;
        for (int ix = 0, siz = _items.Count(); ix < siz && first_duplicate == -1; ++ix)
        {
            if (found.Find(_items[ix]) != found.End())
                first_duplicate = ix;
            else
                found.Add(_items[ix]);
        }

        if (first_duplicate != -1)
        {
            changed = true;

            Array<String> tmp = _items;
            _items.Clear();

            for (int ix = 0, siz = tmp.Count(); ix < siz; ++ix)
            {
                const String &item = tmp[ix];
                if (first_duplicate != -1)
                {
                    if (ix == first_duplicate)
                    {
                        first_duplicate = -1;
                        continue;
                    }
                    _items.Add(item);
                    continue;
                }
                if (found.Find(item) != found.End())
                    continue;
                found.Add(item);
                _items.Add(item);
            }
        }
    }

}

bool FudgetStringListProvider::IsDuplicate(const StringView &value) const
{
    for (int ix = 0, siz = GetCount(); ix < siz; ++ix)
        if (_items[ix] == value)
            return true;
    return false;
}


// FudgetListBox


FudgetListBox::FudgetListBox(const SpawnParams &params) : Base(params), _frame_painter(nullptr), _item_painter(nullptr),
    _data(nullptr), _owned_data(true), _focus_index(-1), _scroll_pos(0), _top_item(0), _top_item_pos(0), _snap_top_item(false),
    _fixed_item_size(true), _default_size(Int2(-1)), _size_processed(0), _hovered_index(-1)
{
    _data = New<FudgetStringListProvider>(SpawnParams(Guid::New(), FudgetStringListProvider::TypeInitializer));
}

FudgetListBox::~FudgetListBox()
{
    if (_data != nullptr && _owned_data)
        Delete(_data);
}

void FudgetListBox::OnInitialize()
{
}

void FudgetListBox::OnStyleInitialize()
{
    //FudgetStyle *frame_style;
    //if (!GetStyleStyle((int)FudgetListBoxIds::FrameStyle, frame_style))
    //    frame_style = nullptr;
    _frame_painter = CreateStylePainter<FudgetFramedFieldPainter>(_frame_painter, (int)FudgetFramedControlPartIds::FramePainter);

    //FudgetStyle *item_style;
    //if (!GetStyleStyle((int)FudgetListBoxIds::ItemStyle, item_style))
    //    item_style = nullptr;
    _item_painter = CreateStylePainter<FudgetListItemPainter>(_item_painter, (int)FudgetListBoxPartIds::ItemPainter);
}

void FudgetListBox::OnDraw()
{
    Rectangle bounds = GetBounds();
    if (_frame_painter != nullptr)
        _frame_painter->Draw(this, bounds, GetVisualState());

    if (_data == nullptr || _item_painter == nullptr)
        return;

    int count = _data->GetCount();
    if (count == 0)
        return;

    bounds = GetInnerPadding().Padded(bounds);
    PushClip(bounds);

    Rectangle r = Rectangle(bounds.GetUpperLeft(), Float2(bounds.GetWidth(), 0.f));
    int pos = ItemIndexAt(r.Location);
    r.Location.Y = GetItemRect(pos).Location.Y;

    for (int ix = pos; ix < count && r.Location.Y < bounds.GetBottom(); ++ix)
    {
        r.Size.Y = (float)GetItemSize(ix).Y;
        FudgetVisualControlState hover_state = (_hovered_index == ix ? FudgetVisualControlState::Hovered : (FudgetVisualControlState)0);
        FudgetVisualControlState select_state = (IsItemSelected(ix) ? FudgetVisualControlState::Selected : (FudgetVisualControlState)0);
        _item_painter->Draw(this, r, Int2(0, pos), ix, _data, (GetVisualState() & ~FudgetVisualControlState::Hovered) | hover_state | select_state);
        r.Location.Y += r.Size.Y;
    }

    PopClip();
}

FudgetInputResult FudgetListBox::OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
    if (button != MouseButton::Left)
        return FudgetInputResult::Consume;

    _hovered_index = -1;

    return FudgetInputResult::Consume;
}

bool FudgetListBox::OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
    if (button == MouseButton::Left)
    {

    }
    return true;
}

void FudgetListBox::OnMouseMove(Float2 pos, Float2 global_pos)
{
    if (MouseIsCaptured())
    {

    }
    else
    {
        _hovered_index = ItemIndexAt(pos);
    }
}

void FudgetListBox::OnMouseLeave()
{
    _hovered_index = -1;
}

bool FudgetListBox::WantsNavigationKey(KeyboardKeys key)
{
    return key == KeyboardKeys::ArrowUp || key == KeyboardKeys::ArrowDown;
}

FudgetInputResult FudgetListBox::OnKeyDown(KeyboardKeys key)
{
    return FudgetInputResult::Consume;
}

void FudgetListBox::SetDataProvider(FudgetStringListProvider *data)
{
    if (_data == data)
        return;

    if (_data != nullptr && _owned_data)
        Delete(_data);

    _owned_data = false;
    _data = data;
    _focus_index = -1;
    _size_processed = 0;
    _scroll_pos = Int2::Zero;
    _item_heights.Clear();

    if (!_fixed_item_size && _data != nullptr)
    {
        _item_heights.AddUninitialized(_data->GetCount());
        for (int ix = 0, siz = _data->GetCount(); ix < siz; ++ix)
            _item_heights[ix] = -1;
    }

    // TODO: react to data changes, item insertion etc.
}

void FudgetListBox::SetDefaultItemSize(Int2 value)
{
    if (_default_size == value)
        return;
    _default_size = value;
}

void FudgetListBox::SetItemsHaveFixedSize(bool value)
{
    if (_fixed_item_size == value)
        return;
    _fixed_item_size = value;
    _size_processed = 0;
    _item_heights.Clear();

    if (!_fixed_item_size && _data != nullptr)
    {
        _item_heights.AddUninitialized(_data->GetCount());
        for (int ix = 0, siz = _data->GetCount(); ix < siz; ++ix)
            _item_heights[ix] = -1;
    }
}

int FudgetListBox::ItemIndexAt(Float2 point)
{
    Int2 pt = Int2((int)point.X, (int)point.Y) - _top_item_pos;

    int index = _top_item;
    int top_pos = _top_item_pos.Y;
    int pos_height = GetItemSize(index).Y;
    int count = _data->GetCount();

    while (pt.Y < top_pos && index > 0)
    {
        pos_height = GetItemSize(--index).Y;
        top_pos -= pos_height;
    }

    while (pt.Y >= top_pos + pos_height && index < count - 1)
    {
        top_pos += pos_height;
        pos_height = GetItemSize(++index).Y;
    }

    return index;
}

bool FudgetListBox::IsItemSelected(int item_index) const
{
    return false;
}

Int2 FudgetListBox::GetItemSize(int item_index)
{
    if (_data == nullptr || _fixed_item_size || _item_heights[item_index] < 0)
    {
        if (_item_painter != nullptr && _data != nullptr && _fixed_item_size && _default_size.Y <= 0)
        {
            _default_size = _item_painter->Measure(this, item_index, _data, FudgetVisualControlState::Normal);
            _default_size.X = (int)GetInnerPadding().Padded(GetBounds()).GetWidth();
        }

        return _default_size;
    }

    if (_item_heights.Count() <= item_index)
        return _default_size;

    return Int2(_default_size.X, _item_heights[item_index]);
}

Rectangle FudgetListBox::GetItemRect(int item_index)
{
    int count = _data->GetCount();
    if (item_index < 0 || item_index > count)
        return Rectangle::Empty;

    int index = _top_item;
    Int2 pos = _top_item_pos;
    Int2 pos_size = GetItemSize(index);

    while (item_index < index)
    {
        pos_size = GetItemSize(--index);
        pos.Y -= pos_size.Y;
    }
    while (item_index > index)
    {
        pos.Y += pos_size.Y;
        pos_size = GetItemSize(++index);
    }

    return Rectangle(pos, pos_size);
}

FudgetControlFlag FudgetListBox::GetInitFlags() const
{
    return FudgetControlFlag::CanHandleMouseMove | FudgetControlFlag::CanHandleMouseEnterLeave | FudgetControlFlag::CanHandleMouseUpDown |
        FudgetControlFlag::CaptureReleaseMouseLeft | FudgetControlFlag::FocusOnMouseLeft |
        FudgetControlFlag::CanHandleKeyEvents | FudgetControlFlag::CanHandleNavigationKeys | Base::GetInitFlags();
}

FudgetPadding FudgetListBox::GetInnerPadding() const
{
    return _frame_painter != nullptr ? _frame_painter->GetContentPadding() : FudgetPadding(0);
}

