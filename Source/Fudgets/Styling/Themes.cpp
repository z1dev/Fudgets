#include "Themes.h"
#include "Token.h"
#include "../MarginStructs.h"
#include "StyleStructs.h"

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
	_resources = ori._resources;
	_painter_ids = ori._painter_ids;
}

bool FudgetTheme::GetResource(FudgetToken token, API_PARAM(Out) Variant &result) const
{
	auto it = _resources.find(token);
	if (it == _resources.end())
		return false;

	result = it->second;
	return true;
}

void FudgetTheme::SetResource(FudgetToken token, Variant value)
{
	_resources[token] = value;
}

FudgetToken FudgetTheme::GetPainterId(FudgetToken token) const
{
	auto it = _painter_ids.find(token);
	if (it == _painter_ids.end())
		return FudgetToken::Invalid;

	return it->second;
}

void FudgetTheme::SetPainterId(FudgetToken token, FudgetToken value)
{
	_painter_ids[token] = value;
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
std::map<FudgetToken, FontAsset*> FudgetThemes::_font_asset_map;
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



void FudgetThemes::Initialize()
{
	if (_themes_initialized)
		return;

	_themes_initialized = true;

	// TODO: Delete EVERYTHING here, and add the reasonable minimum style data. For example basic editor font,
	// basic colors etc.

	FudgetTheme *main_theme = New<FudgetTheme>();
	_theme_map[MainThemeToken] = main_theme;
	main_theme->SetResource(ColorDarkToken, Color(0.6f, 0.6f, 0.6f, 1.0f));
	main_theme->SetResource(ColorLightToken, Color(1.0f));
	main_theme->SetResource(ColorNormalToken, Color(0.8f, 0.8f, 0.8f, 1.0f));
	main_theme->SetResource(HoverAnimationTimeToken, 0.3f);
	main_theme->SetResource(ColorAccentToken, Color(0.3f, 0.5f, 0.8f, 1.0f));
	main_theme->SetResource(FocusRectangleWidthToken, 2.5f);

	main_theme->SetPainterId(RegisterToken(TEXT("FudgetSimpleButton")), SimpleButtonPainterToken);
	main_theme->SetPainterId(ButtonBackgroundPainterToken, ButtonBackgroundPainterToken);
	main_theme->SetPainterId(ButtonBorderPainterToken, ButtonBorderPainterToken);

	FudgetStyle *_default_style = CreateStyle(TEXT("DefaultStyle")); //New<FudgetStyle>(TEXT("DefaultStyle"));

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

	CreateElementPainter<FudgetSimpleButtonPainter>(SimpleButtonPainterToken);
	CreateElementPainter<FudgetButtonBackgroundPainter>(ButtonBackgroundPainterToken);
	CreateElementPainter<FudgetButtonBorderPainter>(ButtonBorderPainterToken);

	//New<FudgetSimpleButtonPainter>(SimpleButtonPainterToken);
	//New<FudgetButtonBackgroundPainter>(ButtonBackgroundPainterToken);
	//New<FudgetButtonBorderPainter>(ButtonBorderPainterToken);

	FudgetToken test_theme_token = RegisterToken(TEXT("TestTheme"));

	FudgetTheme *test_theme = DuplicateTheme(MainThemeToken, test_theme_token);
	if (test_theme == nullptr)
		return;

	//FudgetTheme *test_theme = GetTheme(test_theme_token);

	test_theme->SetResource(ColorDarkToken, Color(0.7, 0.4, 0.1, 1.0f));
	test_theme->SetResource(ColorLightToken, Color(1.0f, 0.8f, 0.3f, 1.0f));
	test_theme->SetResource(ColorNormalToken, Color(0.85f, 0.5f, 0.15f, 1.0f));
	test_theme->SetResource(HoverAnimationTimeToken, 0.15f);
	test_theme->SetResource(ColorAccentToken, Color(0.3f, 0.8f, 0.5f, 1.0f));
	test_theme->SetResource(FocusRectangleWidthToken, 4.5f);
}

void FudgetThemes::Uninitialize()
{
	if (!_themes_initialized)
		return;
	_themes_initialized = false;
	for (const auto &ep : _element_map)
		if (ep.second != nullptr)
			Delete(ep.second);
	_element_map.clear();
	
	for (const auto &st : _style_map)
		if (st.second != nullptr)
			Delete(st.second);
	_style_map.clear();
	
	for (auto p : _theme_map)
		if (p.second != nullptr)
			Delete(p.second);
	_theme_map.clear();

	_font_asset_map.clear();
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

bool FudgetThemes::RegisterFontAsset(FudgetToken token, FontAsset *asset)
{
	if (!token.IsValid())
		return false;
	auto it = _font_asset_map.find(token);
	if (it != _font_asset_map.end())
		return false;
	_font_asset_map[token] = asset;
}

FontAsset* FudgetThemes::GetFontAsset(FudgetToken token)
{
	if (!token.IsValid())
		return nullptr;
	auto it = _font_asset_map.find(token);
	if (it == _font_asset_map.end())
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

FudgetTheme* FudgetThemes::DuplicateTheme(FudgetToken source_token, FudgetToken dest_token)
{
	if (!source_token.IsValid() || !dest_token.IsValid())
		return nullptr;
	auto it_src = _theme_map.find(source_token);
	auto it_dst = _theme_map.find(dest_token);
	if (it_src == _theme_map.end() || it_dst != _theme_map.end())
		return nullptr;

	FudgetTheme *theme = it_src->second->Duplicate();
	_theme_map[dest_token] = theme;
	return theme;
}

FudgetStyle* FudgetThemes::CreateStyle(String name)
{
	return CreateStyle(RegisterToken(name));
}

FudgetStyle* FudgetThemes::CreateStyle(FudgetToken token)
{
	if (!token.IsValid() || GetStyle(token) != nullptr)
		return nullptr;

	FudgetStyle *style = New<FudgetStyle>();
	_style_map[token] = style;
	return style;

}

FudgetStyle* FudgetThemes::GetStyle(String name)
{
	return GetStyle(RegisterToken(name));
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

void FudgetThemes::RegisterElementPainter(FudgetToken token, FudgetElementPainter *painter)
{
	_element_map[token] = const_cast<FudgetElementPainter*>(painter);
}

