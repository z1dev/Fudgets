#pragma once

#include <map>

#include "../Control.h"
#include "Engine/Core/Math/Color.h"

#include "../Styling/Token.h"
#include "../Styling/ElementPainters/SimpleButtonPainter.h"


/// <summary>
/// Provides properties for a FudgetSimpleButton. Can be overriden and replaced in simple buttons, if a
/// new style requires values not available through this class.
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetSimpleButtonPropertyProvider : public FudgetPainterPropertyProvider
{
	using Base = FudgetPainterPropertyProvider;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetSimpleButtonPropertyProvider);
public:

	FudgetSimpleButtonPropertyProvider(FudgetControl *source_control);

	/// <summary>
	/// Returns the control associated with this property provider to provide properties for.
	/// </summary>
	API_PROPERTY() FudgetControl* GetSourceControl() const { return _source_control; }

	/// <inheritdoc />
	bool GetElementBoolProperty(FudgetToken token, API_PARAM(Out) bool &result) override;
private:
	// The control we provide properties about.
	FudgetControl *_source_control;
};


/// <summary>
/// Test control that fills its background with color
/// </summary>
API_CLASS()
class FUDGETS_API FudgetSimpleButton : public FudgetControl
{
	using Base = FudgetControl;
	DECLARE_SCRIPTING_TYPE(FudgetSimpleButton);
public:
	//FudgetSimpleButton();
	~FudgetSimpleButton();

	/// <summary>
	/// Gets the neutral background color used for filling the control.
	/// </summary>
	/// <returns>Background color</returns>
	API_PROPERTY() Color GetColor() const { return _color; }
	/// <summary>
	/// Sets the neutral background color used for filling the control.
	/// </summary>
	/// <returns>Background color</returns>
	API_PROPERTY() void SetColor(Color value) { _color = value; }

	/// <summary>
	/// Background color when the button is down and the mouse is positioned
	/// over its area.
	/// </summary>
	API_FIELD() Color Dark;

	/// <summary>
	/// Background color when the button is not down and the mouse is positioned
	/// over its area.
	/// </summary>
	API_FIELD() Color Light;

	/// <summary>
	/// If the button can be focused, it's the color used to draw a rectangle around it.
	/// </summary>
	API_FIELD() Color FocusColor;

	/// <summary>
	/// Gets whether the button can receive the keyboard input focus when clicked with the left mouse button.
	/// </summary>
	API_PROPERTY() bool GetCanFocus() const { return _can_focus; }

	/// <summary>
	/// Sets whether the button can receive the keyboard input focus when clicked with the left mouse button.
	/// </summary>
	/// <param name="value">Should receive focus or not</param>
	API_PROPERTY() void SetCanFocus(bool value);

	void Draw() override;

	//void OnUpdate(float delta_time) override;

	/// <inheritdoc />
	void OnMouseEnter(Float2 pos, Float2 global_pos) override;

	/// <inheritdoc />
	void OnMouseLeave() override;

	/// <inheritdoc />
	void OnMouseMove(Float2 pos, Float2 global_pos) override;

	/// <inheritdoc />
	FudgetMouseButtonResult OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click) override;

	/// <inheritdoc />
	bool OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button) override;

	/// <summary>
	/// Whether the button is pressed down by holding a mouse button over it and not releasing it.
	/// </summary>
	API_PROPERTY() bool IsPressed() { return _pressed; }
	/// <summary>
	/// Whether the button is in its down state.
	/// </summary>
	API_PROPERTY() bool IsDown() { return _down; }
	/// <summary>
	/// Whether the mouse pointer is over the button
	/// </summary>
	API_PROPERTY() bool IsHovered() { return _over; }
private:
	Color _color;

	bool _pressed;
	bool _down;
	bool _over;

	bool _can_focus;
};
