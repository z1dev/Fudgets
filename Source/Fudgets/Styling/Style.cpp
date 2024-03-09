#include "Style.h"
#include "Themes.h"
#include "Painters/PartPainters.h"
#include "StyleStructs.h"
#include "DrawableBuilder.h"

#include "Engine/Core/Types/BaseTypes.h"
#include "Engine/Core/Log.h"
#include "Engine/Content/AssetReference.h"
#include "Engine/Content/Assets/Texture.h"
#include "Engine/Core/Math/Vector2.h"
#include "Engine/Core/Math/Vector3.h"
#include "Engine/Core/Math/Vector4.h"
#include "Engine/Graphics/Textures/TextureBase.h"
#include "Engine/Scripting/Scripting.h"

FudgetStyle::FudgetStyle() : Base(SpawnParams(Guid::New(), TypeInitializer)),  _parent(nullptr), _owned_style(false)
{

}

FudgetStyle::~FudgetStyle()
{
    for (auto p : _resources)
        Delete(p.second);

    for (auto s : _owned)
        Delete(s);
}

FudgetStyle* FudgetStyle::CreateOwnedStyle(const String &name)
{
    return InheritStyleInternal(name, true);
}

FudgetStyle* FudgetStyle::GetOwnedStyle(const String &name) const
{
    const FudgetStyle *check_style = this;

    for (auto style : _owned)
    {
        if (style->_name == name)
            return style;
    }

    return nullptr;
}

bool FudgetStyle::GetResourceValue(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Variant &result)
{
    if (id < 0)
        return false;

    FudgetStyleResource *res = style != nullptr ? style->GetResource(id) : nullptr;
    if (res == nullptr)
    {
        // Falling through to check the theme directly. This is different from getting the resource from the theme below.
        if (check_theme)
            return theme->GetResource(id, result);
        return false;
    }
    

    if (res->_value_override != Variant::Null)
    {
        result = res->_value_override;
        return true;
    }

    // There was a resource override. Get the value from the theme via the override id.
    if (res->_resource_id >= 0 && theme != nullptr)
        return theme->GetResource(res->_resource_id, result);
    return false;
}

void FudgetStyle::SetValueOverride(int id, const Variant &value)
{
    if (id < 0 || value == Variant::Null)
        return;

    auto it = _resources.find(id);

    if (it == _resources.end())
    {
        if (_parent != nullptr)
        {
            // GetResource will create an override resource with no values set.
            GetResource(id);
            it = _resources.find(id);
        }
        else
        {
            // "Inherit" the resource here.
            it = _resources.insert(std::make_pair(id, New<FudgetStyleResource>(this))).first;
        }
    }

    FudgetStyleResource *res = it->second;
    if (res->_value_override == value)
        return;

    res->_resource_id = -1;
    res->_value_override = value;

    for (FudgetStyle *style : _inherited)
        style->ParentResourceChanged(id, res);
}

void FudgetStyle::SetResourceOverride(int id, int resource_id)
{
    if (id < 0 || resource_id < 0)
        return;

    auto it = _resources.find(id);

    if (it == _resources.end())
    {
        if (_parent != nullptr)
        {
            // GetResource will create an override resource with no values set.
            GetResource(id);
            it = _resources.find(id);
        }
        else
        {
            // "Inherit" the resource here.
            it = _resources.insert(std::make_pair(id, New<FudgetStyleResource>(this))).first;
        }
    }

    FudgetStyleResource *res = it->second;
    if (res->_resource_id == resource_id)
        return;

    res->_resource_id = resource_id;
    res->_value_override = Variant();

    for (FudgetStyle *style : _inherited)
        style->ParentResourceChanged(id, res);
}

void FudgetStyle::ResetResourceOverride(int id)
{
    if (id < 0)
        return;

    auto it = _resources.find(id);
    if (it == _resources.end())
        return;
    FudgetStyleResource *res = it->second;
    if (res->_value_override == Variant::Null || res->_resource_id < 0)
        return;

    res->_value_override = Variant();
    res->_resource_id = -1;

    for (FudgetStyle *style : _inherited)
        style->ParentResourceWasReset(id, res);
}

bool FudgetStyle::GetStyleResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetStyle* &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        String style_name;
        if (StringFromVariant(var, style_name))
        {
            result = FudgetThemes::GetStyle(style_name);
            return result != nullptr;
        }
    }
    result = nullptr;
    return false;
}

bool FudgetStyle::GetPainterMappingResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetPartPainterMapping &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (PainterMappingFromVariant(var, result))
            return true;
    }
    result = FudgetPartPainterMapping();
    return false;
}

