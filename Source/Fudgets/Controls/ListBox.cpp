#include "ListBox.h"
#include "../Styling/Themes.h"


FudgetToken FudgetListBox::FramePainterToken;
FudgetToken FudgetListBox::FrameStyleToken;

void FudgetListBox::InitializeTokens()
{
    FramePainterToken = FudgetThemes::RegisterToken(TEXT("Fudgets_ListBoxBase_FramePainter"));
    FrameStyleToken = FudgetThemes::RegisterToken(TEXT("Fudgets_ListBoxBase_FrameStyle"));
}

FudgetListBox::FudgetListBox(const SpawnParams &params) : Base(params), _frame_painter(nullptr)
{

}

void FudgetListBox::OnInitialize()
{
    FudgetToken frame_style;
    if (!GetStyleToken(FrameStyleToken, frame_style))
        frame_style = FudgetToken::Invalid;

    _frame_painter = CreateStylePainter<FudgetFramedFieldPainter>(FramePainterToken, frame_style);
}

void FudgetListBox::OnDraw()
{
    Rectangle bounds = GetBounds();
    if (_frame_painter != nullptr)
        _frame_painter->Draw(this, bounds, GetVisualState());
}
