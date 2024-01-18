#pragma once

#include "../Control.h"
#include "Engine/Core/Math/Color.h"

/// <summary>
/// Test control that fills its background with color
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetFilledBox : public FudgetControl
{
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetFilledBox);
public:
	FudgetFilledBox();

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
