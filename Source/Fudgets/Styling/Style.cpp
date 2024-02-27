#include "Style.h"
#include "Themes.h"
#include "PartPainters.h"

#include "Engine/Core/Types/BaseTypes.h"
#include "Engine/Core/Log.h"
#include "Engine/Content/AssetReference.h"
#include "Engine/Content/Assets/Texture.h"
#include "Engine/Core/Math/Vector2.h"
#include "Engine/Core/Math/Vector3.h"
#include "Engine/Core/Math/Vector4.h"
#include "Engine/Graphics/Textures/TextureBase.h"

//FudgetStyle::FudgetStyle(String name) : FudgetStyle(FudgetThemes::RegisterToken(name))
//{
//
//}
//
//FudgetStyle::FudgetStyle(FudgetToken name_token) : Base(SpawnParams(Guid::New(), TypeInitializer)),
//	_style_name(name_token), _parent(nullptr)
//{
//	if (!FudgetThemes::RegisterStyle(_style_name, this))
//		LOG(Error, "Invalid token for FudgetStyle. Style wasn't registered.");
//}

FudgetStyle::FudgetStyle() : Base(SpawnParams(Guid::New(), TypeInitializer)),  _parent(nullptr)
{

}

FudgetStyle::~FudgetStyle()
{
}

FudgetStyle* FudgetStyle::CreateInheritedStyle(FudgetToken name_token)
{
	if (!name_token.IsValid())
		return nullptr;

	FudgetStyle *result = FudgetThemes::CreateStyle(name_token);
	if (result == nullptr)
		return nullptr;

	result->_parent = this;
	_derived.Add(result);
	return result;
}

FudgetStyleResource* FudgetStyle::GetResource(FudgetToken token)
{
	if (!token.IsValid())
		return nullptr;

	auto it = _resources.find(token);
	if (it != _resources.end())
	{
		FudgetStyleResource *res = &it->second;
		bool first = true;
		while (res != nullptr)
		{
			if (res->_resource_id.IsValid() || res->_value_override != Variant::Null)
			{
				if (!first)
				{
					// "Inherit" the resource here.
					FudgetStyleResource tmp;
					tmp._inherited_resource = res;
					_resources[token] = tmp;
				}
				return res;
			}

			res = res->_inherited_resource;
			first = false;
		}
	}

	if (_parent != nullptr)
	{
		FudgetStyleResource tmp;

		FudgetStyleResource *res = _parent->GetResource(token);
		if (res != nullptr)
		{
			// "Inherit" the resource here.
			tmp._inherited_resource = res;
		}
		_resources[token] = tmp;
		return res;
	}

	return nullptr;
}

bool FudgetStyle::GetResourceValue(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Variant &result)
{
	if (!token.IsValid())
		return false;

	FudgetStyleResource *res = GetResource(token);
	if (res == nullptr)
		return false;

	if (res->_value_override != Variant::Null)
	{
		result = res->_value_override;
		return true;
	}

	if (res->_resource_id.IsValid() && theme != nullptr)
		return theme->GetResource(res->_resource_id, result);
	return false;
}

bool FudgetStyle::GetResourceValue(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) Variant &result)
{
	for (auto t : tokens)
	{
		if (GetResourceValue(theme, t, result))
			return true;
	}

	result = Variant();
	return false;
}

void FudgetStyle::SetValueOverride(FudgetToken token, const Variant &value)
{
	if (!token.IsValid() || value == Variant::Null)
		return;

	auto it = _resources.find(token);

	if (it == _resources.end())
	{
		if (_parent != nullptr)
		{
			// GetResource will create an override resource with no values set.
			GetResource(token);
			it = _resources.find(token);
		}
		else
		{
			// "Inherit" the resource here.
			FudgetStyleResource tmp;
			_resources[token] = tmp;
			it = _resources.find(token);
		}
	}

	FudgetStyleResource &res = it->second;
	if (res._value_override == value)
		return;

	res._resource_id = FudgetToken::Invalid;
	res._value_override = value;

	for (FudgetStyle *style : _derived)
		style->ParentResourceChanged(token, &res);
}

