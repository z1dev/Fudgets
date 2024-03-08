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


FudgetPartPainter::FudgetPartPainter(const SpawnParams &params) : Base(params)
{

}

FudgetPartPainter::~FudgetPartPainter()
{

}

FudgetStyle* FudgetPartPainter::GetDefaultStyle() const
{
    FudgetStyle *style = FudgetThemes::GetStyle(GetType().Fullname.ToString());
    if (style == nullptr)
        style = FudgetThemes::CreateStyle(GetType().Fullname.ToString());
    return style;
}


bool FudgetPartPainter::GetMappedStyle(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) FudgetStyle* &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();

    int values[2] = { mapping_id, painter_id };
    result = nullptr;
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetStyleResource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStyleStyle(values[ix], result);
        //if (!success)
        //{
        //    Variant var;
        //    if (theme->GetResource(values[ix], var))
        //    {
        //        String str;
        //        if (FudgetStyle::StringFromVariant(values[ix], str))
        //        {
        //            result = FudgetThemes::GetStyle(str);
        //            success = result != nullptr;
        //        }
        //    }
        //}
    }

    return success;
}

bool FudgetPartPainter::GetMappedTexture(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, AssetReference<TextureBase> &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    TextureBase *tex = nullptr;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && tex == nullptr; ++ix)
    {
        if (style_override != nullptr)
            FudgetStyle::GetTextureResource(style_override, theme, values[ix], false, tex);
        if (tex == nullptr)
            control->GetStyleTexture(values[ix], tex);
        //if (tex == nullptr)
        //{
        //    Variant var;
        //    if (theme->GetResource(values[ix], var))
        //        FudgetStyle::TextureFromVariant(var, tex);
        //}
    }
    result = tex;
    return tex != nullptr;
}

bool FudgetPartPainter::GetMappedTexture(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) TextureBase* &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = nullptr;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && result == nullptr; ++ix)
    {
        if (style_override != nullptr)
            FudgetStyle::GetTextureResource(style_override, theme, values[ix], false, result);
        if (result == nullptr)
            control->GetStyleTexture(values[ix], result);
        //if (result == nullptr)
        //{
        //    Variant var;
        //    if (theme->GetResource(values[ix], var))
        //        FudgetStyle::TextureFromVariant(var, result);
        //}
    }

    return result != nullptr;
}

bool FudgetPartPainter::GetMappedColor(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) Color &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = Color::White;
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetColorResource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStyleColor(values[ix], result);
    }

    return success;
}

bool FudgetPartPainter::GetMappedString(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) String &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = String();
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetStringResource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStyleString(values[ix], result);
    }

    return success;
}

bool FudgetPartPainter::GetMappedFloat(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) float &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = 0.f;
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetFloatResource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStyleFloat(values[ix], result);
    }

    return success;
}

bool FudgetPartPainter::GetMappedFloat2(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) Float2 &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = Float2::Zero;
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetFloat2Resource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStyleFloat2(values[ix], result);
    }

    return success;
}

bool FudgetPartPainter::GetMappedFloat3(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) Float3 &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = Float3::Zero;
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetFloat3Resource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStyleFloat3(values[ix], result);
    }

    return success;
}

bool FudgetPartPainter::GetMappedFloat4(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) Float4 &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = Float4::Zero;
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetFloat4Resource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStyleFloat4(values[ix], result);
    }

    return success;
}

bool FudgetPartPainter::GetMappedInt(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) int &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = 0;
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetIntResource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStyleInt(values[ix], result);
    }

    return success;
}

bool FudgetPartPainter::GetMappedInt2(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) Int2 &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = Int2::Zero;
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetInt2Resource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStyleInt2(values[ix], result);
    }

    return success;
}

bool FudgetPartPainter::GetMappedInt3(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) Int3 &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = Int3::Zero;
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetInt3Resource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStyleInt3(values[ix], result);
    }

    return success;
}

bool FudgetPartPainter::GetMappedInt4(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) Int4 &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = Int4::Zero;
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetInt4Resource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStyleInt4(values[ix], result);
    }

    return success;
}

bool FudgetPartPainter::GetMappedPadding(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) FudgetPadding &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = FudgetPadding(0.f);
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetPaddingResource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStylePadding(values[ix], result);
    }

    return success;
}

