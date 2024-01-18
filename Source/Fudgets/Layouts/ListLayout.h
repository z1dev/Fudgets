#pragma once

#include "Layout.h"
#include "../MarginStructs.h"


/// <summary>
/// Derived slot holding attributes necessary for layouting for the list layout.
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetListLayoutSlot : public FudgetLayoutSlot
{
	using Base = FudgetLayoutSlot;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetListLayoutSlot);
	
	FudgetListLayoutSlot(FudgetControl *control);

	FudgetAlignHorizontal _horz_align;
	FudgetAlignVertical _vert_align;

	FudgetSlotPadding _padding;
};

/// <summary>
/// A layout that aligns the owner container's children horizontally or vertically
/// </summary>
API_CLASS()
class FUDGETS_API FudgetListLayout : public FudgetLayout
{
	using Base = FudgetLayout;
	DECLARE_SCRIPTING_TYPE(FudgetListLayout);
public:
	//FudgetListLayout(const SpawnParams &params);
	FudgetListLayout(const SpawnParams &params, FudgetOrientation orientation);

	/// <summary>
	/// Gets the layout orientation of controls on the owner. Depending on the value, they are either
	/// placed next to each other from left to right or from top to bottom
	/// </summary>
	/// <returns>The layout direction for positioning child controls</returns>
	API_PROPERTY() FudgetOrientation GetOrientation() { return _ori; }
	/// <summary>
	/// Sets the layout orientation of controls on the owner. Depending on the value, they are either
	/// placed next to each other from left to right or from top to bottom
	/// </summary>
	/// <param name="value">The layout direction for positioning child controls</param>
	API_PROPERTY() void SetOrientation(FudgetOrientation value);

	void LayoutChildren(bool forced) override;
protected:
	Float2 GetRequestedSize(FudgetSizeType type) const override;

	FudgetLayoutSlot* CreateSlot(FudgetControl *control) override;

	/// <summary>
	/// Replaces the GetSlot function of Layout to return the derived slot type
	/// </summary>
	/// <param name="index">The index of the control to make the slot for</param>
	/// <returns>The slot for layouting attributes or null if the index is invalid</returns>
	API_FUNCTION(New) FudgetListLayoutSlot* GetSlot(int index) const;
private:
	FudgetOrientation _ori;
};
