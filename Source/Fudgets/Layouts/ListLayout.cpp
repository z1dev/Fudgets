#include "ListLayout.h"
#include "../Container.h"

FudgetListLayoutSlot::FudgetListLayoutSlot(FudgetControl *control) : Base(control), _horz_align(FudgetAlignHorizontal::Left), _vert_align(FudgetAlignVertical::Top)
{
}


FudgetListLayout::FudgetListLayout(const SpawnParams &params) : Base(params), _ori(FudgetOrientation::Horizontal)
{

}

FudgetListLayout::FudgetListLayout(const SpawnParams &params, FudgetOrientation orientation) : Base(params), _ori(orientation)
{

}

void FudgetListLayout::SetOrientation(FudgetOrientation value)
{
	_ori = value;
	MakeDirty(FudgetSizeType::None);
}

void FudgetListLayout::LayoutChildren(bool forced)
{
	if (!IsDirty() && !forced)
		return;

	auto owner = GetOwner();
	if (owner == nullptr)
		return;

	Float2 space = owner->GetSize();
	int count = owner->GetChildCount();

	Float2 _wanted = GetHintSize();

	float pos = 0.0f;
	for (int ix = 0; ix < count; ++ix)
	{
		auto slot = GetSlot(ix);

		if (_ori == FudgetOrientation::Horizontal)
		{
			SetControlDimensions(ix, Float2(pos + slot->_padding.left, slot->_padding.top), Float2(slot->_hint_size.X, slot->_hint_size.Y));
			pos += slot->_padding.left + slot->_padding.right + slot->_hint_size.X;
		}
		else
		{
			SetControlDimensions(ix, Float2(slot->_padding.left, pos + slot->_padding.top), Float2(slot->_hint_size.X, slot->_hint_size.Y));
			pos += slot->_padding.top + slot->_padding.bottom + slot->_hint_size.Y;
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

	Float2 result(0.0f);
	float max_size = 0.0f;

	for (int ix = 0, siz = GetOwner()->GetChildCount(); ix < siz; ++ix)
	{
		auto slot = GetSlot(ix);
		auto &pad = slot->_padding;
		result.X += pad.left + pad.right;
		result.Y += pad.top + pad.bottom;
		Float2 size;
		if (_ori == FudgetOrientation::Horizontal)
		{
			size = slot->_control->GetRequestedSize(type);
			result.X += size.X;
			max_size = Math::Max(max_size, size.Y);
		}
		else
		{
			size = slot->_control->GetRequestedSize(type);
			result.Y += size.Y;
			max_size = Math::Max(max_size, size.X);
		}

		switch (type)
		{
			case FudgetSizeType::Hint:
				slot->_hint_size = size;
				break;
			case FudgetSizeType::Min:
				slot->_min_size = size;
				break;
			case FudgetSizeType::Max:
				slot->_max_size = size;
				break;
		}
	}

	if (_ori == FudgetOrientation::Horizontal)
		result.Y += max_size;
	else
		result.X += max_size;
	return result;
}

