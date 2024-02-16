#include "TextBoxPainter.h"
#include "../MarginStructs.h"
#include "../Styling/Style.h"
#include "../Styling/Themes.h"
#include "../Styling/StyleStructs.h"
#include "../Control.h"

#include "Engine/Render2D/FontManager.h"
#include "Engine/Core/Log.h"



// FudgetMultiLineTextPainter


FudgetMultiLineTextPainter::FudgetMultiLineTextPainter(const SpawnParams &params) : Base(params)
{
}


// FudgetTextBoxPainter


FudgetToken FudgetTextBoxPainter::SelfToken = -1;

FudgetToken FudgetTextBoxPainter::SelectionDrawToken = -1;
FudgetToken FudgetTextBoxPainter::FocusedSelectionDrawToken = -1;
FudgetToken FudgetTextBoxPainter::DisabledSelectionDrawToken = -1;
FudgetToken FudgetTextBoxPainter::TextColorToken = -1;
FudgetToken FudgetTextBoxPainter::DisabledTextColorToken = -1;
FudgetToken FudgetTextBoxPainter::SelectedTextColorToken = -1;
FudgetToken FudgetTextBoxPainter::FocusedSelectedTextColorToken = -1;
FudgetToken FudgetTextBoxPainter::DisabledSelectedTextColorToken = -1;
FudgetToken FudgetTextBoxPainter::FontToken = -1;

void FudgetTextBoxPainter::CreateStyle()
{
    SelfToken = FudgetThemes::RegisterToken(TEXT("Fudgets.FudgetTextBoxPainter"));

    SelectionDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetTextBoxPainter_SelectionDrawToken"));
    FocusedSelectionDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetTextBoxPainter_FocusedSelectionDrawToken"));
    DisabledSelectionDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetTextBoxPainter_DisabledSelectionDrawToken"));
    TextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetTextBoxPainter_TextColorToken"));
    DisabledTextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetTextBoxPainter_DisabledTextColorToken"));
    SelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetTextBoxPainter_SelectedTextColorToken"));
    FocusedSelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetTextBoxPainter_FocusedSelectedTextColorToken"));
    DisabledSelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetTextBoxPainter_DisabledSelectedTextColorToken"));
    FontToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetTextBoxPainter_FontToken"));

    FudgetStyle *style = FudgetThemes::CreateStyle(SelfToken);
    if (style == nullptr)
        return;

    style->SetResourceOverride(SelectionDrawToken, SelectionDrawToken);
    style->SetResourceOverride(FocusedSelectionDrawToken, FocusedSelectionDrawToken);
    style->SetResourceOverride(DisabledSelectionDrawToken, DisabledSelectionDrawToken);
    style->SetResourceOverride(TextColorToken, TextColorToken);
    style->SetResourceOverride(DisabledTextColorToken, DisabledTextColorToken);
    style->SetResourceOverride(SelectedTextColorToken, SelectedTextColorToken);
    style->SetResourceOverride(FocusedSelectedTextColorToken, FocusedSelectedTextColorToken);
    style->SetResourceOverride(DisabledSelectedTextColorToken, DisabledSelectedTextColorToken);

    style->SetResourceOverride(FontToken, FontToken);
}

void FudgetTextBoxPainter::Initialize(FudgetTheme *theme, FudgetStyle *style)
{
    Base::Initialize(theme, style);
    if (style == nullptr)
    {
        style = GetStyle();
        if (style == nullptr)
            return;
    }

    if (!style->GetDrawAreaResource(theme, SelectionDrawToken, _sel_area))
        _sel_area = FudgetDrawArea(Color(0.2f, 0.4f, 0.8f, 1.0f));
    if (!style->GetDrawAreaResource(theme, FocusedSelectionDrawToken, _focused_sel_area))
        _focused_sel_area = _sel_area;
    if (!style->GetDrawAreaResource(theme, DisabledSelectionDrawToken, _disabled_sel_area))
        _disabled_sel_area = _sel_area;

    if (!style->GetColorResource(theme, TextColorToken, _text_color))
        _text_color = Color::Black;
    if (!style->GetColorResource(theme, DisabledTextColorToken, _disabled_text_color))
        _disabled_text_color = _text_color;
    if (!style->GetColorResource(theme, SelectedTextColorToken, _selected_text_color))
        _selected_text_color = Color::White;
    if (!style->GetColorResource(theme, FocusedSelectedTextColorToken, _focused_selected_text_color))
        _focused_selected_text_color = _selected_text_color;
    if (!style->GetColorResource(theme, DisabledSelectedTextColorToken, _disabled_selected_text_color))
        _disabled_selected_text_color = _selected_text_color;

    style->GetFontResource(theme, FontToken, _font);
}


