#include "ListLayout.h"
#include "../Container.h"
#include "../Utils/Utils.h"



FudgetListLayoutSlot::FudgetListLayoutSlot(const SpawnParams &params) : Base(SpawnParams(Guid::New(), TypeInitializer)),
	_horz_align(FudgetHorzAlign::Left), _vert_align(FudgetVertAlign::Top), _enforce_limits(true)
{
}

FudgetListLayout::FudgetListLayout(const SpawnParams &params) : Base(params), _ori(FudgetOrientation::Horizontal), _stretched(true)
{

}

void FudgetListLayout::SetOrientation(FudgetOrientation value)
{
	_ori = value;
	MarkDirty(FudgetDirtType::All, true);
}

void FudgetListLayout::SetStretched(bool value)
{
	_stretched = value;
	MarkDirty(FudgetDirtType::All, true);
}

FudgetHorzAlign FudgetListLayout::GetSlotHorizontalAlignment(int index) const
{
	if (!GoodSlotIndex(index))
		return FudgetHorzAlign::Left;

	return GetSlot(index)->_horz_align;
}

void FudgetListLayout::SetSlotHorizontalAlignment(int index, FudgetHorzAlign value)
{
	if (!GoodSlotIndex(index))
		return;

	GetSlot(index)->_horz_align = value;
}

FudgetVertAlign FudgetListLayout::GetSlotVerticalAlignment(int index) const
{
	if (!GoodSlotIndex(index))
		return FudgetVertAlign::Top;

	return GetSlot(index)->_vert_align;
}

void FudgetListLayout::SetSlotVerticalAlignment(int index, FudgetVertAlign value)
{
	if (!GoodSlotIndex(index))
		return;

	GetSlot(index)->_vert_align = value;
}

bool FudgetListLayout::GetSlotLimitsEnforced(int index) const
{
	if (!GoodSlotIndex(index))
		return false;

	return GetSlot(index)->_enforce_limits;
}

void FudgetListLayout::SetSlotLimitsEnforced(int index, bool value)
{
	if (!GoodSlotIndex(index))
		return;

	GetSlot(index)->_enforce_limits = value;
}

FudgetPadding& FudgetListLayout::GetSlotPadding(int index) const
{
	ASSERT(GoodSlotIndex(index));

	return GetSlot(index)->_padding;
}

void FudgetListLayout::SetSlotPadding(int index, FudgetPadding value)
{
	if (!GoodSlotIndex(index))
		return;

	GetSlot(index)->_padding = value;
}

