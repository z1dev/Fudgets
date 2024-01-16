#pragma once

#include "FcControl.h"
#include "Engine/Core/Collections/Array.h"

class FcLayout;

API_CLASS(NoSpawn)
class FUDGETS_API FcContainer : public FcControl
{
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FcContainer);
public:
	FcContainer();
    ~FcContainer();

    template<typename T>
    FORCE_INLINE T* CreateChild()
    {
        T* child = New<T>();
        AddChild(child);
        return child;
    }

    bool AddChild(FcControl *control);
    bool RemoveChild(FcControl *control);

    void Draw() override;
private:
    Array<FcControl*> _children;
    FcLayout *layout;
};