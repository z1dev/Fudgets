#include "Themes.h"
#include "../MarginStructs.h"
#include "StyleStructs.h"
#include "DrawableBuilder.h"

#include "DrawableBuilder.h"

#include "PartPainterIds.h"

#include "Painters/PartPainters.h"
#include "Painters/LineEditTextPainter.h"
#include "Painters/TextBoxPainter.h"
#include "Painters/ListBoxPainter.h"
#include "Painters/DrawablePainter.h"
#include "Painters/ContentPainter.h"
#include "Painters/ScrollBarPainter.h"

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

const String FudgetThemes::FIELD_CONTROL_STYLE = TEXT("Fudgets_FramedControlStyle");
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
const String FudgetThemes::SCROLLBAR_DEFAULT_STYLE = TEXT("Fudgets_NotClass_ScrollBarDefaultStyle");
const String FudgetThemes::SCROLLBAR_WINDOWS_BUTTONS_STYLE = TEXT("Fudgets_NotClass_ScrollBarWindowsButtonsStyle");
const String FudgetThemes::SCROLLBAR_OLDMAC_BUTTONS_STYLE = TEXT("Fudgets_NotClass_ScrollBarOldMacButtonsStyle");

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

    // Basic resources

    FudgetDrawableBuilder::MakeDrawColors(Color(.9f, .9f, .9f, 1.f), FudgetVisualControlState::Disabled);
    FudgetDrawableBuilder::MakeDrawColors(Color(.9f, .9f, 1.f, 1.f), FudgetVisualControlState::Focused);
    FudgetDrawableBuilder::MakeDrawColors(Color::White);
    main_theme->SetResource(FudgetThemePartIds::FieldBackground, FudgetDrawableBuilder::EndDrawColors());

    FudgetDrawableBuilder::Begin(FudgetVisualControlState::Disabled);
    FudgetDrawableBuilder::AddResource(FudgetThemePartIds::FieldBackground);
    FudgetDrawableBuilder::AddDrawBorder(FudgetDrawBorder(Color(.4f, .4f, .4f, 1.f), FudgetBorder(1), FudgetBorderPlacement::Inside));
    FudgetDrawableBuilder::Begin(FudgetVisualControlState::Focused);
    FudgetDrawableBuilder::AddResource(FudgetThemePartIds::FieldBackground);
    FudgetDrawableBuilder::AddDrawBorder(FudgetDrawBorder(Color(.4f, .5f, .8f, 1.f), FudgetBorder(1), FudgetBorderPlacement::Inside));
    FudgetDrawableBuilder::Begin();
    FudgetDrawableBuilder::AddResource(FudgetThemePartIds::FieldBackground);
    FudgetDrawableBuilder::AddDrawBorder(FudgetDrawBorder(Color(.9f, .9f, .8f, 1.f), FudgetBorder(1), FudgetBorderPlacement::Inside));
    main_theme->SetResource(FudgetThemePartIds::FrameDraw, FudgetDrawableBuilder::End());
    main_theme->SetResource(FudgetThemePartIds::FieldFrameThickness, FudgetPadding(1));
    main_theme->SetResource(FudgetThemePartIds::FieldPadding, FudgetPadding(2));
    main_theme->SetResource(FudgetThemePartIds::ButtonPadding, FudgetPadding(4));

    main_theme->SetResource(FudgetThemePartIds::ScrollBarWidth, 12);
    main_theme->SetResource(FudgetThemePartIds::ScrollBarBeforeTrackButtonCount, 0);
    main_theme->SetResource(FudgetThemePartIds::ScrollBarAfterTrackButtonCount, 0);

    for (int ix = 0; ix < 20; ++ix)
    {
        main_theme->SetForwarding((int)FudgetThemePartIds::ScrollBarFirstHorizontalButtonSize + ix, FudgetThemePartIds::ScrollBarWidth);
        main_theme->SetForwarding((int)FudgetThemePartIds::ScrollBarFirstVerticalButtonSize + ix, FudgetThemePartIds::ScrollBarWidth);
    }

    main_theme->SetResource(FudgetThemePartIds::ScrollBarHorizontalThumb, Color::Black);
    main_theme->SetResource(FudgetThemePartIds::ScrollBarVerticalThumb, Color::Black);
    main_theme->SetResource(FudgetThemePartIds::ScrollBarFirstHorizontalButton, Color::Black);
    main_theme->SetResource((int)FudgetThemePartIds::ScrollBarFirstHorizontalButton + 1, Color::Black);
    main_theme->SetResource(FudgetThemePartIds::ScrollBarFirstVerticalButton, Color::Black);
    main_theme->SetResource((int)FudgetThemePartIds::ScrollBarFirstVerticalButton + 1, Color::Black);
    //main_theme->SetResource(FudgetThemePartIds::ScrollBarHorizontalTrack, Color::LightGray);
    //main_theme->SetResource(FudgetThemePartIds::ScrollBarVerticalTrack, Color::LightGray);
    main_theme->SetResource(FudgetThemePartIds::ScrollBarIsThumbSizeFixed, false);
    main_theme->SetResource(FudgetThemePartIds::ScrollBarMinThumbSize, 16);

    main_theme->SetResource(FudgetThemePartIds::ScrollBarLineUpRole, (int)FudgetScrollBarButtonRole::LineUp);
    main_theme->SetResource(FudgetThemePartIds::ScrollBarLineDownRole, (int)FudgetScrollBarButtonRole::LineDown);
    main_theme->SetResource(FudgetThemePartIds::ScrollBarPageUpRole, (int)FudgetScrollBarButtonRole::PageUp);
    main_theme->SetResource(FudgetThemePartIds::ScrollBarPageDownRole, (int)FudgetScrollBarButtonRole::PageDown);
    main_theme->SetResource(FudgetThemePartIds::ScrollBarPageUpLineRole, (int)FudgetScrollBarButtonRole::PageUpLine);
    main_theme->SetResource(FudgetThemePartIds::ScrollBarPageDownLineRole, (int)FudgetScrollBarButtonRole::PageDownLine);
    main_theme->SetResource(FudgetThemePartIds::ScrollBarJumpToStartRole, (int)FudgetScrollBarButtonRole::JumpToStart);
    main_theme->SetResource(FudgetThemePartIds::ScrollBarJumpToEndRole, (int)FudgetScrollBarButtonRole::JumpToEnd);

    main_theme->SetForwarding(FudgetThemePartIds::ScrollBarBeforeTrackRole, FudgetThemePartIds::ScrollBarPageUpRole);
    main_theme->SetForwarding(FudgetThemePartIds::ScrollBarAfterTrackRole, FudgetThemePartIds::ScrollBarPageDownRole);
    main_theme->SetForwarding(FudgetThemePartIds::ScrollBarFirstButtonRole, FudgetThemePartIds::ScrollBarLineUpRole);
    main_theme->SetForwarding((int)FudgetThemePartIds::ScrollBarFirstButtonRole + 1, FudgetThemePartIds::ScrollBarLineDownRole);

    FudgetDrawableBuilder::Begin(FudgetVisualControlState::Disabled);
    FudgetDrawableBuilder::BeginSubData();
    FudgetDrawableBuilder::AddPadding(FudgetPadding(2));
    FudgetDrawableBuilder::AddColor(Color(.6f, .6f, .6f, 1.f));
    FudgetDrawableBuilder::EndSubData();
    FudgetDrawableBuilder::AddDrawBorder(FudgetDrawBorder(Color(.3f, .3f, .3f, 1.f), FudgetBorder(3), FudgetBorderPlacement::Inside));
    FudgetDrawableBuilder::Begin(FudgetVisualControlState::Down);
    FudgetDrawableBuilder::BeginSubData();
    FudgetDrawableBuilder::AddPadding(FudgetPadding(2));
    FudgetDrawableBuilder::AddColor(Color(.6f, .6f, .6f, 1.f));
    FudgetDrawableBuilder::EndSubData();
    FudgetDrawableBuilder::AddDrawBorder(FudgetDrawBorder(Color(.3f, .3f, .3f, 1.f), FudgetBorder(3), FudgetBorderPlacement::Inside));
    FudgetDrawableBuilder::Begin(FudgetVisualControlState::Pressed);
    FudgetDrawableBuilder::BeginSubData();
    FudgetDrawableBuilder::AddPadding(FudgetPadding(2));
    FudgetDrawableBuilder::AddColor(Color(.65f, .65f, .65f, 1.f));
    FudgetDrawableBuilder::EndSubData();
    FudgetDrawableBuilder::AddDrawBorder(FudgetDrawBorder(Color(.3f, .3f, .3f, 1.f), FudgetBorder(3), FudgetBorderPlacement::Inside));
    FudgetDrawableBuilder::Begin(FudgetVisualControlState::Hovered);
    FudgetDrawableBuilder::BeginSubData();
    FudgetDrawableBuilder::AddPadding(FudgetPadding(2));
    FudgetDrawableBuilder::AddColor(Color(.85f, .85f, .85f, 1.f));
    FudgetDrawableBuilder::EndSubData();
    FudgetDrawableBuilder::AddDrawBorder(FudgetDrawBorder(Color(.3f, .3f, .3f, 1.f), FudgetBorder(3), FudgetBorderPlacement::Inside));
    FudgetDrawableBuilder::Begin(FudgetVisualControlState::Focused);
    FudgetDrawableBuilder::BeginSubData();
    FudgetDrawableBuilder::AddPadding(FudgetPadding(2));
    FudgetDrawableBuilder::AddColor(Color(.65f, .65f, .85f, 1.f));
    FudgetDrawableBuilder::EndSubData();
    FudgetDrawableBuilder::AddDrawBorder(FudgetDrawBorder(Color(.3f, .3f, .3f, 1.f), FudgetBorder(3), FudgetBorderPlacement::Inside));
    FudgetDrawableBuilder::Begin(0);
    FudgetDrawableBuilder::BeginSubData();
    FudgetDrawableBuilder::AddPadding(FudgetPadding(2));
    FudgetDrawableBuilder::AddColor(Color(.75f, .75f, .75f, 1.f));
    FudgetDrawableBuilder::EndSubData();
    FudgetDrawableBuilder::AddDrawBorder(FudgetDrawBorder(Color(.3f, .3f, .3f, 1.f), FudgetBorder(3), FudgetBorderPlacement::Inside));
    main_theme->SetResource(FudgetThemePartIds::ButtonSurface, FudgetDrawableBuilder::End());
    main_theme->SetResource(FudgetThemePartIds::ButtonContentPressedOffset, Float2(0.f, 1.f));

    main_theme->SetForwarding(FudgetThemePartIds::ComboBoxButtonSurface, FudgetThemePartIds::ButtonSurface);
    main_theme->SetResource(FudgetThemePartIds::ComboBoxButtonContentPressedOffset, Float2(0.f, 1.f));

    FudgetDrawableBuilder::MakeDrawColors(Color(0.8f, 0.8f, 0.8f, 1.f), FudgetVisualControlState::Disabled);
    FudgetDrawableBuilder::MakeDrawColors(Color::White, FudgetVisualControlState::Selected);
    FudgetDrawableBuilder::MakeDrawColors(Color::Black);
    main_theme->SetResource(FudgetThemePartIds::FieldTextColor, FudgetDrawableBuilder::EndDrawColors());

    FudgetDrawableBuilder::MakeDrawColors(Color(0.5f, 0.5f, 0.5f, 1.f), FudgetVisualControlState::Disabled);
    FudgetDrawableBuilder::MakeDrawColors(Color(0.2f, 0.4f, 0.9f, 1.f), FudgetVisualControlState::Selected | FudgetVisualControlState::Focused);
    FudgetDrawableBuilder::MakeDrawColors(Color(0.2f, 0.3f, 0.6f, 1.f), FudgetVisualControlState::Selected);
    main_theme->SetResource(FudgetThemePartIds::FieldTextBackground, FudgetDrawableBuilder::EndDrawColors());
    main_theme->SetResource(FudgetThemePartIds::CaretDraw, Color::Black);
    main_theme->SetResource(FudgetThemePartIds::CaretBlinkTime, 0.8f);
    main_theme->SetResource(FudgetThemePartIds::CaretWidth, 1.f);
    main_theme->SetResource(FudgetThemePartIds::CaretScrollCount, 5);
    main_theme->SetResource(FudgetThemePartIds::BeamCursor, CursorType::IBeam);

    // Style names used for each default class

    main_theme->SetClassStyleName(TEXT("Fudgets.FudgetButton"), IMAGE_BUTTON_STYLE);
    main_theme->SetClassStyleName(TEXT("Fudgets.FudgetTextBox"), FRAMED_MULTILINE_TEXT_INPUT_STYLE);
    main_theme->SetClassStyleName(TEXT("Fudgets.FudgetLineEdit"), FRAMED_SINGLELINE_TEXT_INPUT_STYLE);
    main_theme->SetClassStyleName(TEXT("Fudgets.FudgetComboBox"), COMBOBOX_STYLE);
    main_theme->SetClassStyleName(TEXT("Fudgets.FudgetListBox"), LISTBOX_STYLE);

    // Painter resource mappings

    FudgetDrawablePainterMapping frame_map;
    frame_map.Drawable = (int)FudgetFramedControlPartIds::FrameDraw;
    frame_map.Margin = (int)FudgetFramedControlPartIds::FrameMargin;
    frame_map.Tint = (int)FudgetFramedControlPartIds::FrameTint;

    frame_map.Padding = (int)FudgetFramedControlPartIds::FrameThickness;
   
    main_theme->SetResource(FudgetThemePartIds::FieldFramePainter, FudgetPartPainter::InitializeMapping<FudgetDrawablePainter>(frame_map));
    main_theme->SetResource(FudgetThemePartIds::ButtonFramePainter, FudgetPartPainter::InitializeMapping<FudgetDrawablePainter>(frame_map));
    main_theme->SetForwarding(FudgetThemePartIds::ComboBoxButtonFramePainter, FudgetThemePartIds::ButtonFramePainter);

    FudgetTextPainterMapping text_field_map;
    text_field_map.TextColor = (int)FudgetTextFieldPartIds::TextColor;
    text_field_map.BackgroundDraw = (int)FudgetTextFieldPartIds::TextBackground;
    text_field_map.BackgroundTint = (int)FudgetTextFieldPartIds::TextBackgroundTint;

    text_field_map.Font = (int)FudgetTextFieldPartIds::Font;
    main_theme->SetResource(FudgetThemePartIds::SingleLineInputTextPainter, FudgetPartPainter::InitializeMapping<FudgetLineEditTextPainter>(text_field_map));

    main_theme->SetResource(FudgetThemePartIds::MultiLineInputTextPainter, FudgetPartPainter::InitializeMapping<FudgetTextBoxPainter>(text_field_map));

    FudgetContentPainterMapping btn_content_map;
    btn_content_map.Drawable = (int)FudgetContentPartIds::Drawable;
    btn_content_map.Margin = (int)FudgetContentPartIds::Margin;
    btn_content_map.Offset = (int)FudgetContentPartIds::Offset;
    btn_content_map.Tint = (int)FudgetContentPartIds::Tint;
    main_theme->SetResource(FudgetThemePartIds::DrawableContentPainter, FudgetPartPainter::InitializeMapping<FudgetContentPainter>(btn_content_map));


    FudgetListBoxItemPainterMapping lb_item_map;
    lb_item_map.TextPainter = (int)FudgetListBoxPartIds::TextPainter;
    lb_item_map.BgDraw = (int)FudgetListBoxPartIds::Background;
    lb_item_map.BgTint = (int)FudgetListBoxPartIds::BackgroundTint;
    main_theme->SetResource(FudgetThemePartIds::ListItemPainter, FudgetPartPainter::InitializeMapping<FudgetListBoxItemPainter>(lb_item_map));


    FudgetScrollBarPainterMapping sb_map;
    sb_map.Orientation = FudgetScrollBarOrientation::Horizontal;
    sb_map.Width = (int)FudgetScrollBarPartIds::Width;
    sb_map.IsThumbSizeFixed = (int)FudgetScrollBarPartIds::IsThumbSizeFixed;
    sb_map.ThumbSize = (int)FudgetScrollBarPartIds::ThumbSize;
    sb_map.MinThumbSize = (int)FudgetScrollBarPartIds::MinThumbSize;

    sb_map.BgDraw = (int)FudgetScrollBarPartIds::HorzBackgroundDraw;
    sb_map.FrameDraw = (int)FudgetScrollBarPartIds::FrameDraw;
    sb_map.ThumbDraw = (int)FudgetScrollBarPartIds::HorzThumbDraw;
    sb_map.TrackDraw = (int)FudgetScrollBarPartIds::HorzTrackDraw;
    sb_map.BeforeTrackDraw = (int)FudgetScrollBarPartIds::HorzBeforeTrackDraw;
    sb_map.AfterTrackDraw = (int)FudgetScrollBarPartIds::HorzAfterTrackDraw;

    sb_map.BeforeTrackButtonCount = (int)FudgetScrollBarPartIds::BeforeTrackButtonCount;
    sb_map.AfterTrackButtonCount = (int)FudgetScrollBarPartIds::AfterTrackButtonCount;

    sb_map.BeforeTrackRole = (int)FudgetScrollBarPartIds::BeforeTrackRole;
    sb_map.AfterTrackRole = (int)FudgetScrollBarPartIds::AfterTrackRole;

    for (int ix = 0, siz = 20; ix < siz; ++ix)
    {
        sb_map.ButtonDraw[ix] = (int)FudgetScrollBarPartIds::FirstHorzButtonDraw + ix;
        sb_map.ButtonSize[ix] = (int)FudgetScrollBarPartIds::FirstHorzButtonSize + ix;

        sb_map.ButtonRole[ix] = (int)FudgetScrollBarPartIds::FirstButtonRole + ix;
    }
    main_theme->SetResource(FudgetThemePartIds::HorzScrollBarPainter, FudgetPartPainter::InitializeMapping<FudgetScrollBarPainter>(sb_map));

    sb_map.Orientation = FudgetScrollBarOrientation::Vertical;
    sb_map.BgDraw = (int)FudgetScrollBarPartIds::VertBackgroundDraw;
    sb_map.ThumbDraw = (int)FudgetScrollBarPartIds::VertThumbDraw;
    sb_map.TrackDraw = (int)FudgetScrollBarPartIds::VertTrackDraw;
    sb_map.BeforeTrackDraw = (int)FudgetScrollBarPartIds::VertBeforeTrackDraw;
    sb_map.AfterTrackDraw = (int)FudgetScrollBarPartIds::VertAfterTrackDraw;

    for (int ix = 0, siz = 20; ix < siz; ++ix)
    {
        sb_map.ButtonDraw[ix] = (int)FudgetScrollBarPartIds::FirstVertButtonDraw + ix;
        sb_map.ButtonSize[ix] = (int)FudgetScrollBarPartIds::FirstVertButtonSize + ix;
    }
    main_theme->SetResource(FudgetThemePartIds::VertScrollBarPainter, FudgetPartPainter::InitializeMapping<FudgetScrollBarPainter>(sb_map));

    // Built-in styles:

    FudgetStyle *sb_default_style = CreateOrGetStyle(SCROLLBAR_DEFAULT_STYLE);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::HorzPainter, FudgetThemePartIds::HorzScrollBarPainter);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::VertPainter, FudgetThemePartIds::VertScrollBarPainter);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::Width, FudgetThemePartIds::ScrollBarWidth);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::IsThumbSizeFixed, FudgetThemePartIds::ScrollBarIsThumbSizeFixed);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::ThumbSize, FudgetThemePartIds::ScrollBarThumbSize);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::MinThumbSize, FudgetThemePartIds::ScrollBarMinThumbSize);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::HorzBackgroundDraw, FudgetThemePartIds::ScrollBarHorizontalBackground);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::VertBackgroundDraw, FudgetThemePartIds::ScrollBarVerticalBackground);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::FrameDraw, FudgetThemePartIds::ScrollBarFrame);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::VertThumbDraw, FudgetThemePartIds::ScrollBarVerticalThumb);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::VertTrackDraw, FudgetThemePartIds::ScrollBarVerticalTrack);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::VertBeforeTrackDraw, FudgetThemePartIds::ScrollBarVerticalBeforeTrack);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::VertAfterTrackDraw, FudgetThemePartIds::ScrollBarVerticalAfterTrack);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::HorzThumbDraw, FudgetThemePartIds::ScrollBarHorizontalThumb);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::HorzTrackDraw, FudgetThemePartIds::ScrollBarHorizontalTrack);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::HorzBeforeTrackDraw, FudgetThemePartIds::ScrollBarHorizontalBeforeTrack);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::HorzAfterTrackDraw, FudgetThemePartIds::ScrollBarHorizontalAfterTrack);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::BeforeTrackButtonCount, FudgetThemePartIds::ScrollBarBeforeTrackButtonCount);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::AfterTrackButtonCount, FudgetThemePartIds::ScrollBarAfterTrackButtonCount);
    for (int ix = 0; ix < 20; ++ix)
    {
        sb_default_style->SetResourceOverride((int)FudgetScrollBarPartIds::FirstVertButtonDraw + ix, (int)FudgetThemePartIds::ScrollBarFirstVerticalButton + ix);
        sb_default_style->SetResourceOverride((int)FudgetScrollBarPartIds::FirstVertButtonSize + ix, (int)FudgetThemePartIds::ScrollBarFirstVerticalButtonSize + ix);
        sb_default_style->SetResourceOverride((int)FudgetScrollBarPartIds::FirstHorzButtonDraw + ix, (int)FudgetThemePartIds::ScrollBarFirstHorizontalButton + ix);
        sb_default_style->SetResourceOverride((int)FudgetScrollBarPartIds::FirstHorzButtonSize + ix, (int)FudgetThemePartIds::ScrollBarFirstHorizontalButtonSize + ix);
    }
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::BeforeTrackRole, FudgetThemePartIds::ScrollBarBeforeTrackRole);
    sb_default_style->SetResourceOverride(FudgetScrollBarPartIds::AfterTrackRole, FudgetThemePartIds::ScrollBarAfterTrackRole);
    for (int ix = 0; ix < 20; ++ix)
    {
        sb_default_style->SetResourceOverride((int)FudgetScrollBarPartIds::FirstButtonRole + ix, (int)FudgetThemePartIds::ScrollBarFirstButtonRole + ix);
    }

    FudgetStyle *sb_windows_style = sb_default_style->CreateInheritedStyle(SCROLLBAR_WINDOWS_BUTTONS_STYLE);
    sb_windows_style->SetValueOverride(FudgetScrollBarPartIds::BeforeTrackButtonCount, 1);
    sb_windows_style->SetValueOverride(FudgetScrollBarPartIds::AfterTrackButtonCount, 1);
    sb_windows_style->SetValueOverride((int)FudgetScrollBarPartIds::FirstButtonRole, (int)FudgetScrollBarButtonRole::LineUp);
    sb_windows_style->SetValueOverride((int)FudgetScrollBarPartIds::FirstButtonRole + 1, (int)FudgetScrollBarButtonRole::LineDown);

    FudgetStyle *sb_oldmac_style = sb_default_style->CreateInheritedStyle(SCROLLBAR_OLDMAC_BUTTONS_STYLE);
    sb_oldmac_style->SetValueOverride(FudgetScrollBarPartIds::BeforeTrackButtonCount, 0);
    sb_oldmac_style->SetValueOverride(FudgetScrollBarPartIds::AfterTrackButtonCount, 2);
    sb_oldmac_style->SetValueOverride((int)FudgetScrollBarPartIds::FirstButtonRole, (int)FudgetScrollBarButtonRole::LineUp);
    sb_oldmac_style->SetValueOverride((int)FudgetScrollBarPartIds::FirstButtonRole + 1, (int)FudgetScrollBarButtonRole::LineDown);

    FudgetStyle *field_style = CreateOrGetStyle(FIELD_CONTROL_STYLE);
    field_style->SetResourceOverride(FudgetFramedControlPartIds::FramePainter, FudgetThemePartIds::FieldFramePainter);
    field_style->SetResourceOverride(FudgetFramedControlPartIds::FrameDraw, FudgetThemePartIds::FieldFrameDraw);

    field_style->SetResourceOverride(FudgetFramedControlPartIds::FrameThickness, FudgetThemePartIds::FieldFrameThickness);
    field_style->SetResourceOverride(FudgetFieldPartIds::Padding, FudgetThemePartIds::FieldPadding);

    field_style->AddReferencesFor(sb_default_style);
    //field_style->AddReferencesFor(sb_windows_style);

    FudgetStyle *text_style = CreateOrGetStyle(TEXT_INPUT_STYLE);
    text_style->SetResourceOverride(FudgetTextFieldPartIds::TextBackground, FudgetThemePartIds::FieldTextBackground);

    text_style->SetResourceOverride(FudgetTextFieldPartIds::TextColor, FudgetThemePartIds::FieldTextColor);
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


    FudgetStyle *sl_framed_text_style = field_style->CreateInheritedStyle(FRAMED_SINGLELINE_TEXT_INPUT_STYLE);
    sl_framed_text_style->AddReferencesFor(sl_text_style);

    FudgetStyle *ml_framed_text_style = field_style->CreateInheritedStyle(FRAMED_MULTILINE_TEXT_INPUT_STYLE);
    ml_framed_text_style->AddReferencesFor(ml_text_style);

    FudgetStyle *button_style = CreateOrGetStyle(BUTTON_STYLE);
    button_style->SetResourceOverride(FudgetFramedControlPartIds::FramePainter, FudgetThemePartIds::ButtonFramePainter);
    button_style->SetResourceOverride(FudgetFramedControlPartIds::FrameDraw, FudgetThemePartIds::ButtonSurface);

    button_style->SetResourceOverride(FudgetButtonPartIds::Padding, FudgetThemePartIds::ButtonPadding);
    button_style->SetResourceOverride(FudgetButtonPartIds::PressedOffset, FudgetThemePartIds::ButtonContentPressedOffset);

    FudgetStyle *image_button_style = button_style->CreateInheritedStyle(IMAGE_BUTTON_STYLE);
    button_style->SetResourceOverride(FudgetContentPartIds::ContentPainter, FudgetThemePartIds::DrawableContentPainter);
    button_style->SetResourceOverride(FudgetContentPartIds::Drawable, FudgetThemePartIds::ButtonImage);
    button_style->SetResourceOverride(FudgetContentPartIds::Tint, FudgetThemePartIds::ButtonImageTint);
    button_style->SetResourceOverride(FudgetContentPartIds::Offset, FudgetThemePartIds::ButtonImageOffset);
    button_style->SetResourceOverride(FudgetContentPartIds::Margin, FudgetThemePartIds::ButtonImageMargin);

    FudgetStyle *combobox_style = field_style->CreateInheritedStyle(COMBOBOX_STYLE);

    FudgetStyle *cb_button_style = CreateOrGetStyle(COMBOBOX_BUTTON_STYLE);
    cb_button_style->SetResourceOverride(FudgetFramedControlPartIds::FramePainter, FudgetThemePartIds::ComboBoxButtonFramePainter);
    cb_button_style->SetResourceOverride(FudgetFramedControlPartIds::FrameDraw, FudgetThemePartIds::ComboBoxButtonSurface);

    cb_button_style->SetResourceOverride(FudgetButtonPartIds::Padding, FudgetThemePartIds::ButtonPadding);

    cb_button_style->SetResourceOverride(FudgetContentPartIds::ContentPainter, FudgetThemePartIds::DrawableContentPainter);
    cb_button_style->SetResourceOverride(FudgetButtonPartIds::PressedOffset, FudgetThemePartIds::ComboBoxButtonContentPressedOffset);
    cb_button_style->SetResourceOverride(FudgetContentPartIds::Drawable, FudgetThemePartIds::ComboBoxButtonImage);
    cb_button_style->SetResourceOverride(FudgetContentPartIds::Tint, FudgetThemePartIds::ComboBoxButtonImageTint);
    cb_button_style->SetResourceOverride(FudgetContentPartIds::Offset, FudgetThemePartIds::ComboBoxButtonImageOffset);
    cb_button_style->SetResourceOverride(FudgetContentPartIds::Margin, FudgetThemePartIds::ComboBoxButtonImageMargin);

    FudgetStyle *listbox_style = field_style->CreateInheritedStyle(LISTBOX_STYLE);
    listbox_style->SetResourceOverride(FudgetListBoxPartIds::ItemPainter, FudgetThemePartIds::ListItemPainter);
    listbox_style->SetResourceOverride(FudgetListBoxPartIds::TextPainter, FudgetThemePartIds::SingleLineInputTextPainter);
    listbox_style->SetResourceOverride(FudgetListBoxPartIds::Background, FudgetThemePartIds::ListItemBackground);
    listbox_style->SetResourceOverride(FudgetListBoxPartIds::BackgroundTint, FudgetThemePartIds::ListItemBackgroundTint);
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

    for (auto inst : _data->_draw_list)
        for (auto d : inst._instructions)
            delete d;

    _data->_draw_list.clear();

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

int FudgetThemes::RegisterDrawInstructionList(const Array<uint64> &statelist, const std::vector<FudgetDrawInstructionList*> &drawlist)
{
    if (drawlist.empty() || statelist.Count() != drawlist.size())
        return -1;

    _data->_draw_list.push_back(StatedDrawInstructions(statelist, drawlist));
    return (int)_data->_draw_list.size() - 1;
}

bool FudgetThemes::GetDrawInstructionList(int drawlist_index, Array<uint64> &states_result, std::vector<FudgetDrawInstructionList*> &list_result)
{
    if (drawlist_index < 0 || drawlist_index >= _data->_draw_list.size())
        return false;

    states_result = _data->_draw_list[drawlist_index]._states;
    list_result = _data->_draw_list[drawlist_index]._instructions;
    return true;
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

