#include "PartPainters.h"
#include "Style.h"
#include "Themes.h"
#include "../MarginStructs.h"
#include "StyleStructs.h"
#include "../Control.h"


// FudgetPartPainter


FudgetPartPainter::FudgetPartPainter(const SpawnParams &params) : Base(params), _theme(nullptr), _style(nullptr)
{

}

FudgetPartPainter::~FudgetPartPainter()
{

}

void FudgetPartPainter::Initialize(FudgetTheme *theme)
{
    _theme = theme;
}

FudgetStyle* FudgetPartPainter::GetStyle()
{
    if (_style == nullptr)
        _style = FudgetThemes::GetStyle(GetStyleToken());
    return _style;
}


// FudgetFramedFieldPainter

FudgetToken FudgetFramedFieldPainter::SelfToken;

FudgetToken FudgetFramedFieldPainter::FieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::DisabledFieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::FocusedFieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::FieldPaddingToken;
FudgetToken FudgetFramedFieldPainter::DisabledFieldPaddingToken;
FudgetToken FudgetFramedFieldPainter::FocusedFieldPaddingToken;

FudgetToken FudgetFramedFieldPainter::BorderDrawToken;
FudgetToken FudgetFramedFieldPainter::FocusedBorderDrawToken;
FudgetToken FudgetFramedFieldPainter::DisabledBorderDrawToken;
FudgetToken FudgetFramedFieldPainter::BorderPaddingToken;
FudgetToken FudgetFramedFieldPainter::FocusedBorderPaddingToken;
FudgetToken FudgetFramedFieldPainter::DisabledBorderPaddingToken;

FudgetToken FudgetFramedFieldPainter::ContentPaddingToken;


/*static*/
void FudgetFramedFieldPainter::CreateStyle()
{
    SelfToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter"));

    FieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FieldBackground"));
    FocusedFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedFieldBackground"));
    DisabledFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledFieldBackground"));
    FieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FieldPadding"));
    FocusedFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedFieldPadding"));
    DisabledFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledFieldPadding"));

    BorderDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_BorderDraw"));
    FocusedBorderDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedBorderDraw"));
    DisabledBorderDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledBorderDraw"));

    BorderPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_BorderPadding"));
    FocusedBorderPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedBorderPadding"));
    DisabledBorderPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledBorderPadding"));

    ContentPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_ContentPadding"));



    FudgetStyle *style = FudgetThemes::CreateStyle(SelfToken);
    if (style == nullptr)
        return;

    style->SetResourceOverride(FieldBackgroundToken, FieldBackgroundToken);
    style->SetResourceOverride(FocusedFieldBackgroundToken, FocusedFieldBackgroundToken);
    style->SetResourceOverride(DisabledFieldBackgroundToken, DisabledFieldBackgroundToken);
    style->SetResourceOverride(FieldPaddingToken, FieldPaddingToken);
    style->SetResourceOverride(FocusedFieldPaddingToken, FocusedFieldPaddingToken);
    style->SetResourceOverride(DisabledFieldPaddingToken, DisabledFieldPaddingToken);

    style->SetResourceOverride(BorderDrawToken, BorderDrawToken);
    style->SetResourceOverride(FocusedBorderDrawToken, FocusedBorderDrawToken);
    style->SetResourceOverride(DisabledBorderDrawToken, DisabledBorderDrawToken);

    style->SetResourceOverride(BorderPaddingToken, BorderPaddingToken);
    style->SetResourceOverride(FocusedBorderPaddingToken, FocusedBorderPaddingToken);
    style->SetResourceOverride(DisabledBorderPaddingToken, DisabledBorderPaddingToken);

    style->SetResourceOverride(ContentPaddingToken, ContentPaddingToken);
}

FudgetFramedFieldPainter::FudgetFramedFieldPainter(const SpawnParams &params) : Base(params)
{
}

