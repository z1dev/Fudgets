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

void FudgetPartPainter::Initialize(FudgetTheme *theme, FudgetStyle *style)
{
    _theme = theme;
    _style = style;
}

FudgetStyle* FudgetPartPainter::GetDefaultStyle() const
{
    return FudgetThemes::GetStyle(GetType().Fullname.ToString());
}


// FudgetFramedFieldPainter


FudgetToken FudgetFramedFieldPainter::SelfToken;

FudgetToken FudgetFramedFieldPainter::FieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::DisabledFieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::FocusedFieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::FieldPaddingToken;
FudgetToken FudgetFramedFieldPainter::DisabledFieldPaddingToken;
FudgetToken FudgetFramedFieldPainter::FocusedFieldPaddingToken;

FudgetToken FudgetFramedFieldPainter::FrameDrawToken;
FudgetToken FudgetFramedFieldPainter::FocusedFrameDrawToken;
FudgetToken FudgetFramedFieldPainter::DisabledFrameDrawToken;
FudgetToken FudgetFramedFieldPainter::FramePaddingToken;
FudgetToken FudgetFramedFieldPainter::FocusedFramePaddingToken;
FudgetToken FudgetFramedFieldPainter::DisabledFramePaddingToken;

FudgetToken FudgetFramedFieldPainter::ContentPaddingToken;


/*static*/
void FudgetFramedFieldPainter::CreateStyle()
{
    SelfToken = FudgetThemes::RegisterToken(TEXT("Fudgets.FudgetFramedFieldPainter"));

    FieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FieldBackground"));
    FocusedFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedFieldBackground"));
    DisabledFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledFieldBackground"));
    FieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FieldPadding"));
    FocusedFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedFieldPadding"));
    DisabledFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledFieldPadding"));

    FrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FrameDraw"));
    FocusedFrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedFrameDraw"));
    DisabledFrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledFrameDraw"));

    FramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FramePadding"));
    FocusedFramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedFramePadding"));
    DisabledFramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledFramePadding"));

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

    style->SetResourceOverride(FrameDrawToken, FrameDrawToken);
    style->SetResourceOverride(FocusedFrameDrawToken, FocusedFrameDrawToken);
    style->SetResourceOverride(DisabledFrameDrawToken, DisabledFrameDrawToken);

    style->SetResourceOverride(FramePaddingToken, FramePaddingToken);
    style->SetResourceOverride(FocusedFramePaddingToken, FocusedFramePaddingToken);
    style->SetResourceOverride(DisabledFramePaddingToken, DisabledFramePaddingToken);

    style->SetResourceOverride(ContentPaddingToken, ContentPaddingToken);
}

FudgetFramedFieldPainter::FudgetFramedFieldPainter(const SpawnParams &params) : Base(params)
{
}

void FudgetFramedFieldPainter::Initialize(FudgetTheme *theme, FudgetStyle *style)
{
    Base::Initialize(theme, style);
    if (style == nullptr)
    {
        style = GetStyle();
        if (style == nullptr)
            return;
    }

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

    if (!style->GetDrawAreaResource(theme, FrameDrawToken, _Frame_area))
        _Frame_area = FudgetDrawArea(FudgetPadding( Math::Max(1.0f, _field_padding.Left), Math::Max(1.0f, _field_padding.Top),
            Math::Max(1.0f, _field_padding.Right), Math::Max(1.0f, _field_padding.Bottom)), Color::Gray, FudgetFrameType::Inside);
    if (!style->GetDrawAreaResource(theme, FocusedFrameDrawToken, _focused_Frame_area))
        _focused_Frame_area = _Frame_area;
    if (!style->GetDrawAreaResource(theme, DisabledFrameDrawToken, _disabled_Frame_area))
        _disabled_Frame_area = _Frame_area;

    if (!style->GetPaddingResource(theme, FramePaddingToken, _Frame_padding))
        _Frame_padding = FudgetPadding(0.0f);
    if (!style->GetPaddingResource(theme, FocusedFramePaddingToken, _focused_Frame_padding))
        _focused_Frame_padding = _Frame_padding;
    if (!style->GetPaddingResource(theme, DisabledFramePaddingToken, _disabled_Frame_padding))
        _disabled_Frame_padding = _Frame_padding;

    if (!style->GetPaddingResource(theme, ContentPaddingToken, _inner_padding))
        _inner_padding = FudgetPadding(0.0f);
}

void FudgetFramedFieldPainter::Draw(FudgetControl *control)
{
    FudgetDrawArea area = !control->VirtuallyEnabled() ? _disabled_field_bg : control->VirtuallyFocused() ? _focused_field_bg : _field_bg;
    FudgetPadding field_padding = !control->VirtuallyEnabled() ? _disabled_field_padding : control->VirtuallyFocused() ? _focused_field_padding : _field_padding;
    control->DrawArea(area, field_padding.Padded(control->GetBounds()));

    FudgetDrawArea Frame = !control->VirtuallyEnabled() ? _disabled_Frame_area : control->VirtuallyFocused() ? _focused_Frame_area : _Frame_area;
    FudgetPadding Frame_padding = !control->VirtuallyEnabled() ? _disabled_Frame_padding : control->VirtuallyFocused() ? _focused_Frame_padding : _Frame_padding;
    control->DrawArea(Frame, Frame_padding.Padded(control->GetBounds()));
}
