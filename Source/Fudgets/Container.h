#pragma once

#include "Control.h"
#include "Engine/Core/Collections/Array.h"

class FudgetLayout;

API_CLASS(NoSpawn)
class FUDGETS_API FudgetContainer : public FudgetControl
{
    using Base = FudgetControl;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetContainer);
public:
	FudgetContainer();
    ~FudgetContainer();

    template<typename T>
    FORCE_INLINE T* CreateChild()
    {
        T* child = New<T>();
        AddChild(child);
        return child;
    }

    template<typename T>
    FORCE_INLINE T* CreateLayout()
    {
        T* layout = New<T>();
        _layout = layout;
        layout->SetOwner(this);
        return layout;
    }

    int AddChild(FudgetControl *control);
    int RemoveChild(FudgetControl *control);
    FudgetControl* RemoveChild(int at);
    FudgetControl* ChildAt(int at) const;
    int GetChildCount() const;

    Float2 GetPreferredSize() const override;
    Float2 GetMinSize() const override;
    Float2 GetMaxSize() const override;

    void MakeLayoutDirty();
    void RequestLayout();

    void Draw() override;

private:
    Array<FudgetControl*> _children;
    FudgetLayout *_layout;
};