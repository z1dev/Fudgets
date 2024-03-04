#include "PartPainters.h"
#include "Style.h"
#include "Themes.h"
#include "../MarginStructs.h"
#include "StyleStructs.h"
#include "../Control.h"
#include "DrawableBuilder.h"

#include "Engine/Render2D/FontManager.h"
#include "Engine/Content/Content.h"

// FudgetPartPainter


FudgetPartPainter::FudgetPartPainter(const SpawnParams &params) : Base(params), _theme(nullptr), _style(nullptr)
{

}

FudgetPartPainter::~FudgetPartPainter()
{

}

void FudgetPartPainter::Initialize(FudgetTheme *theme, FudgetStyle *style)
{
    _theme = theme;
    _style = style;
}

FudgetStyle* FudgetPartPainter::GetDefaultStyle() const
{
    FudgetStyle *style = FudgetThemes::GetStyle(GetType().Fullname.ToString());
    if (style == nullptr)
        style = FudgetThemes::CreateStyle(GetType().Fullname.ToString());
    return style;
}

bool FudgetPartPainter::HasState(FudgetVisualControlState states, FudgetVisualControlState to_check) const
{
    return (states & to_check) == to_check;
}

bool FudgetPartPainter::IsEnabled(FudgetVisualControlState states) const
{
    return (states & FudgetVisualControlState::Disabled) != FudgetVisualControlState::Disabled;
}

bool FudgetPartPainter::IsDisabled(FudgetVisualControlState states) const
{
    return (states & FudgetVisualControlState::Disabled) == FudgetVisualControlState::Disabled;
}

bool FudgetPartPainter::IsHovered(FudgetVisualControlState states) const
{
    return (states & FudgetVisualControlState::Hovered) == FudgetVisualControlState::Hovered;
}

bool FudgetPartPainter::IsFocused(FudgetVisualControlState states) const
{
    return (states & FudgetVisualControlState::Focused) == FudgetVisualControlState::Focused;
}

bool FudgetPartPainter::IsDown(FudgetVisualControlState states) const
{
    return (states & FudgetVisualControlState::Down) == FudgetVisualControlState::Down;
}

bool FudgetPartPainter::IsPressed(FudgetVisualControlState states) const
{
    return (states & FudgetVisualControlState::Pressed) == FudgetVisualControlState::Pressed;
}


// FudgetPainterStateHelper


//FudgetPainterStateHelper::FudgetPainterStateHelper() : State(FudgetVisualControlState::Normal)
//{
//
//}
//
//void FudgetPainterStateHelper::SetState(FudgetVisualControlState value, bool set)
//{
//    if (set)
//        State |= value;
//    else
//        State &= ~value;
//}


// FudgetStatePainter


FudgetStatePainter::FudgetStatePainter(const SpawnParams &params) : Base(params)
{
}


// FudgetAlignedImagePainter



FudgetAlignedImagePainter::FudgetAlignedImagePainter(const SpawnParams &params) : Base(params), 
    _image_tint(Color::White), _hovered_image_tint(Color::White), _pressed_image_tint(Color::White), _down_image_tint(Color::White), _focused_image_tint(Color::White), _disabled_image_tint(Color::White),
    _image_offset(0.f), _hovered_image_offset(0.f), _pressed_image_offset(0.f), _down_image_offset(0.f), _focused_image_offset(0.f), _disabled_image_offset(0.f),
    _horz_align(FudgetImageHorzAlign::Stretch), _vert_align(FudgetImageVertAlign::Stretch)
{
}

