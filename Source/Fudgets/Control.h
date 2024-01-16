#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Math/Vector2.h"


class FcContainer;

API_CLASS(NoSpawn)
class FUDGETS_API FcControl : public ScriptingObject
{
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FcControl);
public:
	FcControl();

	virtual void Draw() {}

	Float2 GetSize() const { return _size; }
	void SetSize(Float2 size) { _size = size; }

	float GetWidth() const { return _size.X;  }
	float GetHeight() const { return _size.Y; }


	virtual void Update(float delta_time) {}

	FcContainer* GetParent() const { return _parent; }
	void SetParent(FcContainer *value);

protected:

private:
	Float2 _size;
	FcContainer *_parent;

	friend class FcContainer;
};