FudgetTextBoxPainter::FudgetTextBoxPainter(const SpawnParams &params) : Base(params),
_text_color(Color::White), _disabled_text_color(Color::White), _selected_text_color(Color::White),
_focused_selected_text_color(Color::White), _disabled_selected_text_color(Color::White)
{
}

void FudgetTextBoxPainter::Draw(FudgetControl *control, const Rectangle &bounds, const Float2 &offset, const FudgetPainterStateHelper &state, const FudgetMultiLineTextOptions &options, const FudgetMultilineTextMeasurements &measurements)
{
    if (_font.Font == nullptr)
        return;

    Color selTextColor = !state.Enabled() ? _disabled_selected_text_color : state.Focused() ? _focused_selected_text_color : _selected_text_color;
    Color textColor = !state.Enabled() ? _disabled_text_color : _text_color;
    FudgetDrawArea sel_bg = !state.Enabled() ? _disabled_sel_area : state.Focused() ? _focused_sel_area : _sel_area;

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
            
            control->DrawText(_font.Font, StringView(measurements.Text.Get() + line.StartIndex, len), textColor, opt);
            continue;
        }

        float skip_width = 0.f;

        // Line before selection 

        if (line.StartIndex < sel_min)
        {
            int len = sel_min - line.StartIndex;
            opt.Bounds = Rectangle(line.Location + bounds.Location + offset, line.Size);

            skip_width = _font.Font->MeasureText(StringView(measurements.Text.Get() + line.StartIndex, len), opt).X;

            if (line.EndIndex < sel_max)
                skip_width += _font.Font->GetKerning(measurements.Text[sel_min - 1], measurements.Text[sel_min]) * scale;

            control->DrawText(_font.Font, StringView(measurements.Text.Get() + line.StartIndex, len), textColor, opt);
        }

        // Selection
        {
            int sel_pos = Math::Max(sel_min, line.StartIndex);
            int sel_end = Math::Min(line.EndIndex, sel_max);
            int sel_len = sel_end - sel_pos;
            opt.Bounds = Rectangle(line.Location + bounds.Location + offset + Float2(skip_width, 0.f), line.Size - Float2(skip_width, 0.f));

            float width = _font.Font->MeasureText(StringView(measurements.Text.Get() + sel_pos, sel_len), opt).X;
            opt.Bounds.Size.X = width;

            skip_width += width;
            if (line.EndIndex > sel_max)
                skip_width += _font.Font->GetKerning(measurements.Text[sel_end - 1], measurements.Text[sel_end]) * scale;

            control->DrawArea(sel_bg, opt.Bounds);
            control->DrawText(_font.Font, StringView(measurements.Text.Get() + sel_pos, sel_len), selTextColor, opt);
        }

        // Line after selection
        if (line.EndIndex > sel_max)
        {
            int len = line.EndIndex - sel_max;
            opt.Bounds = Rectangle(line.Location + bounds.Location + offset + Float2(skip_width, 0.f), line.Size - Float2(skip_width, 0.f));

            control->DrawText(_font.Font, StringView(measurements.Text.Get() + sel_max, len), textColor, opt);
        }
    }
}

float FudgetTextBoxPainter::GetKerning(Char a, Char b, float scale) const
{
    if (_font.Font == nullptr)
        return 0;

    scale = scale / FontManager::FontScale;
    return _font.Font->GetKerning(a, b) * scale;
}

int FudgetTextBoxPainter::HitTest(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, const Float2 &point)
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

        float y_distance = point.Y > line.Location.Y + line.Size.Y ? point.Y - (line.Location.Y + line.Size.Y) : 0.f;
        if (y_distance > 0 && ix < siz - 1 && (point.Y >= measurements.Lines[ix + 1].Location.Y || y_distance > measurements.Lines[ix + 1].Location.Y - point.Y))
            continue;
        
        if (point.X < line.Location.X)
            return line.StartIndex;
        if (point.X > line.Location.X + line.Size.X)
            return line.EndIndex;

        return LineHitTest(control, measurements, ix, point.X);
    }

    return 0;
}

int FudgetTextBoxPainter::LineAtPos(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, float y_position)
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

        float y_distance = y_position > line.Location.Y + line.Size.Y ? y_position - (line.Location.Y + line.Size.Y) : 0.f;
        if (y_distance > 0 && ix < siz - 1 && (y_position >= measurements.Lines[ix + 1].Location.Y || y_distance > measurements.Lines[ix + 1].Location.Y - y_position))
            continue;

        return ix;
    }

    return 0;
}

