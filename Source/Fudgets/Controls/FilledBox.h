#pragma once

#include "../Control.h"
#include "Engine/Core/Math/Color.h"

/// <summary>
/// Test control that fills its background with color
/// </summary>
API_CLASS()
class FUDGETS_API FudgetFilledBox : public FudgetControl
{
	using Base = FudgetControl;
	DECLARE_SCRIPTING_TYPE(FudgetFilledBox);
public:
	//FudgetFilledBox();
	~FudgetFilledBox();

	/// <summary>
	/// Gets the background color used for filling the control.
	/// </summary>
	/// <returns>Background color</returns>
	API_PROPERTY() Color GetColor() const { return _color; }
	/// <summary>
	/// Sets the background color used for filling the control.
	/// </summary>
	/// <returns>Background color</returns>
	API_PROPERTY() void SetColor(Color value) { _color = value; }

	void Draw() override;
private:
	Color _color;
};
