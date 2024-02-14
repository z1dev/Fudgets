#include "PartPainters.h"
#include "Style.h"
#include "Themes.h"
#include "../MarginStructs.h"
#include "StyleStructs.h"
#include "../Control.h"

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
    return FudgetThemes::GetStyle(GetType().Fullname.ToString());
}


// FudgetPainterStateHelper


FudgetPainterStateHelper::FudgetPainterStateHelper() : State(FudgetFramedFieldState::Normal)
{

}

void FudgetPainterStateHelper::SetState(FudgetFramedFieldState value, bool set)
{
    if (set)
        State |= value;
    else
        State &= ~value;
}

// FudgetStatePainter


FudgetStatePainter::FudgetStatePainter(const SpawnParams &params) : Base(params)
{
}


// FudgetAlignedImagePainter


FudgetToken FudgetAlignedImagePainter::SelfToken = -1;

FudgetToken FudgetAlignedImagePainter::ImageToken = -1;
FudgetToken FudgetAlignedImagePainter::HoveredImageToken = -1;
FudgetToken FudgetAlignedImagePainter::PressedImageToken = -1;
FudgetToken FudgetAlignedImagePainter::DownImageToken = -1;
FudgetToken FudgetAlignedImagePainter::FocusedImageToken = -1;
FudgetToken FudgetAlignedImagePainter::DisabledImageToken = -1;

FudgetToken FudgetAlignedImagePainter::ImageTintToken = -1;
FudgetToken FudgetAlignedImagePainter::HoveredImageTintToken = -1;
FudgetToken FudgetAlignedImagePainter::PressedImageTintToken = -1;
FudgetToken FudgetAlignedImagePainter::DownImageTintToken = -1;
FudgetToken FudgetAlignedImagePainter::FocusedImageTintToken = -1;
FudgetToken FudgetAlignedImagePainter::DisabledImageTintToken = -1;

FudgetToken FudgetAlignedImagePainter::ImageOffsetToken = -1;
FudgetToken FudgetAlignedImagePainter::HoveredImageOffsetToken = -1;
FudgetToken FudgetAlignedImagePainter::PressedImageOffsetToken = -1;
FudgetToken FudgetAlignedImagePainter::DownImageOffsetToken = -1;
FudgetToken FudgetAlignedImagePainter::FocusedImageOffsetToken = -1;
FudgetToken FudgetAlignedImagePainter::DisabledImageOffsetToken = -1;

FudgetToken FudgetAlignedImagePainter::ImagePaddingToken = -1;
FudgetToken FudgetAlignedImagePainter::HoveredImagePaddingToken = -1;
FudgetToken FudgetAlignedImagePainter::PressedImagePaddingToken = -1;
FudgetToken FudgetAlignedImagePainter::DownImagePaddingToken = -1;
FudgetToken FudgetAlignedImagePainter::FocusedImagePaddingToken = -1;
FudgetToken FudgetAlignedImagePainter::DisabledImagePaddingToken = -1;

FudgetToken FudgetAlignedImagePainter::HorzAlignToken = -1;
FudgetToken FudgetAlignedImagePainter::VertAlignToken = -1;

