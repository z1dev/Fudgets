#pragma once

#include "../ElementPainter.h"

API_STRUCT()
struct FUDGETS_API FudgetSimpleButtonDrawInfo
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetSimpleButtonDrawInfo);

	/// <summary>
	/// The mouse pointer is hovering above the button or control that wants to draw the button
	/// </summary>
	API_FIELD() bool MouseIsOver = false;
	/// <summary>
	/// The left mouse button was pressed over the button when it was over it.
	/// </summary>
	API_FIELD() bool MousePressed = false;
	/// <summary>
	/// The button is in the down state. Used for toggle buttons.
	/// </summary>
	API_FIELD() bool Down = false;

	/// <summary>
	/// This value should be increased on OnUpdatefor animation. Set to -1 to never animate.
	/// TODO: move this outside to be a general field on controls that have an animation flag set.
	/// </summary>
	API_FIELD() float DeltaTime = -1.0f;


	// Internal. Used by the drawing functions to keep track of what to animate.

	/// <summary>
	/// The time in seconds since the mouse pointer moved over the button and has not left it.
	/// </summary>
	API_FIELD() float _hover_time = 0.0f;

};

API_CLASS(NoSpawn)
class FUDGETS_API FudgetSimpleButtonPainter : public FudgetElementPainter
{
	using Base = FudgetElementPainter;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetSimpleButtonPainter);
public:
	FudgetSimpleButtonPainter();

	void Draw(FudgetControl *control, FudgetSimpleButtonDrawInfo &info);
};

