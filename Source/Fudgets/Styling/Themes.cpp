#include "Themes.h"
#include "Token.h"
#include "../MarginStructs.h"
// temp
#include "ElementPainters/SimpleButtonPainter.h"

#include "Engine/Core/Math/Color.h"
#include "Engine/Content/Content.h"
#include "Engine/Content/Assets/Texture.h"

FudgetTheme::FudgetTheme() : Base(SpawnParams(Guid::New(), TypeInitializer))
{

}

FudgetTheme::FudgetTheme(const FudgetTheme &ori) : FudgetTheme()
{
	Resources = ori.Resources;
	PainterIds = ori.PainterIds;
}

FudgetTheme* FudgetTheme::Duplicate() const
{
	FudgetTheme *result = New<FudgetTheme>(*this);
	return result;
}

std::map<String, FudgetToken> FudgetThemes::_token_map;
std::map<FudgetToken, String> FudgetThemes::_string_map;
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

const FudgetToken FudgetThemes::ButtonBackgroundPaddingToken = FudgetThemes::RegisterToken(TEXT("ButtonBackgroundPadding"));
const FudgetToken FudgetThemes::ButtonBackgroundNormalToken = FudgetThemes::RegisterToken(TEXT("ButtonBackgroundNormal"));
const FudgetToken FudgetThemes::ButtonBackgroundPressedToken = FudgetThemes::RegisterToken(TEXT("ButtonBackgroundPressed"));
const FudgetToken FudgetThemes::ButtonBackgroundHoverToken = FudgetThemes::RegisterToken(TEXT("ButtonBackgroundHover"));
const FudgetToken FudgetThemes::ButtonBorderNormalImageToken = FudgetThemes::RegisterToken(TEXT("ButtonBorderNormalImage"));
const FudgetToken FudgetThemes::ButtonBorderPressedImageToken = FudgetThemes::RegisterToken(TEXT("ButtonBorderPressedImage"));
const FudgetToken FudgetThemes::ButtonBorderHoverImageToken = FudgetThemes::RegisterToken(TEXT("ButtonBorderHoverImage"));
const FudgetToken FudgetThemes::ButtonHoverAnimationTimeToken = FudgetThemes::RegisterToken(TEXT("ButtonHoverAnimationTime"));
const FudgetToken FudgetThemes::ButtonFocusRectangleColorToken = FudgetThemes::RegisterToken(TEXT("ButtonFocusRectangleColor"));
const FudgetToken FudgetThemes::ButtonFocusRectangleWidthToken = FudgetThemes::RegisterToken(TEXT("ButtonFocusRectangleWidth"));