void FudgetAlignedImagePainter::Initialize(FudgetTheme *theme, FudgetStyle *style)
{
    Base::Initialize(theme, style);
    if (style == nullptr)
        return;

    TextureBase *tex;
    if (!style->GetTextureResource(theme, (int)FudgetAlignedImagePainterIds::Image, tex))
        _image = nullptr;
    else
        _image = tex;

    if (!style->GetTextureResource(theme, (int)FudgetAlignedImagePainterIds::HoveredImage, tex))
        _hovered_image = _image;
    else
        _hovered_image = tex;
    if (!style->GetTextureResource(theme, (int)FudgetAlignedImagePainterIds::PressedImage, tex))
        _pressed_image = _image;
    else
        _pressed_image = tex;
    if (!style->GetTextureResource(theme, (int)FudgetAlignedImagePainterIds::DownImage, tex))
        _down_image = _pressed_image;
    else
        _down_image = tex;
    if (!style->GetTextureResource(theme, (int)FudgetAlignedImagePainterIds::FocusedImage, tex))
        _focused_image = _image;
    else
        _focused_image = tex;
    if (!style->GetTextureResource(theme, (int)FudgetAlignedImagePainterIds::DisabledImage, tex))
        _disabled_image = _image;
    else
        _disabled_image = tex;

    if (!style->GetColorResource(theme, (int)FudgetAlignedImagePainterIds::ImageTint, _image_tint))
        _image_tint = Color::White;
    if (!style->GetColorResource(theme, (int)FudgetAlignedImagePainterIds::HoveredImageTint, _hovered_image_tint))
        _hovered_image_tint = _image_tint;
    if (!style->GetColorResource(theme, (int)FudgetAlignedImagePainterIds::PressedImageTint, _pressed_image_tint))
        _pressed_image_tint = _image_tint;
    if (!style->GetColorResource(theme, (int)FudgetAlignedImagePainterIds::DownImageTint, _down_image_tint))
        _down_image_tint = _pressed_image_tint;
    if (!style->GetColorResource(theme, (int)FudgetAlignedImagePainterIds::FocusedImageTint, _focused_image_tint))
        _focused_image_tint = _image_tint;
    if (!style->GetColorResource(theme, (int)FudgetAlignedImagePainterIds::DisabledImageTint, _disabled_image_tint))
        _disabled_image_tint = _image_tint;

    if (!style->GetFloat2Resource(theme, (int)FudgetAlignedImagePainterIds::ImageOffset, _image_offset))
        _image_offset = Float2(0.f);
    if (!style->GetFloat2Resource(theme, (int)FudgetAlignedImagePainterIds::HoveredImageOffset, _hovered_image_offset))
        _hovered_image_offset = _image_offset;
    if (!style->GetFloat2Resource(theme, (int)FudgetAlignedImagePainterIds::PressedImageOffset, _pressed_image_offset))
        _pressed_image_offset = _image_offset;
    if (!style->GetFloat2Resource(theme, (int)FudgetAlignedImagePainterIds::DownImageOffset, _down_image_offset))
        _down_image_offset = _pressed_image_offset;
    if (!style->GetFloat2Resource(theme, (int)FudgetAlignedImagePainterIds::FocusedImageOffset, _focused_image_offset))
        _focused_image_offset = _image_offset;
    if (!style->GetFloat2Resource(theme, (int)FudgetAlignedImagePainterIds::DisabledImageOffset, _disabled_image_offset))
        _disabled_image_offset = _image_offset;

    if (!style->GetPaddingResource(theme, (int)FudgetAlignedImagePainterIds::ImagePadding, _image_padding))
        _image_padding = FudgetPadding(0.0f);
    if (!style->GetPaddingResource(theme, (int)FudgetAlignedImagePainterIds::HoveredImagePadding, _hovered_image_padding))
        _hovered_image_padding = _image_padding;
    if (!style->GetPaddingResource(theme, (int)FudgetAlignedImagePainterIds::PressedImagePadding, _pressed_image_padding))
        _pressed_image_padding = _image_padding;
    if (!style->GetPaddingResource(theme, (int)FudgetAlignedImagePainterIds::DownImagePadding, _down_image_padding))
        _down_image_padding = _pressed_image_padding;
    if (!style->GetPaddingResource(theme, (int)FudgetAlignedImagePainterIds::FocusedImagePadding, _focused_image_padding))
        _focused_image_padding = _image_padding;
    if (!style->GetPaddingResource(theme, (int)FudgetAlignedImagePainterIds::DisabledImagePadding, _disabled_image_padding))
        _disabled_image_padding = _image_padding;

    if (!style->GetEnumResource<FudgetImageHorzAlign>(theme, (int)FudgetAlignedImagePainterIds::HorzAlign, _horz_align))
        _horz_align = FudgetImageHorzAlign::Stretch;
    if (!style->GetEnumResource<FudgetImageVertAlign>(theme, (int)FudgetAlignedImagePainterIds::VertAlign, _vert_align))
        _vert_align = FudgetImageVertAlign::Stretch;
}

