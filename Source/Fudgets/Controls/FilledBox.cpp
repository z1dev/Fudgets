#include "FcFilledBox.h"
#include "Engine/Render2D/Render2D.h"
#include "Engine/Core/Math/Rectangle.h"


FcFilledBox::FcFilledBox() : FcControl(), _color(1.0f)
{
}

void FcFilledBox::Draw()
{
	Render2D::FillRectangle(Rectangle(Float2::Zero, GetSize()), _color);
}