void FudgetFramedFieldPainter::Initialize(FudgetTheme *theme)
{
    Base::Initialize(theme);
    FudgetStyle *style = GetStyle();
    if (style == nullptr)
        return;

    if (!style->GetDrawAreaResource(theme, FieldBackgroundToken, _field_bg))
        _field_bg = FudgetDrawArea(Color::White);
    if (!style->GetDrawAreaResource(theme, FocusedFieldBackgroundToken, _focused_field_bg))
        _focused_field_bg = _field_bg;
    if (!style->GetDrawAreaResource(theme, DisabledFieldBackgroundToken, _disabled_field_bg))
        _disabled_field_bg = _field_bg;

    if (!style->GetPaddingResource(theme, FieldPaddingToken, _field_padding))
        _field_padding = FudgetPadding(0.0f);
    if (!style->GetPaddingResource(theme, FocusedFieldPaddingToken, _focused_field_padding))
        _focused_field_padding = _field_padding;
    if (!style->GetPaddingResource(theme, DisabledFieldPaddingToken, _disabled_field_padding))
        _disabled_field_padding = _field_padding;

    if (!style->GetDrawAreaResource(theme, BorderDrawToken, _border_area))
        _border_area = FudgetDrawArea(FudgetPadding( Math::Max(1.0f, _field_padding.Left), Math::Max(1.0f, _field_padding.Top),
            Math::Max(1.0f, _field_padding.Right), Math::Max(1.0f, _field_padding.Bottom)), Color::Gray, FudgetFrameType::Inside);
    if (!style->GetDrawAreaResource(theme, FocusedBorderDrawToken, _focused_border_area))
        _focused_border_area = _border_area;
    if (!style->GetDrawAreaResource(theme, DisabledBorderDrawToken, _disabled_border_area))
        _disabled_border_area = _border_area;

    if (!style->GetPaddingResource(theme, BorderPaddingToken, _border_padding))
        _border_padding = FudgetPadding(0.0f);
    if (!style->GetPaddingResource(theme, FocusedBorderPaddingToken, _focused_border_padding))
        _focused_border_padding = _border_padding;
    if (!style->GetPaddingResource(theme, DisabledBorderPaddingToken, _disabled_border_padding))
        _disabled_border_padding = _border_padding;

    if (!style->GetPaddingResource(theme, ContentPaddingToken, _inner_padding))
        _inner_padding = FudgetPadding(0.0f);
}

void FudgetFramedFieldPainter::Draw(FudgetControl *control)
{
    FudgetDrawArea area = !control->VirtuallyEnabled() ? _disabled_field_bg : control->VirtuallyFocused() ? _focused_field_bg : _field_bg;
    FudgetPadding field_padding = !control->VirtuallyEnabled() ? _disabled_field_padding : control->VirtuallyFocused() ? _focused_field_padding : _field_padding;
    control->DrawArea(area, field_padding.Padded(control->GetBounds()));

    FudgetDrawArea border = !control->VirtuallyEnabled() ? _disabled_border_area : control->VirtuallyFocused() ? _focused_border_area : _border_area;
    FudgetPadding border_padding = !control->VirtuallyEnabled() ? _disabled_border_padding : control->VirtuallyFocused() ? _focused_border_padding : _border_padding;
    control->DrawArea(border, border_padding.Padded(control->GetBounds()));
}


// FudgetComboBoxFieldPainter

FudgetToken FudgetComboBoxFieldPainter::SelfToken;

FudgetToken FudgetComboBoxFieldPainter::FieldBackgroundToken;
FudgetToken FudgetComboBoxFieldPainter::DisabledFieldBackgroundToken;
FudgetToken FudgetComboBoxFieldPainter::FocusedFieldBackgroundToken;
FudgetToken FudgetComboBoxFieldPainter::FieldPaddingToken;
FudgetToken FudgetComboBoxFieldPainter::DisabledFieldPaddingToken;
FudgetToken FudgetComboBoxFieldPainter::FocusedFieldPaddingToken;

FudgetToken FudgetComboBoxFieldPainter::BorderDrawToken;
FudgetToken FudgetComboBoxFieldPainter::FocusedBorderDrawToken;
FudgetToken FudgetComboBoxFieldPainter::DisabledBorderDrawToken;
FudgetToken FudgetComboBoxFieldPainter::BorderPaddingToken;
FudgetToken FudgetComboBoxFieldPainter::FocusedBorderPaddingToken;
FudgetToken FudgetComboBoxFieldPainter::DisabledBorderPaddingToken;

FudgetToken FudgetComboBoxFieldPainter::ContentPaddingToken;