bool FudgetStyle::GetStringResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) String &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (StringFromVariant(var, result))
            return true;
    }
    result = String();
    return false;
}

bool FudgetStyle::GetColorResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Color &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (ColorFromVariant(var, result))
            return true;
    }
    result = Color();
    return false;
}

bool FudgetStyle::GetBoolResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) bool &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (BoolFromVariant(var, result))
            return true;
    }
    result = 0.0f;
    return false;
}

bool FudgetStyle::GetFloatResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) float &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (FloatFromVariant(var, result))
            return true;
    }
    result = 0.0f;
    return false;
}

bool FudgetStyle::GetFloat2Resource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Float2 &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (Float2FromVariant(var, result))
            return true;
    }
    result = Float2();
    return false;
}

bool FudgetStyle::GetFloat3Resource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Float3 &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (Float3FromVariant(var, result))
            return true;
    }
    result = Float3();
    return false;
}

bool FudgetStyle::GetFloat4Resource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Float4 &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (Float4FromVariant(var, result))
            return true;
    }
    result = Float4();
    return false;
}

bool FudgetStyle::GetIntResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) int &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (IntFromVariant(var, result))
            return true;
    }
    result = 0;
    return false;
}

bool FudgetStyle::GetInt2Resource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Int2 &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (Int2FromVariant(var, result))
            return true;
    }
    result = Int2();
    return false;
}

bool FudgetStyle::GetInt3Resource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Int3 &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (Int3FromVariant(var, result))
            return true;
    }
    result = Int3();
    return false;
}

bool FudgetStyle::GetInt4Resource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Int4 &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (Int4FromVariant(var, result))
            return true;
    }
    result = Int4();
    return false;
}

bool FudgetStyle::GetPaddingResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetPadding &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (PaddingFromVariant(var, result))
            return true;
    }
    result = FudgetPadding();
    return false;
}

bool FudgetStyle::GetTextDrawResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetTextDrawSettings &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (TextDrawSettingsFromVariant(var, result))
            return true;
    }
    result = FudgetTextDrawSettings();
    return false;
}

bool FudgetStyle::GetFontSettingsResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetFontSettings &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (FontSettingsFromVariant(var, result))
            return true;
    }
    result = FudgetFontSettings();
    return false;
}

bool FudgetStyle::GetFontResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetFont &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (FontFromVariant(var, result))
            return true;
    }
    result = FudgetFont();
    return false;
}

bool FudgetStyle::GetDrawAreaResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetDrawArea &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (AreaFromVariant(var, result))
            return true;
    }
    result = FudgetDrawArea();
    return false;
}

bool FudgetStyle::GetDrawableResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetDrawable* &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (DrawableFromVariant(style, theme, var, result))
            return true;
    }
    result = nullptr; // FudgetDrawInstructionList();
    return false;
}

bool FudgetStyle::GetTextureResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) TextureBase* &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (TextureFromVariant(var, result))
            return true;
    }
    result = nullptr;
    return false;
}

bool FudgetStyle::GetTextDrawSettingsResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetTextDrawSettings &result)
{
    Variant var;
    if (GetResourceValue(style, theme, id, check_theme, var))
    {
        if (TextDrawSettingsFromVariant(var, result))
            return true;
    }
    result = FudgetTextDrawSettings();
    return false;
}

FudgetStyle* FudgetStyle::InheritStyleInternal(const String &name, bool owned)
{
    String trimmed_name = name.TrimTrailing();
    if (name.IsEmpty() || name.Contains(TEXT("/")))
        return nullptr;

    FudgetStyle *result = nullptr;
    if (!owned)
        result = FudgetThemes::CreateStyle(trimmed_name);
    else if (GetOwnedStyle(trimmed_name) == nullptr)
    {
        result = New<FudgetStyle>();
        result->_owned_style = true;
        result->_name = trimmed_name;
    }

    if (result == nullptr)
        return nullptr;

    result->_parent = this;

    _inherited.Add(result);
    if (owned)
        _owned.Add(result);
    return result;
}

FudgetStyleResource* FudgetStyle::GetResource(int id)
{
    if (id < 0)
        return nullptr;

    auto it = _resources.find(id);
    if (it != _resources.end())
    {
        FudgetStyleResource *owned = it->second;
        FudgetStyleResource *res = owned;
        while (res != nullptr)
        {
            if (res->_resource_id >= 0 || res->_value_override != Variant::Null)
            {
                if (res != owned)
                {
                    // "Inherit" the resource here.
                    owned->_inherited_resource = res;
                }
                return res;
            }

            res = res->_inherited_resource;
        }
    }

    if (_parent != nullptr)
    {
        FudgetStyleResource *tmp = it != _resources.end() ? it->second : New<FudgetStyleResource>(this);

        FudgetStyleResource *res = _parent->GetResource(id);
        if (res != nullptr)
        {
            // "Inherit" the resource here.
            tmp->_inherited_resource = res;
        }
        if (it == _resources.end())
            _resources[id] = tmp;
        return res;
    }

    return nullptr;
}

