#include "ListBox.h"
#include "../Styling/Painters/ListBoxPainter.h"
#include "../Styling/Painters/DrawablePainter.h"
#include "../Styling/PartPainterIds.h"

FudgetStringListProvider::FudgetStringListProvider(const SpawnParams &params) : Base(params), _allow_duplicates(false)
{
    _consumers = New<FudgetDataConsumerRegistry>(SpawnParams(Guid::New(), FudgetDataConsumerRegistry::TypeInitializer));
}

FudgetStringListProvider::~FudgetStringListProvider()
{
    Delete(_consumers);
}

void FudgetStringListProvider::Clear()
{
    if (_items.IsEmpty())
        return;

    _consumers->ClearBegin();
    _items.Clear();
    _consumers->ClearEnd();
}

void FudgetStringListProvider::SetText(int index, const StringView &value)
{
    if (!_consumers->SetBegin(index))
        return;

    if (_allow_duplicates || !IsDuplicate(value))
        _items[index] = value;

    _consumers->SetEnd(index);
}

int FudgetStringListProvider::AddItem(const StringView &value)
{
    if (!_allow_duplicates && IsDuplicate(value))
        return -1;

    if (!_consumers->AddBegin(1))
        return -1;

    _items.Add(value);

    _consumers->AddEnd(1);

    return _items.Count() - 1;
}

int FudgetStringListProvider::InsertItem(int index, const StringView &value)
{
    if (!_allow_duplicates && IsDuplicate(value))
        return -1;

    int cnt = _items.Count();
    index = Math::Clamp(index, 0, cnt);
    if (index == cnt)
        return AddItem(value);

    if (!_consumers->InsertBegin(index, 1))
        return -1;

    _items.Insert(index, value);
    _consumers->InsertEnd(index, 1);

    return index;
}

void FudgetStringListProvider::DeleteItem(int index)
{
    if (index < 0 || index >= _items.Count())
        return;

    if (!_consumers->RemoveBegin(index, 1))
        return;

    _items.RemoveAtKeepOrder(index);

    _consumers->RemoveEnd(index, 1);
}

