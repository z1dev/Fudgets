#pragma once

#include "../Container.h"
#include "../Styling/PartPainters.h"


API_CLASS()
class FUDGETS_API FudgetListBox : public FudgetContainer
{
    using Base = FudgetContainer;
    DECLARE_SCRIPTING_TYPE(FudgetListBox);
public:
    /// <summary>
    /// Initializes tokens used by the styles
    /// </summary>
    API_FUNCTION() static void InitializeTokens();
    /// <summary>
    /// Frame Painter token
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken FramePainterToken;
    /// <summary>
    /// Frame Style token
    /// </summary>
    API_FIELD(ReadOnly) static FudgetToken FrameStyleToken;

    /// <inheritdoc />
    void OnInitialize() override;

    /// <inheritdoc />
    void OnDraw() override;

private:
    FudgetFramedFieldPainter *_frame_painter;
};
