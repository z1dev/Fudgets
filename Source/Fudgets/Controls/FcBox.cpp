#include "FcBox.h"
#include "Engine/Render2D/Render2D.h"
#include "Engine/Core/Math/Rectangle.h"


FcBox::FcBox() : FcControl(), _color(1.0f)
{
}

void FcBox::Draw()
{
	Render2D::DrawRectangle(Rectangle(Float2::Zero, GetSize()), _color);
}

