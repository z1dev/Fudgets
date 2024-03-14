#include "Themes.h"
#include "../MarginStructs.h"
#include "StyleStructs.h"
#include "DrawableBuilder.h"

#include "DrawableBuilder.h"
#include "StateOrderBuilder.h"

#include "PartPainterIds.h"

#include "Painters/PartPainters.h"
#include "Painters/LineEditTextPainter.h"
#include "Painters/TextBoxPainter.h"
#include "Painters/ListBoxPainter.h"
#include "Painters/FramedFieldPainter.h"
#include "Painters/AlignedImagePainter.h"

#include "Engine/Core/Math/Color.h"
#include "Engine/Content/Content.h"
#include "Engine/Content/Assets/Texture.h"
#include "Engine/Core/Log.h"
#include "Engine/Scripting/Scripting.h"
#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Platform/Base/WindowBase.h"


#if USE_EDITOR
bool FudgetThemes::_runtime_use = false;
FudgetThemes::Data* FudgetThemes::_edittime_data = nullptr;
FudgetThemes::Data* FudgetThemes::_runtime_data = nullptr;
bool FudgetThemes::_edittime_initialized = false;
#endif
bool FudgetThemes::_initialized = false;
FudgetThemes::Data* FudgetThemes::_data = nullptr;
int FudgetThemes::_initialized_count = 0;


const String FudgetThemes::MAIN_THEME = TEXT("Fudgets_MainTheme");

const String FudgetThemes::FRAMED_CONTROL_STYLE = TEXT("Fudgets_FramedControlStyle");
const String FudgetThemes::TEXT_INPUT_STYLE = TEXT("Fudgets_TextInputStyle");
const String FudgetThemes::SINGLELINE_TEXT_INPUT_STYLE = TEXT("Fudgets_SinglelineTextInputStyle");
const String FudgetThemes::MULTILINE_TEXT_INPUT_STYLE = TEXT("Fudgets_MultilineTextInputStyle");
const String FudgetThemes::FRAMED_SINGLELINE_TEXT_INPUT_STYLE = TEXT("Fudgets_FramedSinglelineTextInputStyle");
const String FudgetThemes::FRAMED_MULTILINE_TEXT_INPUT_STYLE = TEXT("Fudgets_FramedMultilineTextInputStyle");
const String FudgetThemes::BUTTON_STYLE = TEXT("Fudgets_ButtonStyle");
const String FudgetThemes::IMAGE_BUTTON_STYLE = TEXT("Fudgets_ImageButtonStyle");
const String FudgetThemes::COMBOBOX_STYLE = TEXT("Fudgets_ComboboxStyle");
const String FudgetThemes::COMBOBOX_EDITOR_STYLE = TEXT("Fudgets_ComboboxEditorStyle");
const String FudgetThemes::COMBOBOX_BUTTON_STYLE = TEXT("Fudgets_ComboboxButtonStyle");
const String FudgetThemes::COMBOBOX_LIST_STYLE = TEXT("Fudgets_ComboboxListStyle");
const String FudgetThemes::LISTBOX_STYLE = TEXT("Fudgets_ListboxStyle");


// FudgetTheme


FudgetTheme::FudgetTheme() : Base(SpawnParams(Guid::New(), TypeInitializer))
{

}

FudgetTheme::FudgetTheme(const FudgetTheme &ori) : FudgetTheme()
{
    _resources = ori._resources;
}

const String& FudgetTheme::GetClassStyleName(const String &class_name) const
{
    String *result = _class_styles.TryGet(class_name);
    if (result != nullptr)
        return *result;
    return class_name;
}

void FudgetTheme::SetClassStyleName(const String &class_name, const String &style_name)
{
    _class_styles[class_name] = style_name;
}

bool FudgetTheme::GetResource(int res_id, API_PARAM(Out) Variant &result) const
{
    auto it = _resources.find(res_id);
    if (it == _resources.end())
    {
        result = Variant();
        return false;
    }

    if (it->second.Type.Type == VariantType::Structure)
    {
        HashSet<int> found;
        while (it->second.Type.Type == VariantType::Structure)
        {
            const FudgetResourceId *id = it->second.AsStructure<FudgetResourceId>();
            if (id == nullptr)
                break;

            if (found.Find(id->Id) != found.End())
            {
                result = Variant();
                return false;
            }
            int next = id->Id;
            found.Add(next);
            it = _resources.find(next);
            if (it == _resources.end())
            {
                result = Variant();
                return false;
            }
        }
    }

    result = it->second;


    return true;
}

void FudgetTheme::SetResource(int res_id, Variant value)
{
    _resources[res_id] = value;
}

void FudgetTheme::SetForwarding(int res_id, int forward_id)
{
    _resources[res_id] = StructToVariant(FudgetResourceId(forward_id));
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

    ++_initialized_count;
}