void FudgetStyle::ParentResourceChanged(int id, FudgetStyleResource *resource)
{
    auto it = _resources.find(id);
    if (it != _resources.end())
    {
        FudgetStyleResource *res = it->second;
        res->_inherited_resource = resource;
        if (res->_resource_id >= 0 || res->_value_override != Variant::Null)
            return;
    }

    for (FudgetStyle *style : _inherited)
        style->ParentResourceChanged(id, resource);
}

void FudgetStyle::ParentResourceWasReset(int id, FudgetStyleResource *resource)
{
    auto it = _resources.find(id);
    if (it != _resources.end())
    {
        FudgetStyleResource *res = it->second;
        res->_inherited_resource = resource->_inherited_resource;
        if (res->_resource_id >= 0 || res->_value_override != Variant::Null)
            return;
    }

    for (FudgetStyle *style : _inherited)
        style->ParentResourceWasReset(id, resource);
}

bool FudgetStyle::PainterMappingFromVariant(const Variant &var, FudgetPartPainterMapping &result)
{
    if (var.Type.Type == VariantType::Structure)
    {
        const FudgetPartPainterMapping *mapping = var.AsStructure<FudgetPartPainterMapping>();
        if (mapping != nullptr)
        {
            const ScriptingTypeHandle type = Scripting::FindScriptingType(mapping->PainterType);

            if (!type || !FudgetPartPainter::TypeInitializer.IsAssignableFrom(type))
            {
                result = FudgetPartPainterMapping();
                return false;
            }

            result = *mapping;
            return true;
        }
    }

    result = FudgetPartPainterMapping();
    return false;
}

bool FudgetStyle::StringFromVariant(const Variant &var, String &result)
{
    if (var.Type.Type == VariantType::String)
    {
        result = var.ToString();
        return true;
    }

    return false;
}


bool FudgetStyle::AreaFromVariant(const Variant &var, FudgetDrawArea &result)
{
    if (var.Type.Type == VariantType::Color)
    {
        result = FudgetDrawArea(var.AsColor());
        return true;
    }

    if (var.Type.Type == VariantType::Asset)
    {
        AssetReference<Texture> asset = dynamic_cast<Texture*>(var.AsAsset);
        if (asset.Get() == nullptr)
            return false;

        result = FudgetDrawArea(asset.Get(), true, false);
        return true;
    }

    if (var.Type.Type == VariantType::Structure)
    {
        const FudgetDrawArea *area = var.AsStructure<FudgetDrawArea>();
        if (area == nullptr)
            return false;
        result = *area;
        return true;
    }

    return false;
}

bool FudgetStyle::DrawableFromVariant(FudgetStyle *style, FudgetTheme *theme, const Variant &var, FudgetDrawable* &result)
{
    if (var.Type.Type == VariantType::Color)
    {
        result = FudgetDrawable::FromColor(var.AsColor());
        return true;
    }

    if (var.Type.Type == VariantType::Asset)
    {
        AssetReference<Texture> asset = dynamic_cast<Texture*>(var.AsAsset);
        if (asset.Get() == nullptr)
            return false;

        result = FudgetDrawable::FromDrawArea(FudgetDrawArea(asset.Get(), true, false));
        return true;
    }

    if (var.Type.Type == VariantType::Structure)
    {
        const FudgetDrawArea *drawarea = var.AsStructure<FudgetDrawArea>();
        if (drawarea != nullptr)
        {
            result = FudgetDrawable::FromDrawArea(*drawarea);
            return true;
        }

        const FudgetDrawableIndex *drawindex = var.AsStructure<FudgetDrawableIndex>();
        if (drawindex != nullptr)
        {
            FudgetDrawInstructionList *arealist = FudgetThemes::GetDrawInstructionList(drawindex->Index);
            if (arealist == nullptr)
                return false;
            result = FudgetDrawable::FromDrawInstructionList(style, theme, arealist);
            return true;
        }
    }

    return false;
}

bool FudgetStyle::TextureFromVariant(const Variant &var, TextureBase* &result)
{
    if (var.Type.Type == VariantType::Asset)
    {
        AssetReference<TextureBase> asset = dynamic_cast<TextureBase*>(var.AsAsset);
        result = asset.Get();
        return result != nullptr;
    }
    return false;
}

