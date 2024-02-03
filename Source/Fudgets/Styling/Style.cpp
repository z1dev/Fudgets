#include "Style.h"
#include "Themes.h"

#include "Engine/Core/Types/BaseTypes.h"
#include "Engine/Core/Log.h"

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