void FudgetThemes::CreateDefaultThemesAndStyles()
{
    FudgetTheme *main_theme = New<FudgetTheme>();
    _data->_theme_map[MAIN_THEME] = main_theme;


    // DOWNPRESSED_HOVERED_FOCUSED_STATE_ORDER = 0
    FudgetStateOrderBuilder::Begin();
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Disabled);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Pressed | (uint64)FudgetVisualControlState::Down);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Hovered);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Focused);
    FudgetStateOrderBuilder::End();
    // DOWNPRESSED_FOCUSED_HOVERED_STATE_ORDER = 1
    FudgetStateOrderBuilder::Begin();
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Disabled);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Pressed | (uint64)FudgetVisualControlState::Down);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Focused);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Hovered);
    FudgetStateOrderBuilder::End();
    // DOWN_PRESSED_HOVERED_FOCUSED_STATE_ORDER = 2
    FudgetStateOrderBuilder::Begin();
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Disabled);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Down);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Pressed);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Hovered);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Focused);
    FudgetStateOrderBuilder::End();
    // DOWN_PRESSED_FOCUSED_HOVERED_STATE_ORDER = 3
    FudgetStateOrderBuilder::Begin();
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Disabled);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Down);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Pressed);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Focused);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Hovered);
    FudgetStateOrderBuilder::End();
    // HOVERED_FOCUSED_STATE_ORDER = 4
    FudgetStateOrderBuilder::Begin();
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Disabled);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Hovered);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Focused);
    FudgetStateOrderBuilder::End();
    // FOCUSED_HOVERED_STATE_ORDER = 5
    FudgetStateOrderBuilder::Begin();
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Disabled);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Focused);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Hovered);
    FudgetStateOrderBuilder::End();
    // FOCUSED_STATE_ORDER = 5
    FudgetStateOrderBuilder::Begin();
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Disabled);
    FudgetStateOrderBuilder::AddState((uint64)FudgetVisualControlState::Focused);
    FudgetStateOrderBuilder::End();

    // Basic resources

    main_theme->SetResource(FudgetThemePartIds::Background, Color::White);
    main_theme->SetResource(FudgetThemePartIds::FocusedBackground, Color::White);
    main_theme->SetResource(FudgetThemePartIds::DisabledBackground, Color(.9f, .9f, .9f, 1.f));

    FudgetDrawableBuilder::Begin();
    FudgetDrawableBuilder::AddDrawArea(FudgetDrawArea(FudgetBorder(1), Color(.5f, .5f, .5f, 1.f), FudgetFrameType::Inside));
    main_theme->SetResource(FudgetThemePartIds::FrameDraw, FudgetDrawableBuilder::End());
    FudgetDrawableBuilder::Begin();
    FudgetDrawableBuilder::AddDrawArea(FudgetDrawArea(FudgetBorder(1), Color(.4f, .5f, .8f, 1.f), FudgetFrameType::Inside));
    main_theme->SetResource(FudgetThemePartIds::FocusedFrameDraw, FudgetDrawableBuilder::End());
    //FudgetDrawableBuilder::Begin();
    //FudgetDrawableBuilder::AddDrawArea(FudgetDrawArea(FudgetBorder(1), Color(.6f, .6f, .6f, 1.f), FudgetFrameType::Inside));
    //main_theme->SetResource(FudgetThemePartIds::HoveredFrameDraw, FudgetDrawableBuilder::End());
    main_theme->SetResource(FudgetThemePartIds::VisualPadding, FudgetPadding(1));
    main_theme->SetResource(FudgetThemePartIds::ContentPadding, FudgetPadding(4));
    main_theme->SetForwarding(FudgetThemePartIds::ButtonContentPadding, FudgetThemePartIds::ContentPadding);

    main_theme->SetResource(FudgetThemePartIds::ButtonSurface, Color(.7f, .7f, .7f, 1.f));
    main_theme->SetResource(FudgetThemePartIds::ButtonHoveredSurface, Color(.85f, .85f, .85f, 1.f));
    main_theme->SetResource(FudgetThemePartIds::ButtonDisabledSurface, Color(.6f, .6f, .6f, 1.f));
    main_theme->SetResource(FudgetThemePartIds::ButtonFocusedSurface, Color(.70f, .75f, .80f, 1.f));
    main_theme->SetResource(FudgetThemePartIds::ButtonPressedSurface, Color(.65f, .65f, .65f, 1.f));
    main_theme->SetResource(FudgetThemePartIds::ButtonDownSurface, Color(.6f, .6f, .6f, 1.f));
    main_theme->SetResource(FudgetThemePartIds::ButtonContentPressedOffset, Float2(0.f, 1.f));
    main_theme->SetResource(FudgetThemePartIds::ButtonPressedImageOffset, Float2(0.f, 1.f));

    main_theme->SetResource(FudgetThemePartIds::ComboBoxButtonPressedImageOffset, Float2(0.f, 1.f));

    main_theme->SetResource(FudgetThemePartIds::TextColor, Color::Black);
    main_theme->SetForwarding(FudgetThemePartIds::FocusedTextColor, FudgetThemePartIds::TextColor);
    main_theme->SetForwarding(FudgetThemePartIds::DisabledTextColor, FudgetThemePartIds::TextColor);
    main_theme->SetResource(FudgetThemePartIds::SelectedTextColor, Color::White);
    main_theme->SetForwarding(FudgetThemePartIds::FocusedSelectedTextColor, FudgetThemePartIds::SelectedTextColor);
    main_theme->SetForwarding(FudgetThemePartIds::DisabledSelectedTextColor, FudgetThemePartIds::SelectedTextColor);
    main_theme->SetResource(FudgetThemePartIds::TextSelectionBackground, Color(0.2f, 0.4f, 0.9f, 1.0f));
    main_theme->SetForwarding(FudgetThemePartIds::FocusedTextSelectionBackground, FudgetThemePartIds::TextSelectionBackground);
    main_theme->SetResource(FudgetThemePartIds::DisabledTextSelectionBackground, Color(0.5f, 0.5f, 0.5f, 1.0f));
    main_theme->SetForwarding(FudgetThemePartIds::CaretDraw, FudgetThemePartIds::TextColor);
    main_theme->SetResource(FudgetThemePartIds::CaretBlinkTime, 0.8f);
    main_theme->SetResource(FudgetThemePartIds::CaretWidth, 1.f);
    main_theme->SetResource(FudgetThemePartIds::CaretScrollCount, 5);
    main_theme->SetResource(FudgetThemePartIds::BeamCursor, CursorType::IBeam);

    // Style names used for each default class

    main_theme->SetClassStyleName(TEXT("Fudgets.FudgetButton"), IMAGE_BUTTON_STYLE);
    main_theme->SetClassStyleName(TEXT("Fudgets.FudgetTextBox"), FRAMED_MULTILINE_TEXT_INPUT_STYLE);
    main_theme->SetClassStyleName(TEXT("Fudgets.FudgetLineEdit"), FRAMED_SINGLELINE_TEXT_INPUT_STYLE);
    main_theme->SetClassStyleName(TEXT("Fudgets.FudgetListBox"), FRAMED_CONTROL_STYLE);
    main_theme->SetClassStyleName(TEXT("Fudgets.FudgetComboBox"), COMBOBOX_STYLE);
    main_theme->SetClassStyleName(TEXT("Fudgets.FudgetListBox"), LISTBOX_STYLE);

    // Painter resource mappings

    FudgetFramedFieldPainterMapping frame_map;

    FudgetFramedFieldLayer bg_layer;
    bg_layer.Mappings.Add(FudgetFieldMapping(FudgetVisualControlState::Focused,
        FudgetFramedControlPartIds::FocusedFieldBackground, FudgetFramedControlPartIds::FocusedFieldPadding, FudgetFramedControlPartIds::FocusedFieldTint));
    bg_layer.Mappings.Add(FudgetFieldMapping(FudgetVisualControlState::Hovered,
        FudgetFramedControlPartIds::HoveredFieldBackground, FudgetFramedControlPartIds::HoveredFieldPadding, FudgetFramedControlPartIds::HoveredFieldTint));
    bg_layer.Mappings.Add(FudgetFieldMapping(FudgetVisualControlState::Pressed,
        FudgetFramedControlPartIds::PressedFieldBackground, FudgetFramedControlPartIds::PressedFieldPadding, FudgetFramedControlPartIds::PressedFieldTint));
    bg_layer.Mappings.Add(FudgetFieldMapping(FudgetVisualControlState::Down,
        FudgetFramedControlPartIds::DownFieldBackground, FudgetFramedControlPartIds::DownFieldPadding, FudgetFramedControlPartIds::DownFieldTint));
    bg_layer.Mappings.Add(FudgetFieldMapping(FudgetVisualControlState::Disabled,
        FudgetFramedControlPartIds::DisabledFieldBackground, FudgetFramedControlPartIds::DisabledFieldPadding, FudgetFramedControlPartIds::DisabledFieldTint));
    bg_layer.Mappings.Add(FudgetFieldMapping(FudgetVisualControlState::Selected,
        FudgetFramedControlPartIds::SelectedFieldBackground, FudgetFramedControlPartIds::SelectedFieldPadding, FudgetFramedControlPartIds::SelectedFieldTint));
    bg_layer.Mappings.Add(FudgetFieldMapping(FudgetVisualControlState::Normal,
        FudgetFramedControlPartIds::FieldBackground, FudgetFramedControlPartIds::FieldPadding, FudgetFramedControlPartIds::FieldTint));
    FudgetFramedFieldLayer frame_layer;
    frame_layer.Mappings.Add(FudgetFieldMapping(FudgetVisualControlState::Focused,
        FudgetFramedControlPartIds::FocusedFrameDraw, FudgetFramedControlPartIds::FocusedFramePadding, FudgetFramedControlPartIds::FocusedFrameTint));
    frame_layer.Mappings.Add(FudgetFieldMapping(FudgetVisualControlState::Hovered,
        FudgetFramedControlPartIds::HoveredFrameDraw, FudgetFramedControlPartIds::HoveredFramePadding, FudgetFramedControlPartIds::HoveredFrameTint));
    frame_layer.Mappings.Add(FudgetFieldMapping(FudgetVisualControlState::Pressed,
        FudgetFramedControlPartIds::PressedFrameDraw, FudgetFramedControlPartIds::PressedFramePadding, FudgetFramedControlPartIds::PressedFrameTint));
    frame_layer.Mappings.Add(FudgetFieldMapping(FudgetVisualControlState::Down,
        FudgetFramedControlPartIds::DownFrameDraw, FudgetFramedControlPartIds::DownFramePadding, FudgetFramedControlPartIds::DownFrameTint));
    frame_layer.Mappings.Add(FudgetFieldMapping(FudgetVisualControlState::Disabled,
        FudgetFramedControlPartIds::DisabledFrameDraw, FudgetFramedControlPartIds::DisabledFramePadding, FudgetFramedControlPartIds::DisabledFrameTint));
    frame_layer.Mappings.Add(FudgetFieldMapping(FudgetVisualControlState::Selected,
        FudgetFramedControlPartIds::SelectedFrameDraw, FudgetFramedControlPartIds::SelectedFramePadding, FudgetFramedControlPartIds::SelectedFrameTint));
    frame_layer.Mappings.Add(FudgetFieldMapping(FudgetVisualControlState::Normal,
        FudgetFramedControlPartIds::FrameDraw, FudgetFramedControlPartIds::FramePadding, FudgetFramedControlPartIds::FrameTint));

    frame_map.Layers.Add(bg_layer);
    frame_map.Layers.Add(frame_layer);

    frame_map.VisualPadding = (int)FudgetFramedControlPartIds::VisualPadding;
    frame_map.ContentPadding = (int)FudgetFramedControlPartIds::ContentPadding;
   
    main_theme->SetResource(FudgetThemePartIds::FieldFramePainter, FudgetPartPainter::InitializeMapping<FudgetFramedFieldPainter>(FudgetThemes::FOCUSED_HOVERED_STATE_ORDER, frame_map));

    main_theme->SetResource(FudgetThemePartIds::ButtonFramePainter, FudgetPartPainter::InitializeMapping<FudgetFramedFieldPainter>(FudgetThemes::DOWN_PRESSED_HOVERED_FOCUSED_STATE_ORDER, frame_map));
    main_theme->SetForwarding(FudgetThemePartIds::ComboBoxButtonFramePainter, FudgetThemePartIds::ButtonFramePainter);

    FudgetTextPainterMapping text_field_map;
    text_field_map.Mappings.Add(FudgetTextFieldMapping(FudgetVisualControlState::Focused,
        FudgetTextFieldPartIds::FocusedTextSelectionBackground, FudgetTextFieldPartIds::FocusedTextColor, FudgetTextFieldPartIds::FocusedSelectedTextColor));
    text_field_map.Mappings.Add(FudgetTextFieldMapping(FudgetVisualControlState::Disabled,
        FudgetTextFieldPartIds::DisabledTextSelectionBackground, FudgetTextFieldPartIds::DisabledTextColor, FudgetTextFieldPartIds::DisabledSelectedTextColor));
    text_field_map.Mappings.Add(FudgetTextFieldMapping(FudgetVisualControlState::Normal,
        FudgetTextFieldPartIds::TextSelectionBackground, FudgetTextFieldPartIds::TextColor, FudgetTextFieldPartIds::SelectedTextColor));

    text_field_map.Font = (int)FudgetTextFieldPartIds::Font;
    main_theme->SetResource(FudgetThemePartIds::SingleLineInputTextPainter, FudgetPartPainter::InitializeMapping<FudgetLineEditTextPainter>(FudgetThemes::FOCUSED_STATE_ORDER, text_field_map));

    main_theme->SetResource(FudgetThemePartIds::MultiLineInputTextPainter, FudgetPartPainter::InitializeMapping<FudgetTextBoxPainter>(FudgetThemes::FOCUSED_STATE_ORDER, text_field_map));

    FudgetAlignedImagePainterMapping btn_img_map;
    
    btn_img_map.Mappings.Add(FudgetAlignedImageMapping(FudgetVisualControlState::Disabled, FudgetAlignedImagePartIds::DisabledImage, FudgetAlignedImagePartIds::DisabledImagePadding,
        FudgetAlignedImagePartIds::DisabledImageOffset, FudgetAlignedImagePartIds::DisabledImageTint));
    btn_img_map.Mappings.Add(FudgetAlignedImageMapping(FudgetVisualControlState::Hovered, FudgetAlignedImagePartIds::HoveredImage, FudgetAlignedImagePartIds::HoveredImagePadding,
        FudgetAlignedImagePartIds::HoveredImageOffset, FudgetAlignedImagePartIds::HoveredImageTint));
    btn_img_map.Mappings.Add(FudgetAlignedImageMapping(FudgetVisualControlState::Pressed, FudgetAlignedImagePartIds::PressedImage, FudgetAlignedImagePartIds::PressedImagePadding,
        FudgetAlignedImagePartIds::PressedImageOffset, FudgetAlignedImagePartIds::PressedImageTint));
    btn_img_map.Mappings.Add(FudgetAlignedImageMapping(FudgetVisualControlState::Down, FudgetAlignedImagePartIds::DownImage, FudgetAlignedImagePartIds::DownImagePadding,
        FudgetAlignedImagePartIds::DownImageOffset, FudgetAlignedImagePartIds::DownImageTint));
    btn_img_map.Mappings.Add(FudgetAlignedImageMapping(FudgetVisualControlState::Focused, FudgetAlignedImagePartIds::FocusedImage, FudgetAlignedImagePartIds::FocusedImagePadding,
        FudgetAlignedImagePartIds::FocusedImageOffset, FudgetAlignedImagePartIds::FocusedImageTint));
    btn_img_map.Mappings.Add(FudgetAlignedImageMapping(FudgetVisualControlState::Normal, FudgetAlignedImagePartIds::Image, FudgetAlignedImagePartIds::ImagePadding,
        FudgetAlignedImagePartIds::ImageOffset, FudgetAlignedImagePartIds::ImageTint));

    btn_img_map.HorzAlign = (int)FudgetAlignedImagePartIds::HorzImageAlign;
    btn_img_map.VertAlign = (int)FudgetAlignedImagePartIds::VertImageAlign;

    main_theme->SetResource(FudgetThemePartIds::AlignedImageContentPainter, FudgetPartPainter::InitializeMapping<FudgetAlignedImagePainter>(FudgetThemes::DOWN_PRESSED_HOVERED_FOCUSED_STATE_ORDER, btn_img_map));


    FudgetListBoxItemPainterMapping lb_item_map;
    lb_item_map.TextPainter = (int)FudgetListBoxPartIds::TextPainter;

    main_theme->SetResource(FudgetThemePartIds::ListItemPainter, FudgetPartPainter::InitializeMapping<FudgetListBoxItemPainter>(FudgetThemes::HOVERED_FOCUSED_STATE_ORDER, lb_item_map));

    // Built-in styles:

    FudgetStyle *frame_style = CreateOrGetStyle(FRAMED_CONTROL_STYLE);
    frame_style->SetResourceOverride(FudgetFramedControlPartIds::FramePainter, FudgetThemePartIds::FieldFramePainter);
    frame_style->SetResourceOverride(FudgetFramedControlPartIds::FieldBackground, FudgetThemePartIds::Background);
    frame_style->SetResourceOverride(FudgetFramedControlPartIds::HoveredFieldBackground, FudgetThemePartIds::HoveredBackground);
    frame_style->SetResourceOverride(FudgetFramedControlPartIds::DisabledFieldBackground, FudgetThemePartIds::DisabledBackground);
    frame_style->SetResourceOverride(FudgetFramedControlPartIds::FrameDraw, FudgetThemePartIds::FrameDraw);
    frame_style->SetResourceOverride(FudgetFramedControlPartIds::FocusedFrameDraw, FudgetThemePartIds::FocusedFrameDraw);
    frame_style->SetResourceOverride(FudgetFramedControlPartIds::HoveredFrameDraw, FudgetThemePartIds::HoveredFrameDraw);

    frame_style->SetResourceOverride(FudgetFramedControlPartIds::VisualPadding, FudgetThemePartIds::VisualPadding);
    frame_style->SetResourceOverride(FudgetFramedControlPartIds::ContentPadding, FudgetThemePartIds::ContentPadding);

    FudgetStyle *text_style = CreateOrGetStyle(TEXT_INPUT_STYLE);
    text_style->SetResourceOverride(FudgetTextFieldPartIds::TextSelectionBackground, FudgetThemePartIds::TextSelectionBackground);
    text_style->SetResourceOverride(FudgetTextFieldPartIds::FocusedTextSelectionBackground, FudgetThemePartIds::FocusedTextSelectionBackground);
    text_style->SetResourceOverride(FudgetTextFieldPartIds::DisabledTextSelectionBackground, FudgetThemePartIds::DisabledTextSelectionBackground);

    text_style->SetResourceOverride(FudgetTextFieldPartIds::TextColor, FudgetThemePartIds::TextColor);
    text_style->SetResourceOverride(FudgetTextFieldPartIds::FocusedTextColor, FudgetThemePartIds::FocusedTextColor);
    text_style->SetResourceOverride(FudgetTextFieldPartIds::DisabledTextColor, FudgetThemePartIds::DisabledTextColor);
    text_style->SetResourceOverride(FudgetTextFieldPartIds::SelectedTextColor, FudgetThemePartIds::SelectedTextColor);
    text_style->SetResourceOverride(FudgetTextFieldPartIds::FocusedSelectedTextColor, FudgetThemePartIds::FocusedSelectedTextColor);
    text_style->SetResourceOverride(FudgetTextFieldPartIds::DisabledSelectedTextColor, FudgetThemePartIds::DisabledSelectedTextColor);
    text_style->SetResourceOverride(FudgetTextFieldPartIds::Font, FudgetThemePartIds::Font);

    text_style->SetResourceOverride(FudgetTextFieldPartIds::CaretDraw, FudgetThemePartIds::CaretDraw);
    text_style->SetResourceOverride(FudgetTextFieldPartIds::CaretBlinkTime, FudgetThemePartIds::CaretBlinkTime);
    text_style->SetResourceOverride(FudgetTextFieldPartIds::CaretWidth, FudgetThemePartIds::CaretWidth);
    text_style->SetResourceOverride(FudgetTextFieldPartIds::CaretScrollCount, FudgetThemePartIds::CaretScrollCount);
    text_style->SetResourceOverride(FudgetTextFieldPartIds::BeamCursor, FudgetThemePartIds::BeamCursor);


    FudgetStyle *sl_text_style = text_style->CreateInheritedStyle(SINGLELINE_TEXT_INPUT_STYLE);
    sl_text_style->SetResourceOverride(FudgetSinglelineTextFieldPartIds::TextPainter, FudgetThemePartIds::SingleLineInputTextPainter);
    FudgetStyle *ml_text_style = text_style->CreateInheritedStyle(MULTILINE_TEXT_INPUT_STYLE);
    ml_text_style->SetResourceOverride(FudgetMultilineTextFieldPartIds::TextPainter, FudgetThemePartIds::MultiLineInputTextPainter);


    FudgetStyle *sl_framed_text_style = CreateOrGetStyle(FRAMED_SINGLELINE_TEXT_INPUT_STYLE);
    sl_framed_text_style->AddReferencesFor(frame_style);
    sl_framed_text_style->AddReferencesFor(sl_text_style);

    FudgetStyle *ml_framed_text_style = CreateOrGetStyle(FRAMED_MULTILINE_TEXT_INPUT_STYLE);
    ml_framed_text_style->AddReferencesFor(frame_style);
    ml_framed_text_style->AddReferencesFor(ml_text_style);

    FudgetStyle *button_style = CreateOrGetStyle(BUTTON_STYLE);
    button_style->SetResourceOverride(FudgetFramedControlPartIds::FramePainter, FudgetThemePartIds::ButtonFramePainter);
    button_style->SetResourceOverride(FudgetFramedControlPartIds::FrameDraw, FudgetThemePartIds::ButtonSurface);
    button_style->SetResourceOverride(FudgetFramedControlPartIds::HoveredFrameDraw, FudgetThemePartIds::ButtonHoveredSurface);
    button_style->SetResourceOverride(FudgetFramedControlPartIds::DisabledFrameDraw, FudgetThemePartIds::ButtonDisabledSurface);
    button_style->SetResourceOverride(FudgetFramedControlPartIds::FocusedFrameDraw, FudgetThemePartIds::ButtonFocusedSurface);
    button_style->SetResourceOverride(FudgetFramedControlPartIds::PressedFrameDraw, FudgetThemePartIds::ButtonPressedSurface);
    button_style->SetResourceOverride(FudgetFramedControlPartIds::DownFrameDraw, FudgetThemePartIds::ButtonDownSurface);

    button_style->SetResourceOverride(FudgetFramedControlPartIds::ContentPadding, FudgetThemePartIds::ButtonContentPadding);
    button_style->SetResourceOverride(FudgetButtonPartIds::ContentPressedOffset, FudgetThemePartIds::ButtonContentPressedOffset);

    FudgetStyle *image_button_style = button_style->CreateInheritedStyle(IMAGE_BUTTON_STYLE);
    button_style->SetResourceOverride(FudgetButtonPartIds::ContentPainter, FudgetThemePartIds::AlignedImageContentPainter);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::Image, FudgetThemePartIds::ButtonImage);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::HoveredImage, FudgetThemePartIds::ButtonHoveredImage);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::PressedImage, FudgetThemePartIds::ButtonPressedImage);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::DownImage, FudgetThemePartIds::ButtonDownImage);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::FocusedImage, FudgetThemePartIds::ButtonFocusedImage);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::DisabledImage, FudgetThemePartIds::ButtonDisabledImage);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::ImageTint, FudgetThemePartIds::ButtonImageTint);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::HoveredImageTint, FudgetThemePartIds::ButtonHoveredImageTint);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::PressedImageTint, FudgetThemePartIds::ButtonPressedImageTint);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::DownImageTint, FudgetThemePartIds::ButtonDownImageTint);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::FocusedImageTint, FudgetThemePartIds::ButtonFocusedImageTint);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::DisabledImageTint, FudgetThemePartIds::ButtonDisabledImageTint);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::ImageOffset, FudgetThemePartIds::ButtonImageOffset);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::HoveredImageOffset, FudgetThemePartIds::ButtonHoveredImageOffset);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::PressedImageOffset, FudgetThemePartIds::ButtonPressedImageOffset);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::DownImageOffset, FudgetThemePartIds::ButtonDownImageOffset);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::FocusedImageOffset, FudgetThemePartIds::ButtonFocusedImageOffset);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::DisabledImageOffset, FudgetThemePartIds::ButtonDisabledImageOffset);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::ImagePadding, FudgetThemePartIds::ButtonImagePadding);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::HoveredImagePadding, FudgetThemePartIds::ButtonHoveredImagePadding);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::PressedImagePadding, FudgetThemePartIds::ButtonPressedImagePadding);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::DownImagePadding, FudgetThemePartIds::ButtonDownImagePadding);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::FocusedImagePadding, FudgetThemePartIds::ButtonFocusedImagePadding);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::DisabledImagePadding, FudgetThemePartIds::ButtonDisabledImagePadding);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::HorzImageAlign, FudgetThemePartIds::ButtonContentHorzAlignment);
    button_style->SetResourceOverride(FudgetAlignedImagePartIds::VertImageAlign, FudgetThemePartIds::ButtonContentVertAlignment);

    FudgetStyle *combobox_style = frame_style->CreateInheritedStyle(COMBOBOX_STYLE);

    FudgetStyle *cb_button_style = CreateOrGetStyle(COMBOBOX_BUTTON_STYLE);
    cb_button_style->SetResourceOverride(FudgetFramedControlPartIds::FramePainter, FudgetThemePartIds::ComboBoxButtonFramePainter);
    cb_button_style->SetResourceOverride(FudgetFramedControlPartIds::FrameDraw, FudgetThemePartIds::ComboBoxButtonSurface);
    cb_button_style->SetResourceOverride(FudgetFramedControlPartIds::HoveredFrameDraw, FudgetThemePartIds::ComboBoxButtonHoveredSurface);
    cb_button_style->SetResourceOverride(FudgetFramedControlPartIds::DisabledFrameDraw, FudgetThemePartIds::ComboBoxButtonDisabledSurface);
    cb_button_style->SetResourceOverride(FudgetFramedControlPartIds::FocusedFrameDraw, FudgetThemePartIds::ComboBoxButtonFocusedSurface);
    cb_button_style->SetResourceOverride(FudgetFramedControlPartIds::PressedFrameDraw, FudgetThemePartIds::ComboBoxButtonPressedSurface);
    cb_button_style->SetResourceOverride(FudgetFramedControlPartIds::DownFrameDraw, FudgetThemePartIds::ComboBoxButtonDownSurface);

    cb_button_style->SetResourceOverride(FudgetFramedControlPartIds::ContentPadding, FudgetThemePartIds::ButtonContentPadding);

    cb_button_style->SetResourceOverride(FudgetButtonPartIds::ContentPainter, FudgetThemePartIds::AlignedImageContentPainter);
    cb_button_style->SetResourceOverride(FudgetButtonPartIds::ContentPressedOffset, FudgetThemePartIds::ComboBoxButtonContentPressedOffset);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::Image, FudgetThemePartIds::ComboBoxButtonImage);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::HoveredImage, FudgetThemePartIds::ComboBoxButtonHoveredImage);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::PressedImage, FudgetThemePartIds::ComboBoxButtonPressedImage);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::DownImage, FudgetThemePartIds::ComboBoxButtonDownImage);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::FocusedImage, FudgetThemePartIds::ComboBoxButtonFocusedImage);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::DisabledImage, FudgetThemePartIds::ComboBoxButtonDisabledImage);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::ImageTint, FudgetThemePartIds::ComboBoxButtonImageTint);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::HoveredImageTint, FudgetThemePartIds::ComboBoxButtonHoveredImageTint);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::PressedImageTint, FudgetThemePartIds::ComboBoxButtonPressedImageTint);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::DownImageTint, FudgetThemePartIds::ComboBoxButtonDownImageTint);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::FocusedImageTint, FudgetThemePartIds::ComboBoxButtonFocusedImageTint);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::DisabledImageTint, FudgetThemePartIds::ComboBoxButtonDisabledImageTint);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::ImageOffset, FudgetThemePartIds::ComboBoxButtonImageOffset);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::HoveredImageOffset, FudgetThemePartIds::ComboBoxButtonHoveredImageOffset);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::PressedImageOffset, FudgetThemePartIds::ComboBoxButtonPressedImageOffset);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::DownImageOffset, FudgetThemePartIds::ComboBoxButtonDownImageOffset);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::FocusedImageOffset, FudgetThemePartIds::ComboBoxButtonFocusedImageOffset);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::DisabledImageOffset, FudgetThemePartIds::ComboBoxButtonDisabledImageOffset);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::ImagePadding, FudgetThemePartIds::ComboBoxButtonImagePadding);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::HoveredImagePadding, FudgetThemePartIds::ComboBoxButtonHoveredImagePadding);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::PressedImagePadding, FudgetThemePartIds::ComboBoxButtonPressedImagePadding);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::DownImagePadding, FudgetThemePartIds::ComboBoxButtonDownImagePadding);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::FocusedImagePadding, FudgetThemePartIds::ComboBoxButtonFocusedImagePadding);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::DisabledImagePadding, FudgetThemePartIds::ComboBoxButtonDisabledImagePadding);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::HorzImageAlign, FudgetThemePartIds::ComboBoxButtonImageHorzAlignment);
    cb_button_style->SetResourceOverride(FudgetAlignedImagePartIds::VertImageAlign, FudgetThemePartIds::ComboBoxButtonImageVertAlignment);

    FudgetStyle *listbox_style = frame_style->CreateInheritedStyle(LISTBOX_STYLE);
    listbox_style->SetResourceOverride(FudgetListBoxPartIds::ItemPainter, FudgetThemePartIds::ListItemPainter);

    listbox_style->SetResourceOverride(FudgetListBoxPartIds::TextPainter, FudgetThemePartIds::SingleLineInputTextPainter);
    listbox_style->AddReferencesFor(text_style);
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


    //for (const auto &st : _data->_style_map)
    //    if (st.Value != nullptr)
    //        Delete(st.Value);
    _data->_style_map.ClearDelete();
    
    //for (auto p : _data->_theme_map)
    //    if (p.Value != nullptr)
    //        Delete(p.Value);
    _data->_theme_map.ClearDelete();

    _data->_font_asset_map.Clear();

    for (auto p : _data->_draw_list)
        delete p;
    _data->_draw_list.clear();


    for (auto p : _data->_state_order_list)
        delete p;
    _data->_state_order_list.clear();

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

    if (--_initialized_count == 0)
        Delete(FudgetDrawable::Empty);
}


