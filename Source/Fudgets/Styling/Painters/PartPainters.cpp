#include "PartPainters.h"
#include "../Style.h"
#include "../Themes.h"
#include "../../MarginStructs.h"
#include "../StyleStructs.h"
#include "../../Control.h"
#include "../DrawableBuilder.h"
#include "../../Control.h"

#include "../StateOrderBuilder.h"


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

void FudgetPartPainter::GetMappedStateOrder(int theme_order_index, FudgetStateOrder* &order) const
{
    order = FudgetThemes::GetStateOrder(theme_order_index);
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


// FudgetStatePainter


FudgetStatePainter::FudgetStatePainter(const SpawnParams &params) : Base(params)
{
}


// FudgetTextPainter


FudgetSingleLineTextPainter::FudgetSingleLineTextPainter(const SpawnParams &params) : Base(params)
{
}


// FudgetListItemPainter


FudgetListItemPainter::FudgetListItemPainter(const SpawnParams &params) : Base(params)
{

}

