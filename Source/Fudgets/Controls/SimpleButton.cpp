#include "SimpleButton.h"
#include "../GUIRoot.h"
#include "../Styling/Themes.h"



FudgetSimpleButtonPropertyProvider::FudgetSimpleButtonPropertyProvider(FudgetControl *source_control) : Base(SpawnParams(Guid::New(), TypeInitializer)), _source_control(source_control)
{

}

bool FudgetSimpleButtonPropertyProvider::GetElementBoolProperty(FudgetToken token, API_PARAM(Out) bool &result)
{
	if (token == FudgetThemes::MouseHoverToken)
	{
		result = ((FudgetSimpleButton*)_source_control)->IsHovered();
		return true;
	}
	if (token == FudgetThemes::LeftButtonPressedToken)
	{
		result = ((FudgetSimpleButton*)_source_control)->IsPressed();
		return true;
	}
	if (token == FudgetThemes::ButtonDownToken)
	{
		result = ((FudgetSimpleButton*)_source_control)->IsDown();
		return true;
	}

	return false;
}



FudgetSimpleButton::FudgetSimpleButton(const SpawnParams &params) : FudgetControl(params),
	Dark(0.6f), Light(1.f), FocusColor(0.4f, 0.6f, 0.8f, 1.0f), _color(0.9f, 0.9f, 0.9f, 1.0f), _pressed(false), _down(false),
	_over(false), _can_focus(false)
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
	auto painter = GetElementPainter();
	if (painter != nullptr)
	{
		if (GetPainterPropertyProvider() == nullptr)
			SetPainterPropertyProvider(New<FudgetSimpleButtonPropertyProvider>(this));
		painter->Draw(this, GetPainterPropertyProvider());
	}

	// TODO: add an element painter that draws the ERROR text over controls that get their token wrong.

	Base::Draw();
}

//void FudgetSimpleButton::OnUpdate(float delta_time)
//{
//	Base::OnUpdate(delta_time);
//}

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

	_pressed = true;
	_over = true;

	return FudgetMouseButtonResult::Consume;
}

bool FudgetSimpleButton::OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
	if (button != MouseButton::Left)
		return true;

	_pressed = false;

	return true;
}

FudgetControlFlags FudgetSimpleButton::GetCreationFlags() const
{
	return FudgetControlFlags::CanHandleMouseMove | FudgetControlFlags::CanHandleMouseEnterLeave | FudgetControlFlags::CanHandleMouseUpDown |
		FudgetControlFlags::CaptureReleaseMouseLeft | FudgetControlFlags::RegisterToUpdates | Base::GetCreationFlags();
}

