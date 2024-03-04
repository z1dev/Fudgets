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
	API_AUTO_SERIALIZATION();

protected:
	/// <inheritdoc />
	void LayoutChildren(Float2 space, FudgetContainer *owner, int count) override;

	/// <inheritdoc />
	FudgetLayoutFlag GetInitFlags() const override;
};
