#pragma once

#include "Layout.h"


/// <summary>
/// Horizontal side of the parent container that can be used as an anchor in an anchor layout
/// </summary>
API_ENUM()
enum class FudgetHorizontalAnchor
{
    /// <summary>
    /// No side given
    /// </summary>
    None,
    /// <summary>
    /// Left side as anchor
    /// </summary>
    Left,
    /// <summary>
    /// Right side as anchor
    /// </summary>
    Right,
};

/// <summary>
/// Vertical side of the parent container that can be used as an anchor in an anchor layout
/// </summary>
API_ENUM()
enum class  FudgetVerticalAnchor
{
    /// <summary>
    /// No side given
    /// </summary>
    None,
    /// <summary>
    /// Top side as anchor
    /// </summary>
    Top,
    /// <summary>
    /// Bottom side as anchor
    /// </summary>
    Bottom,
};

/// <summary>
/// Layout slot for the FudgetAnchorLayout
/// </summary>
API_CLASS()
class FUDGETS_API FudgetAnchorLayoutSlot : public FudgetLayoutSlot
{
    using Base = FudgetLayoutSlot;
    DECLARE_SCRIPTING_TYPE(FudgetAnchorLayoutSlot);

public:
    /// <summary>
    /// Anchor of the control's left side
    /// </summary>
    API_FIELD() FudgetHorizontalAnchor leftAnchor;
    /// <summary>
    /// Anchor of the control's right side
    /// </summary>
    API_FIELD() FudgetHorizontalAnchor rightAnchor;
    /// <summary>
    /// Anchor of the control's top side
    /// </summary>
    API_FIELD() FudgetVerticalAnchor topAnchor;
    /// <summary>
    /// Anchor of the control's bottom side
    /// </summary>
    API_FIELD() FudgetVerticalAnchor bottomAnchor;

    /// <summary>
    /// Distance of the control's left side from its anchor
    /// </summary>
    API_FIELD() float leftPercent;
    /// <summary>
    /// Distance of the control's right side from its anchor
    /// </summary>
    API_FIELD() float rightPercent;
    /// <summary>
    /// Distance of the control's top side from its anchor
    /// </summary>
    API_FIELD() float topPercent;
    /// <summary>
    /// Distance of the control's bottom side from its anchor
    /// </summary>
    API_FIELD() float bottomPercent;
};

/// <summary>
/// A layout that places its container owner's child controls relative to the sides of the container
/// </summary>
API_CLASS()
class FUDGETS_API FudgetAnchorLayout : public FudgetLayout
{
    using Base = FudgetLayout;
    DECLARE_SCRIPTING_TYPE(FudgetAnchorLayout);
public:
    /// <summary>
    /// Anchor setting of a control's left side with a given child index. The side of the control will be
    /// at some distance relative to the container's anchor side.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <returns>The anchor side in the container</returns>
    API_FUNCTION() FudgetHorizontalAnchor GetLeftAnchor(int index) const;

    /// <summary>
    /// Anchor setting of a control's left side with a given child index. The side of the control will be
    /// at some distance relative to the container's anchor side.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <param name="value">The anchor side in the container</param>
    API_FUNCTION() void SetLeftAnchor(int index, FudgetHorizontalAnchor value);

    /// <summary>
    /// Anchor setting of a control's right side with a given child index. The right side of the control will be
    /// at some distance relative to the container's anchor side.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <returns>The anchor side in the container</returns>
    API_FUNCTION() FudgetHorizontalAnchor GetRightAnchor(int index)  const;

    /// <summary>
    /// Anchor setting of a control's right side with a given child index. The side of the control will be
    /// at some distance relative to the container's anchor side.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <param name="value">The anchor side in the container</param>
    API_FUNCTION() void SetRightAnchor(int index, FudgetHorizontalAnchor value);

    /// <summary>
    /// Anchor setting of a control's top side with a given child index. The side of the control will be
    /// at some distance relative to the container's anchor side.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <returns>The anchor side in the container</returns>
    API_FUNCTION() FudgetVerticalAnchor GetTopAnchor(int index)  const;

    /// <summary>
    /// Anchor setting of a control's top side with a given child index. The side of the control will be
    /// at some distance relative to the container's anchor side.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <param name="value">The anchor side in the container</param>
    API_FUNCTION() void SetTopAnchor(int index, FudgetVerticalAnchor value);

    /// <summary>
    /// Anchor setting of a control's bottom side with a given child index. The side of the control will be
    /// at some distance relative to the container's anchor side.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <returns>The anchor side in the container</returns>
    API_FUNCTION() FudgetVerticalAnchor GetBottomAnchor(int index)  const;

    /// <summary>
    /// Anchor setting of a control's bottom side with a given child index. The side of the control will be
    /// at some distance relative to the container's anchor side.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <param name="value">The anchor side in the container</param>
    API_FUNCTION() void SetBottomAnchor(int index, FudgetVerticalAnchor value);

    /// <summary>
    /// Distance of a control's left side to the anchor side of the container, where 100% is the container's
    /// full width.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <returns>Percent distance from the anchor</returns>
    API_FUNCTION() float GetLeftPercent(int index)  const;
    /// <summary>
    /// Distance of a control's left side to the anchor side of the container, where 100% is the container's
    /// full width.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <param name="value">Percent distance from the anchor</param>
    API_FUNCTION() void SetLeftPercent(int index, float value);
    /// <summary>
    /// Distance of a control's right side to the anchor side of the container, where 100% is the container's
    /// full width.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <returns>Percent distance from the anchor</returns>
    API_FUNCTION() float GetRightPercent(int index)  const;
    /// <summary>
    /// Distance of a control's right side to the anchor side of the container, where 100% is the container's
    /// full width.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <param name="value">Percent distance from the anchor</param>
    API_FUNCTION() void SetRightPercent(int index, float value);
    /// <summary>
    /// Distance of a control's top side to the anchor side of the container, where 100% is the container's
    /// full height.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <returns>Percent distance from the anchor</returns>
    API_FUNCTION() float GetTopPercent(int index)  const;
    /// <summary>
    /// Distance of a control's topside to the anchor side of the container, where 100% is the container's
    /// full height.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <param name="value">Percent distance from the anchor</param>
    API_FUNCTION() void SetTopPercent(int index, float value);
    /// <summary>
    /// Distance of a control's bottom side to the anchor side of the container, where 100% is the container's
    /// full height.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <returns>Percent distance from the anchor</returns>
    API_FUNCTION() float GetBottomPercent(int index)  const;
    /// <summary>
    /// Distance of a control's bottom side to the anchor side of the container, where 100% is the container's
    /// full height.
    /// </summary>
    /// <param name="index">Index of the control in its owner container</param>
    /// <param name="value">Percent distance from the anchor</param>
    API_FUNCTION() void SetBottomPercent(int index, float value);

protected:
    /// <inheritdoc />
    void LayoutChildren(Float2 space) override;

    /// <summary>
    /// The slot of a child control at an index in its container
    /// </summary>
    /// <param name="index">Control's index in its parent</param>
    /// <returns>The slot holding layout specific data for the control</returns>
    API_FUNCTION() FudgetAnchorLayoutSlot* GetSlot(int index);

    const FudgetAnchorLayoutSlot* GetSlot(int index) const;

    /// <inheritdoc />
    FudgetLayoutFlag GetInitFlags() const override;
    /// <inheritdoc />
    FudgetLayoutSlot* CreateSlot(FudgetControl *control) override;
private:
};