bool FudgetListLayout::LayoutChildren()
{
	// See FudgetStackLayout for a simpler implementation and better explanation. This function
	// calculates the position and size of controls so they can be placed next to each other.
	// When the layout is "stretched", it tries to occupy the full width or height of the parent
	// (depends on orientation) and makes the controls fill the area.


	auto owner = GetOwner();
	if (owner == nullptr)
		return false;

	int count = owner->GetChildCount();

	if (count == 0)
		return true;

	Float2 space = owner->GetSize();
	Float2 _wanted = GetHintSize();

	if (_ori == FudgetOrientation::Horizontal)
		space.Y = Math::Min(space.Y, _wanted.Y);
	else
		space.X = Math::Min(space.X, _wanted.X);
	if (!_stretched)
	{
		// The controls are placed one after the other, hanging off the edge if they take up
		// more space than what's available

		Float2 pos(0.0f);
		for (int ix = 0; ix < count; ++ix)
		{
			auto slot = GetSlot(ix);

			PlaceControlInSlotRectangle(ix, slot, Float2(pos.X + slot->_padding.Left, pos.Y + slot->_padding.Top), Float2(
				_ori == FudgetOrientation::Horizontal ? Math::Max(0.0f, slot->_hint_size.X) : _wanted.X - OppositePad(slot->_padding),
				_ori == FudgetOrientation::Vertical ? Math::Max(0.0f, slot->_hint_size.Y) : _wanted.Y - OppositePad(slot->_padding)));

			if (_ori == FudgetOrientation::Horizontal)
				pos.X += RelevantPad(slot->_padding) + Relevant(slot->_hint_size);
			else
				pos.Y += RelevantPad(slot->_padding) + Relevant(slot->_hint_size);
		}
	}
	else
	{
		// The controls are stretched across the width or height of the parent. They can't
		// take up more space than what's available.
		// Rules of stretching:
		// 1. every control tries to be as big as its hint size
		// 
		// A.1 if there is more space available, the controls grow equally while keeping
		//     their size ratio. (control with twice the size will always be twice as big)
		// A.2 controls don't grow larger than their maximum size. Once a control reaches
		//     that size, the rule in A.1 is applied only to the other controls.
		//        - exception to this rule are slots don't require that their limit
		//          determines their slot size.
		// A.3 if all controls reach their maximum size and there's still space, they are
		//     placed in their designated place, aligned by their alignment setting.
		//
		// B.1 if there's less available space than the hint size, all controls shrink
		//     equally, keeping their size ratio.
		// B.2 controls don't shrink smaller than their minimum size. Once a control reaches
		//     its minimum size, the rule in B.1 is applied only to the other controls.
		//        - exception to this rule are slots don't require that their limit
		//          determines their slot size.
		// B.3 if there is less space than the minimum size of all controls, they are
		//     shrinked while keeping their ratio of minimum size.
		//
		// In every case the paddings are included in the minimum or maximum total size, but
		// not when calculating the ratios.

		bool big_space = Relevant(_wanted) <= Relevant(space);

		//// Make sure it's not dirty and calculated for every slot
		//Float2 tmp = big_space ? GetMaxSize() : GetMinSize();

		Array<float> hint_sizes(count);
		// This will hold the space with cell padding subtracted.
		float no_padding_space = Relevant(space);
		// The size of all cells added together.
		float sum = 0.f;
		for (int ix = 0; ix < count; ++ix)
		{
			auto slot = GetSlot(ix);
			float size = Math::Max(0.f, Relevant(slot->_hint_size));
			hint_sizes.Add(size);
			sum += size;
			no_padding_space -= RelevantPad(slot->_padding);
		}

		Array<float> sizes(count);
		while (sizes.Count() < count)
			sizes.Add(-1.f);
		bool dirty = true;

		float saved_no_padding = no_padding_space;

		int maxed_count = 0;
		while (dirty && maxed_count != count)
		{
			dirty = false;
			int hint_count = count - maxed_count;
			maxed_count = count;
			float used_space = 0.f;
			float used_sum = 0.f;
			for (int ix = 0; ix < count; ++ix)
			{
				if (sizes[ix] < 0)
				{
					--maxed_count;
					float ratio;
					// Calculating and storing the size ratio of cells. 
					if (sum > 0)
						ratio = hint_sizes[ix] / sum;
					else
						ratio = 1.f / hint_count;
					auto slot = GetSlot(ix);

					// Checking if a slot doesn't want to shrink/grow over its limit.

					float size = ratio * no_padding_space;
					if ((big_space && slot->_enforce_limits && Relevant(slot->_max_size) < size) || (!big_space && slot->_enforce_limits && Relevant(slot->_min_size) > size))
					{
						// Found a slot which doesn't want to shrink/grow over its limit.
						if (big_space)
							size = Relevant(slot->_max_size);
						else
							size = Relevant(slot->_min_size);
						used_space += size;
						used_sum += hint_sizes[ix];
						sizes[ix] = size;
						dirty = true;
					}
				}
			}
			no_padding_space -= used_space;
			sum -= used_sum;
		}
		Array<float> ratios(count);
		if (maxed_count == count)
		{
			// A.3/B.3 recalculate the ratios

			no_padding_space = saved_no_padding;
			sum = 0.0f;
			for (int ix = 0; ix < count; ++ix)
				sum += sizes[ix];
			for (int ix = 0; ix < count; ++ix)
			{
				if (sum > 0)
					ratios.Add(sizes[ix] / sum);
				else
					ratios.Add(1.f / count);
				sizes[ix] = -1.f;
			}
		}
		else
		{
			for (int ix = 0; ix < count; ++ix)
			{
				if (sizes[ix] < 0)
				{
					if (sum > 0)
						ratios.Add(hint_sizes[ix] / sum);
					else
						ratios.Add(1.f / (count - maxed_count));
				}
				else
					ratios.Add(0);
			}
		}

		Float2 pos = 0.f;

		// TODO control alignment inside available slot
		for (int ix = 0; ix < count; ++ix)
		{
			auto slot = GetSlot(ix);
			float size = sizes[ix] < 0 ? no_padding_space * ratios[ix] : sizes[ix];

			float size_x = _ori == FudgetOrientation::Horizontal ? size : Math::Max(space.X - OppositePad(slot->_padding), 0.0f);
			float size_y = _ori == FudgetOrientation::Vertical ? size : Math::Max(space.Y - OppositePad(slot->_padding), 0.0f);

			PlaceControlInSlotRectangle(ix, slot, Float2(pos.X + slot->_padding.Left, pos.Y + slot->_padding.Top), Float2(size_x, size_y));

			if (_ori == FudgetOrientation::Horizontal)
				pos.X += RelevantPad(slot->_padding) + size_x;
			else
				pos.Y += RelevantPad(slot->_padding) + size_y;
		}
	}

	return true;
}

