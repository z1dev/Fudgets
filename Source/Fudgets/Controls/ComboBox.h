#pragma once

#include "../GUIRoot.h"
#include "../Styling/PartPainters.h"
#include "../Layouts/ProxyLayout.h"

class FudgetLineEdit;
class FudgetButton;


/// <summary>
/// Standard combo box control that has an editable part, a button and opens a drop down window with a list of choices.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetComboBox : public FudgetContainer, public IProxyLayoutContainer
{
    using Base = FudgetContainer;
    DECLARE_SCRIPTING_TYPE(FudgetComboBox);
public:
    /// <summary>
    /// Class Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetClassToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetFrameDrawToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetFocusedFrameDrawToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetDisabledFrameDrawToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetFramePainterToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetFrameStyleToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetButtonContentStyleToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetEditorClassToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetButtonClassToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetButtonImageToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetButtonHoveredImageToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetButtonDisabledImageToken();
    /// <summary>
    /// Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetButtonWidthToken();

    /// <inheritdoc />
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
    /// <inheritdoc />
    void OnMouseMove(Float2 pos, Float2 global_pos) override;
    /// <inheritdoc />
    void OnMouseEnter(Float2 pos, Float2 global_pos) override;
    /// <inheritdoc />
    void OnMouseLeave() override;
    /// <inheritdoc />
    void OnMouseCaptured() override;
    /// <inheritdoc />
    void OnMouseReleased() override;

    /// <inheritdoc />
    FudgetLayoutSlot* ProxyInterfaceCreateSlot(FudgetControl *control) override;
    /// <inheritdoc />
    FudgetLayoutFlag ProxyInterfaceGetInitFlags() const override;
    /// <inheritdoc />
    bool ProxyInterfaceLayoutChildren() override;
    /// <inheritdoc />
    bool ProxyInterfaceMeasure(Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size) override;

    /// <inheritdoc />
    bool WantsNavigationKey(KeyboardKeys key) override;
protected:
    /// <inheritdoc />
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
