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
FudgetToken FudgetFramedFieldPainter::HoveredFieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::PressedFieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::DownFieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::DisabledFieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::FocusedFieldBackgroundToken;
FudgetToken FudgetFramedFieldPainter::FieldPaddingToken;
FudgetToken FudgetFramedFieldPainter::HoveredFieldPaddingToken;
FudgetToken FudgetFramedFieldPainter::PressedFieldPaddingToken;
FudgetToken FudgetFramedFieldPainter::DownFieldPaddingToken;
FudgetToken FudgetFramedFieldPainter::DisabledFieldPaddingToken;
FudgetToken FudgetFramedFieldPainter::FocusedFieldPaddingToken;

FudgetToken FudgetFramedFieldPainter::FrameDrawToken;
FudgetToken FudgetFramedFieldPainter::HoveredFrameDrawToken;
FudgetToken FudgetFramedFieldPainter::PressedFrameDrawToken;
FudgetToken FudgetFramedFieldPainter::DownFrameDrawToken;
FudgetToken FudgetFramedFieldPainter::FocusedFrameDrawToken;
FudgetToken FudgetFramedFieldPainter::DisabledFrameDrawToken;
FudgetToken FudgetFramedFieldPainter::FramePaddingToken;
FudgetToken FudgetFramedFieldPainter::HoveredFramePaddingToken;
FudgetToken FudgetFramedFieldPainter::PressedFramePaddingToken;
FudgetToken FudgetFramedFieldPainter::DownFramePaddingToken;
FudgetToken FudgetFramedFieldPainter::FocusedFramePaddingToken;
FudgetToken FudgetFramedFieldPainter::DisabledFramePaddingToken;

FudgetToken FudgetFramedFieldPainter::ContentPaddingToken;


/*static*/
void FudgetFramedFieldPainter::CreateStyle()
{
    SelfToken = FudgetThemes::RegisterToken(TEXT("Fudgets.FudgetFramedFieldPainter"));

    FieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FieldBackground"));
    HoveredFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_HoveredFieldBackground"));
    FocusedFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedFieldBackground"));
    PressedFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_PressedFieldBackground"));
    DownFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DownFieldBackground"));
    DisabledFieldBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledFieldBackground"));
    FieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FieldPadding"));
    HoveredFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_HoveredFieldPadding"));
    PressedFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_PressedFieldPadding"));
    DownFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DownFieldPadding"));
    FocusedFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedFieldPadding"));
    DisabledFieldPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledFieldPadding"));

    FrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_frameDraw"));
    HoveredFrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_HoveredFrameDraw"));
    PressedFrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_PressedFrameDraw"));
    DownFrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DownFrameDraw"));
    FocusedFrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedFrameDraw"));
    DisabledFrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledFrameDraw"));

    FramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_framePadding"));
    HoveredFramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_HoveredFramePadding"));
    PressedFramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_PressedFramePadding"));
    DownFramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DownFramePadding"));
    FocusedFramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_FocusedFramePadding"));
    DisabledFramePaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_DisabledFramePadding"));

    ContentPaddingToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetFramedFieldPainter_ContentPadding"));



    FudgetStyle *style = FudgetThemes::CreateStyle(SelfToken);
    if (style == nullptr)
        return;

    style->SetResourceOverride(FieldBackgroundToken, FieldBackgroundToken);
    style->SetResourceOverride(HoveredFieldBackgroundToken, HoveredFieldBackgroundToken);
    style->SetResourceOverride(PressedFieldBackgroundToken, PressedFieldBackgroundToken);
    style->SetResourceOverride(DownFieldBackgroundToken, DownFieldBackgroundToken);
    style->SetResourceOverride(FocusedFieldBackgroundToken, FocusedFieldBackgroundToken);
    style->SetResourceOverride(DisabledFieldBackgroundToken, DisabledFieldBackgroundToken);
    style->SetResourceOverride(FieldPaddingToken, FieldPaddingToken);
    style->SetResourceOverride(HoveredFieldPaddingToken, HoveredFieldPaddingToken);
    style->SetResourceOverride(PressedFieldPaddingToken, PressedFieldPaddingToken);
    style->SetResourceOverride(DownFieldPaddingToken, DownFieldPaddingToken);
    style->SetResourceOverride(FocusedFieldPaddingToken, FocusedFieldPaddingToken);
    style->SetResourceOverride(DisabledFieldPaddingToken, DisabledFieldPaddingToken);

    style->SetResourceOverride(FrameDrawToken, FrameDrawToken);
    style->SetResourceOverride(HoveredFrameDrawToken, HoveredFrameDrawToken);
    style->SetResourceOverride(PressedFrameDrawToken, PressedFrameDrawToken);
    style->SetResourceOverride(DownFrameDrawToken, DownFrameDrawToken);
    style->SetResourceOverride(FocusedFrameDrawToken, FocusedFrameDrawToken);
    style->SetResourceOverride(DisabledFrameDrawToken, DisabledFrameDrawToken);

    style->SetResourceOverride(FramePaddingToken, FramePaddingToken);
    style->SetResourceOverride(HoveredFramePaddingToken, HoveredFramePaddingToken);
    style->SetResourceOverride(PressedFramePaddingToken, PressedFramePaddingToken);
    style->SetResourceOverride(DownFramePaddingToken, DownFramePaddingToken);
    style->SetResourceOverride(FocusedFramePaddingToken, FocusedFramePaddingToken);
    style->SetResourceOverride(DisabledFramePaddingToken, DisabledFramePaddingToken);

    style->SetResourceOverride(ContentPaddingToken, ContentPaddingToken);
}

