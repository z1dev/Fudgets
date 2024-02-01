#include "StackLayout.h"
#include "../Container.h"
#include "../Utils/Utils.h"


FudgetStackLayout::FudgetStackLayout(const SpawnParams &params) : Base(params)
{

}

bool FudgetStackLayout::LayoutChildren()
{
	// Minimal LayoutChildren() example:
	//
	// It places all the controls inside the parent container's area. The size of the container is
	// already calculated at this point, and the layout sizes are cached too, together with all the slots.
	//
	// To access the size or attributes of a control, use (as an example):
	//		auto slot = GetSlot(control_index);
	// The sizes for the control are stored in slot->_hint_size, slot->_min_size and slot->_max_size
	//
	// For each control calculated, use SetControlDimensions(control_index, position, size) to directly
	// modify the control's dimensions and placement. This will automatically mark the control for
	// recalculation if it's a container.

	auto owner = GetOwner();
	if (owner == nullptr)
		return false;

	int count = owner->GetChildCount();

	if (count == 0)
		return true;

	// The size of the container might depend on the layout. Since it was calculated in RequestSize, this
	// value should be correct.
	Float2 space = owner->GetSize();

	for (int ix = 0; ix < count; ++ix)
	{
		SetControlDimensions(ix, Float2::Zero, space);
	}

	return true;
}

Float2 FudgetStackLayout::RequestSize(FudgetSizeType type) const
{
	// This function is called when one of the sizes is dirty and needs to be recalculated. The layout
	// might be required to do some complicated logic here, since it has to return the size based on
	// the positioning of controls.
	//
	// There are three type of sizes in Fudgets:
	// * Hint size is the size a control or layout would have if not constrained.
	// * Min size is how much a control tolerates to be compressed, until it becomes unusable. This
	//		might be changed by the user if they want a different size.
	// * Max size is the maximum a control wants to grow to. If this size is negative, the control might
	//		grow as big as the layout allows, if the layout forcefully resizes controls.
	// 
	// Layouts can ignore any of these values if they don't have enough space to hold every control, or
	// if they want controls to take up more space. The layouts should have settings to avoid that
	// situation.
	// 
	// 
	// Avoid calls to GetHintSize(), GetMinSize() and GetMaxSize(), as that would result in infinite
	// recursion.
	// 
	// None of the child controls' sizes might be ready at this point. To get them, access the layout data:
	//		auto obj = GetSlot(control_index);
	// and call each child control's size request function:
	//		Float2 size = slot->_control->GetRequestedSize(type);
	// 
	// Min sizes can be negative which should be handled like they were zero.
	//
	// Max sizes can be negative as well. In those cases the control should be able to grow "infinitely"
	// large. The external MaximumFloatLimit will be used to replace it, but it's safe to replace the
	// negative value with it here. (#include "../Utils/Utils.h" in the layout source where it's used)
	// 
	// Importan: if calculating the layout's max size involves adding the controls' max sizes, watch out
	// for overflow. Floating point overflow is undefined behavior by the c++ standard.
	// Use "float result = AddBigFloats(left_number, right_number);" (from ../Utils/Utils.h) for a safe
	// version, that returns MaximumFloatLimit if overflow happened.

	// The container of the layout and controls.
	auto owner = GetOwner();
	if (owner == nullptr)
		return Float2(0.0f);

	// Controls count in the layout.
	int count = GetOwner()->GetChildCount();
	if (count == 0.f)
		return Float2(0.f);

	Float2 result(-1.f);

	for (int ix = 0, siz = count; ix < siz; ++ix)
	{
		// One item with sizing information for a control at index ix.
		auto slot = GetSlot(ix);

		// Calculates (if necessary) and returns one of the sizes
		Float2 size = slot->_control->GetRequestedSize(type);

		// The FudgetStackLayout resizes controls to take up the available space. This means only the largest
		// control sizes are considered for the size of the layout.
		if (type != FudgetSizeType::Max)
		{
			result = Float2::Max(size, result);
		}
		else
		{
			if (size.X < 0)
				size.X = MaximumFloatLimit;
			if (size.Y < 0)
				size.Y = MaximumFloatLimit;
			result = Float2::Min(size, result);
		}
	}

	return result;
}

FudgetLayoutSlot* FudgetStackLayout::CreateSlot(FudgetControl *control)
{
	// See the function of FudgetListLayout::CreateSlot(...) for a description.
	return Base::CreateSlot(control);
}

FudgetLayoutFlag FudgetStackLayout::GetCreationFlags() const
{
	return FudgetLayoutFlag::LayoutOnContainerResize | FudgetLayoutFlag::LayoutOnContentResize | FudgetLayoutFlag::ResizeOnContainerResize |
		FudgetLayoutFlag::CanProvideSizes | Base::GetCreationFlags();
}

