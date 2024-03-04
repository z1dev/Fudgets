#include "ListBox.h"
#include "../Styling/Themes.h"


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


// FudgetListBoxItemPainter


FudgetListBoxItemPainter::FudgetListBoxItemPainter(const SpawnParams &params) : Base(params), _text_painter(nullptr)
{
}

FudgetListBoxItemPainter::~FudgetListBoxItemPainter()
{
    if (_text_painter != nullptr)
        Delete(_text_painter);
}

void FudgetListBoxItemPainter::Initialize(FudgetTheme *theme, FudgetStyle *style)
{
    Base::Initialize(theme, style);
    if (style == nullptr)
    {
        style = GetStyle();
        if (style == nullptr)
            return;
    }

    String text_style;
    if (!style->GetStringResource(theme, (int)FudgetListBoxItemPainterIds::TextStyle, text_style))
        text_style = String();

    _text_painter = style->CreatePainter<FudgetSingleLineTextPainter>(theme, (int)FudgetListBoxItemPainterIds::TextPainter);

    if (_text_painter != nullptr)
        _text_painter->Initialize(theme, FudgetThemes::GetStyle(text_style));
}

void FudgetListBoxItemPainter::Draw(FudgetControl *control, const Rectangle &bounds, Float2 offset, int item_index, IListDataProvider *data, FudgetVisualControlState state)
{
    if (_text_painter == nullptr || data == nullptr || item_index < 0 || item_index >= data->GetCount())
        return;

    String text = data->GetText(item_index);

    // TODO: make the range argument a pointer so it doesn't have to be passed for every painter function.

    FudgetTextRange full_range;
    full_range.StartIndex = 0;
    full_range.EndIndex = text.Length();

    FudgetSingleLineTextOptions opt;
    _text_painter->Draw(control, bounds, text, full_range, state, opt);
}

Float2 FudgetListBoxItemPainter::Measure(FudgetControl *control, int item_index, IListDataProvider *data, FudgetVisualControlState state)
{
    if (_text_painter == nullptr || data == nullptr || item_index < 0 || item_index >= data->GetCount())
        return Float2::Zero;

    String text = data->GetText(item_index);

    FudgetTextRange full_range;
    full_range.StartIndex = 0;
    full_range.EndIndex = text.Length();

    FudgetSingleLineTextOptions opt;
    return _text_painter->Measure(control, data->GetText(item_index), full_range, state, opt);
}


// FudgetListBox


FudgetListBox::FudgetListBox(const SpawnParams &params) : Base(params), _frame_painter(nullptr), _item_painter(nullptr),
    _data(nullptr), _owned_data(true), _current(-1), _scroll_pos(0.f), _fixed_item_size(true), _default_size(Float2(-1.f)), _size_processed(0)
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
    _frame_painter = CreateStylePainter<FudgetFramedFieldPainter>((int)FudgetListBoxIds::FramePainter);
    _item_painter = CreateStylePainter<FudgetListItemPainter, FudgetListBoxItemPainter>((int)FudgetListBoxIds::ItemPainter);
}

void FudgetListBox::OnStyleInitialize()
{
    FudgetStyle *frame_style;
    if (!GetStyleStyle((int)FudgetListBoxIds::FrameStyle, frame_style))
        frame_style = nullptr;

    
    FudgetStyle *item_style;
    if (!GetStyleStyle((int)FudgetListBoxIds::ItemStyle, item_style))
        item_style = nullptr;

    InitializeStylePainter(_frame_painter, frame_style);
    InitializeStylePainter(_item_painter, item_style);
}

void FudgetListBox::OnDraw()
{
    Rectangle bounds = GetBounds();
    if (_frame_painter != nullptr)
        _frame_painter->Draw(this, bounds, GetVisualState());

    if (_data == nullptr || _item_painter == nullptr)
        return;

    int count = _data->GetCount() == 0;
    if (count == 0)
        return;

    bounds = GetInnerPadding().Padded(bounds);

    float pos = 0.f;
    Rectangle r = Rectangle(Float2::Zero, Float2(bounds.GetWidth(), 0.f));
    for (int ix = 0; ix < count; ++ix)
    {
        r.Size.Y = GetItemSize(ix).Y;
        _item_painter->Draw(this, r, Float2(0.f, pos), ix, _data, GetVisualState());
        r.Location.Y += r.Size.Y;
    }
}

void FudgetListBox::SetDataProvider(FudgetStringListProvider *data)
{
    if (_data == data)
        return;

    if (_data != nullptr && _owned_data)
        Delete(_data);

    _owned_data = false;
    _data = data;
    _current = -1;
    _scroll_pos = Float2::Zero;
}

void FudgetListBox::SetDefaultItemSize(Float2 value)
{
    if (_default_size == value)
        return;
    _default_size = value;
}

Float2 FudgetListBox::GetItemSize(int index)
{
    if (_data == nullptr || _size_processed <= index)
        return _default_size;

    if (_item_heights.IsEmpty())
        return _default_size;

    return Float2(_default_size.X, _item_heights[index]);
}

void FudgetListBox::SetItemsHaveFixedSize(bool value)
{
    if (_fixed_item_size == value)
        return;
    _fixed_item_size = value;
}

FudgetPadding FudgetListBox::GetInnerPadding() const
{
    return _frame_painter != nullptr ? _frame_painter->GetContentPadding() : FudgetPadding(0.0f);
}

