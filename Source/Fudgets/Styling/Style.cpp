#include "Style.h"
#include "Themes.h"
#include "PartPainters.h"
#include "StyleStructs.h"

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

bool FudgetStyle::GetResourceValue(FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Variant &result)
{
    if (id < 0)
        return false;

    FudgetStyleResource *res = GetResource(id);
    if (res == nullptr)
    {
        if (check_theme)
            return theme->GetResource(id, result);
        return false;
    }
    

    if (res->_value_override != Variant::Null)
    {
        result = res->_value_override;
        return true;
    }

    if (res->_resource_id >= 0 && theme != nullptr)
        return theme->GetResource(res->_resource_id, result);
    return false;
}

bool FudgetStyle::GetResourceValue(FudgetTheme *theme, const Span<int> &ids, bool check_theme, API_PARAM(Out) Variant &result)
{
    for (auto id : ids)
    {
        if (GetResourceValue(theme, id, check_theme, result))
            return true;
    }

    result = Variant();
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

bool FudgetStyle::GetStyleResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetStyle* &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
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

bool FudgetStyle::GetStyleResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetStyle* &result)
{
    for (auto id : ids)
    {
        if (GetStyleResource(theme, id, result))
            return true;
    }
    return false;
}

bool FudgetStyle::GetPainterMappingResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetPartPainterMapping &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (PainterMappingFromVariant(var, result))
            return true;
    }
    result = FudgetPartPainterMapping();
    return false;
}

bool FudgetStyle::GetPainterMappingResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetPartPainterMapping &result)
{
    for (auto id : ids)
    {
        if (GetPainterMappingResource(theme, id, result))
            return true;
    }
    return false;
}

bool FudgetStyle::GetStringResource(FudgetTheme *theme, int id, API_PARAM(Out) String &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (StringFromVariant(var, result))
            return true;
    }
    result = String();
    return false;
}

bool FudgetStyle::GetStringResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) String &result)
{
    for (auto id : ids)
    {
        if (GetStringResource(theme, id, result))
            return true;
    }
    return false;
}

bool FudgetStyle::GetColorResource(FudgetTheme *theme, int id, API_PARAM(Out) Color &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (ColorFromVariant(var, result))
            return true;
    }
    result = Color();
    return false;
}

bool FudgetStyle::GetColorResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) Color &result)
{
    for (auto id : ids)
    {
        if (GetColorResource(theme, id, result))
            return true;
    }
    result = Color();
    return false;
}

bool FudgetStyle::GetBoolResource(FudgetTheme *theme, int id, API_PARAM(Out) bool &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (BoolFromVariant(var, result))
            return true;
    }
    result = 0.0f;
    return false;
}

bool FudgetStyle::GetBoolResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) bool &result)
{
    for (auto id : ids)
    {
        if (GetBoolResource(theme, id, result))
            return true;
    }
    result = 0.0f;
    return false;
}

bool FudgetStyle::GetFloatResource(FudgetTheme *theme, int id, API_PARAM(Out) float &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (FloatFromVariant(var, result))
            return true;
    }
    result = 0.0f;
    return false;
}

bool FudgetStyle::GetFloatResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) float &result)
{
    for (auto id : ids)
    {
        if (GetFloatResource(theme, id, result))
            return true;
    }
    result = 0.0f;
    return false;
}

bool FudgetStyle::GetFloat2Resource(FudgetTheme *theme, int id, API_PARAM(Out) Float2 &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (Float2FromVariant(var, result))
            return true;
    }
    result = Float2();
    return false;
}

bool FudgetStyle::GetFloat2Resource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) Float2 &result)
{
    for (auto id : ids)
    {
        if (GetFloat2Resource(theme, id, result))
            return true;
    }
    result = Float2();
    return false;
}

bool FudgetStyle::GetFloat3Resource(FudgetTheme *theme, int id, API_PARAM(Out) Float3 &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (Float3FromVariant(var, result))
            return true;
    }
    result = Float3();
    return false;
}

bool FudgetStyle::GetFloat3Resource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) Float3 &result)
{
    for (auto id : ids)
    {
        if (GetFloat3Resource(theme, id, result))
            return true;
    }
    result = Float3();
    return false;
}

bool FudgetStyle::GetFloat4Resource(FudgetTheme *theme, int id, API_PARAM(Out) Float4 &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (Float4FromVariant(var, result))
            return true;
    }
    result = Float4();
    return false;
}