int FudgetTextBoxPainter::LineHitTest(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, int line_index, float x_position)
{
    if (line_index < 0 || line_index >= measurements.Lines.Count())
    {
        LOG(Error, "FudgetTextBoxPainter LineHitTest index out of range.");
        return 0;
    }

    const FudgetLineMeasurements &line = measurements.Lines[line_index];

    Float2 pt = Float2(x_position, line.Size.Y * .5f);

    TextLayoutOptions opt;
    opt.TextWrapping = TextWrapping::NoWrap;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.VerticalAlignment = TextAlignment::Center;
    opt.Scale = measurements.Scale;
    opt.Bounds = Rectangle(line.Location, line.Size);
    return _font.Font->HitTestText(StringView(measurements.Text.Get() + line.StartIndex, line.EndIndex - line.StartIndex), pt, opt) + line.StartIndex;
}

Float2 FudgetTextBoxPainter::GetCharacterPosition(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, int char_index) const
{
    int text_len = measurements.Text.Length();
    if (char_index < 0 || char_index > text_len)
        return Float2::Zero;

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

        opt.Bounds = Rectangle(line.Location, line.Size);
        float measured = _font.Font->MeasureText(StringView(measurements.Text.Get() + line.StartIndex, char_index - line.StartIndex), opt).X;

        return line.Location + Float2(measured, 0.0f);
    }

    return Float2::Zero;
}

Float2 FudgetTextBoxPainter::Measure(FudgetControl *control, const StringView &text, float scale)
{
    if (_font.Font == nullptr)
        return Float2::Zero;

    TextLayoutOptions opt;
    opt.BaseLinesGapScale = 1;
    opt.Scale = scale;
    opt.VerticalAlignment = TextAlignment::Near;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.TextWrapping = TextWrapping::NoWrap;
    opt.Bounds = Rectangle(Float2::Zero, Float2::Zero);

    return _font.Font->MeasureText(text, opt);
}

