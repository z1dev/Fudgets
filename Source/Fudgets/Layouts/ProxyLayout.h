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
    /// Acts in place of a FudgetProxyLayout to provide the CreateSlot method.
    /// </summary>
    /// <param name="control">The control that will be inserted into the slot</param>
    /// <returns>The created object holding layouting properties of the control</returns>
    API_FUNCTION() virtual FudgetLayoutSlot* ProxyInterfaceCreateSlot(FudgetControl *control) = 0;

    /// <summary>
    /// Acts in place of a FudgetProxyLayout to provide the Measure method.
    /// </summary>
    /// <param name="avaliable">The available space for the layout contents or unrestricted when negative</param>
    /// <param name="wanted_size">The size requested by the layout contents. This might be larger than the available space.</param>
    /// <param name="min_size">The minimum size requied by the layout. Should be the same value unless the contents change</param>
    /// <returns>Whether the layout's size depends on available space, or manages a control that influences this behavior</returns>
    API_FUNCTION() virtual bool ProxyInterfaceMeasure(Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size) = 0;

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

    ///// <inheritdoc />
    bool Measure(Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size) override;

    /// <inheritdoc />
    FudgetLayoutSlot* CreateSlot(FudgetControl *control) override;

    /// <inheritdoc />
    FudgetLayoutFlag GetInitFlags() const override;

    /// <inheritdoc />
    void SetControlDimensions(int index, Float2 pos, Float2 size) override;

};