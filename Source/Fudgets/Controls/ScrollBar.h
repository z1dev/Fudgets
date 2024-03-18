#pragma once

#include "../Control.h"

class FudgetScrollBarPainter;
enum class FudgetScrollBarOrientation : uint8;

/// <summary>
/// Visibility setting for scrollbars.
/// </summary>
API_ENUM()
enum class FudgetScrollBarVisibilityMode
{
    /// <summary>
    /// The scrollbar is always visible.
    /// </summary>
    Visible,
    /// <summary>
    /// The scrollbar is always hidden.
    /// </summary>
    Hidden,
    /// <summary>
    /// The scrollbar changes its visibility depending on avaiable space and its size. It is only
    /// visible when its full range is larger than its page size.
    /// </summary>
    Automatic,
};


/// <summary>
/// Provider of scrollbar calculations and drawing in controls that show a scrollbar, whether they have a scrollbar layout
/// or just show one as a visual part of the control.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetScrollBarComponent : public ScriptingObject
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE(FudgetScrollBarComponent);
public:
    /// <summary>
    /// Call from the control that created the scroll bar component for setting it as the owner.
    /// </summary>
    /// <param name="owner">The control that will own this scroll bar component.</param>
    /// <returns></returns>
    API_FUNCTION() void Initialize(FudgetControl *owner);
    /// <summary>
    /// Call from the owner control when the scrollbar painter's style needs to be initialized.
    /// </summary>
    API_FUNCTION() void StyleInitialize();

    /// <summary>
    /// Updates the bounds of the scrollbar where drawing and input handling will take place.
    /// </summary>
    /// <param name="bounds">The new bounding rectangle of the scrollbar.</param>
    API_FUNCTION() FORCE_INLINE void SetBounds(const Rectangle &bounds) { _bounds = bounds; _rects_dirty = true; }

    /// <summary>
    /// Draws the scrollbar in the specified rectangle.
    /// </summary>
    /// <param name="bounds">Rectangle to draw the scrollbar in</param>
    API_FUNCTION() void Draw();

    /// <summary>
    /// Returns the scrollbar's current visibility.
    /// </summary>
    /// <returns>Whether the scrollbar is visible with its current settings or not</returns>
    API_PROPERTY() bool IsVisible() const { return _visible; }

    /// <summary>
    /// Gets the thickness of the scrollbar.
    /// </summary>
    API_PROPERTY() int GetWidth() const;

    /// <summary>
    /// Gets the size of the range that is visible in the scrollbar's owner.
    /// </summary>
    API_PROPERTY() int64 GetPageSize() const { return _page_size; }
    /// <summary>
    /// Sets the size of the range that is visible in the scrollbar's owner.
    /// </summary>
    /// <param name="value">The new page size to set</param>
    API_PROPERTY() void SetPageSize(int64 value);

    /// <summary>
    /// Gets the range of the contents to be scrolled by this scrollbar.
    /// </summary>
    API_PROPERTY() int64 GetScrollRange() const { return _range; }
    /// <summary>
    /// Sets the range of the contents to be scrolled by this scrollbar.
    /// </summary>
    /// <param name="value">The new range size to set</param>
    API_PROPERTY() void SetScrollRange(int64 value);

    /// <summary>
    /// Gets the visibility mode that determines whether a scrollbar is visible based on its current values.
    /// </summary>
    /// <returns></returns>
    API_PROPERTY() FudgetScrollBarVisibilityMode GetVisibilityMode() const { return _visibility_mode; }
    /// <summary>
    /// Sets the visibility mode that determines whether a scrollbar is visible based on its current values.
    /// </summary>
    /// <param name="value">The new visibility mode</param>
    API_PROPERTY() void SetVisibilityMode(FudgetScrollBarVisibilityMode value);
private:
    void UpdateVisibility();
    // Call when a change makes one of the rectangles for drawing and input handling dirty.
    void RecalculateRectangles();

    FudgetControl *_owner;

    FudgetScrollBarPainter *_painter;

    FudgetScrollBarOrientation _orientation;

    /// <summary>
    /// Position and size of the scrollbar.
    /// </summary>
    Rectangle _bounds;

    /// <summary>
    /// Whether the rectangles for each part of the scrollbar need to be calculated again.
    /// </summary>
    bool _rects_dirty;
    /// <summary>
    /// Calculated rectangle of the track for the whole scrollbar.
    /// </summary>
    Rectangle _track_rect;
    /// <summary>
    /// Calculated rectangle of the scrollbar track before the thumb button.
    /// </summary>
    Rectangle _before_track_rect;
    /// <summary>
    /// Calculated rectangle of the scrollbar track after the thumb button.
    /// </summary>
    Rectangle _after_track_rect;
    /// <summary>
    /// Calculated rectangle of the thumb button.
    /// </summary>
    Rectangle _thumb_rect;
    /// <summary>
    /// Rectangles to store the position of each button of the scrollbar. Only the available number of
    /// buttons will have a valid rectangle.
    /// </summary>
    Rectangle _btn_rects[20];
    /// <summary>
    /// Maximum value of scrolling range. Usually scrolling is limited to range minus _page_size.
    /// </summary>
    int64 _range;
    /// <summary>
    /// Current scroll position between 0 and range.
    /// </summary>
    int64 _pos;
    /// <summary>
    /// Size of the visible part of the scroll range.
    /// </summary>
    int64 _page_size;

    /// <summary>
    /// Whether the scroll bar is shown with its current settings or not.
    /// </summary>
    bool _visible;

    FudgetScrollBarVisibilityMode _visibility_mode;
};

