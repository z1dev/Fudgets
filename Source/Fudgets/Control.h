#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Math/Vector2.h"


API_ENUM()
enum class FudgetSizeType : uint8
{
	Preferred,
	Min,
	Max,
	Current
};

class FudgetContainer;

API_CLASS(NoSpawn)
class FUDGETS_API FudgetControl : public ScriptingObject
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetControl);
public:
	FudgetControl();

	virtual void Draw() {}

	FudgetContainer* GetParent() const { return _parent; }
	void SetParent(FudgetContainer *value);

	int GetOrder() { return _order; }

	virtual Float2 GetPreferredSize() const { return _pref_size; }
	virtual void SetPreferredSize(Float2 value);

	virtual Float2 GetMinSize() const { return _min_size; }
	void SetMinSize(Float2 value);

	virtual Float2 GetMaxSize() const { return _max_size; }
	void SetMaxSize(Float2 value);

	virtual Float2 GetSize() const;
	virtual void SetSize(Float2 value);

	Float2 GetRequestedSize(FudgetSizeType type) const;

	virtual float GetWidth() const { return GetSize().X;  }
	virtual float GetHeight() const { return GetSize().Y; }

	virtual Float2 GetPosition() const;
	virtual void SetPosition(Float2 value);

	virtual float GetLeft() const { return _pos.X; }
	virtual float GetTop() const { return _pos.Y; }
	float GetRight() const { return GetLeft() + GetWidth(); }
	float GetBottom() const { return GetTop() + GetHeight(); }


	virtual void Update(float delta_time) {}

protected:
	void MakeLayoutDirty();
private:
	void LayoutUpdate(Float2 pos, Float2 size);

	FudgetContainer *_parent;
	int _order;

	Float2 _pos;
	Float2 _size;

	Float2 _pref_size;
	Float2 _min_size;
	Float2 _max_size;

	friend class FudgetLayout;
};
