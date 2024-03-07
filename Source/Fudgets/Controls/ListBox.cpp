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
}

void FudgetListBoxItemPainter::Initialize(FudgetControl *control, FudgetStyle *style_override, const Variant &mapping)
{
    if (control == nullptr)
        return;

    const ResourceMapping *ptrres = mapping.AsStructure<ResourceMapping>();
    ResourceMapping res;
    if (ptrres != nullptr)
        res = *ptrres;

    FudgetStyle *text_style = nullptr;
    if (!GetMappedStyle(control, style_override, (int)FudgetListBoxItemPainterIds::TextStyle, res.TextStyle, text_style))
        text_style = nullptr;

    _text_painter = control->CreateStylePainter<FudgetSingleLineTextPainter>(_text_painter, res.TextPainter, text_style, nullptr, style_override);
    if (_text_painter == nullptr)
    {
        FudgetLineEditTextPainter::ResourceMapping tex_res;
        tex_res.SelectionDraw = res.SelectionDraw;
        tex_res.FocusedSelectionDraw = res.SelectionDraw;
        tex_res.DisabledSelectionDraw = res.DisabledSelectionDraw;
        tex_res.TextColor = res.TextColor;
        tex_res.DisabledTextColor = res.DisabledTextColor;
        tex_res.SelectedTextColor = res.SelectedTextColor;
        tex_res.FocusedSelectedTextColor = res.SelectedTextColor;
        tex_res.DisabledSelectedTextColor = res.DisabledSelectedTextColor;
        tex_res.Font = res.Font;
        FudgetPartPainterMapping def_text_mapping = FudgetPartPainter::InitializeMapping<FudgetLineEditTextPainter>(tex_res);
        _text_painter = control->CreateStylePainter<FudgetSingleLineTextPainter>(_text_painter, (int)FudgetListBoxItemPainterIds::TextPainter, text_style, &def_text_mapping, style_override);
    }
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
    FudgetFramedFieldPainter::ResourceMapping frame_res;
    frame_res.FrameDraw = (int)FudgetListBoxIds::FrameDraw;
    frame_res.HoveredFrameDraw = (int)FudgetListBoxIds::FrameDraw;
    frame_res.PressedFrameDraw = (int)FudgetListBoxIds::FrameDraw;
    frame_res.DownFrameDraw = (int)FudgetListBoxIds::FrameDraw;
    frame_res.DisabledFrameDraw = (int)FudgetListBoxIds::DisabledFrameDraw;
    frame_res.FocusedFrameDraw = (int)FudgetListBoxIds::FocusedFrameDraw;
    frame_res.ContentPadding = (int)FudgetListBoxIds::ContentPadding;
    _default_frame_painter_mapping = FudgetPartPainter::InitializeMapping<FudgetFramedFieldPainter>(frame_res);

    FudgetListBoxItemPainter::ResourceMapping item_res;

    item_res.TextPainter = (int)FudgetListBoxIds::TextPainter;
    item_res.TextStyle = (int)FudgetListBoxIds::TextStyle;

    item_res.SelectionDraw = (int)FudgetListBoxIds::SelectionDraw;
    item_res.DisabledSelectionDraw = (int)FudgetListBoxIds::DisabledSelectionDraw;
    item_res.TextColor = (int)FudgetListBoxIds::TextColor;
    item_res.DisabledTextColor = (int)FudgetListBoxIds::DisabledTextColor;
    item_res.SelectedTextColor = (int)FudgetListBoxIds::SelectedTextColor;
    item_res.DisabledSelectedTextColor = (int)FudgetListBoxIds::DisabledSelectedTextColor;

    item_res.Font = (int)FudgetListBoxIds::Font;

    _default_item_painter_mapping = FudgetPartPainter::InitializeMapping<FudgetListBoxItemPainter>(item_res);
}

void FudgetListBox::OnStyleInitialize()
{
    FudgetStyle *frame_style;
    if (!GetStyleStyle((int)FudgetListBoxIds::FrameStyle, frame_style))
        frame_style = nullptr;
    _frame_painter = CreateStylePainter<FudgetFramedFieldPainter>(_frame_painter, (int)FudgetListBoxIds::FramePainter, frame_style, &_default_frame_painter_mapping);

    FudgetStyle *item_style;
    if (!GetStyleStyle((int)FudgetListBoxIds::ItemStyle, item_style))
        item_style = nullptr;
    _item_painter = CreateStylePainter<FudgetListItemPainter>(_item_painter, (int)FudgetListBoxIds::ItemPainter, item_style, &_default_item_painter_mapping);
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

