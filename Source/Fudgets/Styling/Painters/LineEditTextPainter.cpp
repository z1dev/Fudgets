#include "LineEditTextPainter.h"
#include "../DrawableBuilder.h"
#include "../StateOrderBuilder.h"
#include "../../Control.h"

#include "Engine/Content/Content.h"
#include "Engine/Render2D/FontManager.h"


FudgetLineEditTextPainter::FudgetLineEditTextPainter(const SpawnParams &params) : Base(params),
    _sel_area(nullptr), _focused_sel_area(nullptr), _disabled_sel_area(nullptr),
    _text_color(Color::White), _disabled_text_color(Color::White), _selected_text_color(Color::White),
    _focused_selected_text_color(Color::White), _disabled_selected_text_color(Color::White), _state_order(nullptr)
{
}

void FudgetLineEditTextPainter::Initialize(FudgetControl *control, FudgetStyle *style_override, const Variant &mapping)
{
    if (control == nullptr)
        return;

    const ResourceMapping *ptrres = mapping.AsStructure<ResourceMapping>();
    ResourceMapping res;
    if (ptrres != nullptr)
        res = *ptrres;

    GetMappedStateOrder(res.StateOrderIndex, _state_order);

    if (!GetMappedDrawable(control, style_override, (int)FudgetLineEditTextPainterIds::SelectionDraw, res.SelectionDraw, _sel_area))
        _sel_area = FudgetDrawable::FromColor(Color(0.2f, 0.4f, 0.8f, 1.0f));
    if (!GetMappedDrawable(control, style_override, (int)FudgetLineEditTextPainterIds::FocusedSelectionDraw, res.FocusedSelectionDraw, _focused_sel_area))
        _focused_sel_area = _sel_area;
    if (!GetMappedDrawable(control, style_override, (int)FudgetLineEditTextPainterIds::DisabledSelectionDraw, res.DisabledSelectionDraw, _disabled_sel_area))
        _disabled_sel_area = _sel_area;

    if (!GetMappedColor(control, style_override, (int)FudgetLineEditTextPainterIds::TextColor, res.TextColor, _text_color))
        _text_color = Color::Black;
    if (!GetMappedColor(control, style_override, (int)FudgetLineEditTextPainterIds::DisabledTextColor, res.DisabledTextColor, _disabled_text_color))
        _disabled_text_color = _text_color;
    if (!GetMappedColor(control, style_override, (int)FudgetLineEditTextPainterIds::SelectedTextColor, res.SelectedTextColor, _selected_text_color))
        _selected_text_color = Color::White;
    if (!GetMappedColor(control, style_override, (int)FudgetLineEditTextPainterIds::FocusedSelectedTextColor, res.FocusedSelectedTextColor, _focused_selected_text_color))
        _focused_selected_text_color = _selected_text_color;
    if (!GetMappedColor(control, style_override, (int)FudgetLineEditTextPainterIds::DisabledSelectedTextColor, res.DisabledSelectedTextColor, _disabled_selected_text_color))
        _disabled_selected_text_color = _selected_text_color;

    if (!GetMappedFont(control, style_override, (int)FudgetLineEditTextPainterIds::Font, res.Font, _font))
    {
        _font.Settings = FudgetFontSettings(-1, 10.f, false, false);
        FontAsset *font_asset = Content::LoadAsyncInternal<FontAsset>(TEXT("Editor/Fonts/Roboto-Regular"));
        if (font_asset != nullptr)
            _font.Font = font_asset->CreateFont(10.f);
    }
}