/*static*/
void FudgetComboBoxFieldPainter::CreateStyle()
{
    SelfToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetComboBoxFieldPainter"));

    FieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetComboBoxFieldPainter_FieldBackground"));
    FocusedFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetComboBoxFieldPainter_FocusedFieldBackground"));
    DisabledFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetComboBoxFieldPainter_DisabledFieldBackground"));
    FieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetComboBoxFieldPainter_FieldPadding"));
    FocusedFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetComboBoxFieldPainter_FocusedFieldPadding"));
    DisabledFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetComboBoxFieldPainter_DisabledFieldPadding"));

    BorderDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetComboBoxFieldPainter_BorderDraw"));
    FocusedBorderDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetComboBoxFieldPainter_FocusedBorderDraw"));
    DisabledBorderDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetComboBoxFieldPainter_DisabledBorderDraw"));

    BorderPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetComboBoxFieldPainter_BorderPadding"));
    FocusedBorderPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetComboBoxFieldPainter_FocusedBorderPadding"));
    DisabledBorderPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetComboBoxFieldPainter_DisabledBorderPadding"));

    ContentPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetComboBoxFieldPainter_ContentPadding"));



    FudgetStyle *style = FudgetThemes::CreateStyle(SelfToken);
    if (style == nullptr)
        return;

    style->SetResourceOverride(FieldBackgroundToken, FieldBackgroundToken);
    style->SetResourceOverride(FocusedFieldBackgroundToken, FocusedFieldBackgroundToken);
    style->SetResourceOverride(DisabledFieldBackgroundToken, DisabledFieldBackgroundToken);
    style->SetResourceOverride(FieldPaddingToken, FieldPaddingToken);
    style->SetResourceOverride(FocusedFieldPaddingToken, FocusedFieldPaddingToken);
    style->SetResourceOverride(DisabledFieldPaddingToken, DisabledFieldPaddingToken);

    style->SetResourceOverride(BorderDrawToken, BorderDrawToken);
    style->SetResourceOverride(FocusedBorderDrawToken, FocusedBorderDrawToken);
    style->SetResourceOverride(DisabledBorderDrawToken, DisabledBorderDrawToken);

    style->SetResourceOverride(BorderPaddingToken, BorderPaddingToken);
    style->SetResourceOverride(FocusedBorderPaddingToken, FocusedBorderPaddingToken);
    style->SetResourceOverride(DisabledBorderPaddingToken, DisabledBorderPaddingToken);

    style->SetResourceOverride(ContentPaddingToken, ContentPaddingToken);
}

FudgetComboBoxFieldPainter::FudgetComboBoxFieldPainter(const SpawnParams &params) : Base(params)
{
}

void FudgetComboBoxFieldPainter::Initialize(FudgetTheme *theme)
{
    Base::Initialize(theme);
    FudgetStyle *style = GetStyle();
    if (style == nullptr)
        return;

    if (!style->GetDrawAreaResource(theme, FieldBackgroundToken, _field_bg))
        _field_bg = FudgetDrawArea(Color::White);
    if (!style->GetDrawAreaResource(theme, FocusedFieldBackgroundToken, _focused_field_bg))
        _focused_field_bg = _field_bg;
    if (!style->GetDrawAreaResource(theme, DisabledFieldBackgroundToken, _disabled_field_bg))
        _disabled_field_bg = _field_bg;

    if (!style->GetPaddingResource(theme, FieldPaddingToken, _field_padding))
        _field_padding = FudgetPadding(0.0f);
    if (!style->GetPaddingResource(theme, FocusedFieldPaddingToken, _focused_field_padding))
        _focused_field_padding = _field_padding;
    if (!style->GetPaddingResource(theme, DisabledFieldPaddingToken, _disabled_field_padding))
        _disabled_field_padding = _field_padding;

    if (!style->GetDrawAreaResource(theme, BorderDrawToken, _border_area))
        _border_area = FudgetDrawArea(FudgetPadding(Math::Max(1.0f, _field_padding.Left), Math::Max(1.0f, _field_padding.Top),
        Math::Max(1.0f, _field_padding.Right), Math::Max(1.0f, _field_padding.Bottom)), Color::Gray, FudgetFrameType::Inside);
    if (!style->GetDrawAreaResource(theme, FocusedBorderDrawToken, _focused_border_area))
        _focused_border_area = _border_area;
    if (!style->GetDrawAreaResource(theme, DisabledBorderDrawToken, _disabled_border_area))
        _disabled_border_area = _border_area;

    if (!style->GetPaddingResource(theme, BorderPaddingToken, _border_padding))
        _border_padding = FudgetPadding(0.0f);
    if (!style->GetPaddingResource(theme, FocusedBorderPaddingToken, _focused_border_padding))
        _focused_border_padding = _border_padding;
    if (!style->GetPaddingResource(theme, DisabledBorderPaddingToken, _disabled_border_padding))
        _disabled_border_padding = _border_padding;

    if (!style->GetPaddingResource(theme, ContentPaddingToken, _inner_padding))
        _inner_padding = FudgetPadding(0.0f);
}

void FudgetComboBoxFieldPainter::Draw(FudgetControl *control)
{
    FudgetDrawArea area = !control->VirtuallyEnabled() ? _disabled_field_bg : control->VirtuallyFocused() ? _focused_field_bg : _field_bg;
    FudgetPadding field_padding = !control->VirtuallyEnabled() ? _disabled_field_padding : control->VirtuallyFocused() ? _focused_field_padding : _field_padding;
    control->DrawArea(area, field_padding.Padded(control->GetBounds()));

    FudgetDrawArea border = !control->VirtuallyEnabled() ? _disabled_border_area : control->VirtuallyFocused() ? _focused_border_area : _border_area;
    FudgetPadding border_padding = !control->VirtuallyEnabled() ? _disabled_border_padding : control->VirtuallyFocused() ? _focused_border_padding : _border_padding;
    control->DrawArea(border, border_padding.Padded(control->GetBounds()));
}


