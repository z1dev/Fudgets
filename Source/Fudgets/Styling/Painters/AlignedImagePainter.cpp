#include "AlignedImagePainter.h"
#include "../../Control.h"



FudgetAlignedImagePainter::FudgetAlignedImagePainter(const SpawnParams &params) : Base(params),
    _image_tint(Color::White), _hovered_image_tint(Color::White), _pressed_image_tint(Color::White),
    _down_image_tint(Color::White), _focused_image_tint(Color::White), _disabled_image_tint(Color::White),
    _image_offset(0.f), _hovered_image_offset(0.f), _pressed_image_offset(0.f), _down_image_offset(0.f),
    _focused_image_offset(0.f), _disabled_image_offset(0.f), _horz_align(FudgetImageHorzAlign::Stretch),
    _vert_align(FudgetImageVertAlign::Stretch), _state_order(nullptr)
{
}

void FudgetAlignedImagePainter::Initialize(FudgetControl *control,  const Variant &mapping)
{
    if (control == nullptr)
        return;

    const ResourceMapping *ptrres = mapping.AsStructure<ResourceMapping>();
    ResourceMapping res;
    if (ptrres != nullptr)
        res = *ptrres;

    GetMappedStateOrder(res.StateOrderIndex, _state_order);

    if (!GetMappedTexture(control, res.Image, _image))
        _image = nullptr;

    if (!GetMappedTexture(control, res.HoveredImage, _hovered_image))
        _hovered_image = _image;

    if (!GetMappedTexture(control, res.PressedImage, _pressed_image))
        _pressed_image = _image;

    if (!GetMappedTexture(control, res.DownImage, _down_image))
        _down_image = _pressed_image;

    if (!GetMappedTexture(control, res.FocusedImage, _focused_image))
        _focused_image = _image;

    if (!GetMappedTexture(control, res.DisabledImage, _disabled_image))
        _disabled_image = _image;

    if (!GetMappedColor(control, res.ImageTint, _image_tint))
        _image_tint = Color::White;
    if (!GetMappedColor(control, res.HoveredImageTint, _hovered_image_tint))
        _hovered_image_tint = _image_tint;
    if (!GetMappedColor(control, res.PressedImageTint, _pressed_image_tint))
        _pressed_image_tint = _image_tint;
    if (!GetMappedColor(control, res.DownImageTint, _down_image_tint))
        _down_image_tint = _pressed_image_tint;
    if (!GetMappedColor(control, res.FocusedImageTint, _focused_image_tint))
        _focused_image_tint = _image_tint;
    if (!GetMappedColor(control, res.DisabledImageTint, _disabled_image_tint))
        _disabled_image_tint = _image_tint;

    if (!GetMappedFloat2(control, res.ImageOffset, _image_offset))
        _image_offset = Float2(0.f);
    if (!GetMappedFloat2(control, res.HoveredImageOffset, _hovered_image_offset))
        _hovered_image_offset = _image_offset;
    if (!GetMappedFloat2(control, res.PressedImageOffset, _pressed_image_offset))
        _pressed_image_offset = _image_offset;
    if (!GetMappedFloat2(control, res.DownImageOffset, _down_image_offset))
        _down_image_offset = _pressed_image_offset;
    if (!GetMappedFloat2(control, res.FocusedImageOffset, _focused_image_offset))
        _focused_image_offset = _image_offset;
    if (!GetMappedFloat2(control, res.DisabledImageOffset, _disabled_image_offset))
        _disabled_image_offset = _image_offset;

    if (!GetMappedPadding(control, res.ImagePadding, _image_padding))
        _image_padding = FudgetPadding(0);
    if (!GetMappedPadding(control, res.HoveredImagePadding, _hovered_image_padding))
        _hovered_image_padding = _image_padding;
    if (!GetMappedPadding(control, res.PressedImagePadding, _pressed_image_padding))
        _pressed_image_padding = _image_padding;
    if (!GetMappedPadding(control, res.DownImagePadding, _down_image_padding))
        _down_image_padding = _pressed_image_padding;
    if (!GetMappedPadding(control, res.FocusedImagePadding, _focused_image_padding))
        _focused_image_padding = _image_padding;
    if (!GetMappedPadding(control, res.DisabledImagePadding, _disabled_image_padding))
        _disabled_image_padding = _image_padding;

    if (!GetMappedEnum<FudgetImageHorzAlign>(control, res.HorzAlign, _horz_align))
        _horz_align = FudgetImageHorzAlign::Stretch;
    if (!GetMappedEnum<FudgetImageVertAlign>(control, res.VertAlign, _vert_align))
        _vert_align = FudgetImageVertAlign::Stretch;
}

void FudgetAlignedImagePainter::Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState states)
{
    uint64 matched_state = _state_order != nullptr ? _state_order->GetMatchingState((uint64)states) : 0;

    AssetReference<TextureBase> image = DrawDisabled(matched_state) ? _disabled_image :
        DrawDown(matched_state) ? _down_image :
        DrawPressed(matched_state) ? _pressed_image :
        DrawFocused(matched_state) ? _focused_image :
        DrawHovered(matched_state) ? _hovered_image :
        _image;

    if (image.Get() == nullptr)
        return;

    Color tint = DrawDisabled(matched_state) ? _disabled_image_tint :
        DrawDown(matched_state) ? _down_image_tint :
        DrawPressed(matched_state) ? _pressed_image_tint :
        DrawFocused(matched_state) ? _focused_image_tint :
        DrawHovered(matched_state) ? _hovered_image_tint :
        _image_tint;

    Float2 offset = DrawDisabled(matched_state) ? _disabled_image_offset :
        DrawDown(matched_state) ? _down_image_offset :
        DrawPressed(matched_state) ? _pressed_image_offset :
        DrawFocused(matched_state) ? _focused_image_offset :
        DrawHovered(matched_state) ? _hovered_image_offset :
        _image_offset;

    FudgetPadding image_padding = DrawDisabled(matched_state) ? _disabled_image_padding :
        DrawDown(matched_state) ? _down_image_padding :
        DrawPressed(matched_state) ? _pressed_image_padding :
        DrawFocused(matched_state) ? _focused_image_padding :
        DrawHovered(matched_state) ? _hovered_image_padding :
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