bool FudgetStyle::TextDrawSettingsFromVariant(const Variant &var, FudgetTextDrawSettings &result)
{
    if (var.Type.Type == VariantType::Color)
    {
        result = FudgetTextDrawSettings(var.AsColor());
        return true;
    }

    if (var.Type.Type == VariantType::Float2)
    {
        result = FudgetTextDrawSettings(var.AsFloat2());
        return true;
    }

    if (var.Type.Type == VariantType::Structure)
    {
        const FudgetTextDrawSettings *settings = var.AsStructure<FudgetTextDrawSettings>();
        if (settings != nullptr)
        {
            result = *settings;
            return true;
        }


        // TODO: materials to draw text with
         
        const FudgetPadding *padding = var.AsStructure<FudgetPadding>();
        if (padding != nullptr)
        {
            result = FudgetTextDrawSettings();
            result.Padding = *padding;
            return true;
        }
    }

    return false;
}

bool FudgetStyle::FontSettingsFromVariant(const Variant &var, FudgetFontSettings &result)
{
    if (var.Type.Type == VariantType::Structure)
    {
        const FudgetFontSettings *ptr = var.AsStructure<FudgetFontSettings>();
        if (ptr != nullptr)
        {
            result = *ptr;
            return true;
        }
    }

    return false;
}

bool FudgetStyle::FontFromVariant(const Variant &var, FudgetFont &result)
{
    if (!FontSettingsFromVariant(var, result.Settings))
    {
        result = FudgetFont();
        return false;
    }

    FontAsset *asset = FudgetThemes::GetFontAsset(result.Settings.FontId);
    if (asset == nullptr)
    {
        result = FudgetFont();
        return false;
    }

    if (result.Settings.Bold)
        asset = asset->GetBold();
    else if (result.Settings.Italics)
        asset = asset->GetItalic();
    // TODO: No bold and italic at the same time? asset doesn't seem to support it.
    result.Font = asset->CreateFont(result.Settings.Size);

    return true;
}

bool FudgetStyle::PaddingFromVariant(const Variant &var, FudgetPadding &result)
{
    if (var.Type.Type == VariantType::Structure)
    {
        const FudgetPadding *padding = var.AsStructure<FudgetPadding>();
        if (padding == nullptr)
        {
            result = FudgetPadding();
            return false;
        }
        result = *padding;
        return true;
    }
    return false;
}

bool FudgetStyle::BoolFromVariant(const Variant &var, bool &result)
{
    if (var.Type.Type == VariantType::Bool)
    {
        result = var.AsBool;
        return true;
    }
    return false;
}

bool FudgetStyle::FloatFromVariant(const Variant &var, float &result)
{
    if (var.Type.Type == VariantType::Float)
    {
        result = var.AsFloat;
        return true;
    }
    return false;
}

bool FudgetStyle::Float2FromVariant(const Variant &var, Float2 &result)
{
    if (var.Type.Type == VariantType::Float2)
    {
        result = var.AsFloat2();
        return true;
    }
    return false;
}

bool FudgetStyle::Float3FromVariant(const Variant &var, Float3 &result)
{
    if (var.Type.Type == VariantType::Float3)
    {
        result = var.AsFloat3();
        return true;
    }
    return false;
}

bool FudgetStyle::Float4FromVariant(const Variant &var, Float4 &result)
{
    if (var.Type.Type == VariantType::Float4)
    {
        result = var.AsFloat4();
        return true;
    }
    return false;
}

bool FudgetStyle::IntFromVariant(const Variant &var, int &result)
{
    if (var.Type.Type == VariantType::Int)
    {
        result = var.AsInt;
        return true;
    }
    return false;
}

bool FudgetStyle::Int2FromVariant(const Variant &var, Int2 &result)
{
    if (var.Type.Type == VariantType::Int2)
    {
        result = var.AsInt2();
        return true;
    }
    return false;
}

bool FudgetStyle::Int3FromVariant(const Variant &var, Int3 &result)
{
    if (var.Type.Type == VariantType::Int3)
    {
        result = var.AsInt3();
        return true;
    }
    return false;
}

bool FudgetStyle::Int4FromVariant(const Variant &var, Int4 &result)
{
    if (var.Type.Type == VariantType::Int4)
    {
        result = var.AsInt4();
        return true;
    }
    return false;
}

bool FudgetStyle::ColorFromVariant(const Variant &var, Color &result)
{
    if (var.Type.Type == VariantType::Color)
    {
        result = var.AsColor();
        return true;
    }
    return false;
}