void FudgetStyle::SetResourceOverride(FudgetToken token, FudgetToken resource_token)
{
	if (!token.IsValid() || !resource_token.IsValid())
		return;

	auto it = _resources.find(token);

	if (it == _resources.end())
	{
		if (_parent != nullptr)
		{
			// GetResource will create an override resource with no values set.
			GetResource(token);
			it = _resources.find(token);
		}
		else
		{
			// "Inherit" the resource here.
			FudgetStyleResource tmp;
			_resources[token] = tmp;
			it = _resources.find(token);
		}
	}

	FudgetStyleResource &res = it->second;
	if (res._resource_id == resource_token)
		return;

	res._resource_id = resource_token;
	res._value_override = Variant::Null;

	for (FudgetStyle *style : _derived)
		style->ParentResourceChanged(token, &res);
}

void FudgetStyle::ResetResourceOverride(FudgetToken token)
{
	if (!token.IsValid())
		return;

	auto it = _resources.find(token);
	if (it == _resources.end())
		return;
	FudgetStyleResource &res = it->second;
	if (res._value_override == Variant::Null || !res._resource_id.IsValid())
		return;

	res._value_override = Variant::Null;
	res._resource_id = FudgetToken::Invalid;

	for (FudgetStyle *style : _derived)
		style->ParentResourceWasReset(token, &res);
}

bool FudgetStyle::GetTokenResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetToken &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (TokenFromVariant(var, result))
			return true;
	}
	result = FudgetToken();
	return false;
}

bool FudgetStyle::GetTokenResource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) FudgetToken &result)
{
	for (auto t : tokens)
	{
		if (GetTokenResource(theme, t, result))
			return true;
	}
	return false;
}

bool FudgetStyle::GetColorResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Color &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (ColorFromVariant(var, result))
			return true;
	}
	result = Color();
	return false;
}

bool FudgetStyle::GetColorResource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) Color &result)
{
	for (auto t : tokens)
	{
		if (GetColorResource(theme, t, result))
			return true;
	}
	result = Color();
	return false;
}

bool FudgetStyle::GetBoolResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) bool &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (BoolFromVariant(var, result))
			return true;
	}
	result = 0.0f;
	return false;
}

bool FudgetStyle::GetBoolResource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) bool &result)
{
	for (auto t : tokens)
	{
		if (GetBoolResource(theme, t, result))
			return true;
	}
	result = 0.0f;
	return false;
}

bool FudgetStyle::GetFloatResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) float &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (FloatFromVariant(var, result))
			return true;
	}
	result = 0.0f;
	return false;
}

bool FudgetStyle::GetFloatResource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) float &result)
{
	for (auto t : tokens)
	{
		if (GetFloatResource(theme, t, result))
			return true;
	}
	result = 0.0f;
	return false;
}

bool FudgetStyle::GetFloat2Resource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Float2 &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (Float2FromVariant(var, result))
			return true;
	}
	result = Float2();
	return false;
}

bool FudgetStyle::GetFloat2Resource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) Float2 &result)
{
	for (auto t : tokens)
	{
		if (GetFloat2Resource(theme, t, result))
			return true;
	}
	result = Float2();
	return false;
}

bool FudgetStyle::GetFloat3Resource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Float3 &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (Float3FromVariant(var, result))
			return true;
	}
	result = Float3();
	return false;
}

bool FudgetStyle::GetFloat3Resource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) Float3 &result)
{
	for (auto t : tokens)
	{
		if (GetFloat3Resource(theme, t, result))
			return true;
	}
	result = Float3();
	return false;
}

bool FudgetStyle::GetFloat4Resource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Float4 &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (Float4FromVariant(var, result))
			return true;
	}
	result = Float4();
	return false;
}

bool FudgetStyle::GetFloat4Resource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) Float4 &result)
{
	for (auto t : tokens)
	{
		if (GetFloat4Resource(theme, t, result))
			return true;
	}
	result = Float4();
	return false;
}

bool FudgetStyle::GetIntResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) int &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (IntFromVariant(var, result))
			return true;
	}
	result = 0;
	return false;
}

bool FudgetStyle::GetIntResource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) int &result)
{
	for (auto t : tokens)
	{
		if (GetIntResource(theme, t, result))
			return true;
	}
	result = 0;
	return false;
}

bool FudgetStyle::GetInt2Resource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Int2 &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (Int2FromVariant(var, result))
			return true;
	}
	result = Int2();
	return false;
}

