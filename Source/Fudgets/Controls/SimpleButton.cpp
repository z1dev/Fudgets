#include "SimpleButton.h"
#include "../GUIRoot.h"
#include "../Styling/Theme.h"



FudgetSimpleButtonPropertyProvider::FudgetSimpleButtonPropertyProvider(FudgetControl *_source_control) : Base(_source_control)
{

}

bool FudgetSimpleButtonPropertyProvider::GetElementBoolProperty(FudgetToken token, /*API_PARAM(Out)*/ bool &result)
{
	if (token == FudgetTheme::MouseHoverToken)
	{
		result = ((FudgetSimpleButton*)_source_control)->IsHovered();
		return true;
	}
	if (token == FudgetTheme::LeftButtonPressedToken)
	{
		result = ((FudgetSimpleButton*)_source_control)->IsPressed();
		return true;
	}
	if (token == FudgetTheme::ButtonDownToken)
	{
		result = ((FudgetSimpleButton*)_source_control)->IsDown();
		return true;
	}
}

bool FudgetSimpleButtonPropertyProvider::GetStoredFloat(FudgetToken token, /*API_PARAM(Out)*/ float &result) const
{
	auto it = _floats.find(token);
	if (it == _floats.end())
		return false;
	result = it->second;
	return true;
}

void FudgetSimpleButtonPropertyProvider::SetStoredFloat(FudgetToken token, float value)
{
	_floats[token] = value;
}




FudgetSimpleButton::FudgetSimpleButton(const SpawnParams &params) : FudgetControl(params, 
	FudgetControlFlags::CanHandleMouseMove | FudgetControlFlags::CanHandleMouseEnterLeave | FudgetControlFlags::CanHandleMouseUpDown |
	FudgetControlFlags::CaptureReleaseMouseLeft | FudgetControlFlags::RegisterToUpdates),
	Dark(0.6f), Light(1.f), FocusColor(0.4f, 0.6f, 0.8f, 1.0f), _color(0.9f, 0.9f, 0.9f, 1.0f), /*_delta_time(0.f), */_pressed(false), _down(false),
	_over(false), _can_focus(false)
{
	if (GetGUIRoot() != nullptr && GetGUIRoot()->GetTheme() != nullptr)
		buttonToken = GetGUIRoot()->GetTheme()->RegisterToken(TEXT("SimpleButton"), false);
}

FudgetSimpleButton::~FudgetSimpleButton()
{
}

void FudgetSimpleButton::SetPropertyProvider(FudgetPainterPropertyProvider *new_provider)
{
	_painter_provider.reset(new_provider);
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
	FudgetElementPainter* painter = GetElementPainter(buttonToken);
	if (painter != nullptr)
	{
		if (_painter_provider == nullptr)
			_painter_provider.reset(New<FudgetSimpleButtonPropertyProvider>(this));
		painter->Draw(_painter_provider);
	}

	// TODO: add an element painter that draws the ERROR text over controls that get their token wrong.

	Base::Draw();
}

void FudgetSimpleButton::OnUpdate(float delta_time)
{
	Base::OnUpdate(delta_time);
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


