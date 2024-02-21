#pragma once

#include "Layout.h"


/// <summary>
/// A simple layout that does nothing. Its the default layout in containers when no layout is specified. Controls
/// inside this layout can move and resize themselves. The layout does not provide sizes to its parent container,
/// which will use its own sizes instead.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetContainerLayout : public FudgetLayout
{
    using Base = FudgetLayout;
    DECLARE_SCRIPTING_TYPE(FudgetContainerLayout);
protected:
	/// <inheritdoc />
	bool LayoutChildren() override;

	/// <inheritdoc />
	bool Measure(Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size) override;

	/// <inheritdoc />
	bool IsControlPositionChangePermitted(const FudgetControl *control) const override { return true; }

	/// <inheritdoc />
	bool IsControlDirectSizeChangePermitted(const FudgetControl *control) const override { return true; }
};
