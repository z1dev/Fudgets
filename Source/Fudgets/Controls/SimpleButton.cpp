#include "SimpleButton.h"
#include "../GUIRoot.h"

FudgetSimpleButton::FudgetSimpleButton(const SpawnParams &params) : FudgetControl(params, 
	FudgetControlFlags::CanHandleMouseMove | FudgetControlFlags::CanHandleMouseEnterLeave | FudgetControlFlags::CanHandleMouseUpDown | FudgetControlFlags::CaptureReleaseMouseLeft),
	Dark(0.6f), Light(1.f), FocusColor(0.4f, 0.6f, 0.8f, 1.0f), _color(0.9f), _down(false), _over(false), _can_focus(false)
{
}

FudgetSimpleButton::~FudgetSimpleButton()
{
}

void FudgetSimpleButton::SetCanFocus(bool value)
{
	if (value)
		SetControlFlags(GetControlFlags() | FudgetControlFlags::FocusOnMouseLeft);
	else
		SetControlFlags(GetControlFlags() & ~FudgetControlFlags::FocusOnMouseLeft);
	_can_focus = value;
}

void FudgetSimpleButton::Draw()
{
	FillRectangle(Float2(0.f), GetSize(), _over && _down ? Dark : _over ? Light : _color);

	if (_can_focus && GetFocused())
		DrawRectangle(Float2(0.f), GetSize(), FocusColor);
	//Render2D::FillRectangle(Rectangle(GetPosition(), GetSize()), _color);
}

void FudgetSimpleButton::OnMouseEnter(Float2 pos, Float2 global_pos)
{
	_over = true;
}

void FudgetSimpleButton::OnMouseLeave()
{
	_over = false;
}

void FudgetSimpleButton::OnMouseMove(Float2 pos, Float2 global_pos)
{
	_over = pos.X >= 0 && pos.Y >= 0 && pos.X < GetSize().X && pos.Y < GetSize().Y;
}

FudgetMouseButtonResult FudgetSimpleButton::OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
	if (button != MouseButton::Left)
		return FudgetMouseButtonResult::Ignore;
	_down = true;
	_over = true;

	//CaptureMouseInput();

	return FudgetMouseButtonResult::Consume;
}

bool FudgetSimpleButton::OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
	if (button != MouseButton::Left)
		return true;

	//if (_down)
	//	ReleaseMouseInput();

	_down = false;

	return true;
}
