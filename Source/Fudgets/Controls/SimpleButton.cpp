#include "SimpleButton.h"
#include "../GUIRoot.h"
#include "../Styling/Theme.h"


FudgetSimpleButton::FudgetSimpleButton(const SpawnParams &params) : FudgetControl(params, 
	FudgetControlFlags::CanHandleMouseMove | FudgetControlFlags::CanHandleMouseEnterLeave | FudgetControlFlags::CanHandleMouseUpDown |
	FudgetControlFlags::CaptureReleaseMouseLeft | FudgetControlFlags::RegisterToUpdates),
	Dark(0.6f), Light(1.f), FocusColor(0.4f, 0.6f, 0.8f, 1.0f), _color(0.9f, 0.9f, 0.9f, 1.0f), /*_down(false), _over(false), */_can_focus(false)
{
	buttonToken = GetGUIRoot()->GetTheme()->RegisterToken(TEXT("SimpleButton"), false);
	_button_info.DeltaTime = 0.0f;
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
	FudgetSimpleButtonPainter* drawer = GetElementPainter<FudgetSimpleButtonPainter>(buttonToken);
	if (drawer != nullptr)
		drawer->Draw(this, _button_info);

	// TODO: add an element drawer that draws the ERROR text over controls that get their token wrong.
}

void FudgetSimpleButton::OnUpdate(float delta_time)
{
	_button_info.DeltaTime += delta_time;
}

void FudgetSimpleButton::OnMouseEnter(Float2 pos, Float2 global_pos)
{
	_button_info.MouseIsOver = true;
}

void FudgetSimpleButton::OnMouseLeave()
{
	_button_info.MouseIsOver = false;
}

void FudgetSimpleButton::OnMouseMove(Float2 pos, Float2 global_pos)
{
	_button_info.MouseIsOver = pos.X >= 0 && pos.Y >= 0 && pos.X < GetSize().X && pos.Y < GetSize().Y;
}

FudgetMouseButtonResult FudgetSimpleButton::OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
	if (button != MouseButton::Left)
		return FudgetMouseButtonResult::Ignore;
	_button_info.MousePressed = true;
	_button_info.MouseIsOver = true;

	//CaptureMouseInput();

	return FudgetMouseButtonResult::Consume;
}

bool FudgetSimpleButton::OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
	if (button != MouseButton::Left)
		return true;

	//if (_down)
	//	ReleaseMouseInput();

	_button_info.MousePressed = false;

	return true;
}