void FudgetAlignedImagePainter::Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState state)
{

    AssetReference<TextureBase> image = IsDisabled(state) ? _disabled_image :
        IsDown(state) ? _down_image :
        IsPressed(state) ? _pressed_image :
        IsFocused(state) ? _focused_image :
        IsHovered(state) ? _hovered_image :
        _image;

    if (image.Get() == nullptr)
        return;

    Color tint = IsDisabled(state) ? _disabled_image_tint :
        IsDown(state) ? _down_image_tint :
        IsPressed(state) ? _pressed_image_tint :
        IsFocused(state) ? _focused_image_tint :
        IsHovered(state) ? _hovered_image_tint :
        _image_tint;

    Float2 offset = IsDisabled(state) ? _disabled_image_offset :
        IsDown(state) ? _down_image_offset :
        IsPressed(state) ? _pressed_image_offset :
        IsFocused(state) ? _focused_image_offset :
        IsHovered(state) ? _hovered_image_offset :
        _image_offset;

    FudgetPadding image_padding = IsDisabled(state) ? _disabled_image_padding :
        IsDown(state) ? _down_image_padding :
        IsPressed(state) ? _pressed_image_padding :
        IsFocused(state) ? _focused_image_padding :
        IsHovered(state) ? _hovered_image_padding :
        _image_padding;

    Rectangle r = image_padding.Padded(bounds);

    if (_horz_align != FudgetImageHorzAlign::Stretch)
    {
        TextureBase *t = image.Get();
        float w = (float)t->Width();
        if (_horz_align == FudgetImageHorzAlign::Right)
            r.Location.X = r.Location.X + (r.Size.X - w);
        else if (_horz_align == FudgetImageHorzAlign::Center)
            r.Location.X = r.Location.X + (r.Size.X - w) * 0.5f;
        r.Size.X = w;
    }

    if (_vert_align != FudgetImageVertAlign::Stretch)
    {
        TextureBase *t = image.Get();
        float h = (float)t->Height();
        if (_vert_align == FudgetImageVertAlign::Bottom)
            r.Location.Y = r.Location.Y + (r.Size.Y - h);
        else if (_vert_align == FudgetImageVertAlign::Center)
            r.Location.Y = r.Location.Y + (r.Size.Y - h) * 0.5f;
        r.Size.Y = h;
    }


    r.Location += offset;
    control->DrawTexture(image, r, tint);
}




FudgetFramedFieldPainter::FudgetFramedFieldPainter(const SpawnParams &params) : Base(params),
    _field_bg(nullptr), _hovered_field_bg(nullptr), _pressed_field_bg(nullptr), _down_field_bg(nullptr), _focused_field_bg(nullptr), 
    _disabled_field_bg(nullptr), _frame_area(nullptr), _hovered_frame_area(nullptr), _pressed_frame_area(nullptr), 
    _down_frame_area(nullptr), _focused_frame_area(nullptr), _disabled_frame_area(nullptr)
{
}

