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


FudgetToken FudgetAlignedImagePainter::SelfToken;

FudgetToken FudgetAlignedImagePainter::ImageToken;
FudgetToken FudgetAlignedImagePainter::HoveredImageToken;
FudgetToken FudgetAlignedImagePainter::PressedImageToken;
FudgetToken FudgetAlignedImagePainter::DownImageToken;
FudgetToken FudgetAlignedImagePainter::FocusedImageToken;
FudgetToken FudgetAlignedImagePainter::DisabledImageToken;

FudgetToken FudgetAlignedImagePainter::ImageTintToken;
FudgetToken FudgetAlignedImagePainter::HoveredImageTintToken;
FudgetToken FudgetAlignedImagePainter::PressedImageTintToken;
FudgetToken FudgetAlignedImagePainter::DownImageTintToken;
FudgetToken FudgetAlignedImagePainter::FocusedImageTintToken;
FudgetToken FudgetAlignedImagePainter::DisabledImageTintToken;

FudgetToken FudgetAlignedImagePainter::ImageOffsetToken;
FudgetToken FudgetAlignedImagePainter::HoveredImageOffsetToken;
FudgetToken FudgetAlignedImagePainter::PressedImageOffsetToken;
FudgetToken FudgetAlignedImagePainter::DownImageOffsetToken;
FudgetToken FudgetAlignedImagePainter::FocusedImageOffsetToken;
FudgetToken FudgetAlignedImagePainter::DisabledImageOffsetToken;

FudgetToken FudgetAlignedImagePainter::ImagePaddingToken;
FudgetToken FudgetAlignedImagePainter::HoveredImagePaddingToken;
FudgetToken FudgetAlignedImagePainter::PressedImagePaddingToken;
FudgetToken FudgetAlignedImagePainter::DownImagePaddingToken;
FudgetToken FudgetAlignedImagePainter::FocusedImagePaddingToken;
FudgetToken FudgetAlignedImagePainter::DisabledImagePaddingToken;

FudgetToken FudgetAlignedImagePainter::HorzAlignToken;
FudgetToken FudgetAlignedImagePainter::VertAlignToken;

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

    Rectangle r = image_padding.Padded(control->GetBounds());

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


FudgetToken FudgetFramedFieldPainter::SelfToken;

FudgetToken FudgetFramedFieldPainter::FieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::HoveredFieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::PressedFieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::DownFieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::DisabledFieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::FocusedFieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::FieldPaddingToken;
FudgetToken FudgetFramedFieldPainter::HoveredFieldPaddingToken;
FudgetToken FudgetFramedFieldPainter::PressedFieldPaddingToken;
FudgetToken FudgetFramedFieldPainter::DownFieldPaddingToken;
FudgetToken FudgetFramedFieldPainter::DisabledFieldPaddingToken;
FudgetToken FudgetFramedFieldPainter::FocusedFieldPaddingToken;

FudgetToken FudgetFramedFieldPainter::FrameDrawToken;
FudgetToken FudgetFramedFieldPainter::HoveredFrameDrawToken;
FudgetToken FudgetFramedFieldPainter::PressedFrameDrawToken;
FudgetToken FudgetFramedFieldPainter::DownFrameDrawToken;
FudgetToken FudgetFramedFieldPainter::FocusedFrameDrawToken;
FudgetToken FudgetFramedFieldPainter::DisabledFrameDrawToken;
FudgetToken FudgetFramedFieldPainter::FramePaddingToken;
FudgetToken FudgetFramedFieldPainter::HoveredFramePaddingToken;
FudgetToken FudgetFramedFieldPainter::PressedFramePaddingToken;
FudgetToken FudgetFramedFieldPainter::DownFramePaddingToken;
FudgetToken FudgetFramedFieldPainter::FocusedFramePaddingToken;
FudgetToken FudgetFramedFieldPainter::DisabledFramePaddingToken;

FudgetToken FudgetFramedFieldPainter::ContentPaddingToken;

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
        _frame_area = FudgetDrawArea(FudgetPadding( Math::Max(1.0f, _field_padding.Left), Math::Max(1.0f, _field_padding.Top),
            Math::Max(1.0f, _field_padding.Right), Math::Max(1.0f, _field_padding.Bottom)), Color::Gray, FudgetFrameType::Inside);
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

    control->DrawArea(area, field_padding.Padded(control->GetBounds()));

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
    control->DrawArea(frame, frame_padding.Padded(control->GetBounds()));
}