void FudgetTextBoxPainter::MeasureLines(FudgetControl *control, float bounds_width, const StringView &text, float scale, const FudgetMultiLineTextOptions &options, API_PARAM(Ref) FudgetMultilineTextMeasurements &result)
{
    result.Size = Float2::Zero;
    result.Lines.Clear();
    result.Text = text;
    result.Scale = scale;

    if (_font.Font == nullptr)
        return;

    scale = scale / FontManager::FontScale;
    float line_height = _font.Font->GetHeight() * scale;

    float pos_x = 0.f;
    float pos_y = 0.f;

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
    float first_width = 0.f;
    // Width of the breaking whitespace including previous and next kerning
    float space_width = 0.f;
    // Width of the word after the breaking whitespace up until the line end is reached.
    float second_width = 0.f;
    // Kerning width after the last fitting character on the current line
    float kerning_width = 0.f;
    // Width of the word after the last fitting character until it is too long to fit a line or a
    // whitespace is encountered early.
    float third_width = 0.f;

    for (int ix = 0, last = text.Length(); ix < last; ++ix)
    {
        if (text[ix] == L'\n')
        {
            if (charbreak != -1)
            {
                // Only reached if word wrapping is true and its the long word breaking mode.
                // Encountering a newline means, the word we tested for being too long isn't
                // too long, and it will be on its own line.

                FudgetLineMeasurements line;
                line.StartIndex = line_first;
                line.EndIndex = last_whitespace;
                line.Location = Float2(pos_x, pos_y);
                line.Size = Float2(first_width, line_height);

                result.Lines.Add(line);
                result.Size.X = Math::Max(result.Size.X, line.Size.X);
                result.Size.Y += line_height;

                pos_y += line_height;

                line.StartIndex = last_whitespace + 1;
                line.EndIndex = ix;
                line.Location = Float2(pos_x, pos_y);
                line.Size = Float2(second_width + kerning_width + third_width, line_height);

                result.Lines.Add(line);
                result.Size.X = Math::Max(result.Size.X, line.Size.X);
                result.Size.Y += line_height;

                prev.IsValid = false;
            }
            else
            {
                FudgetLineMeasurements line;
                line.StartIndex = line_first;
                line.EndIndex = ix;
                line.Location = Float2(pos_x, pos_y);
                line.Size = Float2(first_width + space_width + second_width, line_height);

                result.Lines.Add(line);
                result.Size.X = Math::Max(result.Size.X, line.Size.X);
                result.Size.Y += line_height;

            }
            pos_y += line_height;

            line_first = ix + 1;
            last_whitespace = -1;
            charbreak = -1;
            first_width = 0.f;
            space_width = 0.f;
            second_width = 0.f;
            kerning_width = 0.f;
            third_width = 0.f;

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
                    space_width = 0.f;
                    second_width = 0.f;
                }
                else
                {
                    // Same handling like this was a line-break

                    FudgetLineMeasurements line;
                    line.StartIndex = line_first;
                    line.EndIndex = last_whitespace;
                    line.Location = Float2(pos_x, pos_y);
                    line.Size = Float2(first_width, line_height);

                    result.Lines.Add(line);
                    result.Size.X = Math::Max(result.Size.X, line.Size.X);
                    result.Size.Y += line_height;

                    pos_y += line_height;

                    line.StartIndex = last_whitespace + 1;
                    line.EndIndex = ix;
                    line.Location = Float2(pos_x, pos_y);
                    line.Size = Float2(second_width + kerning_width + third_width, line_height);

                    result.Lines.Add(line);
                    result.Size.X = Math::Max(result.Size.X, line.Size.X);
                    result.Size.Y += line_height;

                    prev.IsValid = false;

                    continue;
                }
            }
        }

        float kerning = float(prev.IsValid ? _font.Font->GetKerning(prev.Character, current.Character) : 0);
        float next_width = 0.f;
        if (ix == last_whitespace)
        {
            space_width = (current.AdvanceX + kerning) * scale;
            kerning = 0.f;
        }
        else if (last_whitespace != -1 && ix == last_whitespace + 1)
        {
            space_width += kerning * scale;
            kerning = 0.f;
            next_width = current.AdvanceX * scale;
        }
        else
        {
            next_width = current.AdvanceX * scale;
            kerning *= scale;
        }

        prev = current;

        if (charbreak != -1)
        {
            if (second_width + kerning_width + third_width + kerning + next_width > bounds_width)
            {
                // Second word is long enough to break and the second part be its own line

                FudgetLineMeasurements line;
                line.StartIndex = line_first;
                line.EndIndex = charbreak;
                line.Location = Float2(pos_x, pos_y);
                line.Size = Float2(first_width + space_width + second_width, line_height);

                result.Lines.Add(line);
                result.Size.X = Math::Max(result.Size.X, line.Size.X);
                result.Size.Y += line_height;

                pos_y += line_height;

                line_first = charbreak;
                last_whitespace = -1;
                charbreak = -1;
                first_width = third_width + kerning + next_width;
                space_width = 0.f;
                second_width = 0.f;
                kerning_width = 0.f;
                third_width = 0.f;

                prev.IsValid = false;
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
                FudgetLineMeasurements line;
                line.StartIndex = line_first;
                line.EndIndex = last_whitespace;
                line.Location = Float2(pos_x, pos_y);
                line.Size = Float2(first_width, line_height);

                result.Lines.Add(line);
                result.Size.X = Math::Max(result.Size.X, line.Size.X);
                result.Size.Y += line_height;

                pos_y += line_height;

                line_first = last_whitespace + 1;
                last_whitespace = -1;
                charbreak = -1;
                first_width = second_width + kerning + next_width;
                space_width = 0.f;
                second_width = 0.f;
                kerning_width = 0.f;
                third_width = 0.f;

                prev.IsValid = false;

                continue;
            }
            else if (options.WrapMode == FudgetLineWrapMode::Anywhere)
            {
                FudgetLineMeasurements line;
                line.StartIndex = line_first;
                line.EndIndex = ix;
                line.Location = Float2(pos_x, pos_y);
                line.Size = Float2(first_width, line_height);

                result.Lines.Add(line);
                result.Size.X = Math::Max(result.Size.X, line.Size.X);
                result.Size.Y += line_height;

                pos_y += line_height;

                line_first = ix;
                last_whitespace = -1;
                charbreak = -1;
                first_width = next_width;
                space_width = 0.f;
                second_width = 0.f;
                kerning_width = 0.f;
                third_width = 0.f;

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
        // Encountering a newline means, the word we tested for being too long isn't
        // too long, and it will be on its own line.

        FudgetLineMeasurements line;
        line.StartIndex = line_first;
        line.EndIndex = last_whitespace;
        line.Location = Float2(pos_x, pos_y);
        line.Size = Float2(first_width, line_height);

        result.Lines.Add(line);
        result.Size.X = Math::Max(result.Size.X, line.Size.X);
        result.Size.Y += line_height;

        pos_y += line_height;

        line.StartIndex = last_whitespace + 1;
        line.EndIndex = text.Length();
        line.Location = Float2(pos_x, pos_y);
        line.Size = Float2(second_width + kerning_width + third_width, line_height);

        result.Lines.Add(line);
        result.Size.X = Math::Max(result.Size.X, line.Size.X);
        result.Size.Y += line_height;

        prev.IsValid = false;
    }
    else
    {
        FudgetLineMeasurements line;
        line.StartIndex = line_first;
        line.EndIndex = text.Length();
        line.Location = Float2(pos_x, pos_y);
        line.Size = Float2(first_width + space_width + second_width, line_height);

        result.Lines.Add(line);
        result.Size.X = Math::Max(result.Size.X, line.Size.X);
        result.Size.Y += line_height;

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

float FudgetTextBoxPainter::GetCharacterLineHeight( const FudgetMultilineTextMeasurements &measurements, int char_index) const
{
    float scale = measurements.Scale / FontManager::FontScale;
    return _font.Font->GetHeight() * scale;
}

