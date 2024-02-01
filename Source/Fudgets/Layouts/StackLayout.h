#pragma once

#include "Layout.h"


// This is a layout with the absolute minimal implementation. See the descriptions in the .cpp file for what the
// functions do. A more complete layout is FudgetListLayout which has more complex layout calculations.
// 
// The constructor: - every layout has a constructor that takes the default SpawnParams.
// 
// Initialize() - this function is called right after the constructor. The only advantage it provides is access
//		to virtual functions in the constructed layout. Don't override, unless the new layout itself needs this.
// 
// GetCreationFlags() - Needed to initialize the layout (called from Initialize). Make sure to override this
//		function and provide all the flags that are necessary for proper layouting.
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

	FudgetLayoutFlag GetCreationFlags() const override;
};