bool FudgetThemes::RegisterFontAsset(int font_id, FontAsset *asset)
{
    if (font_id < 0)
        return false;
    auto it = _data->_font_asset_map.Find(font_id);
    if (it != _data->_font_asset_map.End())
        return false;
    _data->_font_asset_map[font_id] = asset;
    return true;
}

FontAsset* FudgetThemes::GetFontAsset(int font_id)
{
    if (font_id < 0)
        return Content::LoadAsyncInternal<FontAsset>(TEXT("Editor/Fonts/Roboto-Regular"));
    auto it = _data->_font_asset_map.Find(font_id);
    if (it == _data->_font_asset_map.End())
        return nullptr;
    return it->Value;
}


FudgetTheme* FudgetThemes::GetTheme(const String &theme_name)
{
    if (theme_name.IsEmpty())
        return nullptr;
    auto it = _data->_theme_map.Find(theme_name);
    if (it == _data->_theme_map.End())
        return nullptr;
    return it->Value;
}

FudgetTheme* FudgetThemes::DuplicateTheme(const String &src_name, const String &dest_name)
{
    if (src_name.IsEmpty() || dest_name.IsEmpty())
        return nullptr;
    auto it_src = _data->_theme_map.Find(src_name);
    auto it_dst = _data->_theme_map.Find(dest_name);
    if (it_src == _data->_theme_map.End() || it_dst != _data->_theme_map.End())
        return nullptr;

    FudgetTheme *theme = it_src->Value->Duplicate();
    _data->_theme_map[dest_name] = theme;
    return theme;
}

