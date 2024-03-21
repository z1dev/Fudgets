#pragma once

#include "Control.h"
#include "Controls/ScrollBar.h"


API_ENUM()
enum class FudgetScrollBars
{
    None,
    Horizontal,
    Vertical,
    Both
};

API_CLASS()
class FUDGETS_API FudgetScrollingControl : public FudgetControl, public IFudgetScollBarOwner
{
    using Base = FudgetControl;
    DECLARE_SCRIPTING_TYPE(FudgetScrollingControl);
public:
    ~FudgetScrollingControl();

    /// <inheritdoc />
    void OnStyleInitialize() override;

    /// <inheritdoc />
    void OnDraw() override;

    /// <inheritdoc />
    FudgetInputResult DoMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click) override;
    /// <inheritdoc />
    bool DoMouseUp(Float2 pos, Float2 global_pos, MouseButton button) override;
    /// <inheritdoc />
    void DoMouseMove(Float2 pos, Float2 global_pos) override;
    /// <inheritdoc />
    void DoMouseLeave() override;

    /// <summary>
    /// Returns which scrollbars are created for the control.
    /// </summary>
    API_PROPERTY() FudgetScrollBars GetScrollBars() const { return _scrollbars; }
    /// <summary>
    /// Returns which scrollbars are created for the control.
    /// </summary>
    API_PROPERTY() void SetScrollBars(FudgetScrollBars value);

    /// <summary>
    /// Returns the horizontal scrollbar if it was created.
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetScrollBarComponent* GetHorizontalScrollBar() const { return _h_scrollbar; }
    /// <summary>
    /// Returns the vertical scrollbar if it was created.
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetScrollBarComponent* GetVerticalScrollBar() const { return _v_scrollbar; }

    /// <inheritdoc />
    void OnScrollBarScroll(FudgetScrollBarComponent *scrollbar, int64 old_scroll_pos, bool tracking) override;
    /// <inheritdoc />
    void OnScrollBarButtonPressed(FudgetScrollBarComponent *scrollbar, int button_index, bool before_track, bool double_click) override;
    /// <inheritdoc />
    void OnScrollBarButtonReleased(FudgetScrollBarComponent *scrollbar, int button_index, bool before_track) override;
    /// <inheritdoc />
    void OnScrollBarTrackPressed(FudgetScrollBarComponent *scrollbar, bool before_track, bool double_click) override;
    /// <inheritdoc />
    void OnScrollBarTrackReleased(FudgetScrollBarComponent *scrollbar, bool before_track) override;
    /// <inheritdoc />
    void OnScrollBarThumbPressed(FudgetScrollBarComponent *scrollbar, bool double_click) override;
    /// <inheritdoc />
    void OnScrollBarThumbReleased(FudgetScrollBarComponent *scrollbar) override;
    /// <inheritdoc />
    void OnScrollBarShown(FudgetScrollBarComponent *scrollbar) override;
    /// <inheritdoc />
    void OnScrollBarHidden(FudgetScrollBarComponent *scrollbar) override;

    /// <summary>
    /// Call when the content extents are out of date and need to be recalculated in the next requested layout frame
    /// or before drawing.
    /// </summary>
    API_FUNCTION() void MarkExtentsDirty() { _dirty_extents = true; }

    /// <summary>
    /// Returns the dimensions that the horizontal and vertical scrollbars take up in bounds. This depends on whether the
    /// scrollbars are created and if they are visible.
    /// </summary>
    /// <returns>Size of scrollbars they would occupy in the control's bounds</returns>
    API_FUNCTION() Int2 GetScrollBarWidths() const;
    /// <summary>
    /// Returns the width of the horizontal and vertical scrollbars based on the passed visibility and their
    /// settings. The visibility is only used for automatic visibility scrollbars. If a scrollbar is not created,
    /// or it's always hidden, the corresponding result will be 0. If the scrollbar is always visible, the result
    /// will always have its size even if the show*** value is false.
    /// </summary>
    /// <param name="show_horizontal">Include the width of the horizontal scrollbar for automatic visibility scrollbars.</param>
    /// <param name="show_vertical">Include the width of the vertical scrollbar for automatic visibility scrollbars.</param>
    /// <returns>The size each scrollbar would occupy within the control's bounds based on the arguments.</returns>
    API_FUNCTION() Int2 GetScrollBarWidths(bool show_horizontal, bool show_vertical) const;

    /// <summary>
    /// Padding inside the bounds of the control occupied by a frame and the scrollbars with the current visibility.
    /// To get the padding without the scrollbars, subtract the corresponding value returned by GetScrollBarWidths()
    /// from the right and bottom padding.
    /// </summary>
    /// <returns>Padding of contents by the frame and scrollbars.</returns>
    FudgetPadding GetFramePadding() const override;
protected:
    /// <inheritdoc />
    void RequestLayout() override;

    API_FUNCTION() virtual void RequestScrollExtents() {}
private:
    // Which scrollbars need to be created.
    FudgetScrollBars _scrollbars;

    FudgetScrollBarComponent *_h_scrollbar;
    FudgetScrollBarComponent *_v_scrollbar;

    // A dirty flag when the contents changed and their size needs to be calculated to be able to show a scrollbar or limit scrolling.
    bool _dirty_extents;
};