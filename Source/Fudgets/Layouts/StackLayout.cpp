#include "StackLayout.h"
#include "../Container.h"
#include "../Utils/Utils.h"


FudgetStackLayout::FudgetStackLayout(const SpawnParams &params) : Base(params)
{

}

bool FudgetStackLayout::LayoutChildren()
{
	// Minimal LayoutChildren(...) example:
	//
	// It places all the controls inside the parent container's area. The size of the container
	// is already calculated at this point. However the sizes of this layout can be dirty, if they were
	// not necessary for the size calculation.
	// 
	// Call "ClearedDirt();" when done, to clear the dirty tag.
	// 
	// To make sure sizes are calculated properly, call GetHintSize(), GetMaxSize() or GetMinSize(),
	// depending on which sizes are needed. (I'm considering simplifying this and just force recalculate
	// everything if one size was requested.)
	//
	// To access a specific size, use (just an example) "auto slot = GetSlot(control_index);" to get the
	// object holding the layout data for a control. Then use slot->_hint_size, slot->_min_size and
	// slot->_max_size values to read the values. (Only after the sizes are calculated, see above.)
	//
	// Finally, when the calculations are done, use SetControlDimensions(...) to directly modify the
	// controls' position and size. 

	auto owner = GetOwner();
	if (owner == nullptr)
		return false;

	int count = owner->GetChildCount();

	if (count == 0)
		return true;

	Float2 space = owner->GetSize();

	for (int ix = 0; ix < count; ++ix)
	{
		SetControlDimensions(ix, Float2::Zero, space);
	}

	return true;
}

Float2 FudgetStackLayout::RequestSize(FudgetSizeType type) const
{
	// This function is called when one of the sizes is dirty and needs to be recalculated.
	// The layout might be required to do some complicated logic here, since it has to return
	// the size based on how the controls would be positioned.
	//
	// Avoid calls to GetHintSize(), GetMinSize() and GetMaxSize(), as that would result in
	// infinite recursion.
	// 
	// None of the child controls' sizes might be ready at this point. To get them, access
	// the layout data by for example "auto obj = GetSlot(control_index);" and then call each
	// child control's size request function like "Float2 size = slot->_control->GetRequestedSize(type);"
	//
	// Min sizes can be negative. Consider them to be 0 in that case.
	//
	// Max sizes can be negative as well. In those cases, this means the control can go "infinitely" large.
	// The external MaximumFloatLimit will be used in that case instead. It's safe to replace the negative
	// value with it. (#include "../Utils/Utils.h" in the layout source where it's used)
	// 
	// Importan: if calculating the layout's max size involves adding the controls' max sizes, watch out
	// for overflow. Floating point overflow is undefined behavior by the c++ standard.
	// Use "float result = AddBigFloats(left_number, right_number);" (from ../Utils/Utils.h) for a safe
	// version that returns MaximumFloatLimit if overflow happened.


	// This check might be unnecessary, because it is only called from FudgetLayout, but keeping here for now.
	if (type != FudgetSizeType::Hint && type != FudgetSizeType::Min && type != FudgetSizeType::Max)
		return Float2(0.f);

	auto owner = GetOwner();
	if (owner == nullptr)
		return Float2(0.0f);
	int count = GetOwner()->GetChildCount();
	if (count == 0.f)
		return Float2(0.f);

	Float2 result(-1.f);

	for (int ix = 0, siz = count; ix < siz; ++ix)
	{
		auto slot = GetSlot(ix);

		Float2 size = slot->_control->GetRequestedSize(type);


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

	return New<FudgetLayoutSlot>(control);
}
