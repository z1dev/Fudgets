#include "Themes.h"
#include "Token.h"
#include "../MarginStructs.h"
#include "StyleStructs.h"

#include "Engine/Core/Math/Color.h"
#include "Engine/Content/Content.h"
#include "Engine/Content/Assets/Texture.h"
#include "Engine/Core/Log.h"

std::map<String, FudgetToken> FudgetThemes::_token_map;
std::map<FudgetToken, String> FudgetThemes::_string_map;
int FudgetThemes::_highest_token = 0;

//std::map<FudgetToken, FudgetStyle*> FudgetThemes::_style_map;
//std::map<FudgetToken, FudgetTheme*> FudgetThemes::_theme_map;
//std::map<FudgetToken, FontAsset*> FudgetThemes::_font_asset_map;

#if USE_EDITOR
bool FudgetThemes::_runtime_use = false;
FudgetThemes::Data* FudgetThemes::_edittime_data = nullptr;
FudgetThemes::Data* FudgetThemes::_runtime_data = nullptr;
bool FudgetThemes::_edittime_initialized = false;
#endif
bool FudgetThemes::_initialized = false;
FudgetThemes::Data* FudgetThemes::_data = nullptr;

const FudgetToken FudgetThemes::MainThemeToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_MainTheme"));
const FudgetToken FudgetThemes::DefaultStyleToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DefaultStyle"));

const FudgetToken FudgetThemes::BgColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BgColor"));
const FudgetToken FudgetThemes::FieldBgColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldBgColor"));
const FudgetToken FudgetThemes::DisabledFieldBgColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledFieldBgColor"));
const FudgetToken FudgetThemes::FocusedFieldBgColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedFieldBgColor"));
const FudgetToken FudgetThemes::BorderColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderColor"));
const FudgetToken FudgetThemes::FieldBorderColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldBorderColor"));
const FudgetToken FudgetThemes::DisabledFieldBorderColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledFieldBorderColor"));
const FudgetToken FudgetThemes::FocusedFieldBorderColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedFieldBorderColor"));
const FudgetToken FudgetThemes::DarkColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DarkColor"));
const FudgetToken FudgetThemes::LightColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_LightColor"));
const FudgetToken FudgetThemes::MediumColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_MediumColor"));
const FudgetToken FudgetThemes::AccentColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_AccentColor"));
const FudgetToken FudgetThemes::SelectionColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SelectionColor"));
const FudgetToken FudgetThemes::FocusedSelectionColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedSelectionColor"));
const FudgetToken FudgetThemes::DisabledSelectionColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledSelectionColor"));

const FudgetToken FudgetThemes::TextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_TextColor"));
const FudgetToken FudgetThemes::DisabledTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledTextColor"));
const FudgetToken FudgetThemes::FocusedTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedTextColor"));
const FudgetToken FudgetThemes::FieldTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldTextColor"));
const FudgetToken FudgetThemes::DisabledFieldTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledFieldTextColor"));
const FudgetToken FudgetThemes::FocusedFieldTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedFieldTextColor"));
const FudgetToken FudgetThemes::SelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SelectedTextColor"));
const FudgetToken FudgetThemes::FocusedSelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedSelectedTextColor"));
const FudgetToken FudgetThemes::DisabledSelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledSelectedTextColor"));


const FudgetToken FudgetThemes::BgImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BgImage"));
const FudgetToken FudgetThemes::FieldBgImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldBgImage"));
const FudgetToken FudgetThemes::DisabledFieldBgImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledFieldBgImage"));
const FudgetToken FudgetThemes::FocusedFieldBgImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedFieldBgImage"));
const FudgetToken FudgetThemes::BorderImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderImage"));
const FudgetToken FudgetThemes::FieldBorderImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldBorderImage"));
const FudgetToken FudgetThemes::DisabledFieldBorderImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledFieldBorderImage"));
const FudgetToken FudgetThemes::FocusedFieldBorderImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedFieldBorderImage"));
const FudgetToken FudgetThemes::SelectionImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SelectionImage"));
const FudgetToken FudgetThemes::FocusedSelectionImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedSelectionImage"));
const FudgetToken FudgetThemes::DisabledSelectionImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledSelectionImage"));

