#pragma once

//#include "../Control.h"
#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Input/Enums.h"
#include "Engine/Core/Math/Rectangle.h"

class FudgetScrollBarPainter;
class FudgetScrollBarComponent;
class FudgetControl;

/// <summary>
/// Horizontal or vertical orientation of a scrollbar.
/// </summary>
API_ENUM()
enum class FudgetScrollBarOrientation : uint8
{
    /// <summary>
    /// Scrollbar for horizontal scrolling
    /// </summary>
    Horizontal,
    /// <summary>
    /// Scrollbar for vertical scrolling
    /// </summary>
    Vertical,
};


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
/// Roles of scrollbar buttons for default behavior. They need to be converted to integer when set as the role.
/// The roles are not limited to these values.
/// </summary>
API_ENUM()
enum class FudgetScrollBarButtonRole
{
    /// <summary>
    /// No role for the button. Its behavior will be determined by the event handler object.
    /// </summary>
    None,
    /// <summary>
    /// Step one line up (or left).
    /// </summary>
    LineUp,
    /// <summary>
    /// Step one line down (or right).
    /// </summary>
    LineDown,
    /// <summary>
    /// Step one page up (or left).
    /// </summary>
    PageUp,
    /// <summary>
    /// Step one page down (or right).
    /// </summary>
    PageDown,
    /// <summary>
    /// Step one page up (or left), while keeping a single line visible from the last page.
    /// </summary>
    PageUpLine,
    /// <summary>
    /// Step one page down (or right), while keeping a single line visible from the last page.
    /// </summary>
    PageDownLine,
    /// <summary>
    /// Go to the start of the scroll range.
    /// </summary>
    JumpToStart,
    /// <summary>
    /// Go to the end of the scroll range minus the page size.
    /// </summary>
    JumpToEnd,
};

