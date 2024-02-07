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

const FudgetToken FudgetThemes::BackgroundColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BackgroundColor"));
const FudgetToken FudgetThemes::FieldColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldColor"));
const FudgetToken FudgetThemes::HoveredFieldColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredFieldColor"));
const FudgetToken FudgetThemes::FocusedFieldColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedFieldColor"));
const FudgetToken FudgetThemes::DisabledFieldColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledFieldColor"));
const FudgetToken FudgetThemes::BorderColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderColor"));
const FudgetToken FudgetThemes::HoveredBorderColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredBorderColor"));
const FudgetToken FudgetThemes::FocusedBorderColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedBorderColor"));
const FudgetToken FudgetThemes::DisabledBorderColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledBorderColor"));
const FudgetToken FudgetThemes::FieldBorderColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldBorderColor"));
const FudgetToken FudgetThemes::HoveredFieldBorderColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredFieldBorderColor"));
const FudgetToken FudgetThemes::FocusedFieldBorderColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedFieldBorderColor"));
const FudgetToken FudgetThemes::DisabledFieldBorderColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledFieldBorderColor"));
const FudgetToken FudgetThemes::DarkColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DarkColor"));
const FudgetToken FudgetThemes::LightColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_LightColor"));
const FudgetToken FudgetThemes::MediumColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_MediumColor"));
const FudgetToken FudgetThemes::AccentColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_AccentColor"));
const FudgetToken FudgetThemes::SelectionColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SelectionColor"));
const FudgetToken FudgetThemes::HoveredSelectionColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredSelectionColor"));
const FudgetToken FudgetThemes::FocusedSelectionColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedSelectionColor"));
const FudgetToken FudgetThemes::DisabledSelectionColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledSelectionColor"));

const FudgetToken FudgetThemes::TextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_TextColor"));
const FudgetToken FudgetThemes::HoveredTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredTextColor"));
const FudgetToken FudgetThemes::FocusedTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedTextColor"));
const FudgetToken FudgetThemes::DisabledTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledTextColor"));
const FudgetToken FudgetThemes::FieldTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldTextColor"));
const FudgetToken FudgetThemes::HoveredFieldTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredFieldTextColor"));
const FudgetToken FudgetThemes::FocusedFieldTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedFieldTextColor"));
const FudgetToken FudgetThemes::DisabledFieldTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledFieldTextColor"));
const FudgetToken FudgetThemes::SelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SelectedTextColor"));
const FudgetToken FudgetThemes::HoveredSelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredSelectedTextColor"));
const FudgetToken FudgetThemes::FocusedSelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedSelectedTextColor"));
const FudgetToken FudgetThemes::DisabledSelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledSelectedTextColor"));

const FudgetToken FudgetThemes::ControlColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_ControlColor"));
const FudgetToken FudgetThemes::PressedControlColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_PressedControlColor"));
const FudgetToken FudgetThemes::DownControlColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DownControlColor"));
const FudgetToken FudgetThemes::HoveredControlColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredControlColor"));
const FudgetToken FudgetThemes::FocusedControlColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedControlColor"));
const FudgetToken FudgetThemes::SelectedControlColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SelectedControlColor"));
const FudgetToken FudgetThemes::HoveredSelectedControlColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredSelectedControlColor"));
const FudgetToken FudgetThemes::FocusedSelectedControlColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedSelectedControlColor"));
const FudgetToken FudgetThemes::DisabledControlColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledControlColor"));
const FudgetToken FudgetThemes::DisabledDownControlColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledDownControlColor"));
const FudgetToken FudgetThemes::DisabledSelectedControlColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledSelectedControlColor"));

