#pragma once

#include "../ElementPainter.h"

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

	void Draw(FudgetPainterPropertyProvider *provider) override;

	// Token used to fetch the hover time for animations.
	static const FudgetToken hover_token;
};