bool FudgetStyle::GetInt2Resource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) Int2 &result)
{
	for (auto t : tokens)
	{
		if (GetInt2Resource(theme, t, result))
			return true;
	}
	result = Int2();
	return false;
}

bool FudgetStyle::GetInt3Resource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Int3 &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (Int3FromVariant(var, result))
			return true;
	}
	result = Int3();
	return false;
}

bool FudgetStyle::GetInt3Resource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) Int3 &result)
{
	for (auto t : tokens)
	{
		if (GetInt3Resource(theme, t, result))
			return true;
	}
	result = Int3();
	return false;
}

bool FudgetStyle::GetInt4Resource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Int4 &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (Int4FromVariant(var, result))
			return true;
	}
	result = Int4();
	return false;
}

bool FudgetStyle::GetInt4Resource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) Int4 &result)
{
	for (auto t : tokens)
	{
		if (GetInt4Resource(theme, t, result))
			return true;
	}
	result = Int4();
	return false;
}

bool FudgetStyle::GetPaddingResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetPadding &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (PaddingFromVariant(var, result))
			return true;
	}
	result = FudgetPadding();
	return false;
}

bool FudgetStyle::GetPaddingResource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) FudgetPadding &result)
{
	for (auto t : tokens)
	{
		if (GetPaddingResource(theme, t, result))
			return true;
	}
	result = FudgetPadding();
	return false;
}

bool FudgetStyle::GetTextDrawResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetTextDrawSettings &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (TextDrawSettingsFromVariant(var, result))
			return true;
	}
	result = FudgetTextDrawSettings();
	return false;
}

bool FudgetStyle::GetTextDrawResource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) FudgetTextDrawSettings &result)
{
	for (auto t : tokens)
	{
		if (GetTextDrawResource(theme, t, result))
			return true;
	}
	result = FudgetTextDrawSettings();
	return false;
}

bool FudgetStyle::GetFontSettingsResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetFontSettings &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (FontSettingsFromVariant(var, result))
			return true;
	}
	result = FudgetFontSettings();
	return false;
}

bool FudgetStyle::GetFontSettingsResource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) FudgetFontSettings &result)
{
	for (auto t : tokens)
	{
		if (GetFontSettingsResource(theme, t, result))
			return true;
	}
	result = FudgetFontSettings();
	return false;
}

bool FudgetStyle::GetFontResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetFont &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (FontFromVariant(var, result))
			return true;
	}
	result = FudgetFont();
	return false;
}

bool FudgetStyle::GetFontResource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) FudgetFont &result)
{
	for (auto t : tokens)
	{
		if (GetFontResource(theme, t, result))
			return true;
	}
	result = FudgetFont();
	return false;
}

bool FudgetStyle::GetDrawAreaResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetDrawArea &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (AreaFromVariant(var, result))
			return true;
	}
	result = FudgetDrawArea();
	return false;
}

bool FudgetStyle::GetDrawAreaResource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) FudgetDrawArea &result)
{
	for (auto t : tokens)
	{
		if (GetDrawAreaResource(theme, t, result))
			return true;
	}
	result = FudgetDrawArea();
	return false;
}

bool FudgetStyle::GetAreaListResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetStyleAreaList* &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (AreaListFromVariant(var, result))
			return true;
	}
	result = nullptr; // FudgetStyleAreaList();
	return false;
}

bool FudgetStyle::GetAreaListResource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) FudgetStyleAreaList* &result)
{
	for (auto t : tokens)
	{
		if (GetAreaListResource(theme, t, result))
			return true;
	}
	result = nullptr; // FudgetStyleAreaList();
	return false;
}

bool FudgetStyle::GetTextureResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) TextureBase* &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (TextureFromVariant(var, result))
			return true;
	}
	result = nullptr;
	return false;
}

bool FudgetStyle::GetTextureResource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) TextureBase* &result)
{
	for (auto t : tokens)
	{
		if (GetTextureResource(theme, t, result))
			return true;
	}
	result = nullptr;
	return false;
}

bool FudgetStyle::GetTextDrawSettingsResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetTextDrawSettings &result)
{
	Variant var;
	if (GetResourceValue(theme, token, var))
	{
		if (TextDrawSettingsFromVariant(var, result))
			return true;
	}
	result = FudgetTextDrawSettings();
	return false;
}