/// <summary>
/// Interface type that can be implemented by objects that want to use a scrollbar and get notified about user
/// interaction or if the scrollbar changes.
/// </summary>
API_INTERFACE()
class FUDGETS_API IFudgetScollBarOwner
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(IFudgetScollBarOwner)

    /// <summary>
    /// Event called when a scrollbar's scroll position has changed.
    /// </summary>
    /// <param name="scrollbar">The scrollbar calling the event</param>
    /// <param name="old_scroll_pos">The scollbar position before the change</param>
    /// <param name="tracking">Whether this is the result of the scrollbar's thumb being dragged, as opposed to pressing one
    /// of the buttons or changing it manually</param>
    API_FUNCTION() virtual void OnScrollBarScroll(FudgetScrollBarComponent *scrollbar, int64 old_scroll_pos, bool tracking) = 0;

    /// <summary>
    /// Event called when one of the scrollbar buttons was pressed.
    /// </summary>
    /// <param name="scrollbar">The scrollbar calling the event</param>
    /// <param name="button_index">The button index between 0 and 19</param>
    /// <param name="before_track">Whether the button is before the track or after it.</param>
    /// <param name="double_click">Whether the call is the result of a double-click.</param>
    API_FUNCTION() virtual void OnScrollBarButtonPressed(FudgetScrollBarComponent *scrollbar, int button_index, bool before_track, bool double_click) = 0;
    /// <summary>
    /// Event called when one of the scrollbar buttons was released after it was pressed.
    /// </summary>
    /// <param name="scrollbar">The scrollbar calling the event</param>
    /// <param name="button_index">The button index between 0 and 19</param>
    /// <param name="before_track">Whether the button is before the track or after it.</param>
    API_FUNCTION() virtual void OnScrollBarButtonReleased(FudgetScrollBarComponent *scrollbar, int button_index, bool before_track) = 0;

    /// <summary>
    /// Event called when the track of the scrollbar was pressed.
    /// </summary>
    /// <param name="scrollbar">The scrollbar calling the event</param>
    /// <param name="before_track">Whether the track part is before the thumb or after it</param>
    /// <param name="double_click">Whether the call is the result of a double-click.</param>
    API_FUNCTION() virtual void OnScrollBarTrackPressed(FudgetScrollBarComponent *scrollbar, bool before_track, bool double_click) = 0;

    /// <summary>
    /// Event called when the track of the scrollbar was released.
    /// </summary>
    /// <param name="scrollbar">The scrollbar calling the event</param>
    /// <param name="before_track">Whether the track part is before the thumb or after it</param>
    API_FUNCTION() virtual void OnScrollBarTrackReleased(FudgetScrollBarComponent *scrollbar, bool before_track) = 0;

    /// <summary>
    /// Event called when the thumb button of the scrollbar was pressed.
    /// </summary>
    /// <param name="scrollbar">The scrollbar calling the event</param>
    /// <param name="double_click">Whether the call is the result of a double-click.</param>
    API_FUNCTION() virtual void OnScrollBarThumbPressed(FudgetScrollBarComponent *scrollbar, bool double_click) = 0;

    /// <summary>
    /// Event called when the thumb button of the scrollbar was released.
    /// </summary>
    /// <param name="scrollbar">The scrollbar calling the event</param>
    API_FUNCTION() virtual void OnScrollBarThumbReleased(FudgetScrollBarComponent *scrollbar) = 0;

    /// <summary>
    /// Event called when a button or track was clicked and it has a role that's not 0. This is called after the OnScrollBar***Pressed event.
    /// </summary>
    /// <param name="scrollbar">The scrollbar calling the event</param>
    /// <param name="role">The role of the button or track that was clicked.</param>
    /// <returns>Whether the event was handled by this call. Returning true will prevent using the default behavior.</returns>
    API_FUNCTION() virtual bool OnRole(FudgetScrollBarComponent *scrollbar, int role) = 0;

    /// <summary>
    /// Event called when a scrollbar with automatic visibility appeared due to range or page size change. The ecent might be
    /// delayed until the next layout calculation. 
    /// </summary>
    /// <param name="scrollbar">The scrollbar calling the event</param>
    API_FUNCTION() virtual void OnScrollBarShown(FudgetScrollBarComponent *scrollbar) = 0;

    /// <summary>
    /// Event called when a scrollbar with automatic visibility disappeared due to range or page size change. The ecent might be
    /// delayed until the next layout calculation. 
    /// </summary>
    /// <param name="scrollbar">The scrollbar calling the event</param>
    API_FUNCTION() virtual void OnScrollBarHidden(FudgetScrollBarComponent *scrollbar) = 0;
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
    ~FudgetScrollBarComponent();

    /// <summary>
    /// Call from the control that created the scroll bar component for setting it as the owner.
    /// </summary>
    /// <param name="owner">The control that will own this scroll bar component.</param>
    /// <param name="event_owner">The interface that should receive scrollbar change events.
    /// This is usually the same as the owner.</param>
    /// <param name="orientation">The orientation which determines if the scrollbar scrolls horizontally or vertically.</param>
    /// <returns></returns>
    API_FUNCTION() void Initialize(FudgetControl *owner, IFudgetScollBarOwner *event_owner, FudgetScrollBarOrientation orientation);
    /// <summary>
    /// Call from the owner control when the scrollbar painter's style needs to be initialized.
    /// </summary>
    API_FUNCTION() void StyleInitialize();

    /// <summary>
    /// The orientation of the scrollbar, which determines how the buttons and the track are laid out and
    /// if it scrolls horizontally or vertically.
    /// </summary>
    /// <returns>The scrollbar's orientation.</returns>
    API_PROPERTY() FudgetScrollBarOrientation GetOrientation() const { return _orientation; }

    /// <summary>
    /// The orientation of the scrollbar, which determines if it scrolls horizontally or vertically.
    /// </summary>
    /// <param name="value">The new orientation of the scrollbar.</param>
    API_PROPERTY() void SetOrientation(FudgetScrollBarOrientation value);

    /// <summary>
    /// Returns the bounds of the scrollbar where drawing and input handling will take place.
    /// </summary>
    API_PROPERTY() FORCE_INLINE Rectangle GetBounds() const { return _bounds; }
    /// <summary>
    /// Updates the bounds of the scrollbar where drawing and input handling will take place.
    /// </summary>
    /// <param name="bounds">The new bounding rectangle of the scrollbar.</param>
    API_PROPERTY() FORCE_INLINE void SetBounds(const Rectangle &bounds) { _bounds = bounds; _rects_dirty = true; }

    /// <summary>
    /// Draws the scrollbar. The scrollbar should already have a bounding rectangle by calling SetBounds.
    /// </summary>
    API_FUNCTION() void Draw();

    /// <summary>
    /// Returns the scrollbar's current visibility.
    /// </summary>
    /// <returns>Whether the scrollbar is visible with its current settings or not</returns>
    API_PROPERTY() bool IsVisible() const { return _visible; }

    /// <summary>
    /// Returns the visibility of the scrollbar if it is not in automatic visibility mode. Otherwise returns the
    /// value of the visible argument.
    /// </summary>
    /// <param name="visible">Whether the automatic visibility scrollbar should be shown.</param>
    /// <returns>The visibility of the scrollbar with the requested visibility state</returns>
    API_FUNCTION() bool GetAssumedVisible(bool visible) const;

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
    /// Gets the size of a line when navigating the scrollbar going a line up or down.
    /// </summary>
    API_PROPERTY() int64 GetLineSize() const { return _line_size; }
    /// <summary>
    /// Sets the size of a line when navigating the scrollbar going a line up or down.
    /// </summary>
    /// <param name="value">The new line size to set</param>
    API_PROPERTY() void SetLineSize(int64 value) { _line_size = Math::Max(0LL, value); }

    /// <summary>
    /// Gets the lower end of the range where the scrollbar can scroll.
    /// </summary>
    API_PROPERTY() int64 GetMinScrollRange() const { return _range_min; }
    /// <summary>
    /// Sets the lower end of the range where the scrollbar can scroll. Modifies the maximum range as well as the current position
    /// if those were lower than this value.
    /// </summary>
    /// <param name="value">The new minimum range.</param>
    API_PROPERTY() void SetMinScrollRange(int64 value);

    /// <summary>
    /// Gets the high end of the range where the scrollbar can scroll. The scroll position will be lower than this value.
    /// </summary>
    API_PROPERTY() int64 GetMaxScrollRange() const { return _range_max; }
    /// <summary>
    /// Sets the high end of the range where the scrollbar can scroll. Modifies the minimum range as well as the current position
    /// if those were higher than this value.
    /// </summary>
    /// <param name="value">The new minimum range.</param>
    API_PROPERTY() void SetMaxScrollRange(int64 value);

    /// <summary>
    /// Gets the extent of the range of the contents to be scrolled by this scrollbar.
    /// </summary>
    API_PROPERTY(Attributes="HideInEditor") int64 GetScrollRange() const { return _range_max - _range_min + 1; }
    /// <summary>
    /// Sets the extent of the range of the contents to be scrolled by this scrollbar. This sets the max range to value
    /// higher than min range.
    /// </summary>
    /// <param name="value">The new range size to set</param>
    API_PROPERTY(Attributes="HideInEditor") void SetScrollRange(int64 value);

    /// <summary>
    /// Gets the current position in the scrollbar between the minimum and maximum range.
    /// </summary>
    API_PROPERTY() int64 GetScrollPos() const { return _scroll_pos; }

    /// <summary>
    /// Sets the current position in the scrollbar between the minimum and maximum range.
    /// The value will be clamped if it's outside the range.
    /// </summary>
    /// <param name="value"></param>
    /// <returns></returns>
    API_PROPERTY() void SetScrollPos(int64 value);

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

    /// <summary>
    /// Returns whether the scrollbar's owner is currently capturing the mouse and the scrollbar wants to
    /// intercept all mouse input.
    /// </summary>
    /// <returns></returns>
    API_PROPERTY() FORCE_INLINE bool MouseIsCaptured() const { return _mouse_capture != MouseCapture::None; }

    /// <summary>
    /// The mouse move event for the scrollbar.
    /// </summary>
    /// <param name="pos">Mouse pointer position relative to the owner of the scrollbar, not to the scrollbar.</param>
    /// <param name="global_pos">Global mouse pointer position</param>
    /// <returns>Whether the scrollbar wants to use the event and doesn't want the control to handle it.</returns>
    API_FUNCTION() bool MouseMove(Float2 pos, Float2 global_pos);
    /// <summary>
    /// The mouse down event for the scrollbar.
    /// </summary>
    /// <param name="pos">Mouse pointer position relative to the owner of the scrollbar, not to the scrollbar.</param>
    /// <param name="global_pos">Global mouse pointer position</param>
    /// <param name="button">The pressed mouse button.</param>
    /// <param name="double_click">Whether this is an event for double clicking.</param>
    /// <returns>Whether the scrollbar wants to use the event and doesn't want the control to handle it.</returns>
    API_FUNCTION() bool MouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click);
    /// <summary>
    /// The mouse up event for the scrollbar.
    /// </summary>
    /// <param name="pos">Mouse pointer position relative to the owner of the scrollbar, not to the scrollbar.</param>
    /// <param name="global_pos">Global mouse pointer position</param>
    /// <param name="button">The released mouse button.</param>
    /// <returns>Whether the scrollbar wants to use the event and doesn't want the control to handle it.</returns>
    API_FUNCTION() bool MouseUp(Float2 pos, Float2 global_pos, MouseButton button);
    /// <summary>
    /// The mouse leave event for the scrollbar.
    /// </summary>
    /// <returns>Whether the scrollbar wants to use the event and doesn't want the control to handle it.</returns>
    API_FUNCTION() bool MouseLeave();

    /// <summary>
    /// Returns the role set for the part of the track before the thumb button. The role can be used to have
    /// a specific behavior when clicking it. Roles matching the values in FudgetScrollBarButtonRole come with
    /// a default behavior but they can be overriden.
    /// </summary>
    /// <returns>The role set for the track before the thumb button.</returns>
    API_PROPERTY() int GetBeforeTrackRole() const;
    /// <summary>
    /// Returns the role set for the part of the track after the thumb button. The role can be used to have
    /// a specific behavior when clicking it. Roles matching the values in FudgetScrollBarButtonRole come with
    /// a default behavior but they can be overriden.
    /// </summary>
    /// <returns>The role set for the track after the thumb button.</returns>
    API_PROPERTY() int GetAfterTrackRole() const;

    /// <summary>
    /// Returns the role set for a button with index between 0 and 19. The role can be used to have a specific
    /// behavior when clicking it. Roles matching the values in FudgetScrollBarButtonRole come with a default
    /// behavior but they can be overriden.
    /// </summary>
    /// <param name="button_index">The index of the button to get the role for.</param>
    /// <returns>The role set for the button at index.</returns>
    API_FUNCTION() int GetButtonRole(int button_index) const;

    /// <summary>
    /// Handles the given role if it's one of the default roles from FudgetScrollBarButtonRole.
    /// </summary>
    /// <param name="role">The role to handle. Values outside of FudgetScrollBarButtonRole are ignored</param>
    API_FUNCTION() void HandleRole(int role);
