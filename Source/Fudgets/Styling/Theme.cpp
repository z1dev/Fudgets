#include "Theme.h"
#include "Token.h"
// temp
#include "ElementPainters/SimpleButtonPainter.h"

#include "Engine/Core/Math/Color.h"


std::map<String, FudgetToken> FudgetTheme::_token_map;
int FudgetTheme::_highest_token = 0;

const FudgetToken FudgetTheme::ButtonDownToken = FudgetTheme::RegisterToken(TEXT("ButtonDownToken"));
const FudgetToken FudgetTheme::LeftButtonPressedToken = FudgetTheme::RegisterToken(TEXT("LeftButtonPressedToken"));
const FudgetToken FudgetTheme::MouseHoverToken = FudgetTheme::RegisterToken(TEXT("MouseHoverToken"));

const FudgetToken FudgetTheme::ColorDarkToken = FudgetTheme::RegisterToken(TEXT("ColorDark"));
const FudgetToken FudgetTheme::ColorLightToken = FudgetTheme::RegisterToken(TEXT("ColorLight"));
const FudgetToken FudgetTheme::ColorNormalToken = FudgetTheme::RegisterToken(TEXT("ColorNormal"));
const FudgetToken FudgetTheme::ButtonHoverAnimationTimeToken = FudgetTheme::RegisterToken(TEXT("ButtonHoverAnimationTime"));


FudgetTheme::FudgetTheme() : Base(SpawnParams(Guid::New(), TypeInitializer))
{
	Init();
}

FudgetTheme::~FudgetTheme()
{
	for (const auto &ep : _element_map)
		Delete(ep.second);
	_element_map.clear();

	_style_map.clear();
	Delete(_default_style);
}

FudgetToken FudgetTheme::GetToken(String token_name)
{
	auto it = _token_map.find(token_name);
	if (it == _token_map.end())
		return FudgetToken::Invalid;

	return it->second;
}

FudgetToken FudgetTheme::RegisterToken(String token_name, bool duplicate_is_error)
{
	auto it = _token_map.find(token_name);
	if (it != _token_map.end())
	{
		if (duplicate_is_error)
			return FudgetToken::Invalid;
		return it->second;
	}
	_token_map[token_name] = ++_highest_token;
	return _highest_token;
}

FudgetElementPainter* FudgetTheme::GetElementPainter(String token_name) const
{
	FudgetToken token = GetToken(token_name);
	return GetElementPainter(token);
}

API_FUNCTION() FudgetElementPainter* FudgetTheme::GetElementPainter(FudgetToken token) const
{
	if (token == FudgetToken::Invalid)
		return nullptr;
	auto it = _element_map.find(token);
	if (it == _element_map.end())
		return nullptr;
	return it->second;
}

FudgetStyle* FudgetTheme::GetStyle(FudgetToken token) const
{
	if (token == FudgetToken::Invalid)
		return nullptr;
	auto it = _style_map.find(token);
	if (it == _style_map.end())
		return nullptr;
	return it->second;
}

FudgetStyle* FudgetTheme::GetControlStyleOrDefault(const Array<FudgetToken> &class_tokens) const
{
	for (int ix = 0, siz = class_tokens.Count(); ix < siz; ++ix)
	{
		FudgetStyle *style = GetStyle(class_tokens[ix]);
		if (style != nullptr)
			return style;
	}
	return _default_style;
}

void FudgetTheme::Init()
{
	_default_style = New<FudgetStyle>(this);

	_default_style->SetColorOverride(ColorDarkToken, Color(0.6f, 0.6f, 0.6f, 1.0f));
	_default_style->SetColorOverride(ColorLightToken, Color(1.0f));
	_default_style->SetColorOverride(ColorNormalToken, Color(0.8f, 0.8f, 0.8f, 1.0f));
	_default_style->SetFloatOverride(ButtonHoverAnimationTimeToken, 0.3f);

	FudgetSimpleButtonPainter *sbdrawer = New<FudgetSimpleButtonPainter>();
	_element_map[RegisterToken(TEXT("SimpleButton"))] = sbdrawer;
}