void FudgetFramedFieldPainter::Initialize(FudgetTheme *theme, FudgetStyle *style)
{
    Base::Initialize(theme, style);
    if (style == nullptr)
        style = GetStyle();

    if (!style->GetDrawableResource(theme, (int)FudgetFramedFieldPainterIds::FieldBackground, _field_bg))
    {
        _field_bg = FudgetDrawable::FromColor(Color::White);
    }
    if (!style->GetDrawableResource(theme, (int)FudgetFramedFieldPainterIds::HoveredFieldBackground, _hovered_field_bg))
        _hovered_field_bg = _field_bg;
    if (!style->GetDrawableResource(theme, (int)FudgetFramedFieldPainterIds::PressedFieldBackground, _pressed_field_bg))
        _pressed_field_bg = _field_bg;
    if (!style->GetDrawableResource(theme, (int)FudgetFramedFieldPainterIds::DownFieldBackground, _down_field_bg))
        _down_field_bg = _pressed_field_bg;
    if (!style->GetDrawableResource(theme, (int)FudgetFramedFieldPainterIds::FocusedFieldBackground, _focused_field_bg))
        _focused_field_bg = _field_bg;
    if (!style->GetDrawableResource(theme, (int)FudgetFramedFieldPainterIds::DisabledFieldBackground, _disabled_field_bg))
        _disabled_field_bg = _field_bg;

    if (!style->GetPaddingResource(theme, (int)FudgetFramedFieldPainterIds::FieldPadding, _field_padding))
        _field_padding = FudgetPadding(0.0f);
    if (!style->GetPaddingResource(theme, (int)FudgetFramedFieldPainterIds::HoveredFieldPadding, _hovered_field_padding))
        _hovered_field_padding = _field_padding;
    if (!style->GetPaddingResource(theme, (int)FudgetFramedFieldPainterIds::PressedFieldPadding, _pressed_field_padding))
        _pressed_field_padding = _field_padding;
    if (!style->GetPaddingResource(theme, (int)FudgetFramedFieldPainterIds::DownFieldPadding, _down_field_padding))
        _down_field_padding = _pressed_field_padding;
    if (!style->GetPaddingResource(theme, (int)FudgetFramedFieldPainterIds::FocusedFieldPadding, _focused_field_padding))
        _focused_field_padding = _field_padding;
    if (!style->GetPaddingResource(theme, (int)FudgetFramedFieldPainterIds::DisabledFieldPadding, _disabled_field_padding))
        _disabled_field_padding = _field_padding;

    if (!style->GetDrawableResource(theme, (int)FudgetFramedFieldPainterIds::FrameDraw, _frame_area))
    {
        _frame_area = FudgetDrawable::FromDrawArea(FudgetDrawArea(FudgetPadding::Max(_field_padding, 1.0f), Color::Gray, FudgetFrameType::Inside));
    }

    if (!style->GetDrawableResource(theme, (int)FudgetFramedFieldPainterIds::HoveredFrameDraw, _hovered_frame_area))
        _hovered_frame_area = _frame_area;
    if (!style->GetDrawableResource(theme, (int)FudgetFramedFieldPainterIds::PressedFrameDraw, _pressed_frame_area))
        _pressed_frame_area = _frame_area;
    if (!style->GetDrawableResource(theme, (int)FudgetFramedFieldPainterIds::DownFrameDraw, _down_frame_area))
        _down_frame_area = _pressed_frame_area;
    if (!style->GetDrawableResource(theme, (int)FudgetFramedFieldPainterIds::FocusedFrameDraw, _focused_frame_area))
        _focused_frame_area = _frame_area;
    if (!style->GetDrawableResource(theme, (int)FudgetFramedFieldPainterIds::DisabledFrameDraw, _disabled_frame_area))
        _disabled_frame_area = _frame_area;

    if (!style->GetPaddingResource(theme, (int)FudgetFramedFieldPainterIds::FramePadding, _frame_padding))
        _frame_padding = FudgetPadding(0.0f);
    if (!style->GetPaddingResource(theme, (int)FudgetFramedFieldPainterIds::HoveredFramePadding, _hovered_frame_padding))
        _hovered_frame_padding = _frame_padding;
    if (!style->GetPaddingResource(theme, (int)FudgetFramedFieldPainterIds::PressedFramePadding, _pressed_frame_padding))
        _pressed_frame_padding = _frame_padding;
    if (!style->GetPaddingResource(theme, (int)FudgetFramedFieldPainterIds::DownFramePadding, _down_frame_padding))
        _down_frame_padding = _pressed_frame_padding;
    if (!style->GetPaddingResource(theme, (int)FudgetFramedFieldPainterIds::FocusedFramePadding, _focused_frame_padding))
        _focused_frame_padding = _frame_padding;
    if (!style->GetPaddingResource(theme, (int)FudgetFramedFieldPainterIds::DisabledFramePadding, _disabled_frame_padding))
        _disabled_frame_padding = _frame_padding;

    if (!style->GetPaddingResource(theme, (int)FudgetFramedFieldPainterIds::ContentPadding, _inner_padding))
        _inner_padding = FudgetPadding(0.0f);
}