bool FudgetPartPainter::GetMappedDrawable(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) FudgetDrawable* &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = nullptr;
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetDrawableResource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStyleDrawable(values[ix], result);
    }
    return success;
}

bool FudgetPartPainter::GetMappedDrawArea(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) FudgetDrawArea &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = FudgetDrawArea();
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetDrawAreaResource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStyleDrawArea(values[ix], result);
    }
    return success;
}

bool FudgetPartPainter::GetMappedFont(FudgetControl *control, FudgetStyle *style_override, int painter_id, int mapping_id, API_PARAM(Out) FudgetFont &result) const
{
    FudgetTheme *theme = control->GetActiveTheme();
    int values[2] = { mapping_id, painter_id };
    result = FudgetFont();
    bool success = false;
    for (int ix = mapping_id == 0 ? 1 : 0; ix < 2 && !success; ++ix)
    {
        if (style_override != nullptr)
            success = FudgetStyle::GetFontResource(style_override, theme, values[ix], false, result);
        if (!success)
            success = control->GetStyleFont(values[ix], result);
    }

    return success;
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

void FudgetAlignedImagePainter::Initialize(FudgetControl *control, FudgetStyle *style_override, const Variant &mapping)
{
    if (control == nullptr)
        return;

    const ResourceMapping *ptrres = mapping.AsStructure<ResourceMapping>();
    ResourceMapping res;
    if (ptrres != nullptr)
        res = *ptrres;
    
    if (!GetMappedTexture(control, style_override, (int)FudgetAlignedImagePainterIds::Image, res.Image, _image))
        _image = nullptr;

    if (!GetMappedTexture(control, style_override, (int)FudgetAlignedImagePainterIds::HoveredImage, res.HoveredImage, _hovered_image))
        _hovered_image = _image;

    if (!GetMappedTexture(control, style_override, (int)FudgetAlignedImagePainterIds::PressedImage, res.PressedImage, _pressed_image))
        _pressed_image = _image;

    if (!GetMappedTexture(control, style_override, (int)FudgetAlignedImagePainterIds::DownImage, res.DownImage, _down_image))
        _down_image = _pressed_image;

    if (!GetMappedTexture(control, style_override, (int)FudgetAlignedImagePainterIds::FocusedImage, res.FocusedImage, _focused_image))
        _focused_image = _image;

    if (!GetMappedTexture(control, style_override, (int)FudgetAlignedImagePainterIds::DisabledImage, res.DisabledImage, _disabled_image))
        _disabled_image = _image;

    if (!GetMappedColor(control, style_override, (int)FudgetAlignedImagePainterIds::ImageTint, res.ImageTint, _image_tint))
        _image_tint = Color::White;
    if (!GetMappedColor(control, style_override, (int)FudgetAlignedImagePainterIds::HoveredImageTint, res.HoveredImageTint, _hovered_image_tint))
        _hovered_image_tint = _image_tint;
    if (!GetMappedColor(control, style_override, (int)FudgetAlignedImagePainterIds::PressedImageTint, res.PressedImageTint, _pressed_image_tint))
        _pressed_image_tint = _image_tint;
    if (!GetMappedColor(control, style_override, (int)FudgetAlignedImagePainterIds::DownImageTint, res.DownImageTint, _down_image_tint))
        _down_image_tint = _pressed_image_tint;
    if (!GetMappedColor(control, style_override, (int)FudgetAlignedImagePainterIds::FocusedImageTint, res.FocusedImageTint, _focused_image_tint))
        _focused_image_tint = _image_tint;
    if (!GetMappedColor(control, style_override, (int)FudgetAlignedImagePainterIds::DisabledImageTint, res.DisabledImageTint, _disabled_image_tint))
        _disabled_image_tint = _image_tint;

    if (!GetMappedFloat2(control, style_override, (int)FudgetAlignedImagePainterIds::ImageOffset, res.ImageOffset, _image_offset))
        _image_offset = Float2(0.f);
    if (!GetMappedFloat2(control, style_override, (int)FudgetAlignedImagePainterIds::HoveredImageOffset, res.HoveredImageOffset, _hovered_image_offset))
        _hovered_image_offset = _image_offset;
    if (!GetMappedFloat2(control, style_override, (int)FudgetAlignedImagePainterIds::PressedImageOffset, res.PressedImageOffset, _pressed_image_offset))
        _pressed_image_offset = _image_offset;
    if (!GetMappedFloat2(control, style_override, (int)FudgetAlignedImagePainterIds::DownImageOffset, res.DownImageOffset, _down_image_offset))
        _down_image_offset = _pressed_image_offset;
    if (!GetMappedFloat2(control, style_override, (int)FudgetAlignedImagePainterIds::FocusedImageOffset, res.FocusedImageOffset, _focused_image_offset))
        _focused_image_offset = _image_offset;
    if (!GetMappedFloat2(control, style_override, (int)FudgetAlignedImagePainterIds::DisabledImageOffset, res.DisabledImageOffset, _disabled_image_offset))
        _disabled_image_offset = _image_offset;

    if (!GetMappedPadding(control, style_override, (int)FudgetAlignedImagePainterIds::ImagePadding, res.ImagePadding, _image_padding))
        _image_padding = FudgetPadding(0.0f);
    if (!GetMappedPadding(control, style_override, (int)FudgetAlignedImagePainterIds::HoveredImagePadding, res.HoveredImagePadding, _hovered_image_padding))
        _hovered_image_padding = _image_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetAlignedImagePainterIds::PressedImagePadding, res.PressedImagePadding, _pressed_image_padding))
        _pressed_image_padding = _image_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetAlignedImagePainterIds::DownImagePadding, res.DownImagePadding, _down_image_padding))
        _down_image_padding = _pressed_image_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetAlignedImagePainterIds::FocusedImagePadding, res.FocusedImagePadding, _focused_image_padding))
        _focused_image_padding = _image_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetAlignedImagePainterIds::DisabledImagePadding, res.DisabledImagePadding, _disabled_image_padding))
        _disabled_image_padding = _image_padding;

    if (!GetMappedEnum<FudgetImageHorzAlign>(control, style_override, (int)FudgetAlignedImagePainterIds::HorzAlign, res.HorzAlign, _horz_align))
        _horz_align = FudgetImageHorzAlign::Stretch;
    if (!GetMappedEnum<FudgetImageVertAlign>(control, style_override, (int)FudgetAlignedImagePainterIds::VertAlign, res.VertAlign, _vert_align))
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

