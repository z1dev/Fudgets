#include "Style.h"
#include "Theme.h"

#include "Engine/Core/Types/BaseTypes.h"
#include "Engine/Core/Math/Color.h"

FudgetStyle::FudgetStyle(FudgetTheme*theme) : Base(SpawnParams(Guid::New(), TypeInitializer)),
	_theme(theme), _parent(nullptr)
{

}

FudgetStyle::~FudgetStyle()
{

}

FudgetStyle* FudgetStyle::CreateInheritedStyle()
{
	FudgetStyle *result = New<FudgetStyle>(_theme);
	result->_parent = this;
	_derived.Add(result);
	return result;
}

Color FudgetStyle::GetColor(FudgetToken token) const
{
	auto it = _color_map_overrides.find(token);
	if (it != _color_map_overrides.end())
		return it->second;

	auto it2 = _color_map.find(token);
	if (it2 != _color_map.end())
		return it2->second;

	if (_parent != nullptr)
		return _parent->GetColor(token);

	return Color::White;
}

void FudgetStyle::SetColorOverride(FudgetToken token, Color value)
{
	auto it = _color_map_overrides.find(token);
	if (it == _color_map_overrides.end())
		_color_map_overrides[token] = value;
	else
		it->second = value;

	NotifyDerivedColorChanged(token);
}

void FudgetStyle::ResetColor(FudgetToken token)
{
	auto it = _color_map_overrides.find(token);
	if (it == _color_map_overrides.end())
		return;
	_color_map_overrides.erase(it);

	NotifyDerivedColorChanged(token);
}

float FudgetStyle::GetFloat(FudgetToken token) const
{
	auto it = _float_map_overrides.find(token);
	if (it != _float_map_overrides.end())
		return it->second;

	auto it2 = _float_map.find(token);
	if (it2 != _float_map.end())
		return it2->second;

	if (_parent != nullptr)
		return _parent->GetFloat(token);

	return 0.0f;
}

void FudgetStyle::SetFloatOverride(FudgetToken token, float value)
{
	auto it = _float_map_overrides.find(token);
	if (it == _float_map_overrides.end())
		_float_map_overrides[token] = value;
	else
		it->second = value;

	NotifyDerivedFloatChanged(token);
}

void FudgetStyle::ResetFloat(FudgetToken token)
{
	auto it = _float_map_overrides.find(token);
	if (it == _float_map_overrides.end())
		return;
	_float_map_overrides.erase(it);

	NotifyDerivedFloatChanged(token);
}

void FudgetStyle::NotifyDerivedColorChanged(FudgetToken token)
{
	for (FudgetStyle *derived : _derived)
		derived->ParentColorChanged(token);
}

void FudgetStyle::NotifyDerivedFloatChanged(FudgetToken token)
{
	for (FudgetStyle *derived : _derived)
		derived->ParentFloatChanged(token);
}

void FudgetStyle::ParentColorChanged(FudgetToken token)
{
	_color_map[token] = _parent->GetColor(token);
	if (_color_map_overrides.find(token) != _color_map_overrides.end())
		return;

	for (FudgetStyle *derived : _derived)
		derived->ParentColorChanged(token);
}

void FudgetStyle::ParentFloatChanged(FudgetToken token)
{
	_float_map[token] = _parent->GetFloat(token);
	if (_float_map_overrides.find(token) != _float_map_overrides.end())
		return;

	for (FudgetStyle *derived : _derived)
		derived->ParentFloatChanged(token);
}
