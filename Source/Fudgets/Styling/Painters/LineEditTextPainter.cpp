#include "LineEditTextPainter.h"
#include "../DrawableBuilder.h"
#include "../../Control.h"

#include "Engine/Content/Content.h"
#include "Engine/Render2D/FontManager.h"


FudgetLineEditTextPainter::FudgetLineEditTextPainter(const SpawnParams &params) : Base(params), _draw(nullptr), _font()
{
}

void FudgetLineEditTextPainter::Initialize(FudgetControl *control, const Variant &mapping)
{
    Mapping res = *mapping.AsStructure<Mapping>();

    if (!CreateMappedDrawable(control, res.BackgroundDraw, _draw))
        _draw = FudgetDrawable::Empty;
    if (!GetMappedDrawColors(control, res.BackgroundTint, _draw_tint))
        _draw_tint = FudgetDrawColors();
    if (!GetMappedDrawColors(control, res.TextColor, _text_color))
        _text_color = FudgetDrawColors();

    if (!GetMappedFont(control, res.Font, _font))
    {
        _font.Settings = FudgetFontSettings(-1, 10.f, false, false);
        FontAsset *font_asset = Content::LoadAsyncInternal<FontAsset>(TEXT("Editor/Fonts/Roboto-Regular"));
        if (font_asset != nullptr)
            _font.Font = font_asset->CreateFont(10.f);
    }
}

void FudgetLineEditTextPainter::Draw(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, uint64 states, const FudgetSingleLineTextOptions &options)
{
    if (_font.Font == nullptr)
        return;


    Color text_color = _text_color.FindMatchingColor(states);
    Color sel_text_color = _text_color.FindMatchingColor(states | (uint64)FudgetVisualControlState::Selected);
    Color sel_bg_tint = _draw_tint.FindMatchingColor(states | (uint64)FudgetVisualControlState::Selected);
    FudgetDrawable *sel_bg = _draw;

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
        control->DrawText(_font.Font, text, range, text_color, opt);
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
            control->DrawText(_font.Font, text, range2, text_color, opt);

            r.Location = Int2(int(r.Location.X + _font.Font->MeasureText(text, range2).X), (int)r.Location.Y);
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
            control->DrawDrawable(sel_bg, sel_bg->FindMatchingState(states | (uint64)FudgetVisualControlState::Selected), Rectangle(opt.Bounds.Location, Float2(selRectSize.X, opt.Bounds.Size.Y)), sel_bg_tint);
            control->DrawText(_font.Font, text, range2, sel_text_color, opt);

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
            control->DrawText(_font.Font, text, range2, text_color, opt);
        }
    }
}

Int2 FudgetLineEditTextPainter::Measure(FudgetControl *control, const StringView &text, const FudgetTextRange &range, uint64 state, const FudgetSingleLineTextOptions &options)
{
    if (_font.Font == nullptr)
        return Int2::Zero;

    TextLayoutOptions opt;
    opt.BaseLinesGapScale = 1;
    opt.Scale = options.Scale;
    opt.VerticalAlignment = TextAlignment::Near;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.TextWrapping = TextWrapping::NoWrap;
    opt.Bounds = Rectangle(Float2::Zero, Float2::Zero);

    return _font.Font->MeasureText(text, range, opt);
}

int FudgetLineEditTextPainter::GetKerning(Char a, Char b, float scale) const
{
    if (_font.Font == nullptr)
        return 0;

    scale = scale / FontManager::FontScale;
    return int(_font.Font->GetKerning(a, b) * scale);
}

int FudgetLineEditTextPainter::HitTest(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, uint64 state, const FudgetSingleLineTextOptions &options, const Int2 &point)
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
        return 0;

    return _font.Font->GetHeight();
}
