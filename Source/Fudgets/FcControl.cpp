#include "FcControl.h"

#include "FcContainer.h"

FcControl::FcControl() : ScriptingObject(SpawnParams(Guid::New(), TypeInitializer)), _size(0.0f), _parent(nullptr)
{

}

void FcControl::SetParent(FcContainer *value)
{
	if (_parent == value)
		return;
	if (_parent != nullptr)
	{
		_parent->RemoveChild(this);
	}
	_parent = value;
	if (_parent != nullptr)
	{
		_parent->AddChild(this);
	}
}