FudgetStyle* FudgetThemes::CreateStyle(const String &style_name)
{
    if (style_name.IsEmpty() || GetStyle(style_name) != nullptr)
        return nullptr;

    FudgetStyle *style = New<FudgetStyle>();
    style->_name = style_name;
    _data->_style_map[style_name] = style;
    return style;
}

FudgetStyle* FudgetThemes::CreateOrGetStyle(const String &style_name)
{
    if (style_name.IsEmpty())
        return nullptr;

    FudgetStyle *style = CreateStyle(style_name);
    if (style != nullptr)
        return style;

    return GetStyle(style_name);
}

FudgetStyle* FudgetThemes::GetStyle(const String &style_name)
{
    if (style_name.IsEmpty())
        return nullptr;
    auto it = _data->_style_map.Find(style_name);
    if (it == _data->_style_map.end())
        return nullptr;
    return it->Value;
}

FudgetStyle* FudgetThemes::FindMatchingStyle(FudgetTheme *theme, const Array<String> &class_names)
{
    if (theme == nullptr)
        return nullptr;

    for (int ix = 0, siz = class_names.Count(); ix < siz; ++ix)
    {
        FudgetStyle *style = GetStyle(theme->GetClassStyleName(class_names[ix]));
        if (style != nullptr)
            return style;
    }
    return nullptr;
}