const FudgetToken FudgetThemes::ButtonColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_ButtonColor"));
const FudgetToken FudgetThemes::PressedButtonColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_PressedButtonColor"));
const FudgetToken FudgetThemes::DownButtonColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DownButtonColor"));
const FudgetToken FudgetThemes::HoveredButtonColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredButtonColor"));
const FudgetToken FudgetThemes::FocusedButtonColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedButtonColor"));
const FudgetToken FudgetThemes::FocusedSelectedButtonColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedSelectedButtonColor"));
const FudgetToken FudgetThemes::SelectedButtonColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SelectedButtonColor"));
const FudgetToken FudgetThemes::DisabledButtonColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledButtonColor"));
const FudgetToken FudgetThemes::DisabledSelectedButtonColorToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledSelectedButtonColor"));


const FudgetToken FudgetThemes::BackgroundImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BackgroundImage"));
const FudgetToken FudgetThemes::FieldImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldImage"));
const FudgetToken FudgetThemes::HoveredFieldImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredFieldImage"));
const FudgetToken FudgetThemes::FocusedFieldImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedFieldImage"));
const FudgetToken FudgetThemes::DisabledFieldImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledFieldImage"));
const FudgetToken FudgetThemes::BorderImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderImage"));
const FudgetToken FudgetThemes::HoveredBorderImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredBorderImage"));
const FudgetToken FudgetThemes::FocusedBorderImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedBorderImage"));
const FudgetToken FudgetThemes::DisabledBorderImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledBorderImage"));
const FudgetToken FudgetThemes::FieldBorderImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldBorderImage"));
const FudgetToken FudgetThemes::HoveredFieldBorderImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredFieldBorderImage"));
const FudgetToken FudgetThemes::FocusedFieldBorderImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedFieldBorderImage"));
const FudgetToken FudgetThemes::SelectionImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SelectionImage"));
const FudgetToken FudgetThemes::HoveredSelectionImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredSelectionImage"));
const FudgetToken FudgetThemes::FocusedSelectionImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedSelectionImage"));
const FudgetToken FudgetThemes::DisabledFieldBorderImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledFieldBorderImage"));
const FudgetToken FudgetThemes::DisabledSelectionImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledSelectionImage"));

const FudgetToken FudgetThemes::ControlImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_ControlImage"));
const FudgetToken FudgetThemes::PressedControlImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_PressedControlImage"));
const FudgetToken FudgetThemes::DownControlImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DownControlImage"));
const FudgetToken FudgetThemes::HoveredControlImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredControlImage"));
const FudgetToken FudgetThemes::FocusedControlImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedControlImage"));
const FudgetToken FudgetThemes::SelectedControlImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SelectedControlImage"));
const FudgetToken FudgetThemes::HoveredSelectedControlImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredSelectedControlImage"));
const FudgetToken FudgetThemes::FocusedSelectedControlImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedSelectedControlImage"));
const FudgetToken FudgetThemes::DisabledControlImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledControlImage"));
const FudgetToken FudgetThemes::DisabledDownControlImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledDownControlImage"));
const FudgetToken FudgetThemes::DisabledSelectedControlImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledSelectedControlImage"));

const FudgetToken FudgetThemes::ButtonImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_ButtonImage"));
const FudgetToken FudgetThemes::PressedButtonImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_PressedButtonImage"));
const FudgetToken FudgetThemes::DownButtonImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DownButtonImage"));
const FudgetToken FudgetThemes::HoveredButtonImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredButtonImage"));
const FudgetToken FudgetThemes::FocusedButtonImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedButtonImage"));
const FudgetToken FudgetThemes::FocusedSelectedButtonImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedSelectedButtonImage"));
const FudgetToken FudgetThemes::SelectedButtonImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SelectedButtonImage"));
const FudgetToken FudgetThemes::DisabledButtonImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledButtonImage"));
const FudgetToken FudgetThemes::DisabledSelectedButtonImageToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledSelectedButtonImage"));