void FudgetFramedFieldPainter::Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState state)
{
    FudgetDrawable *area = IsDisabled(state) ? _disabled_field_bg :
        IsDown(state) ? _down_field_bg :
        IsPressed(state) ? _pressed_field_bg :
        IsFocused(state) ? _focused_field_bg :
        IsHovered(state) ? _hovered_field_bg :
        _field_bg;
    FudgetPadding field_padding = IsDisabled(state) ? _disabled_field_padding :
        IsDown(state) ? _down_field_padding :
        IsPressed(state) ? _pressed_field_padding :
        IsFocused(state) ? _focused_field_padding :
        IsHovered(state) ? _hovered_field_padding :
        _field_padding;

    control->DrawDrawable(area, field_padding.Padded(bounds));

    FudgetDrawable *frame = IsDisabled(state) ? _disabled_frame_area :
        IsDown(state) ? _down_frame_area :
        IsPressed(state) ? _pressed_frame_area :
        IsFocused(state) ? _focused_frame_area :
        IsHovered(state) ? _hovered_frame_area :
        _frame_area;
    FudgetPadding frame_padding = IsDisabled(state) ? _disabled_frame_padding :
        IsDown(state) ? _down_frame_padding :
        IsPressed(state) ? _pressed_frame_padding :
        IsFocused(state) ? _focused_frame_padding :
        IsHovered(state) ? _hovered_frame_padding :
        _frame_padding;
    control->DrawDrawable(frame, frame_padding.Padded(bounds));
}


// FudgetTextPainter


FudgetSingleLineTextPainter::FudgetSingleLineTextPainter(const SpawnParams &params) : Base(params)
{
}


// FudgetLineEditTextPainter


void FudgetLineEditTextPainter::Initialize(FudgetTheme *theme, FudgetStyle *style)
{
    Base::Initialize(theme, style);
    if (style == nullptr)
        style = GetStyle();

    if (!style->GetDrawableResource(theme, (int)FudgetLineEditTextPainterIds::SelectionDraw, _sel_area))
    {
        _sel_area = FudgetDrawable::FromColor(Color(0.2f, 0.4f, 0.8f, 1.0f));
    }
    if (!style->GetDrawableResource(theme, (int)FudgetLineEditTextPainterIds::FocusedSelectionDraw, _focused_sel_area))
        _focused_sel_area = _sel_area;
    if (!style->GetDrawableResource(theme, (int)FudgetLineEditTextPainterIds::DisabledSelectionDraw, _disabled_sel_area))
        _disabled_sel_area = _sel_area;

    if (!style->GetColorResource(theme, (int)FudgetLineEditTextPainterIds::TextColor, _text_color))
        _text_color = Color::Black;
    if (!style->GetColorResource(theme, (int)FudgetLineEditTextPainterIds::DisabledTextColor, _disabled_text_color))
        _disabled_text_color = _text_color;
    if (!style->GetColorResource(theme, (int)FudgetLineEditTextPainterIds::SelectedTextColor, _selected_text_color))
        _selected_text_color = Color::White;
    if (!style->GetColorResource(theme, (int)FudgetLineEditTextPainterIds::FocusedSelectedTextColor, _focused_selected_text_color))
        _focused_selected_text_color = _selected_text_color;
    if (!style->GetColorResource(theme, (int)FudgetLineEditTextPainterIds::DisabledSelectedTextColor, _disabled_selected_text_color))
        _disabled_selected_text_color = _selected_text_color;

    if (!style->GetFontResource(theme, (int)FudgetLineEditTextPainterIds::Font, _font))
    {
        _font.Settings = FudgetFontSettings(-1, 10.f, false, false);
        FontAsset *font_asset = Content::LoadAsyncInternal<FontAsset>(TEXT("Editor/Fonts/Roboto-Regular"));
        if (font_asset != nullptr)
            _font.Font = font_asset->CreateFont(10.f);
    }
}