/*static*/
void FudgetAlignedImagePainter::CreateStyle()
{
    SelfToken = FudgetThemes::RegisterToken(TEXT("Fudgets.FudgetAlignedImagePainter"));

    ImageToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_Image"));
    HoveredImageToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_HoveredImage"));
    PressedImageToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_PressedImage"));
    DownImageToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_DownImage"));
    FocusedImageToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_FocusedImage"));
    DisabledImageToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_DisabledImage"));

    ImageTintToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_ImageTint"));
    HoveredImageTintToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_HoveredImageTint"));
    PressedImageTintToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_PressedImageTint"));
    DownImageTintToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_DownImageTint"));
    FocusedImageTintToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_FocusedImageTint"));
    DisabledImageTintToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_DisabledImageTint"));

    ImageOffsetToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_ImageOffset"));
    HoveredImageOffsetToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_HoveredImageOffset"));
    PressedImageOffsetToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_PressedImageOffset"));
    DownImageOffsetToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_DownImageOffset"));
    FocusedImageOffsetToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_FocusedImageOffset"));
    DisabledImageOffsetToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_DisabledImageOffset"));

    ImagePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_imagePadding"));
    HoveredImagePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_HoveredImagePadding"));
    PressedImagePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_PressedImagePadding"));
    DownImagePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_DownImagePadding"));
    FocusedImagePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_FocusedImagePadding"));
    DisabledImagePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_DisabledImagePadding"));

    HorzAlignToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_HorzAlign"));
    VertAlignToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetAlignedImagePainter_VertAlign"));

    // There are no reasonable defaults unless we add a default image that this painter can draw.

    //FudgetStyle *style = FudgetThemes::CreateStyle(SelfToken);
    //if (style == nullptr)
    //    return;

    //style->SetResourceOverride(PaddingToken, PaddingToken);
    //style->SetResourceOverride(OffsetToken, OffsetToken);

    //style->SetResourceOverride(ImageToken, ImageToken);
    //style->SetResourceOverride(HoveredImageToken, HoveredImageToken);
    //style->SetResourceOverride(PressedImageToken, PressedImageToken);
    //style->SetResourceOverride(DownImageToken, DownImageToken);
    //style->SetResourceOverride(FocusedImageToken, FocusedImageToken);
    //style->SetResourceOverride(DisabledImageToken, DisabledImageToken);

    //style->SetResourceOverride(ImageTintToken, ImageTintToken);
    //style->SetResourceOverride(HoveredImageTintToken, HoveredImageTintToken);
    //style->SetResourceOverride(PressedImageTintToken, PressedImageTintToken);
    //style->SetResourceOverride(DownImageTintToken, DownImageTintToken);
    //style->SetResourceOverride(FocusedImageTintToken, FocusedImageTintToken);
    //style->SetResourceOverride(DisabledImageTintToken, DisabledImageTintToken);

    //style->SetResourceOverride(ImagePaddingToken, ImagePaddingToken);
    //style->SetResourceOverride(HoveredImagePaddingToken, HoveredImagePaddingToken);
    //style->SetResourceOverride(PressedImagePaddingToken, PressedImagePaddingToken);
    //style->SetResourceOverride(DownImagePaddingToken, DownImagePaddingToken);
    //style->SetResourceOverride(FocusedImagePaddingToken, FocusedImagePaddingToken);
    //style->SetResourceOverride(DisabledImagePaddingToken, DisabledImagePaddingToken);

    //style->SetResourceOverride(HorzAlignToken, HorzAlignToken);
    //style->SetResourceOverride(VertAlignToken, VertAlignToken);
}

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
    if (!style->GetTextureResource(theme, ImageToken, tex))
        _image = nullptr;
    else
        _image = tex;

    if (!style->GetTextureResource(theme, HoveredImageToken, tex))
        _hovered_image = _image;
    else
        _hovered_image = tex;
    if (!style->GetTextureResource(theme, PressedImageToken, tex))
        _pressed_image = _image;
    else
        _pressed_image = tex;
    if (!style->GetTextureResource(theme, DownImageToken, tex))
        _down_image = _pressed_image;
    else
        _down_image = tex;
    if (!style->GetTextureResource(theme, FocusedImageToken, tex))
        _focused_image = _image;
    else
        _focused_image = tex;
    if (!style->GetTextureResource(theme, DisabledImageToken, tex))
        _disabled_image = _image;
    else
        _disabled_image = tex;

    if (!style->GetColorResource(theme, ImageTintToken, _image_tint))
        _image_tint = Color::White;
    if (!style->GetColorResource(theme, HoveredImageTintToken, _hovered_image_tint))
        _hovered_image_tint = _image_tint;
    if (!style->GetColorResource(theme, PressedImageTintToken, _pressed_image_tint))
        _pressed_image_tint = _image_tint;
    if (!style->GetColorResource(theme, DownImageTintToken, _down_image_tint))
        _down_image_tint = _pressed_image_tint;
    if (!style->GetColorResource(theme, FocusedImageTintToken, _focused_image_tint))
        _focused_image_tint = _image_tint;
    if (!style->GetColorResource(theme, DisabledImageTintToken, _disabled_image_tint))
        _disabled_image_tint = _image_tint;

    if (!style->GetFloat2Resource(theme, ImageOffsetToken, _image_offset))
        _image_offset = Float2(0.f);
    if (!style->GetFloat2Resource(theme, HoveredImageOffsetToken, _hovered_image_offset))
        _hovered_image_offset = _image_offset;
    if (!style->GetFloat2Resource(theme, PressedImageOffsetToken, _pressed_image_offset))
        _pressed_image_offset = _image_offset;
    if (!style->GetFloat2Resource(theme, DownImageOffsetToken, _down_image_offset))
        _down_image_offset = _pressed_image_offset;
    if (!style->GetFloat2Resource(theme, FocusedImageOffsetToken, _focused_image_offset))
        _focused_image_offset = _image_offset;
    if (!style->GetFloat2Resource(theme, DisabledImageOffsetToken, _disabled_image_offset))
        _disabled_image_offset = _image_offset;

    if (!style->GetPaddingResource(theme, ImagePaddingToken, _image_padding))
        _image_padding = FudgetPadding(0.0f);
    if (!style->GetPaddingResource(theme, HoveredImagePaddingToken, _hovered_image_padding))
        _hovered_image_padding = _image_padding;
    if (!style->GetPaddingResource(theme, PressedImagePaddingToken, _pressed_image_padding))
        _pressed_image_padding = _image_padding;
    if (!style->GetPaddingResource(theme, DownImagePaddingToken, _down_image_padding))
        _down_image_padding = _pressed_image_padding;
    if (!style->GetPaddingResource(theme, FocusedImagePaddingToken, _focused_image_padding))
        _focused_image_padding = _image_padding;
    if (!style->GetPaddingResource(theme, DisabledImagePaddingToken, _disabled_image_padding))
        _disabled_image_padding = _image_padding;

    if (!style->GetEnumResource<FudgetImageHorzAlign>(theme, HorzAlignToken, _horz_align))
        _horz_align = FudgetImageHorzAlign::Stretch;
    if (!style->GetEnumResource<FudgetImageVertAlign>(theme, HorzAlignToken, _vert_align))
        _vert_align = FudgetImageVertAlign::Stretch;
}