FudgetLayoutSlot* FudgetListLayout::CreateSlot(FudgetControl *control)
{
	// This function is supposed to return a FudgetlayoutSlot object, but layouts can
	// provide their own derived value. To make use easier, you can define a new GetSlot()
	// function that will hide the original call. See below.

	FudgetLayoutSlot *slot = New<FudgetListLayoutSlot>(SpawnParams(Guid::New(), FudgetLayoutSlot::TypeInitializer));
	slot->_control = control;
	return slot;
}

FudgetListLayoutSlot* FudgetListLayout::GetSlot(int index) const
{
	// Since every slot that GetSlot(...) returns was created by its layout, it's safe to
	// cast without checking. This function hides the original, so to play nice with C#, it
	// was declared with API_FUNCTION(new).

	return (FudgetListLayoutSlot*)Base::GetSlot(index);
}

FudgetLayoutFlag FudgetListLayout::GetCreationFlags() const
{
	return FudgetLayoutFlag::LayoutOnContainerResize | FudgetLayoutFlag::LayoutOnContentResize | FudgetLayoutFlag::ResizeOnContainerResize |
		FudgetLayoutFlag::ResizeOnContentResize | FudgetLayoutFlag::LayoutOnContentIndexChange | Base::GetCreationFlags();
}

Float2 FudgetListLayout::RequestSize(FudgetSizeType type) const
{
	auto owner = GetOwner();
	if (owner == nullptr)
		return Float2(0.0f);
	int count = GetOwner()->GetChildCount();
	if (count == 0.f)
		return Float2(0.f);

	Float2 result(0.0f);
	// The extent of the largest control in the unrelated direction.
	float extent = 0.0f;

	for (int ix = 0, siz = count; ix < siz; ++ix)
	{
		auto slot = GetSlot(ix);
		auto &pad = slot->_padding;
		result.X += pad.Left + pad.Right;
		result.Y += pad.Top + pad.Bottom;
		Float2 size = slot->_control->GetRequestedSize(type);
		Float2 fixed_size = size;

		if (Relevant(fixed_size) < 0)
			RelevantRef(fixed_size) = 0;
		if (Opposite(fixed_size) < 0)
			OppositeRef(fixed_size) = 0;

		// Overflow checking replacement of RelevantRef(result) += Relevant(fixed_size)
		RelevantRef(result) = AddBigFloats(Relevant(result), Relevant(fixed_size));

		extent = Math::Max(extent, Opposite(fixed_size));

		switch (type)
		{
			case FudgetSizeType::Hint:
				slot->_hint_size = fixed_size;
				extent = Math::Max(extent, Opposite(slot->_hint_size));
				break;
			case FudgetSizeType::Min:
				slot->_min_size = fixed_size;
				break;
			case FudgetSizeType::Max:
				if (Relevant(size) < 0)
				{
					RelevantRef(size) = MaximumFloatLimit;
					RelevantRef(result) = MaximumFloatLimit;
				}
				if (Opposite(size) < 0)
				{
					OppositeRef(size) = MaximumFloatLimit;
					extent = MaximumFloatLimit;
				}

				slot->_max_size = size;
				break;
		}
	}

	OppositeRef(result) = AddBigFloats(Opposite(result), extent);

	return result;
}

