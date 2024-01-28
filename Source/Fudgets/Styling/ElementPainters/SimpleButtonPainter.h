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
	FudgetButtonBackgroundPainter();

	void Draw(FudgetControl *control, FudgetPainterPropertyProvider *provider) override;

	// Token used to save and fetch the custom value for hover time for animations.
	static const FudgetToken hover_token;
};



/// <summary>
/// Draws the surface of a simple button.
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetSimpleButtonPainter : public FudgetElementPainter
{
	using Base = FudgetElementPainter;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetSimpleButtonPainter);
public:
	FudgetSimpleButtonPainter();

	void Draw(FudgetControl *control, FudgetPainterPropertyProvider *provider) override;

	static const FudgetToken simple_btn_bg_token;
};

