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


// FudgetControlFieldPainter

FudgetToken FudgetControlFieldPainter::SelfToken;

FudgetToken FudgetControlFieldPainter::FieldBackgroundToken;
FudgetToken FudgetControlFieldPainter::DisabledFieldBackgroundToken;
FudgetToken FudgetControlFieldPainter::FieldPaddingToken;
FudgetToken FudgetControlFieldPainter::DisabledFieldPaddingToken;

FudgetToken FudgetControlFieldPainter::BorderDrawToken;
FudgetToken FudgetControlFieldPainter::FocusedBorderDrawToken;
FudgetToken FudgetControlFieldPainter::DisabledBorderDrawToken;

FudgetToken FudgetControlFieldPainter::BorderPaddingToken;
FudgetToken FudgetControlFieldPainter::FocusedBorderPaddingToken;
FudgetToken FudgetControlFieldPainter::DisabledBorderPaddingToken;

FudgetToken FudgetControlFieldPainter::ContentPaddingToken;


/*static*/
void FudgetControlFieldPainter::CreateStyle()
{
    SelfToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetControlFieldPainter"));

    FieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetControlFieldPainter_FieldBackground"));
    DisabledFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetControlFieldPainter_DisabledFieldBackground"));
    FieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetControlFieldPainter_FieldPadding"));
    DisabledFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetControlFieldPainter_DisabledFieldPadding"));

    BorderDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetControlFieldPainter_BorderDraw"));
    FocusedBorderDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetControlFieldPainter_FocusedBorderDraw"));
    DisabledBorderDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetControlFieldPainter_DisabledBorderDraw"));

    BorderPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetControlFieldPainter_BorderPadding"));
    FocusedBorderPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetControlFieldPainter_FocusedBorderPadding"));
    DisabledBorderPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetControlFieldPainter_DisabledBorderPadding"));

    ContentPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetControlFieldPainter_ContentPadding"));



    FudgetStyle *style = FudgetThemes::CreateStyle(SelfToken);
    if (style == nullptr)
        return;

    style->SetResourceOverride(FieldBackgroundToken, FieldBackgroundToken);
    style->SetResourceOverride(DisabledFieldBackgroundToken, DisabledFieldBackgroundToken);
    style->SetResourceOverride(FieldPaddingToken, FieldPaddingToken);
    style->SetResourceOverride(DisabledFieldPaddingToken, DisabledFieldPaddingToken);

    style->SetResourceOverride(BorderDrawToken, BorderDrawToken);
    style->SetResourceOverride(FocusedBorderDrawToken, FocusedBorderDrawToken);
    style->SetResourceOverride(DisabledBorderDrawToken, DisabledBorderDrawToken);

    style->SetResourceOverride(BorderPaddingToken, BorderPaddingToken);
    style->SetResourceOverride(FocusedBorderPaddingToken, FocusedBorderPaddingToken);
    style->SetResourceOverride(DisabledBorderPaddingToken, DisabledBorderPaddingToken);

    style->SetResourceOverride(ContentPaddingToken, ContentPaddingToken);
}

FudgetControlFieldPainter::FudgetControlFieldPainter(const SpawnParams &params) : Base(params)
{
}

void FudgetControlFieldPainter::Initialize(FudgetTheme *theme)
{
    Base::Initialize(theme);
    FudgetStyle *style = GetStyle();
    if (style == nullptr)
        return;

    if (!style->GetDrawAreaResource(theme, FieldBackgroundToken, _field_bg))
        _field_bg = FudgetDrawArea(Color::White);
    if (!style->GetDrawAreaResource(theme, DisabledFieldBackgroundToken, _disabled_field_bg))
        _disabled_field_bg = _field_bg;

    if (!style->GetPaddingResource(theme, FieldPaddingToken, _field_padding))
        _field_padding = FudgetPadding(0.0f);
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

void FudgetControlFieldPainter::Draw(FudgetControl *control)
{
    FudgetDrawArea area = !control->VirtuallyEnabled() ? _disabled_field_bg : _field_bg;
    FudgetPadding field_padding = !control->VirtuallyEnabled() ? _disabled_field_padding : _field_padding;
    control->DrawArea(area, field_padding.Padded(control->GetBounds()));

    FudgetDrawArea border = !control->VirtuallyEnabled() ? _disabled_border_area : control->VirtuallyFocused() ? _focused_border_area : _border_area;
    FudgetPadding border_padding = !control->VirtuallyEnabled() ? _disabled_border_padding : control->VirtuallyFocused() ? _focused_border_padding : _border_padding;
    control->DrawArea(border, border_padding.Padded(control->GetBounds()));
}