FudgetPartPainter* FudgetThemes::CreatePainter(const StringAnsi &painter_name)
{
    if (painter_name.IsEmpty())
        return nullptr;

    const ScriptingTypeHandle type = Scripting::FindScriptingType(painter_name);
    if (!type)
        return nullptr;

    if (!FudgetPartPainter::TypeInitializer.IsAssignableFrom(type))
        return nullptr;

    return (FudgetPartPainter*)type.GetType().Script.Spawn(ScriptingObjectSpawnParams(Guid::New(), type));
}

int FudgetThemes::RegisterDrawInstructionList(FudgetDrawInstructionList *drawlist)
{
    if (drawlist == nullptr || IsDrawInstructionListRegistered(drawlist))
        return -1;

    _data->_draw_list.push_back(drawlist);
    return (int)_data->_draw_list.size() - 1;
}

bool FudgetThemes::IsDrawInstructionListRegistered(FudgetDrawInstructionList *drawlist)
{
    return std::find(_data->_draw_list.begin(), _data->_draw_list.end(), drawlist) != _data->_draw_list.end();
}

FudgetDrawInstructionList* FudgetThemes::GetDrawInstructionList(int drawlist_index)
{
    if (drawlist_index < 0 || drawlist_index >= _data->_draw_list.size())
        return nullptr;
    return _data->_draw_list[drawlist_index];
}

int FudgetThemes::RegisterStateOrder(FudgetStateOrder *orderlist)
{
    if (orderlist == nullptr || IsStateOrderRegistered(orderlist))
        return -1;

    _data->_state_order_list.push_back(orderlist);
    return (int)_data->_state_order_list.size() - 1;
}

bool FudgetThemes::IsStateOrderRegistered(FudgetStateOrder *orderlist)
{
    return std::find(_data->_state_order_list.begin(), _data->_state_order_list.end(), orderlist) != _data->_state_order_list.end();
}

FudgetStateOrder* FudgetThemes::GetStateOrder(int orderlist_index)
{
    if (orderlist_index < 0 || orderlist_index >= _data->_state_order_list.size())
        return nullptr;
    return _data->_state_order_list[orderlist_index];
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