FudgetLineEditTextPainter::FudgetLineEditTextPainter(const SpawnParams &params) : Base(params),
    _sel_area(nullptr), _focused_sel_area(nullptr), _disabled_sel_area(nullptr),
    _text_color(Color::White), _disabled_text_color(Color::White), _selected_text_color(Color::White),
    _focused_selected_text_color(Color::White), _disabled_selected_text_color(Color::White)
{
}

void FudgetLineEditTextPainter::Draw(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state, const FudgetSingleLineTextOptions &options)
{
    if (_font.Font == nullptr)
        return;

    Color selTextColor = IsDisabled(state) ? _disabled_selected_text_color : IsFocused(state) ? _focused_selected_text_color : _selected_text_color;
    Color textColor = IsDisabled(state) ? _disabled_text_color : _text_color;

    TextLayoutOptions opt;
    opt.BaseLinesGapScale = 1;
    opt.Scale = options.Scale;
    opt.VerticalAlignment = options.VerticalAlignment;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.TextWrapping = TextWrapping::NoWrap;

    float caret_left = 0;
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

            r.Location = Float2(r.Location.X + _font.Font->MeasureText(text, range2).X, r.Location.Y);
            if (range2.EndIndex < range.EndIndex)
            {
                Char prev_char = text[range2.EndIndex - 1];
                Char next_char = text[range2.EndIndex];
                r.Location.X += (float)_font.Font->GetKerning(prev_char, next_char) * scale;
            }
            opt.Bounds = r;
        }

        range2.StartIndex = Math::Clamp(sel_min, range.StartIndex, range.EndIndex);
        range2.EndIndex = Math::Clamp(sel_max, range.StartIndex, range.EndIndex);

        if (range2.StartIndex < range2.EndIndex)
        {
            Float2 selRectSize = _font.Font->MeasureText(text, range2);
            FudgetDrawable *sel_bg = IsDisabled(state) ? _disabled_sel_area : IsFocused(state) ? _focused_sel_area : _sel_area;
            control->DrawDrawable(sel_bg, Rectangle(opt.Bounds.Location, Float2(selRectSize.X, opt.Bounds.Size.Y)));
            control->DrawText(_font.Font, text, range2, selTextColor, opt);

            if (range2.EndIndex < range.EndIndex && sel_max < range.EndIndex)
            {
                Char prev_char = text[range2.EndIndex - 1];
                Char next_char = text[range2.EndIndex];
                r.Location.X += (float)_font.Font->GetKerning(prev_char, next_char) * scale;

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

Float2 FudgetLineEditTextPainter::Measure(FudgetControl *control, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state, const FudgetSingleLineTextOptions &options)
{
    if (_font.Font == nullptr)
        return Float2::Zero;

    TextLayoutOptions opt;
    opt.BaseLinesGapScale = 1;
    opt.Scale = options.Scale;
    opt.VerticalAlignment = options.VerticalAlignment;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.TextWrapping = TextWrapping::NoWrap;
    opt.Bounds = Rectangle(Float2::Zero, Float2::Zero);

    return _font.Font->MeasureText(text, range, opt);
}

float FudgetLineEditTextPainter::GetKerning(Char a, Char b, float scale) const
{
    if (_font.Font == nullptr)
        return 0.f;

    scale = scale / FontManager::FontScale;
    return _font.Font->GetKerning(a, b) * scale;
}

int FudgetLineEditTextPainter::HitTest(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state, const FudgetSingleLineTextOptions &options, const Float2 &point)
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

float FudgetLineEditTextPainter::GetFontHeight() const
{
    if (_font.Font == nullptr)
        return 0.f;

    return (float)_font.Font->GetHeight();
}


// FudgetListItemPainter


FudgetListItemPainter::FudgetListItemPainter(const SpawnParams &params) : Base(params)
{

}