private:
    FORCE_INLINE float Relevant(Float2 size) const { return _orientation == FudgetScrollBarOrientation::Horizontal ? size.X : size.Y; }
    FORCE_INLINE float Opposite(Float2 size) const { return _orientation == FudgetScrollBarOrientation::Horizontal ? size.Y : size.X; }
    FORCE_INLINE float &RelevantRef(Float2 &size) const { return _orientation == FudgetScrollBarOrientation::Horizontal ? size.X : size.Y; }
    FORCE_INLINE float &OppositeRef(Float2 &size) const { return _orientation == FudgetScrollBarOrientation::Horizontal ? size.Y : size.X; }

    void UpdateVisibility();
    // Call when a change makes one of the rectangles for drawing and input handling dirty.
    void RecalculateRectangles();

    FudgetControl *_owner;
    IFudgetScollBarOwner *_event_owner;

    FudgetScrollBarPainter *_painter;
    FudgetScrollBarOrientation _orientation;

    bool style_inited;

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
    /// Minimum value of scrolling range. Usually scrolling is limited to range minus _page_size.
    /// </summary>
    int64 _range_min;
    /// <summary>
    /// Maximum value of scrolling range. Usually scrolling is limited to range minus _page_size.
    /// </summary>
    int64 _range_max;
    /// <summary>
    /// Current scroll position between 0 and range.
    /// </summary>
    int64 _scroll_pos;
    /// <summary>
    /// Size of the visible part of the scroll range.
    /// </summary>
    int64 _page_size;
    /// <summary>
    /// Size of a single line to step when using the line up or line down roles.
    /// </summary>
    int64 _line_size;

    /// <summary>
    /// Whether the scroll bar is shown with its current settings or not.
    /// </summary>
    bool _visible;

    enum class MouseCapture
    {
        None,
        Thumb,
        Button1, Button2, Button3, Button4, Button5,
        Button6, Button7, Button8, Button9, Button10,
        Button11, Button12, Button13, Button14, Button15,
        Button16, Button17, Button18, Button19, Button20,
        BeforeTrack, AfterTrack
    };
    MouseCapture _mouse_capture;
    Float2 _old_mouse_pos;
    float _thumb_mouse_pos;

    FudgetScrollBarVisibilityMode _visibility_mode;
};