void FudgetFramedFieldPainter::Initialize(FudgetControl *control, FudgetStyle *style_override, const Variant &mapping)
{
    if (control == nullptr)
        return;

    const ResourceMapping *ptrres = mapping.AsStructure<ResourceMapping>();
    ResourceMapping res;
    if (ptrres != nullptr)
        res = *ptrres;

    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::FieldBackground, res.FieldBackground, _field_bg))
        _field_bg = FudgetDrawable::FromColor(Color::White);
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::HoveredFieldBackground, res.HoveredFieldBackground, _hovered_field_bg))
        _hovered_field_bg = _field_bg;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::PressedFieldBackground, res.PressedFieldBackground, _pressed_field_bg))
        _pressed_field_bg = _field_bg;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::DownFieldBackground, res.DownFieldBackground, _down_field_bg))
        _down_field_bg = _pressed_field_bg;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::FocusedFieldBackground, res.FocusedFieldBackground, _focused_field_bg))
        _focused_field_bg = _field_bg;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::DisabledFieldBackground, res.DisabledFieldBackground, _disabled_field_bg))
        _disabled_field_bg = _field_bg;

    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::FieldPadding, res.FieldPadding, _field_padding))
        _field_padding = FudgetPadding(0.0f);
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::HoveredFieldPadding, res.HoveredFieldPadding, _hovered_field_padding))
        _hovered_field_padding = _field_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::PressedFieldPadding, res.PressedFieldPadding, _pressed_field_padding))
        _pressed_field_padding = _field_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::DownFieldPadding, res.DownFieldPadding, _down_field_padding))
        _down_field_padding = _pressed_field_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::FocusedFieldPadding, res.FocusedFieldPadding, _focused_field_padding))
        _focused_field_padding = _field_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::DisabledFieldPadding, res.DisabledFieldPadding, _disabled_field_padding))
        _disabled_field_padding = _field_padding;

    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::FrameDraw, res.FrameDraw, _frame_area))
        _frame_area = FudgetDrawable::FromDrawArea(FudgetDrawArea(FudgetPadding::Max(_field_padding, 1.0f), Color::Gray, FudgetFrameType::Inside));
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::HoveredFrameDraw, res.HoveredFrameDraw, _hovered_frame_area))
        _hovered_frame_area = _frame_area;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::PressedFrameDraw, res.PressedFrameDraw, _pressed_frame_area))
        _pressed_frame_area = _frame_area;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::DownFrameDraw, res.DownFrameDraw, _down_frame_area))
        _down_frame_area = _pressed_frame_area;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::FocusedFrameDraw, res.FocusedFrameDraw, _focused_frame_area))
        _focused_frame_area = _frame_area;
    if (!GetMappedDrawable(control, style_override, (int)FudgetFramedFieldPainterIds::DisabledFrameDraw, res.DisabledFrameDraw, _disabled_frame_area))
        _disabled_frame_area = _frame_area;

    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::FramePadding, res.FramePadding, _frame_padding))
        _frame_padding = FudgetPadding(0.0f);
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::HoveredFramePadding, res.HoveredFramePadding, _hovered_frame_padding))
        _hovered_frame_padding = _frame_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::PressedFramePadding, res.PressedFramePadding, _pressed_frame_padding))
        _pressed_frame_padding = _frame_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::DownFramePadding, res.DownFramePadding, _down_frame_padding))
        _down_frame_padding = _pressed_frame_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::FocusedFramePadding, res.FocusedFramePadding, _focused_frame_padding))
        _focused_frame_padding = _frame_padding;
    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::DisabledFramePadding, res.DisabledFramePadding, _disabled_frame_padding))
        _disabled_frame_padding = _frame_padding;

    if (!GetMappedPadding(control, style_override, (int)FudgetFramedFieldPainterIds::ContentPadding, res.ContentPadding, _inner_padding))
        _inner_padding = FudgetPadding(0.0f);
}