void FudgetAlignedImagePainter::Draw(FudgetControl *control, const FudgetPainterStateHelper &state_object)
{

    AssetReference<TextureBase> image = !state_object.Enabled() ? _disabled_image :
        state_object.Down() ? _down_image :
        state_object.Pressed() ? _pressed_image :
        state_object.Focused() ? _focused_image :
        state_object.Hovered() ? _hovered_image :
        _image;

    if (image.Get() == nullptr)
        return;

    Color tint = !state_object.Enabled() ? _disabled_image_tint :
        state_object.Down() ? _down_image_tint :
        state_object.Pressed() ? _pressed_image_tint :
        state_object.Focused() ? _focused_image_tint :
        state_object.Hovered() ? _hovered_image_tint :
        _image_tint;

    Float2 offset = !state_object.Enabled() ? _disabled_image_offset :
        state_object.Down() ? _down_image_offset :
        state_object.Pressed() ? _pressed_image_offset :
        state_object.Focused() ? _focused_image_offset :
        state_object.Hovered() ? _hovered_image_offset :
        _image_offset;

    FudgetPadding image_padding = !state_object.Enabled() ? _disabled_image_padding :
        state_object.Down() ? _down_image_padding :
        state_object.Pressed() ? _pressed_image_padding :
        state_object.Focused() ? _focused_image_padding :
        state_object.Hovered() ? _hovered_image_padding :
        _image_padding;

    Rectangle r = image_padding.Padded(state_object.Bounds);

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


// FudgetFramedFieldPainter


FudgetToken FudgetFramedFieldPainter::SelfToken = -1;

FudgetToken FudgetFramedFieldPainter::FieldBackgroundToken = -1;
FudgetToken FudgetFramedFieldPainter::HoveredFieldBackgroundToken = -1;
FudgetToken FudgetFramedFieldPainter::PressedFieldBackgroundToken = -1;
FudgetToken FudgetFramedFieldPainter::DownFieldBackgroundToken = -1;
FudgetToken FudgetFramedFieldPainter::DisabledFieldBackgroundToken = -1;
FudgetToken FudgetFramedFieldPainter::FocusedFieldBackgroundToken = -1;
FudgetToken FudgetFramedFieldPainter::FieldPaddingToken = -1;
FudgetToken FudgetFramedFieldPainter::HoveredFieldPaddingToken = -1;
FudgetToken FudgetFramedFieldPainter::PressedFieldPaddingToken = -1;
FudgetToken FudgetFramedFieldPainter::DownFieldPaddingToken = -1;
FudgetToken FudgetFramedFieldPainter::DisabledFieldPaddingToken = -1;
FudgetToken FudgetFramedFieldPainter::FocusedFieldPaddingToken = -1;

FudgetToken FudgetFramedFieldPainter::FrameDrawToken = -1;
FudgetToken FudgetFramedFieldPainter::HoveredFrameDrawToken = -1;
FudgetToken FudgetFramedFieldPainter::PressedFrameDrawToken = -1;
FudgetToken FudgetFramedFieldPainter::DownFrameDrawToken = -1;
FudgetToken FudgetFramedFieldPainter::FocusedFrameDrawToken = -1;
FudgetToken FudgetFramedFieldPainter::DisabledFrameDrawToken = -1;
FudgetToken FudgetFramedFieldPainter::FramePaddingToken = -1;
FudgetToken FudgetFramedFieldPainter::HoveredFramePaddingToken = -1;
FudgetToken FudgetFramedFieldPainter::PressedFramePaddingToken = -1;
FudgetToken FudgetFramedFieldPainter::DownFramePaddingToken = -1;
FudgetToken FudgetFramedFieldPainter::FocusedFramePaddingToken = -1;
FudgetToken FudgetFramedFieldPainter::DisabledFramePaddingToken = -1;

FudgetToken FudgetFramedFieldPainter::ContentPaddingToken = -1;

/*static*/
void FudgetFramedFieldPainter::CreateStyle()
{
    SelfToken = FudgetThemes::RegisterToken(TEXT("Fudgets.FudgetFramedFieldPainter"));

    FieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FieldBackground"));
    HoveredFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_HoveredFieldBackground"));
    FocusedFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedFieldBackground"));
    PressedFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_PressedFieldBackground"));
    DownFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DownFieldBackground"));
    DisabledFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledFieldBackground"));
    FieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FieldPadding"));
    HoveredFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_HoveredFieldPadding"));
    PressedFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_PressedFieldPadding"));
    DownFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DownFieldPadding"));
    FocusedFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedFieldPadding"));
    DisabledFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledFieldPadding"));

    FrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FrameDraw"));
    HoveredFrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_HoveredFrameDraw"));
    PressedFrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_PressedFrameDraw"));
    DownFrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DownFrameDraw"));
    FocusedFrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedFrameDraw"));
    DisabledFrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledFrameDraw"));

    FramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_framePadding"));
    HoveredFramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_HoveredFramePadding"));
    PressedFramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_PressedFramePadding"));
    DownFramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DownFramePadding"));
    FocusedFramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedFramePadding"));
    DisabledFramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledFramePadding"));

    ContentPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_ContentPadding"));

    FudgetStyle *style = FudgetThemes::CreateStyle(SelfToken);
    if (style == nullptr)
        return;

    style->SetResourceOverride(FieldBackgroundToken, FieldBackgroundToken);
    style->SetResourceOverride(HoveredFieldBackgroundToken, HoveredFieldBackgroundToken);
    style->SetResourceOverride(PressedFieldBackgroundToken, PressedFieldBackgroundToken);
    style->SetResourceOverride(DownFieldBackgroundToken, DownFieldBackgroundToken);
    style->SetResourceOverride(FocusedFieldBackgroundToken, FocusedFieldBackgroundToken);
    style->SetResourceOverride(DisabledFieldBackgroundToken, DisabledFieldBackgroundToken);
    style->SetResourceOverride(FieldPaddingToken, FieldPaddingToken);
    style->SetResourceOverride(HoveredFieldPaddingToken, HoveredFieldPaddingToken);
    style->SetResourceOverride(PressedFieldPaddingToken, PressedFieldPaddingToken);
    style->SetResourceOverride(DownFieldPaddingToken, DownFieldPaddingToken);
    style->SetResourceOverride(FocusedFieldPaddingToken, FocusedFieldPaddingToken);
    style->SetResourceOverride(DisabledFieldPaddingToken, DisabledFieldPaddingToken);

    style->SetResourceOverride(FrameDrawToken, FrameDrawToken);
    style->SetResourceOverride(HoveredFrameDrawToken, HoveredFrameDrawToken);
    style->SetResourceOverride(PressedFrameDrawToken, PressedFrameDrawToken);
    style->SetResourceOverride(DownFrameDrawToken, DownFrameDrawToken);
    style->SetResourceOverride(FocusedFrameDrawToken, FocusedFrameDrawToken);
    style->SetResourceOverride(DisabledFrameDrawToken, DisabledFrameDrawToken);

    style->SetResourceOverride(FramePaddingToken, FramePaddingToken);
    style->SetResourceOverride(HoveredFramePaddingToken, HoveredFramePaddingToken);
    style->SetResourceOverride(PressedFramePaddingToken, PressedFramePaddingToken);
    style->SetResourceOverride(DownFramePaddingToken, DownFramePaddingToken);
    style->SetResourceOverride(FocusedFramePaddingToken, FocusedFramePaddingToken);
    style->SetResourceOverride(DisabledFramePaddingToken, DisabledFramePaddingToken);

    style->SetResourceOverride(ContentPaddingToken, ContentPaddingToken);
}

FudgetFramedFieldPainter::FudgetFramedFieldPainter(const SpawnParams &params) : Base(params)
{
}

void FudgetFramedFieldPainter::Initialize(FudgetTheme *theme, FudgetStyle *style)
{
    Base::Initialize(theme, style);
    if (style == nullptr)
    {
        style = GetStyle();
        if (style == nullptr)
            return;
    }

    if (!style->GetDrawAreaResource(theme, FieldBackgroundToken, _field_bg))
        _field_bg = FudgetDrawArea(Color::White);
    if (!style->GetDrawAreaResource(theme, HoveredFieldBackgroundToken, _hovered_field_bg))
        _hovered_field_bg = _field_bg;
    if (!style->GetDrawAreaResource(theme, PressedFieldBackgroundToken, _pressed_field_bg))
        _pressed_field_bg = _field_bg;
    if (!style->GetDrawAreaResource(theme, DownFieldBackgroundToken, _down_field_bg))
        _down_field_bg = _pressed_field_bg;
    if (!style->GetDrawAreaResource(theme, FocusedFieldBackgroundToken, _focused_field_bg))
        _focused_field_bg = _field_bg;
    if (!style->GetDrawAreaResource(theme, DisabledFieldBackgroundToken, _disabled_field_bg))
        _disabled_field_bg = _field_bg;

    if (!style->GetPaddingResource(theme, FieldPaddingToken, _field_padding))
        _field_padding = FudgetPadding(0.0f);
    if (!style->GetPaddingResource(theme, HoveredFieldPaddingToken, _hovered_field_padding))
        _hovered_field_padding = _field_padding;
    if (!style->GetPaddingResource(theme, PressedFieldPaddingToken, _pressed_field_padding))
        _pressed_field_padding = _field_padding;
    if (!style->GetPaddingResource(theme, DownFieldPaddingToken, _down_field_padding))
        _down_field_padding = _pressed_field_padding;
    if (!style->GetPaddingResource(theme, FocusedFieldPaddingToken, _focused_field_padding))
        _focused_field_padding = _field_padding;
    if (!style->GetPaddingResource(theme, DisabledFieldPaddingToken, _disabled_field_padding))
        _disabled_field_padding = _field_padding;

    if (!style->GetDrawAreaResource(theme, FrameDrawToken, _frame_area))
        _frame_area = FudgetDrawArea(FudgetPadding::Max(_field_padding, 1.0f), Color::Gray, FudgetFrameType::Inside);
    if (!style->GetDrawAreaResource(theme, HoveredFrameDrawToken, _hovered_frame_area))
        _hovered_frame_area = _frame_area;
    if (!style->GetDrawAreaResource(theme, PressedFrameDrawToken, _pressed_frame_area))
        _pressed_frame_area = _frame_area;
    if (!style->GetDrawAreaResource(theme, DownFrameDrawToken, _down_frame_area))
        _down_frame_area = _pressed_frame_area;
    if (!style->GetDrawAreaResource(theme, FocusedFrameDrawToken, _focused_frame_area))
        _focused_frame_area = _frame_area;
    if (!style->GetDrawAreaResource(theme, DisabledFrameDrawToken, _disabled_frame_area))
        _disabled_frame_area = _frame_area;

    if (!style->GetPaddingResource(theme, FramePaddingToken, _frame_padding))
        _frame_padding = FudgetPadding(0.0f);
    if (!style->GetPaddingResource(theme, HoveredFramePaddingToken, _hovered_frame_padding))
        _hovered_frame_padding = _frame_padding;
    if (!style->GetPaddingResource(theme, PressedFramePaddingToken, _pressed_frame_padding))
        _pressed_frame_padding = _frame_padding;
    if (!style->GetPaddingResource(theme, DownFramePaddingToken, _down_frame_padding))
        _down_frame_padding = _pressed_frame_padding;
    if (!style->GetPaddingResource(theme, FocusedFramePaddingToken, _focused_frame_padding))
        _focused_frame_padding = _frame_padding;
    if (!style->GetPaddingResource(theme, DisabledFramePaddingToken, _disabled_frame_padding))
        _disabled_frame_padding = _frame_padding;

    if (!style->GetPaddingResource(theme, ContentPaddingToken, _inner_padding))
        _inner_padding = FudgetPadding(0.0f);
}

void FudgetFramedFieldPainter::Draw(FudgetControl *control, const FudgetPainterStateHelper &state_object)
{
    FudgetDrawArea area = !state_object.Enabled() ? _disabled_field_bg :
        state_object.Down() ? _down_field_bg :
        state_object.Pressed() ? _pressed_field_bg :
        state_object.Focused() ? _focused_field_bg :
        state_object.Hovered() ? _hovered_field_bg :
        _field_bg;
    FudgetPadding field_padding = !state_object.Enabled() ? _disabled_field_padding :
        state_object.Down() ? _down_field_padding :
        state_object.Pressed() ? _pressed_field_padding :
        state_object.Focused() ? _focused_field_padding :
        state_object.Hovered() ? _hovered_field_padding :
        _field_padding;

    control->DrawArea(area, field_padding.Padded(state_object.Bounds));

    FudgetDrawArea frame = !state_object.Enabled() ? _disabled_frame_area :
        state_object.Down() ? _down_frame_area :
        state_object.Pressed() ? _pressed_frame_area :
        state_object.Focused() ? _focused_frame_area :
        state_object.Hovered() ? _hovered_frame_area :
        _frame_area;
    FudgetPadding frame_padding = !state_object.Enabled() ? _disabled_frame_padding :
        state_object.Down() ? _down_frame_padding :
        state_object.Pressed() ? _pressed_frame_padding :
        state_object.Focused() ? _focused_frame_padding :
        state_object.Hovered() ? _hovered_frame_padding :
        _frame_padding;
    control->DrawArea(frame, frame_padding.Padded(state_object.Bounds));
}


// FudgetTextPainter


FudgetTextPainter::FudgetTextPainter(const SpawnParams &params) : Base(params)
{
}


// FudgetLineEditTextPainter


FudgetToken FudgetLineEditTextPainter::SelfToken = -1;

FudgetToken FudgetLineEditTextPainter::SelectionDrawToken = -1;
FudgetToken FudgetLineEditTextPainter::FocusedSelectionDrawToken = -1;
FudgetToken FudgetLineEditTextPainter::DisabledSelectionDrawToken = -1;
FudgetToken FudgetLineEditTextPainter::TextColorToken = -1;
FudgetToken FudgetLineEditTextPainter::DisabledTextColorToken = -1;
FudgetToken FudgetLineEditTextPainter::SelectedTextColorToken = -1;
FudgetToken FudgetLineEditTextPainter::FocusedSelectedTextColorToken = -1;
FudgetToken FudgetLineEditTextPainter::DisabledSelectedTextColorToken = -1;
FudgetToken FudgetLineEditTextPainter::FontToken = -1;

void FudgetLineEditTextPainter::CreateStyle()
{
    SelfToken = FudgetThemes::RegisterToken(TEXT("Fudgets.FudgetLineEditTextPainter"));

    SelectionDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetLineEditTextPainter_SelectionDrawToken"));
    FocusedSelectionDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetLineEditTextPainter_FocusedSelectionDrawToken"));
    DisabledSelectionDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetLineEditTextPainter_DisabledSelectionDrawToken"));
    TextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetLineEditTextPainter_TextColorToken"));
    DisabledTextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetLineEditTextPainter_DisabledTextColorToken"));
    SelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetLineEditTextPainter_SelectedTextColorToken"));
    FocusedSelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetLineEditTextPainter_FocusedSelectedTextColorToken"));
    DisabledSelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetLineEditTextPainter_DisabledSelectedTextColorToken"));
    FontToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetLineEditTextPainter_FontToken"));

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

void FudgetLineEditTextPainter::Initialize(FudgetTheme *theme, FudgetStyle *style)
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


FudgetLineEditTextPainter::FudgetLineEditTextPainter(const SpawnParams &params) : Base(params),
    _text_color(Color::White), _disabled_text_color(Color::White), _selected_text_color(Color::White),
    _focused_selected_text_color(Color::White), _disabled_selected_text_color(Color::White)
{
}

void FudgetLineEditTextPainter::Draw(FudgetControl *control, const FudgetPainterStateHelper &state_object, const FudgetPainterTextDrawOptions &text_options)
{
    if (_font.Font == nullptr)
        return;

    Color selTextColor = !state_object.Enabled() ? _disabled_selected_text_color : state_object.Focused() ? _focused_selected_text_color : _selected_text_color;
    Color textColor = !state_object.Enabled() ? _disabled_text_color : _text_color;

    TextLayoutOptions opt;
    opt.BaseLinesGapScale = 1;
    opt.Scale = 1;
    opt.VerticalAlignment = TextAlignment::Center;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.TextWrapping = TextWrapping::NoWrap;

    float caret_left = 0;
    TextRange range;
    if (text_options.Spans.IsInvalid() || text_options.Spans.Length() == 0 || (text_options.Spans.Length() > 0 && (text_options.Spans[0].RangeSpan.IsInvalid() || text_options.Spans[0].RangeSpan.Length() == 0)))
    {
        FudgetPadding tmp(0.f);
        tmp.Left += text_options.Offset.X;
        tmp.Top += text_options.Offset.Y;
        opt.Bounds = tmp.Padded(state_object.Bounds);
        TextRange range;
        range.StartIndex = text_options.Range.X;
        range.EndIndex = text_options.Range.Y;

        control->DrawText(_font.Font, text_options.Text, range, textColor, opt);
    }
    else
    {
        Rectangle r = state_object.Bounds;
        r.Location += text_options.Offset;
        opt.Bounds = r;

        int sel_min = text_options.Spans[0].RangeSpan[0].X;
        int sel_max = text_options.Spans[0].RangeSpan[0].Y;

        range.StartIndex = text_options.Range.X + 0;
        range.EndIndex = Math::Min(text_options.Range.Y, text_options.Range.X + sel_min);
        if (range.EndIndex > range.StartIndex)
            control->DrawText(_font.Font, text_options.Text, range, textColor, opt);

        r.Location = Float2(r.Location.X + _font.Font->MeasureText(text_options.Text, range).X, r.Location.Y);
        opt.Bounds = r;

        //if (GetCaretPos() == sel_min)
        //    caret_left = r.Location.X;

        range.StartIndex = Math::Min(text_options.Range.Y, text_options.Range.X + sel_min);
        range.EndIndex = Math::Min(text_options.Range.Y, text_options.Range.X + sel_max);
        Float2 selRectSize = _font.Font->MeasureText(text_options.Text, range);

        FudgetDrawArea sel_bg = !state_object.Enabled() ? _disabled_sel_area : state_object.Focused() ? _focused_sel_area : _sel_area;

        control->DrawArea(sel_bg, Rectangle(opt.Bounds.Location, Float2(selRectSize.X, opt.Bounds.Size.Y)));

        control->DrawText(_font.Font, text_options.Text, range, selTextColor, opt);

        if (sel_max < text_options.Range.Y - text_options.Range.X  /*GetTextLength() || GetCaretPos() == sel_max*/)
        {
            r.Location = Float2(r.Location.X + _font.Font->MeasureText(text_options.Text, range).X, r.Location.Y);
            opt.Bounds = r;

            //if (GetCaretPos() == sel_max)
            //    caret_left = r.Location.X;

            //if (sel_max != GetTextLength())
            //{
            range.StartIndex = text_options.Range.X + sel_max;
            range.EndIndex = text_options.Range.Y;
            control->DrawText(_font.Font, text_options.Text, range, textColor, opt);
            //}
        }
    }
}

Float2 FudgetLineEditTextPainter::Measure(FudgetControl *control, const FudgetPainterStateHelper &state_object, const FudgetPainterTextDrawOptions &text_options)
{
    if (_font.Font == nullptr)
        return Float2::Zero;

    TextLayoutOptions opt;
    opt.BaseLinesGapScale = 1;
    opt.Scale = 1;
    opt.VerticalAlignment = TextAlignment::Center;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.TextWrapping = TextWrapping::NoWrap;
    FudgetPadding tmp(0.f);
    tmp.Left += text_options.Offset.X;
    tmp.Top += text_options.Offset.Y;
    opt.Bounds = tmp.Padded(state_object.Bounds);

    TextRange range;
    range.StartIndex = text_options.Range.X;
    range.EndIndex = text_options.Range.Y;

    return _font.Font->MeasureText(text_options.Text, range, opt);
}

int FudgetLineEditTextPainter::HitTest(FudgetControl *control, const FudgetPainterStateHelper &state_object, const FudgetPainterTextDrawOptions &text_options, const Float2 &point)
{
    if (_font.Font == nullptr)
        return 0;

    TextLayoutOptions opt;
    opt.BaseLinesGapScale = 1;
    opt.Scale = 1;
    opt.VerticalAlignment = TextAlignment::Center;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.TextWrapping = TextWrapping::NoWrap;
    FudgetPadding tmp(0.f);
    tmp.Left += text_options.Offset.X;
    tmp.Top += text_options.Offset.Y;
    opt.Bounds = tmp.Padded(state_object.Bounds);

    TextRange range;
    range.StartIndex = text_options.Range.X;
    range.EndIndex = text_options.Range.Y;

    return _font.Font->HitTestText(text_options.Text, range, point, opt);
}

