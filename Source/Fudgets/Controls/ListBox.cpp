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


FudgetListBox::FudgetListBox(const SpawnParams &params) : Base(params), _frame_painter(nullptr), _item_painter(nullptr),
    _data(nullptr), _owned_data(true), _focus_index(-1), _scroll_pos(0), _top_item(0), _top_item_pos(0), _snap_top_item(false),
    _fixed_item_size(true), _default_size(Int2(-1)), _dirty_extents(true), _list_extents(0), _size_processed(0), _hovered_index(-1)
{
    _data = New<FudgetStringListProvider>(SpawnParams(Guid::New(), FudgetStringListProvider::TypeInitializer));
    _data->RegisterDataConsumer(_data_proxy);
    _v_scrollbar = New<FudgetScrollBarComponent>(SpawnParams(Guid::New(), FudgetScrollBarComponent::TypeInitializer));
    _v_scrollbar->Initialize(this);
}

FudgetListBox::~FudgetListBox()
{
    if (_data != nullptr && _owned_data)
        Delete(_data);
    Delete(_v_scrollbar);
}

void FudgetListBox::OnInitialize()
{
}

void FudgetListBox::OnStyleInitialize()
{
    Int2 up_left = GetFramePadding().UpperLeft();

    if (_frame_painter != nullptr)
        _top_item_pos -= up_left;

    _frame_painter = CreateStylePainter<FudgetDrawablePainter>(_frame_painter, (int)FudgetFramedControlPartIds::FramePainter);
    _item_painter = CreateStylePainter<FudgetListItemPainter>(_item_painter, (int)FudgetListBoxPartIds::ItemPainter);

    if (_frame_painter != nullptr)
        _top_item_pos += up_left;

    _v_scrollbar->StyleInitialize();
}

void FudgetListBox::OnDraw()
{
    if (_dirty_extents)
        RequestLayout();

    Rectangle bounds = GetBounds();
    if (_frame_painter != nullptr)
        _frame_painter->Draw(this, bounds, GetVisualState());

    if (_data == nullptr || _item_painter == nullptr)
        return;

    int count = _data->GetCount();
    if (count == 0)
        return;

    if (_v_scrollbar->IsVisible())
        _v_scrollbar->Draw();

    bounds = GetContentPadding().Padded(bounds);
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
        EnsureDefaultSize();
        return _default_size;
    }

    if (_item_heights.size() <= item_index)
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

void FudgetListBox::RequestLayout()
{
    if (_dirty_extents)
        RecalculateListExtents();
    Base::RequestLayout();
    _v_scrollbar->SetBounds(GetScrollBarBounds());
    _v_scrollbar->SetPageSize((int)GetContentPadding().Padded(GetBounds()).Size.Y);
}

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
        FudgetControlFlag::CanHandleKeyEvents | FudgetControlFlag::CanHandleNavigationKeys | Base::GetInitFlags();
}

FudgetPadding FudgetListBox::GetFramePadding() const
{
    return _frame_painter != nullptr ? _frame_painter->GetVisualPadding() : FudgetPadding(0);
}

FudgetPadding FudgetListBox::GetContentPadding() const
{
    if (_frame_painter != nullptr)
    {
        FudgetPadding pad = _frame_painter->GetContentPadding();
        pad.Right += _v_scrollbar->GetWidth();
        return pad;
    }
    return FudgetPadding(0);
}

Rectangle FudgetListBox::GetScrollBarBounds() const
{
    Rectangle r = GetFramePadding().Padded(GetBounds());
    float scroll_width = Math::Min(r.Size.X, (float)_v_scrollbar->GetWidth());
    r.Location.X += r.Size.X - scroll_width;
    r.Size.X = scroll_width;
    return r;
}

void FudgetListBox::EnsureDefaultSize()
{
    if (_item_painter != nullptr && _data != nullptr && _fixed_item_size && _default_size.Y <= 0 && _data->GetCount() > 0)
    {
        _default_size = _item_painter->Measure(this, 0, _data, 0);
        _default_size.X = (int)GetContentPadding().Padded(GetBounds()).GetWidth();

        MarkExtentsDirty();
        RecalculateListExtents();
    }
}

void FudgetListBox::RecalculateListExtents()
{
    if (!_dirty_extents)
        return;

    EnsureDefaultSize();

    if (_fixed_item_size)
    {
        _list_extents = Int2(_default_size.X, _default_size.Y * _data->GetCount());
        _v_scrollbar->SetScrollRange(_list_extents.Y);
        _dirty_extents = false;
        return;
    }

    _list_extents = Int2(_default_size.X, 0);
    for (int ix = 0, siz = (int)_item_heights.size(); ix < siz; ++ix)
    {
        int h = _item_heights[ix];
        if (h == -1)
            _list_extents.Y += _default_size.Y;
        else
            _list_extents.Y += _item_heights[ix];
    }

    _v_scrollbar->SetScrollRange(_list_extents.Y);
    _dirty_extents = false;
}

