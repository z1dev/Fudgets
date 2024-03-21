#include "TextBoxPainter.h"
#include "../DrawableBuilder.h"
#include "../../MarginStructs.h"
#include "../Style.h"
#include "../Themes.h"
#include "../StyleStructs.h"
#include "../../Control.h"

#include "Engine/Render2D/FontManager.h"
#include "Engine/Core/Log.h"
#include "Engine/Content/Content.h"



// FudgetMultiLineTextPainter


FudgetMultiLineTextPainter::FudgetMultiLineTextPainter(const SpawnParams &params) : Base(params)
{
}

void FudgetMultiLineTextPainter::AddLine(API_PARAM(Ref) Int2 &pos, int start_index, int end_index, int line_width, int line_height, API_PARAM(Ref) FudgetMultilineTextMeasurements &result)
{
    FudgetLineMeasurements line;
    line.StartIndex = start_index;
    line.EndIndex = end_index;
    line.Location = pos;
    line.Size = Int2(line_width, line_height);

    result.Lines.Add(line);
    result.Size.X = Math::Max(result.Size.X, line.Size.X);
    result.Size.Y += line_height;

    pos.Y += line_height;
}


// FudgetTextBoxPainter



FudgetTextBoxPainter::FudgetTextBoxPainter(const SpawnParams &params) : Base(params), _draw(nullptr), _font()
{
}

void FudgetTextBoxPainter::Initialize(FudgetControl *control, const Variant &mapping)
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

void FudgetTextBoxPainter::Draw(FudgetControl *control, const Rectangle &bounds, const Int2 &offset, uint64 states, const FudgetMultiLineTextOptions &options, const FudgetMultilineTextMeasurements &measurements)
{
    if (_font.Font == nullptr)
        return;

    int sel_min = -1; 
    int sel_max = -1; 
    if (options.Spans.IsValid() && options.Spans.Length() > 0 && options.Spans[0].RangeSpan.IsValid() && options.Spans[0].RangeSpan.Length() > 0)
    {
        sel_min = options.Spans[0].RangeSpan[0].StartIndex;
        sel_max = options.Spans[0].RangeSpan[0].EndIndex;
        if (sel_min == sel_max)
            sel_min = sel_max = -1;
    }

    TextLayoutOptions opt;
    opt.TextWrapping = TextWrapping::NoWrap;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.VerticalAlignment = TextAlignment::Center;
    opt.Scale = measurements.Scale;

    int text_len = measurements.Text.Length();

    float scale = measurements.Scale / FontManager::FontScale;

    Color text_color = _text_color.FindMatchingColor(states);
    Color sel_text_color = _text_color.FindMatchingColor(states | (uint64)FudgetVisualControlState::Selected);
    Color sel_draw_tint = _draw_tint.FindMatchingColor(states | (uint64)FudgetVisualControlState::Selected);
    int sel_bg_index = _draw->FindMatchingState(states | (uint64)FudgetVisualControlState::Selected);


    for (int ix = 0, siz = measurements.Lines.Count(); ix < siz; ++ix)
    {
        const FudgetLineMeasurements &line = measurements.Lines[ix];
        if (line.StartIndex < 0 || line.EndIndex > text_len)
        {
            LOG(Error, "FudgetTextBoxPainter draw line index out of range.");
            break;
        }

        if (line.Location.Y + line.Size.Y + offset.Y <= bounds.Location.Y)
            continue;
        if (line.Location.Y + offset.Y >= bounds.GetBottom())
            break;

        if (line.StartIndex >= sel_max || line.EndIndex <= sel_min)
        {
            int len = line.EndIndex - line.StartIndex;
            opt.Bounds = Rectangle(line.Location + bounds.Location + offset, line.Size);
            
            control->DrawText(_font.Font, StringView(measurements.Text.Get() + line.StartIndex, len), text_color, opt);
            continue;
        }

        int skip_width = 0;

        // Line before selection 

        if (line.StartIndex < sel_min)
        {
            int len = sel_min - line.StartIndex;
            opt.Bounds = Rectangle(line.Location + bounds.Location + offset, line.Size);

            skip_width = Math::CeilToInt(_font.Font->MeasureText(StringView(measurements.Text.Get() + line.StartIndex, len), opt).X);

            if (line.EndIndex < sel_max)
                skip_width += Math::CeilToInt(_font.Font->GetKerning(measurements.Text[sel_min - 1], measurements.Text[sel_min]) * scale);

            control->DrawText(_font.Font, StringView(measurements.Text.Get() + line.StartIndex, len), text_color, opt);
        }

        // Selection
        {
            int sel_pos = Math::Max(sel_min, line.StartIndex);
            int sel_end = Math::Min(line.EndIndex, sel_max);
            int sel_len = sel_end - sel_pos;
            opt.Bounds = Rectangle(line.Location + bounds.Location + offset + Int2(skip_width, 0), line.Size - Int2(skip_width, 0));

            bool full_line = sel_min == line.StartIndex && sel_end == line.EndIndex;
            int width = full_line ? line.Size.X : Math::CeilToInt(_font.Font->MeasureText(StringView(measurements.Text.Get() + sel_pos, sel_len), opt).X);
            opt.Bounds.Size.X = (float)width;

            skip_width += width;
            if (line.EndIndex > sel_max)
                skip_width += Math::CeilToInt(_font.Font->GetKerning(measurements.Text[sel_end - 1], measurements.Text[sel_end]) * scale);

            control->DrawDrawable(_draw, sel_bg_index, opt.Bounds, sel_draw_tint);
            control->DrawText(_font.Font, StringView(measurements.Text.Get() + sel_pos, sel_len), sel_text_color, opt);
        }

        // Line after selection
        if (line.EndIndex > sel_max)
        {
            int len = line.EndIndex - sel_max;
            opt.Bounds = Rectangle(line.Location + bounds.Location + offset + Int2(skip_width, 0), line.Size - Int2(skip_width, 0));

            control->DrawText(_font.Font, StringView(measurements.Text.Get() + sel_max, len), text_color, opt);
        }
    }
}

