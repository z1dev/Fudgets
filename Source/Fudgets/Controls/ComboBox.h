#pragma once

#include "../GUIRoot.h"
//#include "../Styling/Painters/PartPainters.h"
#include "../Layouts/ProxyLayout.h"

class FudgetLineEdit;
class FudgetButton;
class FudgetListBox;
class FudgetStringListProvider;
class FudgetDrawablePainter;



/// <summary>
/// Standard combo box control that has an editable part, a button and opens a drop down window with a list of choices.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetComboBox : public FudgetContainer, public IProxyLayoutContainer
{
    using Base = FudgetContainer;
    DECLARE_SCRIPTING_TYPE(FudgetComboBox);
public:
    ~FudgetComboBox();

    /// <inheritdoc />
    void OnInitialize() override;
    /// <inheritdoc />
    void OnStyleInitialize() override;

    /// <inheritdoc />
    void OnDraw() override;

    ///// <inheritdoc />
    //void OnFocusChanged(bool focused, FudgetControl *other) override;
    ///// <inheritdoc />
    //void OnVirtuallyEnabledChanged() override;

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
    void OnSizeChanged() override;

    /// <inheritdoc />
    FudgetLayoutSlot* ProxyInterfaceCreateSlot(FudgetControl *control) override;
    /// <inheritdoc />
    FudgetLayoutFlag ProxyInterfaceGetInitFlags() const override;
    /// <inheritdoc />
    void ProxyInterfacePreLayoutChildren(Int2 space) override;
    /// <inheritdoc />
    void ProxyInterfaceLayoutChildren(Int2 space) override;

    ///// <inheritdoc />
    //bool ProxyInterfaceMeasure(Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size) override;

    /// <inheritdoc />
    bool ProxyInterfacePlaceControlInSlotRectangle(int index) override { return false; }

    /// <inheritdoc />
    bool WantsNavigationKey(KeyboardKeys key) override;
protected:
    /// <summary>
    /// Handles the press of the combo box button to show the floating list box.
    /// </summary>
    API_FUNCTION() virtual void ButtonPressed();

    /// <inheritdoc />
    FudgetControlFlag GetInitFlags() const override;

    /// <summary>
    /// Padding of the text with the frame padding.
    /// </summary>
    API_PROPERTY() FudgetPadding GetCombinedPadding() const;
private:
    //FudgetPadding GetTextPadding() const;
    //FudgetPadding GetButtonPadding() const;

    bool PosOnEditor(Float2 pos) const;
    bool PosOnButton(Float2 pos) const;

    void HandleEnterLeaveMouse(Float2 pos, Float2 global_pos, bool on_enter);

    /// The layout that lets controls do their own layouts
    FudgetProxyLayout *_layout;

    //FudgetDrawablePainter *_frame_painter;

    FudgetPadding _content_padding;
    int _button_width;

    /// <summary>
    /// LineEdit control for input
    /// </summary>
    FudgetLineEdit *_editor;
    /// <summary>
    /// Button for the drop down arrow
    /// </summary>
    FudgetButton *_button;
    /// <summary>
    /// List box for combo box items list
    /// </summary>
    FudgetListBox *_list_box;

    FudgetStringListProvider *_list_data;

    bool _editor_capturing;
    bool _button_capturing;
    Float2 _last_mouse_pos;
};
