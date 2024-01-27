#include "Themes.h"
#include "Token.h"
// temp
#include "ElementPainters/SimpleButtonPainter.h"

#include "Engine/Core/Math/Color.h"


FudgetTheme::FudgetTheme() : Base(SpawnParams(Guid::New(), TypeInitializer))
{

}

std::map<String, FudgetToken> FudgetThemes::_token_map;
int FudgetThemes::_highest_token = 0;

std::map<FudgetToken, FudgetElementPainter*> FudgetThemes::_element_map;
std::map<FudgetToken, FudgetStyle*> FudgetThemes::_style_map;
std::map<FudgetToken, FudgetTheme*> FudgetThemes::_theme_map;
bool FudgetThemes::_themes_initialized = false;

const FudgetToken FudgetThemes::MainThemeToken = FudgetThemes::RegisterToken(TEXT("MainTheme"));
const FudgetToken FudgetThemes::DefaultStyleToken = FudgetThemes::RegisterToken(TEXT("DefaultStyle"));

const FudgetToken FudgetThemes::ButtonDownToken = FudgetThemes::RegisterToken(TEXT("ButtonDownToken"));
const FudgetToken FudgetThemes::LeftButtonPressedToken = FudgetThemes::RegisterToken(TEXT("LeftButtonPressedToken"));
const FudgetToken FudgetThemes::MouseHoverToken = FudgetThemes::RegisterToken(TEXT("MouseHoverToken"));

const FudgetToken FudgetThemes::ColorDarkToken = FudgetThemes::RegisterToken(TEXT("ColorDark"));
const FudgetToken FudgetThemes::ColorLightToken = FudgetThemes::RegisterToken(TEXT("ColorLight"));
const FudgetToken FudgetThemes::ColorNormalToken = FudgetThemes::RegisterToken(TEXT("ColorNormal"));
const FudgetToken FudgetThemes::HoverAnimationTimeToken = FudgetThemes::RegisterToken(TEXT("HoverAnimationTime"));

const FudgetToken FudgetThemes::ColorAccentToken = FudgetThemes::RegisterToken(TEXT("ColorAccent"));
const FudgetToken FudgetThemes::FocusRectangleWidthToken = FudgetThemes::RegisterToken(TEXT("FocusRectangleWidth"));

const FudgetToken FudgetThemes::ButtonBackgroundNormalToken = FudgetThemes::RegisterToken(TEXT("ButtonBackgroundNormal"));
const FudgetToken FudgetThemes::ButtonBackgroundPressedToken = FudgetThemes::RegisterToken(TEXT("ButtonBackgroundPressed"));
const FudgetToken FudgetThemes::ButtonBackgroundHoverToken = FudgetThemes::RegisterToken(TEXT("ButtonBackgroundHover"));
const FudgetToken FudgetThemes::ButtonHoverAnimationTimeToken = FudgetThemes::RegisterToken(TEXT("ButtonHoverAnimationTime"));
const FudgetToken FudgetThemes::ButtonFocusRectangleColorToken = FudgetThemes::RegisterToken(TEXT("ButtonFocusRectangleColor"));
const FudgetToken FudgetThemes::ButtonFocusRectangleWidthToken = FudgetThemes::RegisterToken(TEXT("ButtonFocusRectangleWidth"));


//FudgetThemes::FudgetThemes() : Base(SpawnParams(Guid::New(), TypeInitializer))
//{
//	Init();
//}

//FudgetThemes::~FudgetThemes()
//{
//	for (const auto &ep : _element_map)
//		Delete(ep.second);
//	_element_map.clear();
//
//	_style_map.clear();
//
//	for (auto p : _theme_map)
//		Delete(p.second);
//	_theme_map.clear();
//}

