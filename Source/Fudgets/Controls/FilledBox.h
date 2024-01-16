#pragma once

#include "../FcControl.h"
#include "Engine/Core/Math/Color.h"

API_CLASS(NoSpawn)
class FUDGETS_API FcFilledBox : public FcControl
{
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FcFilledBox);
public:
	FcFilledBox();

	Color GetColor() const { return _color; }
	void SetColor(Color value) { _color = value; }

	void Draw() override;
private:
	Color _color;
};
