#pragma once

#include "Layout.h"

// This is a layout with the absolute minimal implementation. See the descriptions in the .cpp
// file for what these functions do. A more complicated layout is FudgetListLayout, with an example
// on how to store values for each control in slots, that are specific to that layout.
// 
// LayoutChildren(...) - calculates the position and size of controls, relative to the parent
//		container's origin.
// RequestSize(...) - uses the child controls to calculate the min, max and hint sizes.
// CreateSlot(...) - Creates an object that's derived from FudgetLayoutSlot (or this class itself)
//      to store custom data for the layout. This simple example layout doesn't implement it. Check
//		FudgetListLayout with some description.





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