void FudgetThemes::Initialize()
{
	if (_themes_initialized)
		return;

	_themes_initialized = true;

	_theme_map[MainThemeToken] = New<FudgetTheme>();
	FudgetTheme *main_theme = _theme_map[MainThemeToken];
	main_theme->_resources.Add(ColorDarkToken, Color(0.6f, 0.6f, 0.6f, 1.0f));
	main_theme->_resources.Add(ColorLightToken, Color(1.0f));
	main_theme->_resources.Add(ColorNormalToken, Color(0.8f, 0.8f, 0.8f, 1.0f));
	main_theme->_resources.Add(HoverAnimationTimeToken, 0.3f);
	main_theme->_resources.Add(ColorAccentToken, Color(0.3f, 0.5f, 0.8f, 1.0f));
	main_theme->_resources.Add(FocusRectangleWidthToken, 4.5f);

	main_theme->_painter_ids.Add(RegisterToken(TEXT("FudgetSimpleButton")), RegisterToken(TEXT("SimpleButton")));

	FudgetStyle *_default_style = New<FudgetStyle>(TEXT("DefaultStyle"));
	_default_style->SetResourceOverride(ButtonBackgroundNormalToken, ColorNormalToken);
	_default_style->SetResourceOverride(ButtonBackgroundPressedToken, ColorDarkToken);
	_default_style->SetResourceOverride(ButtonBackgroundHoverToken, ColorLightToken);
	_default_style->SetResourceOverride(ButtonHoverAnimationTimeToken, HoverAnimationTimeToken);
	_default_style->SetResourceOverride(ButtonFocusRectangleColorToken, ColorAccentToken);
	_default_style->SetResourceOverride(ButtonFocusRectangleWidthToken, FocusRectangleWidthToken);

	FudgetSimpleButtonPainter *sbdrawer = New<FudgetSimpleButtonPainter>();
	_element_map[RegisterToken(TEXT("SimpleButton"))] = sbdrawer;

}

void FudgetThemes::Uninitialize()
{
	if (!_themes_initialized)
		return;
	_themes_initialized = false;
	for (const auto &ep : _element_map)
		Delete(ep.second);
	_element_map.clear();
	
	_style_map.clear();
	
	for (auto p : _theme_map)
		Delete(p.second);
	_theme_map.clear();
}

FudgetToken FudgetThemes::GetToken(String token_name)
{
	auto it = _token_map.find(token_name);
	if (it == _token_map.end())
		return FudgetToken::Invalid;

	return it->second;
}

FudgetToken FudgetThemes::RegisterToken(String token_name, bool duplicate_is_error)
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

FudgetElementPainter* FudgetThemes::GetElementPainter(String token_name)
{
	FudgetToken token = GetToken(token_name);
	return GetElementPainter(token);
}

API_FUNCTION() FudgetElementPainter* FudgetThemes::GetElementPainter(FudgetToken token)
{
	if (!token.IsValid())
		return nullptr;
	auto it = _element_map.find(token);
	if (it == _element_map.end())
		return nullptr;
	return it->second;
}

FudgetTheme* FudgetThemes::GetTheme(FudgetToken token)
{
	if (!token.IsValid())
		return nullptr;
	auto it = _theme_map.find(token);
	if (it == _theme_map.end())
		return nullptr;
	return it->second;
}

FudgetStyle* FudgetThemes::GetStyle(FudgetToken token)
{
	if (!token.IsValid())
		return nullptr;
	auto it = _style_map.find(token);
	if (it == _style_map.end())
		return nullptr;
	return it->second;
}

FudgetStyle* FudgetThemes::GetControlStyleOrDefault(const Array<FudgetToken> &class_tokens)
{
	for (int ix = 0, siz = class_tokens.Count(); ix < siz; ++ix)
	{
		FudgetStyle *style = GetStyle(class_tokens[ix]);
		if (style != nullptr)
			return style;
	}
	return GetStyle(DefaultStyleToken);
}

bool FudgetThemes::GetValueFromTheme(FudgetToken theme_token, FudgetToken value_token, API_PARAM(Out) Variant &result)
{
	if (!theme_token.IsValid() || !value_token.IsValid())
		return false;

	auto it = _theme_map.find(theme_token);
	if (it == _theme_map.end())
		return false;

	auto itr = it->second->_resources.Find(value_token);
	if (itr == it->second->_resources.End())
		return false;
	result = (*itr).Value;
	return true;
}

void FudgetThemes::RegisterStyle(FudgetToken token, FudgetStyle *style)
{
	if (!token.IsValid())
		return;
	auto it = _style_map.find(token);
	if (it != _style_map.end())
		return;
	_style_map[token] = style;
}

void FudgetThemes::UnregisterStyle(FudgetToken token, FudgetStyle *style)
{
	if (!token.IsValid())
		return;
	_style_map.erase(token);
}