void FudgetLineEditTextPainter::Draw(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState states, const FudgetSingleLineTextOptions &options)
{
    if (_font.Font == nullptr)
        return;

    uint64 matching_states = _state_order != nullptr ? _state_order->GetMatchingState((uint64)states) : 0;

    Color selTextColor = DrawDisabled(matching_states) ? _disabled_selected_text_color : DrawFocused(matching_states) ? _focused_selected_text_color : _selected_text_color;
    Color textColor = DrawDisabled(matching_states) ? _disabled_text_color : _text_color;

    TextLayoutOptions opt;
    opt.BaseLinesGapScale = 1;
    opt.Scale = options.Scale;
    opt.VerticalAlignment = options.VerticalAlignment;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.TextWrapping = TextWrapping::NoWrap;

    int caret_left = 0;
    float scale = options.Scale / FontManager::FontScale;
    if (options.Spans.IsInvalid() || options.Spans.Length() == 0 || (options.Spans.Length() > 0 && (options.Spans[0].RangeSpan.IsInvalid() || options.Spans[0].RangeSpan.Length() == 0)))
    {
        opt.Bounds = bounds;
        control->DrawText(_font.Font, text, range, textColor, opt);
    }
    else
    {
        TextRange range2;


        Rectangle r = bounds;
        opt.Bounds = r;

        int sel_min = options.Spans[0].RangeSpan[0].StartIndex;
        int sel_max = options.Spans[0].RangeSpan[0].EndIndex;

        range2.StartIndex = range.StartIndex;
        range2.EndIndex = Math::Min(range.EndIndex, sel_min);

        if (range2.StartIndex < range2.EndIndex)
        {
            control->DrawText(_font.Font, text, range2, textColor, opt);

            r.Location = Int2(r.Location.X + _font.Font->MeasureText(text, range2).X, r.Location.Y);
            if (range2.EndIndex < range.EndIndex)
            {
                Char prev_char = text[range2.EndIndex - 1];
                Char next_char = text[range2.EndIndex];
                r.Location.X += _font.Font->GetKerning(prev_char, next_char) * scale;
            }
            opt.Bounds = r;
        }

        range2.StartIndex = Math::Clamp(sel_min, range.StartIndex, range.EndIndex);
        range2.EndIndex = Math::Clamp(sel_max, range.StartIndex, range.EndIndex);

        if (range2.StartIndex < range2.EndIndex)
        {
            Float2 selRectSize = _font.Font->MeasureText(text, range2);
            FudgetDrawable *sel_bg = DrawDisabled(matching_states) ? _disabled_sel_area : DrawFocused(matching_states) ? _focused_sel_area : _sel_area;
            control->DrawDrawable(sel_bg, Rectangle(opt.Bounds.Location, Float2(selRectSize.X, opt.Bounds.Size.Y)));
            control->DrawText(_font.Font, text, range2, selTextColor, opt);

            if (range2.EndIndex < range.EndIndex && sel_max < range.EndIndex)
            {
                Char prev_char = text[range2.EndIndex - 1];
                Char next_char = text[range2.EndIndex];
                r.Location.X += _font.Font->GetKerning(prev_char, next_char) * scale;

                r.Location = Float2(r.Location.X + _font.Font->MeasureText(text, range2).X, r.Location.Y);
                opt.Bounds = r;
            }
        }

        if (sel_max < range.EndIndex)
        {
            range2.StartIndex = Math::Max(range.StartIndex, sel_max);
            range2.EndIndex = range.EndIndex;
            control->DrawText(_font.Font, text, range2, textColor, opt);
        }
    }
}

Int2 FudgetLineEditTextPainter::Measure(FudgetControl *control, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state, const FudgetSingleLineTextOptions &options)
{
    if (_font.Font == nullptr)
        return Int2::Zero;

    TextLayoutOptions opt;
    opt.BaseLinesGapScale = 1;
    opt.Scale = options.Scale;
    opt.VerticalAlignment = options.VerticalAlignment;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.TextWrapping = TextWrapping::NoWrap;
    opt.Bounds = Rectangle(Float2::Zero, Float2::Zero);

    return _font.Font->MeasureText(text, range, opt);
}

int FudgetLineEditTextPainter::GetKerning(Char a, Char b, float scale) const
{
    if (_font.Font == nullptr)
        return 0.f;

    scale = scale / FontManager::FontScale;
    return _font.Font->GetKerning(a, b) * scale;
}

int FudgetLineEditTextPainter::HitTest(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state, const FudgetSingleLineTextOptions &options, const Int2 &point)
{
    if (_font.Font == nullptr)
        return 0;

    TextLayoutOptions opt;
    opt.BaseLinesGapScale = 1;
    opt.Scale = options.Scale;
    opt.VerticalAlignment = options.VerticalAlignment;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.TextWrapping = TextWrapping::NoWrap;
    opt.Bounds = bounds;

    return _font.Font->HitTestText(text, range, point, opt);
}

int FudgetLineEditTextPainter::GetFontHeight() const
{
    if (_font.Font == nullptr)
        return 0.f;

    return _font.Font->GetHeight();
}