bool FudgetStyle::GetFloat4Resource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) Float4 &result)
{
    for (auto id : ids)
    {
        if (GetFloat4Resource(theme, id, result))
            return true;
    }
    result = Float4();
    return false;
}

bool FudgetStyle::GetIntResource(FudgetTheme *theme, int id, API_PARAM(Out) int &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (IntFromVariant(var, result))
            return true;
    }
    result = 0;
    return false;
}

bool FudgetStyle::GetIntResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) int &result)
{
    for (auto id : ids)
    {
        if (GetIntResource(theme, id, result))
            return true;
    }
    result = 0;
    return false;
}

bool FudgetStyle::GetInt2Resource(FudgetTheme *theme, int id, API_PARAM(Out) Int2 &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (Int2FromVariant(var, result))
            return true;
    }
    result = Int2();
    return false;
}

bool FudgetStyle::GetInt2Resource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) Int2 &result)
{
    for (auto id : ids)
    {
        if (GetInt2Resource(theme, id, result))
            return true;
    }
    result = Int2();
    return false;
}

bool FudgetStyle::GetInt3Resource(FudgetTheme *theme, int id, API_PARAM(Out) Int3 &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (Int3FromVariant(var, result))
            return true;
    }
    result = Int3();
    return false;
}

bool FudgetStyle::GetInt3Resource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) Int3 &result)
{
    for (auto id : ids)
    {
        if (GetInt3Resource(theme, id, result))
            return true;
    }
    result = Int3();
    return false;
}

bool FudgetStyle::GetInt4Resource(FudgetTheme *theme, int id, API_PARAM(Out) Int4 &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (Int4FromVariant(var, result))
            return true;
    }
    result = Int4();
    return false;
}

bool FudgetStyle::GetInt4Resource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) Int4 &result)
{
    for (auto id : ids)
    {
        if (GetInt4Resource(theme, id, result))
            return true;
    }
    result = Int4();
    return false;
}

bool FudgetStyle::GetPaddingResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetPadding &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (PaddingFromVariant(var, result))
            return true;
    }
    result = FudgetPadding();
    return false;
}

bool FudgetStyle::GetPaddingResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetPadding &result)
{
    for (auto id : ids)
    {
        if (GetPaddingResource(theme, id, result))
            return true;
    }
    result = FudgetPadding();
    return false;
}

bool FudgetStyle::GetTextDrawResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetTextDrawSettings &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (TextDrawSettingsFromVariant(var, result))
            return true;
    }
    result = FudgetTextDrawSettings();
    return false;
}

bool FudgetStyle::GetTextDrawResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetTextDrawSettings &result)
{
    for (auto id : ids)
    {
        if (GetTextDrawResource(theme, id, result))
            return true;
    }
    result = FudgetTextDrawSettings();
    return false;
}

bool FudgetStyle::GetFontSettingsResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetFontSettings &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (FontSettingsFromVariant(var, result))
            return true;
    }
    result = FudgetFontSettings();
    return false;
}

bool FudgetStyle::GetFontSettingsResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetFontSettings &result)
{
    for (auto id : ids)
    {
        if (GetFontSettingsResource(theme, id, result))
            return true;
    }
    result = FudgetFontSettings();
    return false;
}

bool FudgetStyle::GetFontResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetFont &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (FontFromVariant(var, result))
            return true;
    }
    result = FudgetFont();
    return false;
}

bool FudgetStyle::GetFontResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetFont &result)
{
    for (auto id : ids)
    {
        if (GetFontResource(theme, id, result))
            return true;
    }
    result = FudgetFont();
    return false;
}

bool FudgetStyle::GetDrawAreaResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetDrawArea &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (AreaFromVariant(var, result))
            return true;
    }
    result = FudgetDrawArea();
    return false;
}

bool FudgetStyle::GetDrawAreaResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetDrawArea &result)
{
    for (auto id : ids)
    {
        if (GetDrawAreaResource(theme, id, result))
            return true;
    }
    result = FudgetDrawArea();
    return false;
}

bool FudgetStyle::GetDrawableResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetDrawable* &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (DrawableFromVariant(theme, var, result))
            return true;
    }
    result = nullptr; // FudgetDrawInstructionList();
    return false;
}

bool FudgetStyle::GetDrawableResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetDrawable* &result)
{
    for (auto id : ids)
    {
        if (GetDrawableResource(theme, id, result))
            return true;
    }
    result = nullptr; // FudgetDrawInstructionList();
    return false;
}

