#pragma once

#include "Container.h"
#include "Engine/Input/Enums.h"

API_CLASS(NoSpawn, Hidden)
class FUDGETS_API FudgetGUIRoot : public FudgetContainer
{
    using Base = FudgetContainer;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetGUIRoot);
public:
    FudgetGUIRoot();
    FudgetGUIRoot(Fudget *root);

    ~FudgetGUIRoot();

    /// <inheritdoc />
    Float2 GetSize() const override;
    /// <inheritdoc />
    Float2 GetHintSize() const override;
    /// <inheritdoc />
    Float2 GetMinSize() const override;
    /// <inheritdoc />
    Float2 GetMaxSize() const override;


private:
    void InitializeEvents();
    void UninitializeEvents();

    // Mouse and Touch input:
    void OnMouseDown(const Float2 &pos, MouseButton button);
    // Mouse and Touch input:
    void OnMouseUp(const Float2 &pos, MouseButton button);
    // Mouse and Touch input:
    void OnMouseMove(const Float2 &pos);
    // Mouse and Touch input:
    void OnMouseLeave();
    // Mouse and Touch input:
    void OnMouseDoubleClick(const Float2 &pos, MouseButton button);


	// The actor on the scene forwarding all the events to the root.
    Fudget *_root;

    bool events_initialized;
};