void FudgetStringListProvider::SetAllowDuplicates(bool value)
{
    if (_allow_duplicates == value)
        return;
    _allow_duplicates = value;

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
            _consumers->BeginDataReset();

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

            _consumers->EndDataReset();
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


FudgetListBox::FudgetListBox(const SpawnParams &params) : Base(params), _item_painter(nullptr),
    _data(nullptr), _owned_data(true), _focus_index(-1), _scroll_pos(0), _top_item(0), _top_item_pos(0), _snap_top_item(false),
    _fixed_item_size(true), _default_size(Int2(-1)), _list_extents(0), _size_processed(0), _hovered_index(-1)
{
    _data = New<FudgetStringListProvider>(SpawnParams(Guid::New(), FudgetStringListProvider::TypeInitializer));
    _data->RegisterDataConsumer(_data_proxy);

    SetScrollBars(FudgetScrollBars::Vertical);
}

FudgetListBox::~FudgetListBox()
{
    if (_data != nullptr && _owned_data)
        Delete(_data);
    //Delete(_v_scrollbar);
}

void FudgetListBox::OnInitialize()
{
}

void FudgetListBox::OnStyleInitialize()
{
    Base::OnStyleInitialize();

    _item_painter = CreateStylePainter<FudgetListItemPainter>(_item_painter, (int)FudgetListBoxPartIds::ItemPainter);

    if (!GetStylePadding((int)FudgetFieldPartIds::Padding, _content_padding))
        _content_padding = FudgetPadding(0);
}

void FudgetListBox::OnDraw()
{
    Base::OnDraw();
    //if (_frame_painter != nullptr)
    //    _frame_painter->Draw(this, bounds, GetVisualState());

    if (_data == nullptr || _item_painter == nullptr)
        return;

    int count = _data->GetCount();
    if (count == 0)
        return;

    Rectangle bounds = GetBounds();


    bounds = GetCombinedPadding().Padded(bounds);
    PushClip(bounds);

    Rectangle r = Rectangle(bounds.GetUpperLeft(), Float2(bounds.GetWidth(), 0.f));
    int pos = ItemIndexAt(r.Location);
    r.Location.Y = GetItemRect(pos).Location.Y;

    FudgetVisualControlState state = GetVisualStateAsEnum();
    FudgetVisualControlState disabled_state = state & FudgetVisualControlState::Disabled;
    FudgetVisualControlState focused_state = state & FudgetVisualControlState::Focused;
    for (int ix = pos; ix < count && r.Location.Y < bounds.GetBottom(); ++ix)
    {
        r.Size.Y = (float)GetItemSize(ix).Y;
        FudgetVisualControlState hover_state = (_hovered_index == ix ? FudgetVisualControlState::Hovered : (FudgetVisualControlState)0);
        FudgetVisualControlState select_state = (IsItemSelected(ix) ? FudgetVisualControlState::Selected : (FudgetVisualControlState)0);
        _item_painter->Draw(this, r, Int2::Zero, ix, _data, uint64(focused_state | disabled_state | hover_state | select_state));
        r.Location.Y += r.Size.Y;
    }

    PopClip();
}

FudgetInputResult FudgetListBox::OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
    if (button != MouseButton::Left)
    {
        return FudgetInputResult::Consume;
    }

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

void FudgetListBox::OnScrollBarScroll(FudgetScrollBarComponent *scrollbar, int64 old_scroll_pos, bool tracking)
{
    if (_default_size.Y == -1 || scrollbar != GetVerticalScrollBar())
        return;
    
    Int2 topleft = GetCombinedPadding().UpperLeft();
    int32 scroll_pos = (int32)scrollbar->GetScrollPos();
    if (_fixed_item_size)
    {
        
        _top_item = scroll_pos / _default_size.Y;
        _top_item_pos.Y = _top_item * _default_size.Y;
        if (_snap_top_item)
            _scroll_pos.Y = _top_item_pos.Y;
        else
            _scroll_pos.Y = int32(scroll_pos);
        return;
    }

    int count = _data->GetCount();

    while (_top_item > 0 && _top_item_pos.Y > scroll_pos)
    {
        _top_item_pos.Y -= GetItemSize(--_top_item).Y;
    }

    int item_size = GetItemSize(_top_item).Y;
    while (_top_item < count - 1 && _top_item_pos.Y + item_size <= scroll_pos)
    {
        _top_item_pos.Y += item_size;
        item_size = GetItemSize(++_top_item).Y;
    }

    if (_snap_top_item)
        _scroll_pos.Y = _top_item_pos.Y;
    else
        _scroll_pos.Y = int32(scroll_pos);
}

//void FudgetListBox::OnScrollBarButtonPressed(FudgetScrollBarComponent *scrollbar, int button_index, bool before_track, bool double_click)
//{
//}
//
//void FudgetListBox::OnScrollBarButtonReleased(FudgetScrollBarComponent *scrollbar, int button_index, bool before_track)
//{
//
//}

//void FudgetListBox::OnScrollBarTrackPressed(FudgetScrollBarComponent *scrollbar, bool before_track, bool double_click)
//{
//    if (scrollbar != _v_scrollbar)
//        return;
//    if (before_track)
//        _v_scrollbar->SetScrollPos(_v_scrollbar->GetScrollPos() - _v_scrollbar->GetPageSize());
//    else
//        _v_scrollbar->SetScrollPos(_v_scrollbar->GetScrollPos() + _v_scrollbar->GetPageSize());
//
//}
//
//void FudgetListBox::OnScrollBarTrackReleased(FudgetScrollBarComponent *scrollbar, bool before_track)
//{
//
//}
//
//void FudgetListBox::OnScrollBarShown(FudgetScrollBarComponent *scrollbar)
//{
//}
//
//void FudgetListBox::OnScrollBarHidden(FudgetScrollBarComponent *scrollbar)
//{
//}

void FudgetListBox::SetDataProvider(FudgetStringListProvider *data)
{
    if (_data == data || (_owned_data && data == nullptr))
        return;

    _data->UnregisterDataConsumer(_data_proxy);

    if ( _owned_data)
        Delete(_data);

    if (data == nullptr)
    {
        _data = New<FudgetStringListProvider>(SpawnParams(Guid::New(), FudgetStringListProvider::TypeInitializer));
        _owned_data = true;
    }
    else
    {
        _owned_data = false;
        _data = data;
    }


    _data->RegisterDataConsumer(_data_proxy);
    DataReset();
}

void FudgetListBox::SetDefaultItemSize(Int2 value)
{
    if (_default_size == value)
        return;
    _default_size = value;

    MarkExtentsDirty();
}

void FudgetListBox::SetItemsHaveFixedSize(bool value)
{
    if (_fixed_item_size == value)
        return;
    _fixed_item_size = value;
    _size_processed = 0;
    _item_heights.clear();

    DataReset();
}

int FudgetListBox::ItemIndexAt(Float2 point)
{
    FudgetPadding content_padding = GetCombinedPadding();
    if (!content_padding.Padded(GetBounds()).Contains(point))
        return -1;

    Int2 pt = Int2((int)point.X, (int)point.Y) - content_padding.UpperLeft() + _scroll_pos;

    if (_fixed_item_size)
        return pt.Y / _default_size.Y;

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
    EnsureDefaultSize();

    if (_data == nullptr || _fixed_item_size || _item_heights[item_index] < 0)
    {
        return _default_size;
    }

    if (_item_heights.size() <= item_index)
        return _default_size;

    return Int2(_default_size.X, _item_heights[item_index]);
}

Rectangle FudgetListBox::GetItemRect(int item_index)
{
    int count = _data->GetCount();
    if (item_index < 0 || item_index >= count)
        return Rectangle::Empty;

    Int2 topleft = GetCombinedPadding().UpperLeft();
    if (_fixed_item_size)
    {
        return Rectangle(Float2((float)topleft.X, (float)topleft.Y + item_index * _default_size.Y - _scroll_pos.Y), _default_size);
    }

    int index = _top_item;
    Int2 pos = _top_item_pos - _scroll_pos + topleft;
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

void FudgetListBox::SetSnapTopItem(bool value)
{
    if (_snap_top_item == value)
        return;
    _snap_top_item = value;
    if (value)
        _scroll_pos.Y = _top_item_pos.Y;
    MarkExtentsDirty();
}

//void FudgetListBox::RequestLayout()
//{
//    if (_dirty_extents)
//        RecalculateListExtents();
//    Base::RequestLayout();
//}

void FudgetListBox::DataChangeBegin()
{

}

void FudgetListBox::DataChangeEnd(bool changed)
{

}

void FudgetListBox::DataToBeReset()
{

}

void FudgetListBox::DataReset()
{
    _focus_index = -1;
    _size_processed = 0;
    _scroll_pos = Int2::Zero;

    _item_heights.clear();
    if (!_fixed_item_size && _data != nullptr)
    {
        _item_heights.insert(_item_heights.begin(), _data->GetCount(), -1);
    }

    MarkExtentsDirty();
}

void FudgetListBox::DataToBeCleared()
{

}

void FudgetListBox::DataCleared()
{
    _focus_index = -1;
    _size_processed = 0;
    _scroll_pos = Int2::Zero;

    _item_heights.clear();

    MarkExtentsDirty();
}

void FudgetListBox::DataToBeUpdated(int index)
{

}

void FudgetListBox::DataUpdated(int index)
{
    if (!_fixed_item_size)
    {
        if (_item_heights[index] != -1)
        {
            --_size_processed;
            _item_heights[index] = -1;
        }

        MarkExtentsDirty();
    }
}

void FudgetListBox::DataToBeAdded(int count)
{

}

void FudgetListBox::DataAdded(int count)
{
    if (!_fixed_item_size)
    {
        _item_heights.insert(_item_heights.end(), _data->GetCount(), -1);
    }

    MarkExtentsDirty();
}

void FudgetListBox::DataToBeRemoved(int index, int count)
{

}

void FudgetListBox::DataRemoved(int index, int count)
{
    if (!_fixed_item_size)
    {
        for (int ix = 0; ix < count; ++ix)
            if (_item_heights[ix + index] != -1)
                --_size_processed;
        _item_heights.erase(_item_heights.cbegin() + index, _item_heights.cbegin() + index + count);
    }

    MarkExtentsDirty();
}

void FudgetListBox::DataToBeInserted(int index, int count)
{

}
void FudgetListBox::DataInserted(int index, int count)
{
    if (!_fixed_item_size)
    {
        _item_heights.insert(_item_heights.cbegin() + index, count, -1);
    }

    MarkExtentsDirty();
}


FudgetControlFlag FudgetListBox::GetInitFlags() const
{
    return FudgetControlFlag::CanHandleMouseMove | FudgetControlFlag::CanHandleMouseEnterLeave | FudgetControlFlag::CanHandleMouseUpDown |
        FudgetControlFlag::CaptureReleaseMouseLeft | FudgetControlFlag::FocusOnMouseLeft |
        FudgetControlFlag::CanHandleKeyEvents | FudgetControlFlag::CanHandleNavigationKeys | FudgetControlFlag::Framed | Base::GetInitFlags();
}

FudgetPadding FudgetListBox::GetCombinedPadding() const
{
    return _content_padding + GetFramePadding();
}

void FudgetListBox::EnsureDefaultSize()
{
    if (_item_painter != nullptr && _data != nullptr && _fixed_item_size && _default_size.Y <= 0 && _data->GetCount() > 0)
    {
        _default_size = _item_painter->Measure(this, 0, _data, 0);
        _default_size.X = (int)GetCombinedPadding().Padded(GetBounds()).GetWidth();

        MarkExtentsDirty();
    }
}

void FudgetListBox::RequestScrollExtents()
{
    if (_data == nullptr)
        return;

    EnsureDefaultSize();

    if (_fixed_item_size)
    {
        _list_extents = Int2(_default_size.X, _default_size.Y * _data->GetCount());
    }
    else
    {
        _list_extents = Int2(_default_size.X, 0);
        for (int ix = 0, siz = (int)_item_heights.size(); ix < siz; ++ix)
        {
            int h = _item_heights[ix];
            if (h == -1)
                _list_extents.Y += _default_size.Y;
            else
                _list_extents.Y += _item_heights[ix];
        }
    }

    Rectangle bounds = GetCombinedPadding().Padded(GetBounds());
    bounds.Size += GetScrollBarWidths();
    bool hvis = _list_extents.X > bounds.Size.X;
    bool vvis = _list_extents.Y > bounds.Size.Y;
    Int2 bounds_size = bounds.Size + GetScrollBarWidths(hvis, vvis);

    if (_snap_top_item)
    {
        bool changed = true;
        while (changed)
        {
            int height = (int)bounds_size.Y;
            int sum = 0;
            int count = _data->GetCount();

            int last_top_height = 0;

            if (_fixed_item_size)
            {
                sum = Math::Min(count * _default_size.Y, _default_size.Y * ((height + _default_size.Y - 1) / _default_size.Y));
                last_top_height = _default_size.Y;
            }
            else
            {
                int pos = count - 1;
                while (pos >= 0 && sum < height)
                {
                    last_top_height = GetItemSize(pos--).Y;
                    sum += last_top_height;
                }
            }
            int expand = 0;
            if (sum > height)
            {
                if (sum > last_top_height)
                    _list_extents.Y += expand = last_top_height;
                else
                    _list_extents.Y += expand = sum - height;
            }

            changed = (!hvis && _list_extents.X > bounds.Size.X) || (!vvis && _list_extents.Y > bounds.Size.Y);
            if (changed)
            {
                hvis |= _list_extents.X > bounds.Size.X;
                vvis |= _list_extents.Y > bounds.Size.Y;
                bounds_size = bounds.Size - GetScrollBarWidths(hvis, vvis);
                _list_extents.Y -= expand;
            }
        }
    }

    FudgetScrollBarComponent *vbar = GetVerticalScrollBar();
    if (vbar == nullptr)
        return;

    vbar->SetScrollRange(_list_extents.Y);
    vbar->SetPageSize((int)bounds_size.Y);
}

