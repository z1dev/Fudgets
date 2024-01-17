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

    int AddChild(FudgetControl *control, int index = -1);
    int RemoveChild(FudgetControl *control);
    FudgetControl* RemoveChild(int index);
    bool MoveChildToIndex(int from, int to);

    FudgetControl* ChildAt(int index) const;
    int GetChildCount() const;
    int ChildIndex(FudgetControl *control) const;
    void DeleteAll();

    Float2 GetHintSize() const override;
    Float2 GetMinSize() const override;
    Float2 GetMaxSize() const override;

    void MakeLayoutDirty(FudgetSizeType sizeType);
    void RequestLayout();

    void Draw() override;

private:
    Array<FudgetControl*> _children;
    FudgetLayout *_layout;

    // Used locally to avoid double calling functions from child controls.
    bool _changing;
};