void FudgetListLayout::PlaceControlInSlotRectangle(int index, FudgetListLayoutSlot *slot, Float2 pos, Float2 size)
{
	Float2 control_pos(0.f);
	Float2 control_size(0.f);

	control_pos.X = pos.X;
	control_pos.Y = pos.Y;
	control_size.X = size.X;
	control_size.Y = size.Y;

	// Making sure it's calculated.
	GetHintSize();

	if (!Math::NearEqual(size.X, slot->_hint_size.X))
	{
		if (size.X > slot->_hint_size.X)
		{
			if (slot->_horz_align != FudgetHorzAlign::Fill || slot->_enforce_limits)
			{
				float size_X = slot->_hint_size.X;

				// Making sure it's calculated.
				if (slot->_horz_align == FudgetHorzAlign::Fill)
				{
					GetMaxSize();
					size_X = slot->_max_size.X;
				}

				if (size_X < size.X)
				{
					control_size.X = size_X;
					if (slot->_horz_align == FudgetHorzAlign::Right || slot->_horz_align == FudgetHorzAlign::ClipRight)
						control_pos.X += size.X - size_X;
					else if (slot->_horz_align == FudgetHorzAlign::Center || slot->_horz_align == FudgetHorzAlign::ClipCenter)
						control_pos.X += (size.X - size_X) * 0.5f;
				}
			}
		}
		else if (slot->_horz_align == FudgetHorzAlign::ClipLeft)
		{
			control_size.X = slot->_hint_size.X;
		}
		else if (slot->_horz_align == FudgetHorzAlign::ClipRight)
		{
			control_size.X = slot->_hint_size.X;
			control_pos.X -= slot->_hint_size.X - size.X;
		}
	}

	if (!Math::NearEqual(size.Y, slot->_hint_size.Y))
	{
		if (size.Y > slot->_hint_size.Y)
		{
			if (slot->_vert_align != FudgetVertAlign::Fill || slot->_enforce_limits)
			{
				float size_Y = slot->_hint_size.Y;

				// Making sure it's calculated.
				if (slot->_horz_align == FudgetHorzAlign::Fill)
				{
					GetMaxSize();
					size_Y = slot->_max_size.Y;
				}

				if (size_Y < size.Y)
				{
					control_size.Y = size_Y;
					if (slot->_vert_align == FudgetVertAlign::Bottom || slot->_vert_align == FudgetVertAlign::ClipBottom)
						control_pos.Y += size.Y - size_Y;
					else if (slot->_vert_align == FudgetVertAlign::Center || slot->_vert_align == FudgetVertAlign::ClipCenter)
						control_pos.Y += (size.Y - size_Y) * 0.5f;
				}
			}
		}
		else if (slot->_vert_align == FudgetVertAlign::ClipTop)
		{
			control_size.Y = slot->_hint_size.Y;
		}
		else if (slot->_vert_align == FudgetVertAlign::ClipBottom)
		{
			control_size.Y = slot->_hint_size.Y;
			control_pos.Y -= slot->_hint_size.Y - size.Y;
		}
	}


	SetControlDimensions(index, control_pos, control_size);

}