const FudgetToken FudgetThemes::BgDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BgDraw"));
const FudgetToken FudgetThemes::FieldBgDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldBgDraw"));
const FudgetToken FudgetThemes::DisabledFieldBgDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledFieldBgDraw"));
const FudgetToken FudgetThemes::FocusedFieldBgDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedFieldBgDraw"));
const FudgetToken FudgetThemes::BorderDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderDraw"));
const FudgetToken FudgetThemes::FieldBorderDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldBorderDraw"));
const FudgetToken FudgetThemes::DisabledFieldBorderDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledFieldBorderDraw"));
const FudgetToken FudgetThemes::FocusedFieldBorderDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedFieldBorderDraw"));
const FudgetToken FudgetThemes::SelectionDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SelectionDraw"));
const FudgetToken FudgetThemes::FocusedSelectionDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedSelectionDraw"));
const FudgetToken FudgetThemes::DisabledSelectionDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledSelectionDraw"));

const FudgetToken FudgetThemes::ControlPaddingToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_ControlPadding"));
const FudgetToken FudgetThemes::FieldPaddingToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldPadding"));

const FudgetToken FudgetThemes::CaretBlinkTimeToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_CaretBlinkTime"));
const FudgetToken FudgetThemes::CaretPixelWidthToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_CaretPixelWidth"));
const FudgetToken FudgetThemes::CharacterScrollCountToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_CharacterScrollCount"));
const FudgetToken FudgetThemes::HorizontalAlignmentToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HorizontalAlignment"));
const FudgetToken FudgetThemes::VerticalAlignmentToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_VerticalAlignment"));
const FudgetToken FudgetThemes::LeftToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_Left"));
const FudgetToken FudgetThemes::RightToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_Right"));
const FudgetToken FudgetThemes::TopToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_Top"));
const FudgetToken FudgetThemes::BottomToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_Bottom"));
const FudgetToken FudgetThemes::UpToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_Up"));
const FudgetToken FudgetThemes::DownToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_Down"));
const FudgetToken FudgetThemes::PaddingWidthToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_PaddingWidth"));
const FudgetToken FudgetThemes::PaddingLeftToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_PaddingLeft"));
const FudgetToken FudgetThemes::PaddingRightToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_PaddingRight"));
const FudgetToken FudgetThemes::PaddingTopToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_PaddingTop"));
const FudgetToken FudgetThemes::PaddingBottomToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_PaddingBottom"));

const FudgetToken FudgetThemes::BorderWidthToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderWidthToken"));
const FudgetToken FudgetThemes::BorderWidthLeftToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderWidthLeftToken"));
const FudgetToken FudgetThemes::BorderWidthRightToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderWidthRightToken"));
const FudgetToken FudgetThemes::BorderWidthTopToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderWidthTopToken"));
const FudgetToken FudgetThemes::BorderWidthBottomToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderWidthBottomToken"));


// FudgetTheme


FudgetTheme::FudgetTheme() : Base(SpawnParams(Guid::New(), TypeInitializer))
{

}