int FudgetTextBoxPainter::GetKerning(Char a, Char b, float scale) const
{
    if (_font.Font == nullptr)
        return 0;

    scale = scale / FontManager::FontScale;
    return Math::CeilToInt(_font.Font->GetKerning(a, b) * scale);
}

int FudgetTextBoxPainter::HitTest(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, const Int2 &point)
{
    if (_font.Font == nullptr)
        return 0;

    int text_len = measurements.Text.Length();

    for (int ix = 0, siz = measurements.Lines.Count(); ix < siz; ++ix)
    {
        const FudgetLineMeasurements &line = measurements.Lines[ix];
        if (line.StartIndex < 0 || line.EndIndex > text_len)
        {
            LOG(Error, "FudgetTextBoxPainter HitTest index out of range.");
            break;
        }

        int y_distance = point.Y >= line.Location.Y + line.Size.Y ? point.Y - (line.Location.Y + line.Size.Y) : -1;
        if (y_distance >= 0 && ix < siz - 1 && (point.Y >= measurements.Lines[ix + 1].Location.Y || y_distance >= measurements.Lines[ix + 1].Location.Y - point.Y))
            continue;
        
        if (point.X < line.Location.X)
            return line.StartIndex;
        if (point.X > line.Location.X + line.Size.X)
            return line.EndIndex;

        return LineHitTest(control, measurements, ix, point.X);
    }

    return 0;
}

