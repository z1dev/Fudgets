#pragma once

#include "Layout.h"


API_ENUM()
enum class FudgetHorizontalAnchor
{
    None,
    Left,
    Right,
};

API_ENUM()
enum class  FudgetVerticalAnchor
{
    None,
    Top,
    Bottom,
};


API_CLASS()
class FUDGETS_API FudgetAnchorLayoutSlot : public FudgetLayoutSlot
{
    using Base = FudgetLayoutSlot;
    DECLARE_SCRIPTING_TYPE(FudgetAnchorLayoutSlot);

public:
    API_FIELD() FudgetHorizontalAnchor leftAnchor;
    API_FIELD() FudgetHorizontalAnchor rightAnchor;
    API_FIELD() FudgetVerticalAnchor topAnchor;
    API_FIELD() FudgetVerticalAnchor bottomAnchor;

    API_FIELD() float leftPercent;
    API_FIELD() float rightPercent;
    API_FIELD() float topPercent;
    API_FIELD() float bottomPercent;
};

API_CLASS()
class FUDGETS_API FudgetAnchorLayout : public FudgetLayout
{
    using Base = FudgetLayout;
    DECLARE_SCRIPTING_TYPE(FudgetAnchorLayout);
public:

    API_FUNCTION() FudgetHorizontalAnchor GetLeftAnchor(int index) const;
    API_FUNCTION() void SetLeftAnchor(int index, FudgetHorizontalAnchor value);
    API_FUNCTION() FudgetHorizontalAnchor GetRightAnchor(int index)  const;
    API_FUNCTION() void SetRightAnchor(int index, FudgetHorizontalAnchor value);
    API_FUNCTION() FudgetVerticalAnchor GetTopAnchor(int index)  const;
    API_FUNCTION() void SetTopAnchor(int index, FudgetVerticalAnchor value);
    API_FUNCTION() FudgetVerticalAnchor GetBottomAnchor(int index)  const;
    API_FUNCTION() void SetBottomAnchor(int index, FudgetVerticalAnchor value);
    API_FUNCTION() float GetLeftPercent(int index)  const;
    API_FUNCTION() void SetLeftPercent(int index, float value);
    API_FUNCTION() float GetRightPercent(int index)  const;
    API_FUNCTION() void SetRightPercent(int index, float value);
    API_FUNCTION() float GetTopPercent(int index)  const;
    API_FUNCTION() void SetTopPercent(int index, float value);
    API_FUNCTION() float GetBottomPercent(int index)  const;
    API_FUNCTION() void SetBottomPercent(int index, float value);

protected:
    /// <inheritdoc />
    bool LayoutChildren() override;

    API_FUNCTION() FudgetAnchorLayoutSlot* GetSlot(int index);
    const FudgetAnchorLayoutSlot* GetSlot(int index) const;

    /// <inheritdoc />
    FudgetLayoutFlag GetInitFlags() const override;
    /// <inheritdoc />
    FudgetLayoutSlot* CreateSlot(FudgetControl *control) override;
private:
};
