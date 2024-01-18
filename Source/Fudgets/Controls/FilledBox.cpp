#include "FilledBox.h"
#include "Engine/Render2D/Render2D.h"
#include "Engine/Core/Math/Rectangle.h"
#include "Engine/Core/Log.h"

FudgetFilledBox::FudgetFilledBox(const SpawnParams &params) : FudgetControl(params), _color(1.0f)
{
}

FudgetFilledBox::~FudgetFilledBox()
{
}

void FudgetFilledBox::Draw()
{
	Render2D::FillRectangle(Rectangle(GetPosition(), GetSize()), _color);
}

