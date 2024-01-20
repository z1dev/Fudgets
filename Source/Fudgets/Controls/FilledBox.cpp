#include "FilledBox.h"

FudgetFilledBox::FudgetFilledBox(const SpawnParams &params) : FudgetControl(params, FudgetControlFlags::CanHandleMouseMove), _color(1.0f)
{
}

FudgetFilledBox::~FudgetFilledBox()
{
}

void FudgetFilledBox::Draw()
{
	FillRectangle(Float2(0.f), GetSize(), _color);

	//Render2D::FillRectangle(Rectangle(GetPosition(), GetSize()), _color);
}

