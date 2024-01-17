#include "FilledBox.h"
#include "Engine/Render2D/Render2D.h"
#include "Engine/Core/Math/Rectangle.h"


FudgetFilledBox::FudgetFilledBox() : FudgetControl(), _color(1.0f)
{
}

void FudgetFilledBox::Draw()
{
	Render2D::FillRectangle(Rectangle(GetPosition(), GetSize()), _color);
}