const FudgetToken FudgetThemes::BackgroundDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BackgroundDraw"));
const FudgetToken FudgetThemes::FieldDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldDraw"));
const FudgetToken FudgetThemes::HoveredFieldDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredFieldDraw"));
const FudgetToken FudgetThemes::FocusedFieldDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedFieldDraw"));
const FudgetToken FudgetThemes::DisabledFieldDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledFieldDraw"));
const FudgetToken FudgetThemes::BorderDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderDraw"));
const FudgetToken FudgetThemes::HoveredBorderDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredBorderDraw"));
const FudgetToken FudgetThemes::FocusedBorderDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedBorderDraw"));
const FudgetToken FudgetThemes::DisabledBorderDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledBorderDraw"));
const FudgetToken FudgetThemes::FieldBorderDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldBorderDraw"));
const FudgetToken FudgetThemes::HoveredFieldBorderDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredFieldBorderDraw"));
const FudgetToken FudgetThemes::FocusedFieldBorderDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedFieldBorderDraw"));
const FudgetToken FudgetThemes::DisabledFieldBorderDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledFieldBorderDraw"));
const FudgetToken FudgetThemes::SelectionDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SelectionDraw"));
const FudgetToken FudgetThemes::HoveredSelectionDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredSelectionDraw"));
const FudgetToken FudgetThemes::FocusedSelectionDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedSelectionDraw"));
const FudgetToken FudgetThemes::DisabledSelectionDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledSelectionDraw"));

const FudgetToken FudgetThemes::ControlDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_ControlDraw"));
const FudgetToken FudgetThemes::PressedControlDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_PressedControlDraw"));
const FudgetToken FudgetThemes::DownControlDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DownControlDraw"));
const FudgetToken FudgetThemes::HoveredControlDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredControlDraw"));
const FudgetToken FudgetThemes::FocusedControlDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedControlDraw"));
const FudgetToken FudgetThemes::SelectedControlDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SelectedControlDraw"));
const FudgetToken FudgetThemes::HoveredSelectedControlDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredSelectedControlDraw"));
const FudgetToken FudgetThemes::FocusedSelectedControlDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedSelectedControlDraw"));
const FudgetToken FudgetThemes::DisabledControlDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledControlDraw"));
const FudgetToken FudgetThemes::DisabledDownControlDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledDownControlDraw"));
const FudgetToken FudgetThemes::DisabledSelectedControlDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledSelectedControlDraw"));

const FudgetToken FudgetThemes::ButtonDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_ButtonDraw"));
const FudgetToken FudgetThemes::PressedButtonDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_PressedButtonDraw"));
const FudgetToken FudgetThemes::DownButtonDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DownButtonDraw"));
const FudgetToken FudgetThemes::HoveredButtonDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HoveredButtonDraw"));
const FudgetToken FudgetThemes::FocusedButtonDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedButtonDraw"));
const FudgetToken FudgetThemes::FocusedSelectedButtonDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FocusedSelectedButtonDraw"));
const FudgetToken FudgetThemes::SelectedButtonDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SelectedButtonDraw"));
const FudgetToken FudgetThemes::DisabledButtonDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledButtonDraw"));
const FudgetToken FudgetThemes::DisabledSelectedButtonDrawToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_DisabledSelectedButtonDraw"));

const FudgetToken FudgetThemes::ControlFontToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_ControlFont"));
const FudgetToken FudgetThemes::SmallControlFontToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_SmallControlFont"));
const FudgetToken FudgetThemes::LargeControlFontToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_LargeControlFont"));
const FudgetToken FudgetThemes::HeaderFontToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_HeaderFont"));
const FudgetToken FudgetThemes::FieldFontToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_FieldFont"));

const FudgetToken FudgetThemes::ControlPaddingToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_ControlPadding"));
const FudgetToken FudgetThemes::BackgroundPaddingToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BackgroundPadding"));
const FudgetToken FudgetThemes::BorderPaddingToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderPadding"));
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

const FudgetToken FudgetThemes::BorderWidthToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderWidth"));
const FudgetToken FudgetThemes::BorderWidthLeftToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderWidthLeft"));
const FudgetToken FudgetThemes::BorderWidthRightToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderWidthRight"));
const FudgetToken FudgetThemes::BorderWidthTopToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderWidthTop"));
const FudgetToken FudgetThemes::BorderWidthBottomToken = FudgetThemes::RegisterToken(TEXT("FudgetThemes_BorderWidthBottom"));


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

