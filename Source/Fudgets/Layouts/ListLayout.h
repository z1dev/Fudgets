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

	/// <summary>
	/// Gets a control's horizontal alignment given its index in the owner container. The horizontal alignment
	/// determines how the control is placed in its designated slot.
	/// </summary>
	/// <param name="index">The control's index in its container</param>
	/// <returns>The control's horizontal alignment</returns>
	API_FUNCTION() FudgetHorzAlign GetItemHorizontalAlignment(int index) const;

	/// <summary>
	/// Sets a control's horizontal alignment given its index in the owner container. The horizontal alignment
	/// determines how the control is placed in its designated slot.
	/// </summary>
	/// <param name="index">The control's index in its container</param>
	/// <param name="value">The control's new horizontal alignment</param>
	API_FUNCTION() void SetItemHorizontalAlignment(int index, FudgetHorzAlign value);

	/// <summary>
	/// Gets a control's vertical alignment given its index in the owner container. The vertical alignment
	/// determines how the control is placed in its designated slot.
	/// </summary>
	/// <param name="index">The control's index in its container</param>
	/// <returns>The control's vertical alignment</returns>
	API_FUNCTION() FudgetVertAlign GetItemVerticalAlignment(int index) const;

	/// <summary>
	/// Sets a control's vertical alignment given its index in the owner container. The vertical alignment
	/// determines how the control is placed in its designated slot.
	/// </summary>
	/// <param name="index">The control's index in its container</param>
	/// <param name="value">The control's new vertical alignment</param>
	API_FUNCTION() void SetItemVerticalAlignment(int index, FudgetVertAlign value);

	/// <summary>
	/// Returns true if the control wants its minimum or maximum size respected in the layout when it's
	/// possible. During the size calculation of the layout, the minimum and maximum sizes are taken into
	/// account independent of this setting. It's only relevant, when the control sizing and placement is
	/// calculated.
	/// </summary>
	/// <param name="index">The control's index in its container</param>
	/// <returns>Whether control's size limits will be respected during layout or not</returns>
	API_FUNCTION() bool GetItemLimitsEnforced(int index) const;

	/// <summary>
	/// Tells the layout if the control wants its minimum or maximum size respected when it is possible.
	/// During the size calculation of the layout, the minimum and maximum sizes are taken into account
	/// independent of this setting. It's only relevant, when the control sizing and placement is
	/// calculated.
	/// </summary>
	/// <param name="index">The control's index in its container</param>
	/// <param name="value">Whether control's size limits should be respected during layout or not</param>
	API_FUNCTION() void SetItemLimitsEnforced(int index, bool value);

	/// <summary>
	/// Gets the padding of a control in its slot. The padding with the control together counts as the
	/// content of a slot, and it is calculated as an inner border around the control
	/// </summary>
	/// <param name="index">The control's index in its container</param>
	/// <returns>The padding values for the sides</returns>
	API_FUNCTION() FudgetSlotPadding& GetItemPadding(int index) const;

	/// <summary>
	/// Sets the padding of a control in its slot. The padding with the control together counts as the
	/// content of a slot, and it is calculated as an inner border around the control
	/// </summary>
	/// <param name="index">The control's index in its container</param>
	/// <param name="value">The padding values for the sides</param>
	API_FUNCTION() void SetItemPadding(int index, FudgetSlotPadding value);
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
			return padding.Left + padding.Top;
		return padding.Top + padding.Bottom;
	}
	FORCE_INLINE float OppositePad(const FudgetSlotPadding &padding) const
	{
		if (_ori == FudgetOrientation::Horizontal)
			return padding.Top + padding.Bottom;
		return padding.Left + padding.Top;
	}

	FudgetOrientation _ori;
	bool _stretched;
};
