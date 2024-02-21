#pragma once

#include "Layout.h"


/// <summary>
/// A layout that places all its child controls on top of each other, making them fill the available
/// space. Currently ignores the maximum sizes.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetStackLayout : public FudgetLayout
{
	using Base = FudgetLayout;
	DECLARE_SCRIPTING_TYPE(FudgetStackLayout);

protected:
	/// <inheritdoc />
	bool LayoutChildren() override;

	/// <inheritdoc />
	bool Measure(Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size) override;

	/// <inheritdoc />
	FudgetLayoutFlag GetInitFlags() const override;
private:
	bool RepeatMeasure(Float2 available);
};
