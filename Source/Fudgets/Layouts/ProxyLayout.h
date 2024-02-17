#pragma once

#include "Layout.h"
#include "Engine/Scripting/ScriptingType.h"

#include "Engine/Core/Math/Vector2.h"


/// <summary>
/// Interface required on container classes that use a FudgetProxyLayout layout.
/// </summary>
API_INTERFACE() class FUDGETS_API IProxyLayoutContainer
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(IProxyLayoutContainer);
public:
	virtual ~IProxyLayoutContainer() = default;

	/// <summary>
	/// Acts in place of a FudgetProxyLayout to provide the LayoutChildren method.
	/// </summary>
	/// <returns>Whether the layout was successful and the layout dirty flag can be cleared</returns>
	API_FUNCTION() virtual bool ProxyInterfaceLayoutChildren() = 0;

	/// <summary>
	/// Acts in place of a FudgetProxyLayout to provide the RequestSize method.
	/// </summary>
	/// <param name="type">Which size to calculate and return</param>
	/// <returns>The calculated size</returns>
	API_FUNCTION() virtual Float2 ProxyInterfaceRequestSize(FudgetSizeType type) const = 0;

	/// <summary>
	/// Acts in place of a FudgetProxyLayout to provide the CreateSlot method.
	/// </summary>
	/// <param name="control">The control that will be inserted into the slot</param>
	/// <returns>The created object holding layouting properties of the control</returns>
	API_FUNCTION() virtual FudgetLayoutSlot* ProxyInterfaceCreateSlot(FudgetControl *control) = 0;

	/// <summary>
	/// Acts in place of a FudgetProxyLayout to provide the GetInitFlags method.
	/// </summary>
	/// <returns>Layout flags that should be set to the layout.</returns>
	API_FUNCTION() virtual FudgetLayoutFlag ProxyInterfaceGetInitFlags() const = 0;
};


/// <summary>
/// Layout type that manages the dirty flags, like a normal layout would, but lets the parent container
/// do the layout calculations directly. Used for very simple layouts or when the container is a compound
/// control that has very specific layouting needs. Container classes that use this layout should inherit
/// from the IProxyLayoutContainer interface.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetProxyLayout : public FudgetLayout
{
    using Base = FudgetLayout;
    DECLARE_SCRIPTING_TYPE(FudgetProxyLayout);
public:
    ~FudgetProxyLayout();

	/// <inheritdoc />
	bool LayoutChildren() override;

	/// <inheritdoc />
	Float2 RequestSize(FudgetSizeType type) const override;

	/// <inheritdoc />
	FudgetLayoutSlot* CreateSlot(FudgetControl *control) override;

	/// <inheritdoc />
	FudgetLayoutFlag GetInitFlags() const override;

	/// <inheritdoc />
	void SetControlDimensions(int index, Float2 pos, Float2 size) override;

};