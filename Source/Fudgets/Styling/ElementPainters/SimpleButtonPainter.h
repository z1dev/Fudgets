#pragma once

#include "../ElementPainter.h"



/// <summary>
/// Draws the surface of a simple button.
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetButtonBackgroundPainter : public FudgetElementPainter
{
	using Base = FudgetElementPainter;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetButtonBackgroundPainter);
public:
	FudgetButtonBackgroundPainter(String name);
	FudgetButtonBackgroundPainter(FudgetToken name_token);

	void Draw(FudgetControl *control, FudgetPainterPropertyProvider *provider) override;
};

/// <summary>
/// Draws the border of a simple button.
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetButtonBorderPainter : public FudgetElementPainter
{
	using Base = FudgetElementPainter;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetButtonBorderPainter);
public:
	FudgetButtonBorderPainter(String name);
	FudgetButtonBorderPainter(FudgetToken name_token);

	void Draw(FudgetControl *control, FudgetPainterPropertyProvider *provider) override;
};

/// <summary>
/// Draws the simple button with the help of the background and border painters.
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetSimpleButtonPainter : public FudgetElementPainter
{
	using Base = FudgetElementPainter;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetSimpleButtonPainter);
public:
	FudgetSimpleButtonPainter(String name);
	FudgetSimpleButtonPainter(FudgetToken name_token);

	void Draw(FudgetControl *control, FudgetPainterPropertyProvider *provider) override;

	static const FudgetToken simple_btn_bg_token; 
	static const FudgetToken simple_btn_border_token;
};

