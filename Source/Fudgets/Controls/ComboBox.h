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
    void OnRootChanged(FudgetGUIRoot *old_root) override;

    /// <summary>
    /// Gets the data provider currently set for the control.
    /// </summary>
    /// <returns></returns>
    API_PROPERTY() FudgetStringListProvider* GetDataProvider() const { return _data; }

    /// <summary>
    /// Sets a data provider for the control, replacing any data provider previously. Data providers created
    /// by the user must be manually destroyed when they are no longer needed.
    /// </summary>
    /// <param name="value">The new data provider</param>
    API_PROPERTY() void SetDataProvider(FudgetStringListProvider *value);

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

    /// <inheritdoc />
    void DataChangeBegin() override;
    /// <inheritdoc />
    void DataChangeEnd(bool changed) override;
    /// <inheritdoc />
    void DataToBeReset() override;
    /// <inheritdoc />
    void DataReset() override;
    /// <inheritdoc />
    void DataToBeCleared() override;
    /// <inheritdoc />
    void DataCleared() override;
    /// <inheritdoc />
    void DataToBeUpdated(int index) override;
    /// <inheritdoc />
    void DataUpdated(int index) override;
    /// <inheritdoc />
    void DataToBeAdded(int count) override;
    /// <inheritdoc />
    void DataAdded(int count) override;
    /// <inheritdoc />
    void DataToBeRemoved(int index, int count) override;
    /// <inheritdoc />
    void DataRemoved(int index, int count) override;
    /// <inheritdoc />
    void DataToBeInserted(int index, int count) override;
    /// <inheritdoc />
    void DataInserted(int index, int count) override;
private:
    bool PosOnEditor(Float2 pos) const;
    bool PosOnButton(Float2 pos) const;

    void HandleEnterLeaveMouse(Float2 pos, Float2 global_pos, bool on_enter);

    void UnbindEvents(FudgetGUIRoot *root);
    void BindEvents();
    void HandleMouseReleasedEvent(FudgetControl *control);

    FudgetStringListProvider *_data;

    /// The layout that lets controls do their own layouts
    FudgetProxyLayout *_layout;

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
    FudgetListBox *_listbox;

    FudgetStringListProvider *_list_data;

    bool _editor_capturing;
    bool _button_capturing;
    bool _listbox_capturing;
    //bool _start_showing;
    Float2 _last_mouse_pos;
};