bool FudgetStyle::GetTextDrawSettingsResource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) FudgetTextDrawSettings &result)
{
	for (auto t : tokens)
	{
		if (GetTextDrawSettingsResource(theme, t, result))
			return true;
	}
	result = FudgetTextDrawSettings();
	return false;
}

void FudgetStyle::ParentResourceChanged(FudgetToken token, FudgetStyleResource *resource)
{
	auto it = _resources.find(token);
	if (it != _resources.end())
	{
		FudgetStyleResource &res = it->second;
		res._inherited_resource = resource;
		if (res._resource_id.IsValid() || res._value_override != Variant::Null)
			return;
	}

	for (FudgetStyle *style : _derived)
		style->ParentResourceChanged(token, resource);
}

void FudgetStyle::ParentResourceWasReset(FudgetToken token, FudgetStyleResource *resource)
{
	auto it = _resources.find(token);
	if (it != _resources.end())
	{
		FudgetStyleResource &res = it->second;
		res._inherited_resource = resource->_inherited_resource;
		if (res._resource_id.IsValid() || res._value_override != Variant::Null)
			return;
	}

	for (FudgetStyle *style : _derived)
		style->ParentResourceWasReset(token, resource);
}

bool FudgetStyle::TokenFromVariant(const Variant &var, FudgetToken &result) const
{
	if (var.Type.Type == VariantType::Int)
	{
		result = FudgetToken(var.AsInt);
		return true;
	}

	if (var.Type.Type == VariantType::Uint)
	{
		result = FudgetToken(var.AsUint);
		return true;
	}

	if (var.Type.Type == VariantType::Structure)
	{
		const FudgetToken *area = var.AsStructure<FudgetToken>();
		if (area == nullptr)
			return false;
		result = *area;
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

bool FudgetStyle::AreaListFromVariant(const Variant &var, FudgetStyleAreaList* &result) const
{
	if (var.Type.Type == VariantType::Color)
	{
		result = New<FudgetStyleAreaList>(SpawnParams(Guid::New(), FudgetStyleAreaList::TypeInitializer));
		result->Types.Add(FudgetStyleAreaType::FillColor);
		result->Var.Add(Variant(var.AsColor()));
		return true;
	}

	if (var.Type.Type == VariantType::Asset)
	{
		AssetReference<Texture> asset = dynamic_cast<Texture*>(var.AsAsset);
		if (asset.Get() == nullptr)
			return false;

		result = New<FudgetStyleAreaList>(SpawnParams(Guid::New(), FudgetStyleAreaList::TypeInitializer));
		result->Types.Add(FudgetStyleAreaType::DrawArea);

		VariantType vtype;
		vtype.SetTypeName(FudgetDrawArea::TypeInitializer.GetType().Fullname);
		vtype.Type = VariantType::Types::Structure;

		result->Var.Add(Variant::Structure<FudgetDrawArea>(std::move(vtype), FudgetDrawArea(asset.Get(), true, false)));
		return true;
	}

	if (var.Type.Type == VariantType::Structure)
	{
		const FudgetDrawArea *drawarea = var.AsStructure<FudgetDrawArea>();
		if (drawarea == nullptr)
			return false;

		VariantType vtype;
		vtype.SetTypeName(FudgetDrawArea::TypeInitializer.GetType().Fullname);
		vtype.Type = VariantType::Types::Structure;

		result = New<FudgetStyleAreaList>(SpawnParams(Guid::New(), FudgetStyleAreaList::TypeInitializer));
		result->Types.Add(FudgetStyleAreaType::DrawArea);
		result->Var.Add(Variant::Structure<FudgetDrawArea>(std::move(vtype), *drawarea));

		return true;
	}

	if (var.Type.Type == VariantType::Object)
	{
		FudgetStyleAreaList *area = dynamic_cast<FudgetStyleAreaList*>(var.AsObject);
		if (area == nullptr)
			return false;
		result = area;
		return true;
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

		const FudgetToken *material_token = var.AsStructure<FudgetToken>();
		if (material_token != nullptr)
		{
			result = FudgetTextDrawSettings();
			result.MaterialToken = *material_token;
			return true;
		}

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

	FontAsset *asset = FudgetThemes::GetFontAsset(result.Settings.FontToken);
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

FudgetPartPainter* FudgetStyle::CreatePainterInternal(FudgetToken token) const
{
	return FudgetThemes::CreatePainter(token);
}