bool FudgetStyle::GetTextureResource(FudgetTheme *theme, int id, API_PARAM(Out) TextureBase* &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (TextureFromVariant(var, result))
            return true;
    }
    result = nullptr;
    return false;
}

bool FudgetStyle::GetTextureResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) TextureBase* &result)
{
    for (auto id : ids)
    {
        if (GetTextureResource(theme, id, result))
            return true;
    }
    result = nullptr;
    return false;
}

bool FudgetStyle::GetTextDrawSettingsResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetTextDrawSettings &result)
{
    Variant var;
    if (GetResourceValue(theme, id, true, var))
    {
        if (TextDrawSettingsFromVariant(var, result))
            return true;
    }
    result = FudgetTextDrawSettings();
    return false;
}

bool FudgetStyle::GetTextDrawSettingsResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetTextDrawSettings &result)
{
    for (auto id : ids)
    {
        if (GetTextDrawSettingsResource(theme, id, result))
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

bool FudgetStyle::PainterMappingFromVariant(const Variant &var, FudgetPartPainterMapping &result) const
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

bool FudgetStyle::StringFromVariant(const Variant &var, String &result) const
{
    if (var.Type.Type == VariantType::String)
    {
        result = var.ToString();
        return true;
    }

    return false;
}


bool FudgetStyle::AreaFromVariant(const Variant &var, FudgetDrawArea &result) const
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

bool FudgetStyle::DrawableFromVariant(FudgetTheme *theme, const Variant &var, FudgetDrawable* &result)
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
            result = FudgetDrawable::FromDrawInstructionList(this, theme, arealist);
            return true;
        }
    }

    return false;
}

bool FudgetStyle::TextureFromVariant(const Variant &var, TextureBase* &result) const
{
    if (var.Type.Type == VariantType::Asset)
    {
        AssetReference<TextureBase> asset = dynamic_cast<TextureBase*>(var.AsAsset);
        result = asset.Get();
        return result != nullptr;
    }
    return false;
}

bool FudgetStyle::TextDrawSettingsFromVariant(const Variant &var, FudgetTextDrawSettings &result) const
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

bool FudgetStyle::FontSettingsFromVariant(const Variant &var, FudgetFontSettings &result) const
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

bool FudgetStyle::FontFromVariant(const Variant &var, FudgetFont &result) const
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

bool FudgetStyle::PaddingFromVariant(const Variant &var, FudgetPadding &result) const
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

bool FudgetStyle::BoolFromVariant(const Variant &var, bool &result) const
{
    if (var.Type.Type == VariantType::Bool)
    {
        result = var.AsBool;
        return true;
    }
    return false;
}

bool FudgetStyle::FloatFromVariant(const Variant &var, float &result) const
{
    if (var.Type.Type == VariantType::Float)
    {
        result = var.AsFloat;
        return true;
    }
    return false;
}

bool FudgetStyle::Float2FromVariant(const Variant &var, Float2 &result) const
{
    if (var.Type.Type == VariantType::Float2)
    {
        result = var.AsFloat2();
        return true;
    }
    return false;
}

bool FudgetStyle::Float3FromVariant(const Variant &var, Float3 &result) const
{
    if (var.Type.Type == VariantType::Float3)
    {
        result = var.AsFloat3();
        return true;
    }
    return false;
}

bool FudgetStyle::Float4FromVariant(const Variant &var, Float4 &result) const
{
    if (var.Type.Type == VariantType::Float4)
    {
        result = var.AsFloat4();
        return true;
    }
    return false;
}

bool FudgetStyle::IntFromVariant(const Variant &var, int &result) const
{
    if (var.Type.Type == VariantType::Int)
    {
        result = var.AsInt;
        return true;
    }
    return false;
}

bool FudgetStyle::Int2FromVariant(const Variant &var, Int2 &result) const
{
    if (var.Type.Type == VariantType::Int2)
    {
        result = var.AsInt2();
        return true;
    }
    return false;
}

bool FudgetStyle::Int3FromVariant(const Variant &var, Int3 &result) const
{
    if (var.Type.Type == VariantType::Int3)
    {
        result = var.AsInt3();
        return true;
    }
    return false;
}

bool FudgetStyle::Int4FromVariant(const Variant &var, Int4 &result) const
{
    if (var.Type.Type == VariantType::Int4)
    {
        result = var.AsInt4();
        return true;
    }
    return false;
}

bool FudgetStyle::ColorFromVariant(const Variant &var, Color &result) const
{
    if (var.Type.Type == VariantType::Color)
    {
        result = var.AsColor();
        return true;
    }
    return false;
}
