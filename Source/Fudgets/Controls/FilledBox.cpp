#include "FilledBox.h"

FudgetFilledBox::FudgetFilledBox(const SpawnParams &params) : Base(params), DrawBorder(false), BorderColor(0.5f, 0.5f, 0.5f, 1.0f), _color(1.0f)
{
}

FudgetFilledBox::~FudgetFilledBox()
{
}

void FudgetFilledBox::Draw()
{
	FillRectangle(Float2(0.f), GetSize(), _color);

	if (DrawBorder)
		DrawRectangle(Float2(0.f), GetSize(), BorderColor);

	//Render2D::FillRectangle(Rectangle(GetPosition(), GetSize()), _color);
}

