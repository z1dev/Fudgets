#pragma once

#include "Layout.h"
#include "../MarginStructs.h"


enum class FudgetAlignHorizontal
{
	Left,
	Center,
	Right,
	Fill,
	FillLeft,
	FillCenter,
	FillRight,
};

enum class FudgetAlignVertical
{
	Top,
	Center,
	Bottom,
	Fill,
	FillTop,
	FillCenter,
	FillBottom,
};

enum class FudgetOrientation
{
	Horizontal,
	Vertical
};

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


API_CLASS(NoSpawn)
class FUDGETS_API FudgetListLayout : public FudgetLayout
{
	using Base = FudgetLayout;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetListLayout);
public:
	FudgetListLayout();
	FudgetListLayout(FudgetOrientation orientation);


	FudgetOrientation GetOrientation() { return _ori; }
	void SetOrientation(FudgetOrientation value);

	void LayoutChildren() override;
protected:
	Float2 GetRequestedSize(FudgetSizeType type) const override;

	FudgetLayoutSlot* CreateSlot(FudgetControl *control) override;
	FudgetListLayoutSlot* GetSlot(int at) const;
private:
	enum class SizeTypes
	{
		Preferred,
		Min,
		Max
	};

	FudgetOrientation _ori;
};
