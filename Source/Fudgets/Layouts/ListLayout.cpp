#include "ListLayout.h"
#include "../Container.h"



#define IMAGINARY_LIMIT     (std::numeric_limits<float>::max() - 1.0f);

FudgetListLayoutSlot::FudgetListLayoutSlot(FudgetControl *control) : Base(control),
	_horz_align(FudgetHorzAlign::Left), _vert_align(FudgetVertAlign::Top), _enforce_limits(false)
{
}


FudgetListLayout::FudgetListLayout(const SpawnParams &params) : Base(params), _ori(FudgetOrientation::Horizontal), _stretched(true)
{

}

FudgetListLayout::FudgetListLayout(const SpawnParams &params, FudgetOrientation orientation) : Base(params), _ori(orientation), _stretched(true)
{

}

void FudgetListLayout::SetOrientation(FudgetOrientation value)
{
	_ori = value;
	MakeDirty(FudgetSizeType::None);
}

void FudgetListLayout::SetStretched(bool value)
{
	_stretched = value;
	MakeDirty(FudgetSizeType::All);
}

void FudgetListLayout::LayoutChildren(bool forced)
{
	if (!IsDirty() && !forced)
		return;

	auto owner = GetOwner();
	if (owner == nullptr)
		return;

	int count = owner->GetChildCount();

	if (count == 0)
		return;

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

			SetControlDimensions(ix, Float2(pos.X + slot->_padding.left, pos.Y + slot->_padding.top), slot->_hint_size);
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
		// Make sure it's not dirty and calculated for every slot
		Float2 tmp = big_space ? GetMaxSize() : GetMinSize();

		Array<float> hint_sizes(count);
		// This will hold the space without any cell padding.
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
			SetControlDimensions(ix, Float2(pos.X + slot->_padding.left, pos.Y + slot->_padding.top),
				Float2(size_x, size_y));

			if (_ori == FudgetOrientation::Horizontal)
				pos.X += RelevantPad(slot->_padding) + size_x;
			else
				pos.Y += RelevantPad(slot->_padding) + size_y;
		}
	}

	ClearedDirt();
}

FudgetLayoutSlot* FudgetListLayout::CreateSlot(FudgetControl *control)
{
	return New<FudgetListLayoutSlot>(control);
}

FudgetListLayoutSlot* FudgetListLayout::GetSlot(int index) const
{
	return (FudgetListLayoutSlot*)Base::GetSlot(index);
}

Float2 FudgetListLayout::GetRequestedSize(FudgetSizeType type) const
{
	if (type != FudgetSizeType::Hint && type != FudgetSizeType::Min && type != FudgetSizeType::Max)
		return Float2(0.f);

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
		result.X += pad.left + pad.right;
		result.Y += pad.top + pad.bottom;
		Float2 size = slot->_control->GetRequestedSize(type);

		if (Relevant(size) < 0)
			RelevantRef(size) = 0;
		if (Opposite(size) < 0)
			OppositeRef(size) = 0;
		RelevantRef(result) += Relevant(size);
		// Overflow
		if (Relevant(result) < Relevant(size))
			RelevantRef(result) = IMAGINARY_LIMIT;

		extent = Math::Max(extent, Opposite(size));

		switch (type)
		{
			case FudgetSizeType::Hint:
				slot->_hint_size = Float2::Max(size, slot->_min_size);
				extent = Math::Max(extent, Opposite(slot->_hint_size));
				break;
			case FudgetSizeType::Min:
				slot->_min_size = size;
				break;
			case FudgetSizeType::Max:
				if (Relevant(slot->_max_size) < 0)
				{
					RelevantRef(size) = IMAGINARY_LIMIT;
					RelevantRef(result) = IMAGINARY_LIMIT;
				}
				if (Opposite(slot->_max_size) < 0)
				{
					OppositeRef(size) = IMAGINARY_LIMIT;
					extent = IMAGINARY_LIMIT;
				}

				slot->_max_size = size;
				break;
		}
	}

	OppositeRef(result) += extent;
	if (Opposite(result) < extent)
		OppositeRef(result) = IMAGINARY_LIMIT;

	return result;
}