const FudgetToken FudgetThemes::SimpleButtonPainterToken = RegisterToken(TEXT("SimpleButton"));
const FudgetToken FudgetThemes::ButtonBackgroundPainterToken = RegisterToken(TEXT("ButtonBackground"));
const FudgetToken FudgetThemes::ButtonBorderPainterToken = RegisterToken(TEXT("ButtonBorder"));


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

	FudgetTheme *main_theme = New<FudgetTheme>();
	_theme_map[MainThemeToken] = main_theme;
	main_theme->Resources.Add(ColorDarkToken, Color(0.6f, 0.6f, 0.6f, 1.0f));
	main_theme->Resources.Add(ColorLightToken, Color(1.0f));
	main_theme->Resources.Add(ColorNormalToken, Color(0.8f, 0.8f, 0.8f, 1.0f));
	main_theme->Resources.Add(HoverAnimationTimeToken, 0.3f);
	main_theme->Resources.Add(ColorAccentToken, Color(0.3f, 0.5f, 0.8f, 1.0f));
	main_theme->Resources.Add(FocusRectangleWidthToken, 2.5f);

	main_theme->PainterIds.Add(RegisterToken(TEXT("FudgetSimpleButton")), SimpleButtonPainterToken);
	main_theme->PainterIds.Add(ButtonBackgroundPainterToken, ButtonBackgroundPainterToken);
	main_theme->PainterIds.Add(ButtonBorderPainterToken, ButtonBorderPainterToken);

	FudgetStyle *_default_style = New<FudgetStyle>(TEXT("DefaultStyle"));
	_default_style->SetResourceOverride(ButtonBackgroundNormalToken, ColorNormalToken);
	_default_style->SetResourceOverride(ButtonBackgroundPressedToken, ColorDarkToken);
	_default_style->SetResourceOverride(ButtonBackgroundHoverToken, ColorLightToken);
	_default_style->SetResourceOverride(ButtonHoverAnimationTimeToken, HoverAnimationTimeToken);
	_default_style->SetResourceOverride(ButtonFocusRectangleColorToken, ColorAccentToken);
	_default_style->SetResourceOverride(ButtonFocusRectangleWidthToken, FocusRectangleWidthToken);

	_default_style->SetPainterOverride(RegisterToken(TEXT("SimpleButtonBackground")), ButtonBackgroundPainterToken);

	FudgetStyle *_btn_style = _default_style->CreateInheritedStyle(RegisterToken(TEXT("FudgetSimpleButton")));

	FudgetStyle *_img_button_style = _btn_style->CreateInheritedStyle(RegisterToken(TEXT("ImageButtonStyle")));
	//_img_button_style->SetResourceOverride(ButtonBackgroundPressedToken, ColorLightToken);
	//_img_button_style->SetResourceOverride(ButtonBackgroundHoverToken, ColorDarkToken);

	FudgetPadding img_btn_padding(5.f);
	Variant padvar = Variant::Structure(VariantType(VariantType::Structure, TEXT("Fudgets.FudgetPadding")), img_btn_padding);
	_img_button_style->SetValueOverride(ButtonBackgroundPaddingToken, padvar);
	_img_button_style->SetPainterOverride(RegisterToken(TEXT("SimpleButtonBorderImage")), ButtonBorderPainterToken);

	AssetReference<Texture> tex = Content::Load<Texture>(TEXT("g:/projects/Flax/UIPluginTest/Content/UI/Images/bg01.flax"));
	AssetReference<Texture> tex2 = Content::Load<Texture>(TEXT("g:/projects/Flax/UIPluginTest/Content/UI/Images/border01.flax"));

	FudgetFillAreaSettings texsettings(tex, false, false, Color::White);
	Variant texvar = Variant::Structure(VariantType(VariantType::Structure, TEXT("Fudgets.FudgetFillAreaSettings")), texsettings);
	_img_button_style->SetValueOverride(ButtonBackgroundNormalToken, texvar);
	texsettings.Color = Color(1.2f, 1.2f, 1.2f, 1.0f);
	texvar = Variant::Structure(VariantType(VariantType::Structure, TEXT("Fudgets.FudgetFillAreaSettings")), texsettings);
	_img_button_style->SetValueOverride(ButtonBackgroundHoverToken, texvar);
	texsettings.Color = Color(0.8f, 0.8f, 0.8f, 1.0f);
	texvar = Variant::Structure(VariantType(VariantType::Structure, TEXT("Fudgets.FudgetFillAreaSettings")), texsettings);
	_img_button_style->SetValueOverride(ButtonBackgroundPressedToken, texvar);

	FudgetFillAreaSettings texsettings2(tex2, Float4(16.f), false, Color::White);
	texsettings2.AreaType = FudgetFillAreaType::Texture9;
	texvar = Variant::Structure(VariantType(VariantType::Structure, TEXT("Fudgets.FudgetFillAreaSettings")), texsettings2);
	_img_button_style->SetValueOverride(ButtonBorderNormalImageToken, texvar);
	texsettings2.Color = Color(1.2f, 1.2f, 1.2f, 1.0f);
	texvar = Variant::Structure(VariantType(VariantType::Structure, TEXT("Fudgets.FudgetFillAreaSettings")), texsettings2);
	_img_button_style->SetValueOverride(ButtonBorderHoverImageToken, texvar);
	texsettings2.Color = Color(0.8f, 0.8f, 0.8f, 1.0f);
	texvar = Variant::Structure(VariantType(VariantType::Structure, TEXT("Fudgets.FudgetFillAreaSettings")), texsettings2);
	_img_button_style->SetValueOverride(ButtonBorderPressedImageToken, texvar);

	New<FudgetSimpleButtonPainter>(SimpleButtonPainterToken);
	New<FudgetButtonBackgroundPainter>(ButtonBackgroundPainterToken);
	New<FudgetButtonBorderPainter>(ButtonBorderPainterToken);

	FudgetToken test_theme_token = RegisterToken(TEXT("TestTheme"));
	if (!DuplicateTheme(MainThemeToken, test_theme_token))
		return;

	FudgetTheme *test_theme = GetTheme(test_theme_token);

	test_theme->Resources[ColorDarkToken] = Color(0.7, 0.4, 0.1, 1.0f);
	test_theme->Resources[ColorLightToken] = Color(1.0f, 0.8f, 0.3f, 1.0f);
	test_theme->Resources[ColorNormalToken] = Color(0.85f, 0.5f, 0.15f, 1.0f);
	test_theme->Resources[HoverAnimationTimeToken] = 0.15f;
	test_theme->Resources[ColorAccentToken] = Color(0.3f, 0.8f, 0.5f, 1.0f);
	test_theme->Resources[FocusRectangleWidthToken] = 4.5f;
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

String FudgetThemes::GetTokenName(FudgetToken token)
{
	auto it = _string_map.find(token);
	if (it == _string_map.end())
		return TEXT("");
	return it->second;
}

FudgetToken FudgetThemes::RegisterToken(String token_name, bool duplicate_is_error)
{
	if (token_name.IsEmpty())
		return FudgetToken::Invalid;

	auto it = _token_map.find(token_name);
	if (it != _token_map.end())
	{
		if (duplicate_is_error)
			return FudgetToken::Invalid;
		return it->second;
	}
	_token_map[token_name] = ++_highest_token;
	_string_map[_highest_token] = token_name;
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

bool FudgetThemes::DuplicateTheme(FudgetToken source_token, FudgetToken dest_token)
{
	if (!source_token.IsValid() || !dest_token.IsValid())
		return false;
	auto it_src = _theme_map.find(source_token);
	auto it_dst = _theme_map.find(dest_token);
	if (it_src == _theme_map.end() || it_dst != _theme_map.end())
		return false;

	_theme_map[dest_token] = it_src->second->Duplicate();
	return true;
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

	auto itr = it->second->Resources.Find(value_token);
	if (itr == it->second->Resources.End())
		return false;
	result = (*itr).Value;
	return true;
}

bool FudgetThemes::RegisterStyle(FudgetToken token, FudgetStyle *style)
{
	if (!token.IsValid())
		return false;
	auto it = _style_map.find(token);
	if (it != _style_map.end())
		return false;
	_style_map[token] = style;
	return true;
}

void FudgetThemes::UnregisterStyle(FudgetToken token, FudgetStyle *style)
{
	if (!token.IsValid())
		return;
	_style_map.erase(token);
}

bool FudgetThemes::RegisterElementPainter(FudgetToken token, FudgetElementPainter *painter)
{
	if (!token.IsValid())
		return false;
	auto it = _element_map.find(token);
	if (it != _element_map.end())
		return false;
	_element_map[token] = painter;
	return true;
}

void FudgetThemes::UnregisterElementPainter(FudgetToken token, FudgetElementPainter *painter)
{
	if (!token.IsValid())
		return;
	_element_map.erase(token);
}