FudgetFramedFieldPainter::FudgetFramedFieldPainter(const SpawnParams &params) : Base(params), _state(FudgetFramedFieldState::Normal)
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
    if (!style->GetDrawAreaResource(theme, HoveredFieldBackgroundToken, _hovered_field_bg))
        _hovered_field_bg = _field_bg;
    if (!style->GetDrawAreaResource(theme, PressedFieldBackgroundToken, _pressed_field_bg))
        _pressed_field_bg = _field_bg;
    if (!style->GetDrawAreaResource(theme, DownFieldBackgroundToken, _down_field_bg))
        _down_field_bg = _pressed_field_bg;
    if (!style->GetDrawAreaResource(theme, FocusedFieldBackgroundToken, _focused_field_bg))
        _focused_field_bg = _field_bg;
    if (!style->GetDrawAreaResource(theme, DisabledFieldBackgroundToken, _disabled_field_bg))
        _disabled_field_bg = _field_bg;

    if (!style->GetPaddingResource(theme, FieldPaddingToken, _field_padding))
        _field_padding = FudgetPadding(0.0f);
    if (!style->GetPaddingResource(theme, HoveredFieldPaddingToken, _hovered_field_padding))
        _hovered_field_padding = _field_padding;
    if (!style->GetPaddingResource(theme, PressedFieldPaddingToken, _pressed_field_padding))
        _pressed_field_padding = _field_padding;
    if (!style->GetPaddingResource(theme, DownFieldPaddingToken, _down_field_padding))
        _down_field_padding = _pressed_field_padding;
    if (!style->GetPaddingResource(theme, FocusedFieldPaddingToken, _focused_field_padding))
        _focused_field_padding = _field_padding;
    if (!style->GetPaddingResource(theme, DisabledFieldPaddingToken, _disabled_field_padding))
        _disabled_field_padding = _field_padding;

    if (!style->GetDrawAreaResource(theme, FrameDrawToken, _frame_area))
        _frame_area = FudgetDrawArea(FudgetPadding( Math::Max(1.0f, _field_padding.Left), Math::Max(1.0f, _field_padding.Top),
            Math::Max(1.0f, _field_padding.Right), Math::Max(1.0f, _field_padding.Bottom)), Color::Gray, FudgetFrameType::Inside);
    if (!style->GetDrawAreaResource(theme, HoveredFrameDrawToken, _hovered_frame_area))
        _hovered_frame_area = _frame_area;
    if (!style->GetDrawAreaResource(theme, PressedFrameDrawToken, _pressed_frame_area))
        _pressed_frame_area = _frame_area;
    if (!style->GetDrawAreaResource(theme, DownFrameDrawToken, _down_frame_area))
        _down_frame_area = _pressed_frame_area;
    if (!style->GetDrawAreaResource(theme, FocusedFrameDrawToken, _focused_frame_area))
        _focused_frame_area = _frame_area;
    if (!style->GetDrawAreaResource(theme, DisabledFrameDrawToken, _disabled_frame_area))
        _disabled_frame_area = _frame_area;

    if (!style->GetPaddingResource(theme, FramePaddingToken, _frame_padding))
        _frame_padding = FudgetPadding(0.0f);
    if (!style->GetPaddingResource(theme, HoveredFramePaddingToken, _hovered_frame_padding))
        _hovered_frame_padding = _frame_padding;
    if (!style->GetPaddingResource(theme, PressedFramePaddingToken, _pressed_frame_padding))
        _pressed_frame_padding = _frame_padding;
    if (!style->GetPaddingResource(theme, DownFramePaddingToken, _down_frame_padding))
        _down_frame_padding = _pressed_frame_padding;
    if (!style->GetPaddingResource(theme, FocusedFramePaddingToken, _focused_frame_padding))
        _focused_frame_padding = _frame_padding;
    if (!style->GetPaddingResource(theme, DisabledFramePaddingToken, _disabled_frame_padding))
        _disabled_frame_padding = _frame_padding;

    if (!style->GetPaddingResource(theme, ContentPaddingToken, _inner_padding))
        _inner_padding = FudgetPadding(0.0f);
}

void FudgetFramedFieldPainter::Draw(FudgetControl *control)
{
    FudgetDrawArea area = !GetEnabled() ? _disabled_field_bg :
        GetDown() ? _down_field_bg :
        GetPressed() ? _pressed_field_bg :
        GetFocused() ? _focused_field_bg :
        GetHovered() ? _hovered_field_bg :
        _field_bg;
    FudgetPadding field_padding = !GetEnabled() ? _disabled_field_padding :
        GetDown() ? _down_field_padding :
        GetPressed() ? _pressed_field_padding :
        GetFocused() ? _focused_field_padding :
        GetHovered() ? _hovered_field_padding :
        _field_padding;

    control->DrawArea(area, field_padding.Padded(control->GetBounds()));

    FudgetDrawArea frame = !GetEnabled() ? _disabled_frame_area :
        GetDown() ? _down_frame_area :
        GetPressed() ? _pressed_frame_area :
        GetFocused() ? _focused_frame_area :
        GetHovered() ? _hovered_frame_area :
        _frame_area;
    FudgetPadding frame_padding = !GetEnabled() ? _disabled_frame_padding :
        GetDown() ? _down_frame_padding :
        GetPressed() ? _pressed_frame_padding :
        GetFocused() ? _focused_frame_padding :
        GetHovered() ? _hovered_frame_padding :
        _frame_padding;
    control->DrawArea(frame, frame_padding.Padded(control->GetBounds()));
}

void FudgetFramedFieldPainter::SetState(FudgetFramedFieldState value, bool set)
{
    if (set)
        _state |= value;
    else
        _state &= ~value;
}
