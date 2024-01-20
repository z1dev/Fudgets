#pragma once

#include "Layout.h"


// This is a layout with the absolute minimal implementation. See the descriptions in the .cpp
// file for what the functions do. A more complicated layout is FudgetListLayout, but it's too complicated for
// a complete explanation.
// 
// The constructor: - every layout has a constructor that takes the default SpawnParams, and a flags variable.
//		the flags are necessary in the layouting step. They decide when recalculation is necessary.
//		See the FudgetLayoutFlag enum in Layout.h
// 
// LayoutChildren(...) - calculates the position and size of controls, relative to the parent container's top-left
//		origin.
// RequestSize(...) - uses the child controls to calculate the min, max and hint sizes.
// CreateSlot(...) - Creates an object that's derived from FudgetLayoutSlot to store custom data for the layout.
//		This simple example creates the default object.


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
	Float2 RequestSize(FudgetSizeType type) const override;

	/// <inheritdoc />
	FudgetLayoutSlot* CreateSlot(FudgetControl *control) override;
};
