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
    API_FUNCTION() virtual void ProxyInterfacePreLayoutChildren(Int2 space) = 0;

    /// <summary>
    /// Acts in place of a FudgetProxyLayout to provide the LayoutChildren method.
    /// </summary>
    API_FUNCTION() virtual void ProxyInterfaceLayoutChildren(Int2 space) = 0;


    /// <summary>
    /// Acts in place of a FudgetProxyLayout to provide the CreateSlot method.
    /// </summary>
    /// <param name="control">The control that will be inserted into the slot</param>
    /// <returns>The created object holding layouting properties of the control</returns>
    API_FUNCTION() virtual FudgetLayoutSlot* ProxyInterfaceCreateSlot(FudgetControl *control) = 0;

    ///// <summary>
    ///// Acts in place of a FudgetProxyLayout to provide the Measure method.
    ///// </summary>
    ///// <param name="avaliable">The available space for the layout contents or unrestricted when negative</param>
    ///// <param name="wanted_size">The size requested by the layout contents. This might be larger than the available space.</param>
    ///// <param name="min_size">The minimum size requied by the layout. Should be the same value unless the contents change</param>
    ///// <returns>Whether the layout's size depends on available space, or manages a control that influences this behavior</returns>
    //API_FUNCTION() virtual bool ProxyInterfaceMeasure(Int2 available, API_PARAM(Out) Int2 &wanted_size, API_PARAM(Out) Int2 &min_size, API_PARAM(Out) Int2 &max_size) = 0;

    /// <summary>
    /// Acts in place of a FudgetProxyLayout to provide the GetInitFlags method.
    /// </summary>
    /// <returns>Layout flags that should be set to the layout.</returns>
    API_FUNCTION() virtual FudgetLayoutFlag ProxyInterfaceGetInitFlags() const = 0;

    /// <summary>
    /// 
    /// </summary>
    /// <param name="index"></param>
    /// <returns></returns>
    API_FUNCTION() virtual bool ProxyInterfacePlaceControlInSlotRectangle(int index) = 0;
};


/// <summary>
/// Layout type that manages the dirty flags, like a normal layout would, but lets the parent container
/// do the layout calculations directly. Used for very simple layouts or when the container is a compound
/// control that has very specific layouting needs. Container classes that use this layout should inherit
/// from the IProxyLayoutContainer interface.
/// </summary>
API_CLASS(Attributes="HideInEditor")
class FUDGETS_API FudgetProxyLayout : public FudgetLayout
{
    using Base = FudgetLayout;
    DECLARE_SCRIPTING_TYPE(FudgetProxyLayout);
public:
    ~FudgetProxyLayout();

    /// <summary>
    /// Call from an IProxyLayoutContainer's ProxyInterfaceLayoutChildren to set the calculated position and size of it controls.
    /// </summary>
    /// <param name="index">Index of the control</param>
    /// <param name="pos">Calculated position of control relative to the IProxyLayoutContainer</param>
    /// <param name="size">Calculated size of the control</param>
    API_FUNCTION() void SetComputedBounds(int index, Int2 pos, Int2 size);

    /// <summary>
    /// Call from an IProxyLayoutContainer's ProxyInterfaceLayoutChildren when the proxy layout calculated its size requirements.
    /// If the available space is unrestricted (checked by IsUnrestrictedSpace), only the layout sizes should be calculated and
    /// updating any slot layout data with SetComputedBounds should be avoided. Care must be taken not to cause overflow
    /// when calculating any of the sizes.
    /// </summary>
    /// <param name="sizes">The space used for measurement and the measured sizes of the layout. Every field should be set in the struct.</param>
    API_FUNCTION() void SetControlSizes(const FudgetLayoutSizeCache &sizes);

    /// <summary>
    /// Call from an IProxyLayoutContainer to get the measurements of child controls. Same as the Layout's MeasureSlot function.
    /// </summary>
    /// <param name="index">Slot index</param>
    /// <param name="available">The available space for the layout contents. Negative values mean unrestricted space</param>
    /// <param name="wanted_size">Receives the slot's content's requested normal size</param>
    /// <param name="wanted_min">Receives the slot's content's requested minimal size</param>
    /// <param name="wanted_max">Receives the slot's content's requested maximum size</param>
    API_FUNCTION() bool GetSlotMeasures(int index, Int2 available, API_PARAM(Out) Int2 &wanted_size, API_PARAM(Out) Int2 &wanted_min, API_PARAM(Out) Int2 &wanted_max);
protected:
    /// <inheritdoc />
    void PreLayoutChildren(Int2 space, FudgetContainer *owner, int count) override;

    /// <inheritdoc />
    void LayoutChildren(Int2 space, FudgetContainer *owner, int count) override;

    ///// <inheritdoc />
    //bool Measure(Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size) override;

    /// <inheritdoc />
    FudgetLayoutSlot* CreateSlot(FudgetControl *control) override;

    /// <inheritdoc />
    FudgetLayoutFlag GetInitFlags() const override;

    /// <inheritdoc />
    void SetControlDimensions(int index, Int2 pos, Int2 size) override;

    /// <inheritdoc />
    void PlaceControlInSlotRectangle(int index) override;
};