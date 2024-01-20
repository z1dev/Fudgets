#include "SimpleButton.h"
#include "../GUIRoot.h"

FudgetSimpleButton::FudgetSimpleButton(const SpawnParams &params) : FudgetControl(params, 
	FudgetControlFlags::CanHandleMouseMove | FudgetControlFlags::CanHandleMouseEnterLeave | FudgetControlFlags::CanHandleMouseUpDown | FudgetControlFlags::ConvertDoubleClickToSingle),
	Dark(0.6f), Light(1.f), _color(0.9f), _down(false), _over(false)
{
}

FudgetSimpleButton::~FudgetSimpleButton()
{
}

void FudgetSimpleButton::Draw()
{
	FillRectangle(Float2(0.f), GetSize(), _over && _down ? Dark : _over ? Light : _color);

	//Render2D::FillRectangle(Rectangle(GetPosition(), GetSize()), _color);
}

void FudgetSimpleButton::OnMouseEnter(Float2 pos)
{
	_over = true;
}

void FudgetSimpleButton::OnMouseLeave()
{
	_over = false;
}

void FudgetSimpleButton::OnMouseMove(Float2 pos)
{
	_over = pos.X >= 0 && pos.Y >= 0 && pos.X < GetSize().X && pos.Y < GetSize().Y;
}

bool FudgetSimpleButton::OnMouseDown(Float2 pos, MouseButton button)
{
	if (button != MouseButton::Left)
		return true;
	_down = true;
	_over = true;

	GetGUIRoot()->StartMouseCapture(this);

	return true;
}

bool FudgetSimpleButton::OnMouseUp(Float2 pos, MouseButton button)
{
	if (button != MouseButton::Left)
		return true;

	if (_down)
		GetGUIRoot()->ReleaseMouseCapture();

	_down = false;

	return true;
}
