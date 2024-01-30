#include "Style.h"
#include "Themes.h"

#include "Engine/Core/Types/BaseTypes.h"
#include "Engine/Core/Log.h"

FudgetStyle::FudgetStyle(String name) : FudgetStyle(FudgetThemes::RegisterToken(name))
{

}

FudgetStyle::FudgetStyle(FudgetToken name_token) : Base(SpawnParams(Guid::New(), TypeInitializer)),
	_style_name(name_token), _parent(nullptr)
{
	if (!FudgetThemes::RegisterStyle(_style_name, this))
		LOG(Error, "Invalid token for FudgetStyle. Style wasn't registered.");
}

FudgetStyle::~FudgetStyle()
{
	_derived.ClearDelete();
	FudgetThemes::UnregisterStyle(_style_name, this);
}

FudgetStyle* FudgetStyle::CreateInheritedStyle(FudgetToken name_token)
{
	if (FudgetThemes::GetStyle(name_token) != nullptr)
		return nullptr;

	FudgetStyle *result = New<FudgetStyle>(name_token);
	result->_parent = this;
	_derived.Add(result);
	return result;
}

FudgetStyleResource* FudgetStyle::GetResource(FudgetToken token)
{
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
	FudgetStyleResource *res = GetResource(token);
	if (res == nullptr)
		return false;

	if (res->_value_override != Variant::Null)
	{
		result = res->_value_override;
		return true;
	}

	if (res->_resource_id.IsValid())
	{
		auto it = theme->Resources.Find(res->_resource_id);
		if (it == theme->Resources.End())
			return false;
		result = (*it).Value;
		return true;
	}
	return false;
}


void FudgetStyle::SetValueOverride(FudgetToken token, const Variant &value)
{
	if (value == Variant::Null)
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
	if (!resource_token.IsValid())
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


FudgetStylePainterResource* FudgetStyle::GetPainterResource(FudgetToken token)
{
	auto it = _painters.find(token);
	if (it != _painters.end())
	{
		FudgetStylePainterResource *res = &it->second;
		bool first = true;
		while (res != nullptr)
		{
			if ( res->_value_override.IsValid())
			{
				if (!first)
				{
					// "Inherit" the resource here.
					FudgetStylePainterResource tmp;
					tmp._inherited_resource = res;
					_painters[token] = tmp;
				}
				return res;
			}

			res = res->_inherited_resource;
			first = false;
		}
	}

	if (_parent != nullptr)
	{
		FudgetStylePainterResource tmp;
		FudgetStylePainterResource *res = _parent->GetPainterResource(token);
		if (res != nullptr)
		{
			// "Inherit" the resource here.
			tmp._inherited_resource = res;
		}
		_painters[token] = tmp;
		return res;
	}

	return nullptr;
}

FudgetElementPainter* FudgetStyle::GetElementPainter(FudgetTheme *theme, FudgetToken token)
{
	FudgetStylePainterResource *res = GetPainterResource(token);
	if (res != nullptr && res->_value_override.IsValid())
	{
		auto painter = FudgetThemes::GetElementPainter(res->_value_override);
		if (painter != nullptr)
			return painter;
	}
	return nullptr;
}

FudgetElementPainter* FudgetStyle::GetControlPainter(FudgetTheme *theme, Array<FudgetToken> class_tokens)
{
	for (FudgetToken token : class_tokens)
	{
		FudgetStylePainterResource *res = GetPainterResource(token);
		if (res != nullptr && res->_value_override.IsValid())
		{
			auto painter = FudgetThemes::GetElementPainter(res->_value_override);
			if (painter != nullptr)
				return painter;
		}
		auto it = theme->PainterIds.Find(token);
		if (it != theme->PainterIds.End())
			return FudgetThemes::GetElementPainter((*it).Value);
	}
	return nullptr;
}

void FudgetStyle::SetPainterOverride(FudgetToken token, FudgetToken painter_token)
{
	if (!token.IsValid() || !painter_token.IsValid())
		return;

	auto it = _painters.find(token);

	if (it == _painters.end())
	{
		if (_parent != nullptr)
		{
			// GetPainterResource will create an override resource with no values set.
			GetPainterResource(token);
			it = _painters.find(token);
		}
		else
		{
			// "Inherit" the resource here.
			FudgetStylePainterResource tmp;
			_painters[token] = tmp;
			it = _painters.find(token);
		}
	}

	FudgetStylePainterResource &res = it->second;
	if (res._value_override == painter_token)
		return;

	res._value_override = painter_token;

	for (FudgetStyle *style : _derived)
		style->ParentPainterChanged(token, &res);
}

void FudgetStyle::ResetPainterOverride(FudgetToken token)
{
	auto it = _painters.find(token);
	if (it == _painters.end())
		return;
	FudgetStylePainterResource &res = it->second;
	if (!res._value_override.IsValid())
		return;

	res._value_override = FudgetToken::Invalid;

	for (FudgetStyle *style : _derived)
		style->ParentPainterWasReset(token, &res);
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

void FudgetStyle::ParentPainterChanged(FudgetToken token, FudgetStylePainterResource *resource)
{
	auto it = _painters.find(token);
	if (it != _painters.end())
	{
		FudgetStylePainterResource &res = it->second;
		res._inherited_resource = resource;
		if (res._value_override.IsValid())
			return;
	}

	for (FudgetStyle *style : _derived)
		style->ParentPainterChanged(token, resource);
}

void FudgetStyle::ParentPainterWasReset(FudgetToken token, FudgetStylePainterResource *resource)
{
	auto it = _painters.find(token);
	if (it != _painters.end())
	{
		FudgetStylePainterResource &res = it->second;
		res._inherited_resource = resource->_inherited_resource;
		if (res._value_override.IsValid())
			return;
	}

	for (FudgetStyle *style : _derived)
		style->ParentPainterWasReset(token, resource);
}
