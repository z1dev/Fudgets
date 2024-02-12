#pragma once

#include "../GUIRoot.h"
#include "../Styling/PartPainters.h"
#include "../Layouts/ProxyLayout.h"

class FudgetLineEdit;
class FudgetButton;


API_CLASS()
class FUDGETS_API FudgetComboBox : public FudgetContainer, public IProxyLayoutContainer
{
    using Base = FudgetContainer;
    DECLARE_SCRIPTING_TYPE(FudgetComboBox);
public:

    API_PROPERTY() static FudgetToken GetClassToken();
    API_PROPERTY() static FudgetToken GetFrameDrawToken();
    API_PROPERTY() static FudgetToken GetFocusedFrameDrawToken();
    API_PROPERTY() static FudgetToken GetDisabledFrameDrawToken();
    API_PROPERTY() static FudgetToken GetFramePainterToken();
    API_PROPERTY() static FudgetToken GetFrameStyleToken();
    API_PROPERTY() static FudgetToken GetButtonContentStyleToken();
    API_PROPERTY() static FudgetToken GetEditorClassToken();
    API_PROPERTY() static FudgetToken GetButtonClassToken();
    API_PROPERTY() static FudgetToken GetButtonImageToken();
    API_PROPERTY() static FudgetToken GetButtonHoveredImageToken();
    API_PROPERTY() static FudgetToken GetButtonDisabledImageToken();
    API_PROPERTY() static FudgetToken GetButtonWidthToken();

    void OnInitialize() override;

    /// <inheritdoc />
    void OnDraw() override;
    /// <inheritdoc />
    void OnFocusChanged(bool focused, FudgetControl *other) override;
    /// <inheritdoc />
    void OnVirtuallyEnabledChanged() override;
    /// <inheritdoc />
    FudgetInputResult OnCharInput(Char ch) override;
    /// <inheritdoc />
    FudgetInputResult OnKeyDown(KeyboardKeys key) override;
    /// <inheritdoc />
    bool OnKeyUp(KeyboardKeys key) override;
    /// <inheritdoc />
    FudgetInputResult OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click) override;
    /// <inheritdoc />
    bool OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button) override;

    void OnMouseMove(Float2 pos, Float2 global_pos) override;
    void OnMouseEnter(Float2 pos, Float2 global_pos) override;
    void OnMouseLeave() override;
    void OnMouseCaptured() override;
    void OnMouseReleased() override;

    FudgetLayoutSlot* ProxyInterfaceCreateSlot(FudgetControl *control) override;
    FudgetLayoutFlag ProxyInterfaceGetInitFlags() const override;
    bool ProxyInterfaceLayoutChildren() override;
    Float2 ProxyInterfaceRequestSize(FudgetSizeType type) const override;

    bool WantsNavigationKey(KeyboardKeys key) override;
protected:
    FudgetControlFlags GetInitFlags() const override;
private:
    static void InitializeTokens();

    static FudgetToken ClassToken;

    static FudgetToken FrameDrawToken;
    static FudgetToken FocusedFrameDrawToken;
    static FudgetToken DisabledFrameDrawToken;

    static FudgetToken FramePainterToken;
    static FudgetToken FrameStyleToken;

    static FudgetToken ButtonContentStyleToken;

    static FudgetToken EditorClassToken;
    static FudgetToken ButtonClassToken;

    static FudgetToken ButtonImageToken;
    static FudgetToken ButtonHoveredImageToken;
    static FudgetToken ButtonDisabledImageToken;

    static FudgetToken ButtonWidthToken;

    FudgetPadding GetInnerPadding() const;

    void HandleEnterLeaveMouse(Float2 pos, Float2 global_pos, bool on_enter);

    FudgetPainterStateHelper _draw_state;

    /// <summary>
    /// The layout that lets controls do their own layouts
    /// </summary>
    FudgetProxyLayout *_layout;

    FudgetFramedFieldPainter *_frame_painter;

    float _button_width;

    /// <summary>
    /// LineEdit control for input
    /// </summary>
    FudgetLineEdit *_editor;
    /// <summary>
    /// Button for the drop down arrow
    /// </summary>
    FudgetButton *_button;

    bool _editor_capturing;
    Float2 _last_mouse_pos;
};