int FudgetTextBoxPainter::LineAtPos(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, int y_position)
{
    if (_font.Font == nullptr)
        return 0;

    int text_len = measurements.Text.Length();

    for (int ix = 0, siz = measurements.Lines.Count(); ix < siz; ++ix)
    {
        const FudgetLineMeasurements &line = measurements.Lines[ix];
        if (line.StartIndex < 0 || line.EndIndex > text_len)
        {
            LOG(Error, "FudgetTextBoxPainter HitTest index out of range.");
            break;
        }

        int y_distance = y_position >= line.Location.Y + line.Size.Y ? y_position - (line.Location.Y + line.Size.Y) : -1;
        if (y_distance >= 0 && ix < siz - 1 && (y_position >= measurements.Lines[ix + 1].Location.Y || y_distance >= measurements.Lines[ix + 1].Location.Y - y_position))
            continue;

        return ix;
    }

    return 0;
}

int FudgetTextBoxPainter::LineHitTest(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, int line_index, int x_position)
{
    if (line_index < 0 || line_index >= measurements.Lines.Count())
    {
        LOG(Error, "FudgetTextBoxPainter LineHitTest index out of range.");
        return 0;
    }

    const FudgetLineMeasurements &line = measurements.Lines[line_index];

    Int2 pt = Int2(x_position, Math::Max(1, (int)(line.Size.Y * .5f)));

    TextLayoutOptions opt;
    opt.TextWrapping = TextWrapping::NoWrap;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.VerticalAlignment = TextAlignment::Center;
    opt.Scale = measurements.Scale;
    opt.Bounds = Rectangle(Float2(line.Location), Float2(line.Size));
    return _font.Font->HitTestText(StringView(measurements.Text.Get() + line.StartIndex, line.EndIndex - line.StartIndex), pt, opt) + line.StartIndex;
}

Int2 FudgetTextBoxPainter::GetCharacterPosition(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, int char_index) const
{
    int text_len = measurements.Text.Length();
    if (char_index < 0 || char_index > text_len)
        return Int2::Zero;

    float scale = measurements.Scale / FontManager::FontScale;

    int last_endindex = 0;

    TextLayoutOptions opt;
    opt.TextWrapping = TextWrapping::NoWrap;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.VerticalAlignment = TextAlignment::Center;
    opt.Scale = measurements.Scale;

    for (int ix = 0, siz = measurements.Lines.Count(); ix < siz; ++ix)
    {
        const FudgetLineMeasurements &line = measurements.Lines[ix];
        if (line.StartIndex < 0 || line.EndIndex > text_len)
        {
            LOG(Error, "FudgetTextBoxPainter GetCharacterPosition index out of range.");
            break;
        }

        if (line.EndIndex < char_index)
        {
            last_endindex = line.EndIndex;
            continue;
        }

        if (char_index < line.StartIndex)
            return line.Location;

        opt.Bounds = Rectangle(Float2(line.Location), Float2(line.Size));
        int measured = Math::CeilToInt(_font.Font->MeasureText(StringView(measurements.Text.Get() + line.StartIndex, char_index - line.StartIndex), opt).X);

        return line.Location + Int2(measured, 0);
    }

    return Int2::Zero;
}

Int2 FudgetTextBoxPainter::Measure(FudgetControl *control, const StringView &text, float scale)
{
    if (_font.Font == nullptr)
        return Int2::Zero;

    TextLayoutOptions opt;
    opt.BaseLinesGapScale = 1;
    opt.Scale = scale;
    opt.VerticalAlignment = TextAlignment::Near;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.TextWrapping = TextWrapping::NoWrap;
    opt.Bounds = Rectangle(Float2::Zero, Float2::Zero);

    return _font.Font->MeasureText(text, opt);
}

