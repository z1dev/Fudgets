#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Math/Vector2.h"
#include "Engine/Core/Collections/Array.h"

class FudgetControl;
class FudgetContainer;

enum class FudgetSizeType : uint8;

API_CLASS(NoSpawn)
class FUDGETS_API FudgetLayoutSlot : public ScriptingObject
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetLayoutSlot);

	FudgetLayoutSlot(FudgetControl *control);

	// Hide in editor
	FudgetControl *_control;
	Float2 _hint_size;
	Float2 _max_size;
	Float2 _min_size;
};


API_CLASS(Abstract, NoSpawn)
class FUDGETS_API FudgetLayout : public ScriptingObject
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetLayout);
public:
	FudgetLayout();
	~FudgetLayout();

	FudgetContainer* GetOwner() const { return _owner; }
	void SetOwner(FudgetContainer *value);

	bool IsDirty() const { return _dirty; }
	virtual void MakeDirty(FudgetSizeType sizeType);

	virtual void ChildAdded(FudgetControl *control, int index = -1);
	virtual void ChildRemoved(int index);
	virtual void ChildMoved(int from, int to);

	Float2 GetHintSize() const;
	Float2 GetMinSize() const;
	Float2 GetMaxSize() const;

	void OnDeleteObject() override;

	virtual void LayoutChildren() = 0;
protected:
	virtual void SetControlDimensions(int index, Float2 pos, Float2 size);
	FudgetLayoutSlot* GetSlot(int at) const;
	virtual void CleanUp();
	virtual void FillSlots();

	void ClearedDirt();

	virtual Float2 GetRequestedSize(FudgetSizeType type) const = 0;
	virtual FudgetLayoutSlot* CreateSlot(FudgetControl *control) = 0;
private:
	bool _dirty;

	FudgetContainer *_owner;
	Array<FudgetLayoutSlot*> _slots;

	mutable bool _hint_dirty;
	mutable bool _min_dirty;
	mutable bool _max_dirty;
	mutable Float2 _cached_hint;
	mutable Float2 _cached_min;
	mutable Float2 _cached_max;
};