FudgetTheme::FudgetTheme(const FudgetTheme &ori) : FudgetTheme()
{
	_resources = ori._resources;
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

FudgetTheme* FudgetTheme::Duplicate() const
{
	FudgetTheme *result = New<FudgetTheme>(*this);
	return result;
}


// FudgetThemes


void FudgetThemes::Initialize(bool in_game)
{
	if (in_game)
	{
		if (_initialized)
		{
			LOG(Error, "Initializing FudgetThemes twice.");
			return;
		}

		_initialized = true;
		_data = new Data;
#if USE_EDITOR
		_runtime_use = true;
		_runtime_data = _data;
#endif
	}

#if USE_EDITOR
	if (!in_game)
	{
		if (_edittime_initialized)
		{
			LOG(Error, "Initializing FudgetThemes twice in editor.");
			return;
		}
		_edittime_initialized = true;
		_runtime_use = false;
		_data = _edittime_data = new Data;
	}
#endif


	// TODO: Add the reasonable minimum style data. For example basic editor font, basic colors etc.

	FudgetTheme *main_theme = New<FudgetTheme>();
	_data->_theme_map[MainThemeToken] = main_theme;

	FudgetStyle *_default_style = CreateStyle(TEXT("DefaultStyle")); //New<FudgetStyle>(TEXT("DefaultStyle"));

	//FudgetDrawArea texsettings(tex, false, false, Color::White);
	//Variant texvar = Variant::Structure(VariantType(VariantType::Structure, TEXT("Fudgets.FudgetDrawArea")), texsettings);
	//_img_button_style->SetValueOverride(ButtonBackgroundNormalToken, texvar);
}

void FudgetThemes::Uninitialize(bool in_game)
{
#if USE_EDITOR
	if (!in_game)
	{
		if (!_edittime_initialized)
		{
			LOG(Error, "Trying to uninitialize FudgetThemes in editor when it wasn't initialized.");
			return;
		}

		_runtime_use = false;
		_data = _edittime_data;
		_edittime_initialized = false;
	}
	else
	{
		_runtime_use = true;
		_data = _runtime_data;
	}
#endif
	if (in_game)
	{
		if (!_initialized)
		{
			LOG(Error, "Trying to uninitialize FudgetThemes when it wasn't initialized.");
			return;
		}
		_initialized = false;
	}


	for (const auto &st : _data->_style_map)
		if (st.second != nullptr)
			Delete(st.second);
	_data->_style_map.clear();
	
	for (auto p : _data->_theme_map)
		if (p.second != nullptr)
			Delete(p.second);
	_data->_theme_map.clear();

	_data->_font_asset_map.clear();

#if USE_EDITOR
	if (!in_game)
	{
		delete(_edittime_data);
		_edittime_data = nullptr;
		_runtime_use = true;
		_data = _runtime_data;
	}
	else
	{
		delete(_runtime_data);
		_runtime_data = nullptr;
		_runtime_use = false;
		_data = _edittime_data;
	}
#else
	delete(_data);
	_data = nullptr;
#endif
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

bool FudgetThemes::RegisterFontAsset(FudgetToken token, FontAsset *asset)
{
	if (!token.IsValid())
		return false;
	auto it = _data->_font_asset_map.find(token);
	if (it != _data->_font_asset_map.end())
		return false;
	_data->_font_asset_map[token] = asset;
	return true;
}

FontAsset* FudgetThemes::GetFontAsset(FudgetToken token)
{
	if (!token.IsValid())
		return nullptr;
	auto it = _data->_font_asset_map.find(token);
	if (it == _data->_font_asset_map.end())
		return nullptr;
	return it->second;
}


FudgetTheme* FudgetThemes::GetTheme(FudgetToken token)
{
	if (!token.IsValid())
		return nullptr;
	auto it = _data->_theme_map.find(token);
	if (it == _data->_theme_map.end())
		return nullptr;
	return it->second;
}

FudgetTheme* FudgetThemes::DuplicateTheme(FudgetToken source_token, FudgetToken dest_token)
{
	if (!source_token.IsValid() || !dest_token.IsValid())
		return nullptr;
	auto it_src = _data->_theme_map.find(source_token);
	auto it_dst = _data->_theme_map.find(dest_token);
	if (it_src == _data->_theme_map.end() || it_dst != _data->_theme_map.end())
		return nullptr;

	FudgetTheme *theme = it_src->second->Duplicate();
	_data->_theme_map[dest_token] = theme;
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
	_data->_style_map[token] = style;
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
	auto it = _data->_style_map.find(token);
	if (it == _data->_style_map.end())
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

#if USE_EDITOR

void FudgetThemes::SetRuntimeUse(bool value)
{
	if (value)
		_data = _runtime_data;
	else
		_data = _edittime_data;
	_runtime_use = value;
}

#endif