void FudgetTextBoxPainter::MeasureLines(FudgetControl *control, int bounds_width, const StringView &text, float scale, const FudgetMultiLineTextOptions &options, API_PARAM(Ref) FudgetMultilineTextMeasurements &result)
{
    result.Size = Int2::Zero;
    result.Lines.Clear();
    result.Text = text;
    result.Scale = scale;

    if (_font.Font == nullptr)
        return;

    scale = scale / FontManager::FontScale;
    int line_height = Math::CeilToInt(_font.Font->GetHeight() * scale);

    Int2 pos = Int2::Zero;

    FontCharacterEntry current;
    FontCharacterEntry prev;

    bool word_wrap = options.Wrapping && (options.WrapMode == FudgetLineWrapMode::Whitespace || options.WrapMode == FudgetLineWrapMode::WhitespaceLongWord);

    // First character of the line
    int line_first = 0;

    // Index of last whitespace character in the line or -1 if no whitespace character was found yet
    int last_whitespace = -1;
    // Index of the character in the word to break at, if long word breaking is used.
    int charbreak = -1;

    // Width of the first word(s) in the line until the breaking whitespace.
    int first_width = 0;
    // Width of the breaking whitespace including previous and next kerning
    int space_width = 0;
    // Width of the word after the breaking whitespace up until the line end is reached.
    int second_width = 0;
    // Kerning width after the last fitting character on the current line
    int kerning_width = 0;
    // Width of the word after the last fitting character until it is too long to fit a line or a
    // whitespace is encountered early.
    int third_width = 0;

    for (int ix = 0, last = text.Length(); ix < last; ++ix)
    {
        if (text[ix] == L'\n')
        {
            if (charbreak != -1)
            {
                // Only reached if word wrapping is true and its the long word breaking mode.
                // Encountering a newline means, the word we tested for being too long isn't
                // too long, and it will be on its own line.

                if (last_whitespace == -1)
                {
                    AddLine(pos, line_first, charbreak, first_width, line_height, result);
                    AddLine(pos, charbreak, ix, third_width, line_height, result);
                }
                else
                {
                    AddLine(pos, line_first, last_whitespace, first_width, line_height, result);
                    AddLine(pos, last_whitespace + 1, ix, second_width + kerning_width + third_width, line_height, result);
                }

            }
            else
            {
                AddLine(pos, line_first, ix, first_width + space_width + second_width, line_height, result);
            }

            line_first = ix + 1;
            last_whitespace = -1;
            charbreak = -1;
            first_width = 0;
            space_width = 0;
            second_width = 0;
            kerning_width = 0;
            third_width = 0;

            prev.IsValid = false;

            continue;
        }

        _font.Font->GetCharacter(text[ix], current);

        if (options.Wrapping && options.WrapMode != FudgetLineWrapMode::Anywhere && options.WrapMode != FudgetLineWrapMode::Custom)
        {
            // We only care about whitespace in one of the word wrap modes.

            if (StringUtils::IsWhitespace(current.Character))
            {
                if (charbreak == -1)
                {
                    last_whitespace = ix;

                    first_width += space_width + second_width;
                    space_width = 0;
                    second_width = 0;
                }
                else if (last_whitespace == -1)
                {
                    AddLine(pos, line_first, charbreak, first_width, line_height, result);

                    line_first = charbreak;
                    last_whitespace = ix;
                    charbreak = -1;
                    first_width = third_width;
                    space_width = 0;
                    second_width = 0;
                    kerning_width = 0;
                    third_width = 0;
                }
                else
                {
                    AddLine(pos, line_first, last_whitespace, first_width, line_height, result);

                    line_first = last_whitespace + 1;
                    last_whitespace = ix;
                    charbreak = -1;
                    first_width = second_width + kerning_width + third_width;
                    space_width = 0;
                    second_width = 0;
                    kerning_width = 0;
                    third_width = 0;
                }
                
            }
        }

        int kerning = prev.IsValid ? _font.Font->GetKerning(prev.Character, current.Character) : 0;
        int next_width = 0;
        if (ix == last_whitespace)
        {
            space_width = Math::CeilToInt((current.AdvanceX + kerning) * scale);
            kerning = 0;
        }
        else if (last_whitespace != -1 && ix == last_whitespace + 1)
        {
            space_width += Math::CeilToInt(kerning * scale);
            kerning = 0;
            next_width = Math::CeilToInt(current.AdvanceX * scale);
        }
        else
        {
            next_width = Math::CeilToInt(current.AdvanceX * scale);
            kerning = Math::CeilToInt(kerning * scale);
        }

        prev = current;

        if (charbreak != -1)
        {
            if (second_width + kerning_width + third_width + kerning + next_width > bounds_width)
            {
                // Second word is long enough to break and the second part be its own line

                AddLine(pos, line_first, charbreak, first_width + space_width + second_width, line_height, result);

                if (last_whitespace == -1)
                {
                    AddLine(pos, charbreak, ix, third_width, line_height, result);

                    line_first = ix;
                    charbreak = -1;
                    first_width = next_width;
                    space_width = 0;
                    second_width = 0;
                    kerning_width = 0;
                    third_width = 0;
                }
                else
                {
                    line_first = charbreak;
                    last_whitespace = -1;
                    charbreak = -1;
                    first_width = third_width + kerning + next_width;
                    space_width = 0;
                    second_width = 0;
                    kerning_width = 0;
                    third_width = 0;

                    prev.IsValid = false;
                }
            }
            else
            {
                third_width += kerning + next_width;
            }
        }
        else if (options.Wrapping && ix > line_first && first_width + space_width + second_width + kerning + next_width > bounds_width)
        {
            if (options.WrapMode == FudgetLineWrapMode::WhitespaceLongWord && ix != last_whitespace)
            {
                charbreak = ix;
                kerning_width = kerning;
                third_width = next_width;
                continue;
            }
            else if (options.WrapMode == FudgetLineWrapMode::Whitespace || (options.WrapMode == FudgetLineWrapMode::WhitespaceLongWord && ix == last_whitespace))
            {
                AddLine(pos, line_first, last_whitespace, first_width, line_height, result);

                line_first = last_whitespace + 1;
                last_whitespace = -1;
                charbreak = -1;
                first_width = second_width + kerning + next_width;
                space_width = 0;
                second_width = 0;
                kerning_width = 0;
                third_width = 0;

                prev.IsValid = false;

                continue;
            }
            else if (options.WrapMode == FudgetLineWrapMode::Anywhere)
            {
                AddLine(pos, line_first, ix, first_width, line_height, result);

                line_first = ix;
                last_whitespace = -1;
                charbreak = -1;
                first_width = next_width;
                space_width = 0;
                second_width = 0;
                kerning_width = 0;
                third_width = 0;

                prev.IsValid = false;

                continue;
            }
        }
        else if (last_whitespace == -1)
        {
            first_width += next_width + kerning;
        }
        else
        {
            second_width += next_width + kerning;
        }

    }

    if (charbreak != -1)
    {
        // Only reached if word wrapping is true and its the long word breaking mode.
        // Encountering the end of text means the word we tested for being too long isn't
        // too long, and it will be on its own line.

        if (last_whitespace == -1)
        {
            AddLine(pos, line_first, charbreak, first_width, line_height, result);
            AddLine(pos, charbreak, text.Length(), third_width, line_height, result);
        }
        else
        {
            AddLine(pos, line_first, last_whitespace, first_width, line_height, result);
            AddLine(pos, last_whitespace + 1, text.Length(), second_width + kerning_width + third_width, line_height, result);
        }

    }
    else
    {
        AddLine(pos, line_first, text.Length(), first_width + space_width + second_width, line_height, result);
    }

}

int FudgetTextBoxPainter::GetCharacterLine(FudgetMultilineTextMeasurements &measurements, int char_index) const
{
    for (int ix = 0, siz = measurements.Lines.Count(); ix < siz; ++ix)
    {
        const FudgetLineMeasurements &line = measurements.Lines[ix];
        if (line.EndIndex >= char_index)
            return ix;
    }
    return measurements.Lines.Count();
}

int FudgetTextBoxPainter::GetCharacterLineHeight(const FudgetMultilineTextMeasurements &measurements, int char_index) const
{
    float scale = measurements.Scale / FontManager::FontScale;
    return Math::CeilToInt(_font.Font->GetHeight() * scale);
}

int FudgetTextBoxPainter::GetFontHeight() const
{
    if (_font.Font == nullptr)
        return 0;

    return _font.Font->GetHeight();
}