void FudgetFramedFieldPainter::Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState state)
{
    FudgetDrawable *area = IsDisabled(state) ? _disabled_field_bg :
        IsDown(state) ? _down_field_bg :
        IsPressed(state) ? _pressed_field_bg :
        IsHovered(state) ? _hovered_field_bg :
        IsFocused(state) ? _focused_field_bg :
        _field_bg;
    FudgetPadding field_padding = IsDisabled(state) ? _disabled_field_padding :
        IsDown(state) ? _down_field_padding :
        IsPressed(state) ? _pressed_field_padding :
        IsHovered(state) ? _hovered_field_padding :
        IsFocused(state) ? _focused_field_padding :
        _field_padding;

    if (area != nullptr)
        control->DrawDrawable(area, field_padding.Padded(bounds));

    FudgetDrawable *frame = IsDisabled(state) ? _disabled_frame_area :
        IsDown(state) ? _down_frame_area :
        IsPressed(state) ? _pressed_frame_area :
        IsHovered(state) ? _hovered_frame_area :
        IsFocused(state) ? _focused_frame_area :
        _frame_area;
    FudgetPadding frame_padding = IsDisabled(state) ? _disabled_frame_padding :
        IsDown(state) ? _down_frame_padding :
        IsPressed(state) ? _pressed_frame_padding :
        IsHovered(state) ? _hovered_frame_padding :
        IsFocused(state) ? _focused_frame_padding :
        _frame_padding;

    if (frame != nullptr)
        control->DrawDrawable(frame, frame_padding.Padded(bounds));
}


// FudgetTextPainter


FudgetSingleLineTextPainter::FudgetSingleLineTextPainter(const SpawnParams &params) : Base(params)
{
}


// FudgetLineEditTextPainter


void FudgetLineEditTextPainter::Initialize(FudgetControl *control, FudgetStyle *style_override, const Variant &mapping)
{
    if (control == nullptr)
        return;

    const ResourceMapping *ptrres = mapping.AsStructure<ResourceMapping>();
    ResourceMapping res;
    if (ptrres != nullptr)
        res = *ptrres;

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

