#pragma once

#include "Layout.h"
#include "../MarginStructs.h"


/// <summary>
/// Derived slot holding attributes necessary for layouting for the list layout
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetListLayoutSlot : public FudgetLayoutSlot
{
	using Base = FudgetLayoutSlot;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetListLayoutSlot);
	
	FudgetListLayoutSlot(FudgetControl *control);

	/// <summary>
	/// Horizontal alignment of control in its column or row
	/// </summary>
	API_FIELD() FudgetHorzAlign _horz_align;

	/// <summary>
	/// Vertical alignment of control in its column or row
	/// </summary>
	API_FIELD() FudgetVertAlign _vert_align;

	/// <summary>
	/// Whether the min or max size of this slot is respected during resize. When it's false, the sizes
	/// are used when calculating the preferred minimum and maximum size of the parent container, but not
	/// when layouting
	/// </summary>
	API_FIELD() bool _enforce_limits;
	/// <summary>
	/// Padding around the control in its slot
	/// </summary>
	API_FIELD() FudgetSlotPadding _padding;
};

/// <summary>
/// A layout that aligns the owner container's children horizontally or vertically like a list of items
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

	/// <summary>
	/// Whether the layout tries to size its controls to fill up the available space in the orientation
	/// of the layout. When this is false, the controls are placed one after the other with their preferred
	/// size.
	/// </summary>
	/// <returns>Whether the layout fills the available space</returns>
	API_PROPERTY() bool IsStretched() const { return _stretched; }
	/// <summary>
	/// Sets whether the layout tries to size its controls to fill up the available space in the orientation
	/// of the layout. When this is false, the controls are placed one after the other with their preferred
	/// size.
	/// </summary>
	/// <param name="value">Whether the layout fills the available space</param>
	API_PROPERTY() void SetStretched(bool value);

protected:
	/// <inheritdoc />
	Float2 RequestSize(FudgetSizeType type) const override;

	/// <inheritdoc />
	bool LayoutChildren() override;

	/// <inheritdoc />
	FudgetLayoutSlot* CreateSlot(FudgetControl *control) override;

	/// <summary>
	/// Replaces the GetSlot function of Layout to return the derived slot type
	/// </summary>
	/// <param name="index">The index of the control to make the slot for</param>
	/// <returns>The slot for layouting attributes or null if the index is invalid</returns>
	API_FUNCTION(new) FudgetListLayoutSlot* GetSlot(int index) const;
private:
	void PlaceControlInSlotRectangle(int index, FudgetListLayoutSlot *slot, Float2 pos, Float2 size);

	FORCE_INLINE float Relevant(Float2 value) const
	{
		if (_ori == FudgetOrientation::Horizontal)
			return value.X;
		return value.Y;
	}
	FORCE_INLINE float& RelevantRef(Float2 &value) const
	{
		if (_ori == FudgetOrientation::Horizontal)
			return value.X;
		return value.Y;
	}
	FORCE_INLINE float Opposite(Float2 value) const
	{
		if (_ori == FudgetOrientation::Horizontal)
			return value.Y;
		return value.X;
	}
	FORCE_INLINE float& OppositeRef(Float2 &value) const
	{
		if (_ori == FudgetOrientation::Horizontal)
			return value.Y;
		return value.X;
	}
	FORCE_INLINE float RelevantPad(const FudgetSlotPadding &padding) const
	{
		if (_ori == FudgetOrientation::Horizontal)
			return padding.left + padding.top;
		return padding.top + padding.bottom;
	}
	FORCE_INLINE float OppositePad(const FudgetSlotPadding &padding) const
	{
		if (_ori == FudgetOrientation::Horizontal)
			return padding.top + padding.bottom;
		return padding.left + padding.top;
	}

	FudgetOrientation _ori;
	bool _stretched;
};
