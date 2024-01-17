#pragma once

#include "../Control.h"
#include "Engine/Core/Math/Color.h"

API_CLASS(NoSpawn)
class FUDGETS_API FudgetFilledBox : public FudgetControl
{
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetFilledBox);
public:
	FudgetFilledBox();

	Color GetColor() const { return _color; }
	void SetColor(Color value) { _color = value; }

	void Draw() override;
private:
	Color _color;
};
