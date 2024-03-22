#pragma once

#include "PartPainters.h"
#include "../../Controls/ScrollBar.h"

class FudgetDrawable;
enum class FudgetScrollBarOrientation : uint8;

/// <summary>
/// Mapping for FudgetScrollBarPainter. Mapping is used to tell a part painter what Ids to look up
/// in its owner control's style.
/// </summary>
API_STRUCT(Attributes)
struct FUDGETS_API FudgetScrollBarPainterMapping
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetScrollBarPainterMapping);

    /// <summary>
    /// Orientation of the scrollbar to be painted. This is not an id. It's a direct setting, because
    /// the painter needs to know whether it should read the horizontal or vertical draw resources
    /// during initialization.
    /// </summary>
    API_FIELD() FudgetScrollBarOrientation Orientation;

    /// <summary>
    /// The id for the scrollbar's integer width. The width is not used for drawing by the painter, but objects can
    /// get it with GetWidth to know how big the scrollbar bounds should be.
    /// </summary>
    API_FIELD() int Width;
    /// <summary>
    /// The id for a bool that determines if the thumb button has a fixed size.
    /// </summary>
    API_FIELD() int IsThumbSizeFixed;
    /// <summary>
    /// The id for an integer that is the size length of the thumb button if it is fixed sized.
    /// </summary>
    API_FIELD() int ThumbSize;
    /// <summary>
    /// The id for an integer that limits the size of the thumb button. As long as there is enough space
    /// the thumb button won't be smaller than the value.
    /// </summary>
    API_FIELD() int MinThumbSize;

    /// <summary>
    /// The id for a background drawable for the scrollbar. It is drawn on the full bounding rectangle when drawing is
    /// requested.
    /// </summary>
    API_FIELD() int BgDraw;
    /// <summary>
    /// The id for a frame drawable for the scrollbar. The frame is drawn separately once the other parts finished
    /// drawing.
    /// </summary>
    API_FIELD() int FrameDraw;
    /// <summary>
    /// The id for the thumb button's drawable that can be dragged and which indicates the current scrolling position.
    /// </summary>
    API_FIELD() int ThumbDraw;
    /// <summary>
    /// The id for the drawable of the track that the thumb button moves on.
    /// </summary>
    API_FIELD() int TrackDraw;
    /// <summary>
    /// The id for a drawable of the left or top part of the track in front of the thumb button. It is drawn after the
    /// full track finished drawing.
    /// </summary>
    API_FIELD() int BeforeTrackDraw;
    /// <summary>
    /// The id for a drawable of the right or bottom part of the track following the thumb button. It is drawn after the
    /// full track finished drawing.
    /// </summary>
    API_FIELD() int AfterTrackDraw;

    /// <summary>
    /// The id for an integer number of buttons that can be shown to the front side of a scrollbar. This value and
    /// AfterTrackButtonCount can't exceed 20 when added together.
    /// </summary>
    API_FIELD() int BeforeTrackButtonCount;
    /// <summary>
    /// The id for an integer number of buttons that can be shown after the end side of a scrollbar. This value and
    /// BeforeTrackButtonCount can't exceed 20 when added together.
    /// </summary>
    API_FIELD() int AfterTrackButtonCount;
    /// <summary>
    /// The id for at most 20 drawable resources that can be used for drawing the buttons of the scrollbar, excluding
    /// the thumb button.
    /// </summary>
    API_FIELD() int ButtonDraw[20];
    /// <summary>
    /// The id for at most 20 integers that determine the length of the buttons of the scrollbar.
    /// </summary>
    API_FIELD() int ButtonSize[20];

    /// <summary>
    /// The id for the integer role to set default behavior for the left or top part of the track in front of the thumb button.
    /// </summary>
    API_FIELD() int BeforeTrackRole;
    /// <summary>
    /// The id for the integer role to set default behavior for the right or bottom part of the track after the thumb button.
    /// </summary>
    API_FIELD() int AfterTrackRole;
    /// <summary>
    /// The id for at most 20 integer roles to set default behavior for scrollbar buttons.
    /// </summary>
    API_FIELD() int ButtonRole[20];
};

template<>
struct TIsPODType<FudgetScrollBarPainterMapping>
{
    enum { Value = true };
};

/// <summary>
/// Painter for scrollbar components and scrollbar controls
/// </summary>
API_CLASS()
class FUDGETS_API FudgetScrollBarPainter : public FudgetPartPainter
{
    using Base = FudgetPartPainter;
    DECLARE_SCRIPTING_TYPE(FudgetScrollBarPainter);
public:
    using Mapping = FudgetScrollBarPainterMapping;

    /// <inheritdoc />
    void Initialize(FudgetControl *control, const Variant &mapping) override;

    /// <summary>
    /// Returns the width of the scrollbar to be used to calculate its bounds in a control.
    /// </summary>
    API_PROPERTY() int GetWidth() { return _width; }

    /// <summary>
    /// Returns the number of buttons before and after the scrollbar's track.
    /// </summary>
    API_PROPERTY() int GetButtonCount() { return _before_btn_count + _after_btn_count; }
    /// <summary>
    /// Returns the number of buttons in front of the scrollbar's track.
    /// </summary>
    API_PROPERTY() int GetBeforeButtonCount() { return _before_btn_count; }
    /// <summary>
    /// Returns the number of buttons after the scrollbar's track.
    /// </summary>
    API_PROPERTY() int GetAfterButtonCount() { return _after_btn_count; }

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
    API_FUNCTION() int GetButtonRole(int button_index) const;


    /// <summary>
    /// Returns the position and size of the parts of the scrollbar based on the scrollbar's current values.
    /// </summary>
    /// <param name="control">The control to draw the scrollbar in.</param>
    /// <param name="bounds">The bounding rectangle where the scrollbar would be drawn.</param>
    /// <param name="range">Maximum value of the scrollbar if the page size is ignored.</param>
    /// <param name="page_size">Size of the visible page that is represented by the thumb button's size when it is not fixed sized.</param>
    /// <param name="thumb_pos">The current scroll position.</param>
    /// <param name="track">Receives the bounding rectangle of the whole painted track where the thumb button is dragged.</param>
    /// <param name="before_track">Receives part of the bounding rectangle of the painted track where the thumb button is dragged. This is the part before the thumb button.</param>
    /// <param name="after_track">Receives part of the bounding rectangle of the painted track where the thumb button is dragged. This is the part after the thumb button.</param>
    /// <param name="thumb_button">Receives the bounding rectangle of the thumb button that shows the current position.</param>
    /// <param name="buttons">Receives the bounding rectangle of the buttons before and after the track. If the array is large enough to receive the values, no allocation will take place.</param>
    API_FUNCTION() virtual void GetPartBounds(FudgetControl *control, const Rectangle &bounds, int64 range, int64 page_size, int64 thumb_pos,
        API_PARAM(Out) Rectangle &track, API_PARAM(Out) Rectangle &before_track, API_PARAM(Out) Rectangle &after_track,
        API_PARAM(Out) Rectangle &thumb_button, API_PARAM(Out) Array<Rectangle> &buttons) const;

    /// <summary>
    /// Same as the overloaded GetPartBounds, but the buttons array is instead a pointer to an array. Make sure it has enough space for at least ButtonCount number of rectangles.
    /// </summary>
    void GetPartBounds(FudgetControl *control, const Rectangle &bounds, int64 range, int64 page_size, int64 thumb_pos, Rectangle &track,
        Rectangle &before_track, Rectangle &after_track, Rectangle &thumb_button, Rectangle *buttons) const;

    /// <summary>
    /// Draws the frame and the full track of the scrollbar. The bounds should be the bounds of the scrollbar.
    /// </summary>
    /// <param name="control">The control to paint in.</param>
    /// <param name="bounds">Bounding rectangle of the scrollbar</param>
    /// <param name="track_bounds">Bounding rectangle of the track where the thumb is moving</param>
    /// <param name="states">The state of the control or scrollbar</param>
    API_FUNCTION() virtual void DrawBackground(FudgetControl *control, const Rectangle &bounds, const Rectangle &track_bounds, uint64 states);
    /// <summary>
    /// Draws the frame of the scrollbar. Should be called last. The bounds should be the bounds of the scrollbar.
    /// </summary>
    /// <param name="control">The control to paint in.</param>
    /// <param name="bounds">Bounding rectangle of the scrollbar</param>
    /// <param name="states">The state of the control or scrollbar</param>
    API_FUNCTION() virtual void DrawFrame(FudgetControl *control, const Rectangle &bounds, uint64 states);
    /// <summary>
    /// Draws the track under, before and after the dragged thumb button. The passed bounds were the values returned in GetPartBounds.
    /// </summary>
    /// <param name="control">The control to paint in.</param>
    /// <param name="before_bounds">Bounding rectangle of the track before the thumb button.</param>
    /// <param name="after_bounds">Bounding rectangle of the track after the thumb button.</param>
    /// <param name="before_states">State of the track of the scrollbar before the thumb button</param>
    /// <param name="after_states">State of the track of the scrollbar after the thumb button</param>
    API_FUNCTION() virtual void DrawTrack(FudgetControl *control, const Rectangle &before_bounds, const Rectangle &after_bounds, uint64 before_states, uint64 after_states);
    /// <summary>
    /// Draws the thumb button of the scrollbar on the track. The passed bounds were the values returned in GetPartBounds.
    /// </summary>
    /// <param name="control">The control to paint in.</param>
    /// <param name="bounds">The bounds of the thumb button.</param>
    /// <param name="states">The state of the thumb button.</param>
    API_FUNCTION() virtual void DrawThumb(FudgetControl *control, const Rectangle &bounds, uint64 states);
    /// <summary>
    /// Draws one of the buttons of the scrollbar before or after the track. The passed bounds were the values returned in GetPartBounds.
    /// It's an error to try to draw a button with index higher than the button count passed from styles.
    /// </summary>
    /// <param name="control">The control to paint in.</param>
    /// <param name="button_index">Index of the button.</param>
    /// <param name="bounds">Bounds of the button.</param>
    /// <param name="states">The current state of the button.</param>
    API_FUNCTION() virtual void DrawButton(FudgetControl *control, int button_index, const Rectangle &bounds, uint64 states);
private:

    FORCE_INLINE float Relevant(Float2 size) const { return _orientation == FudgetScrollBarOrientation::Horizontal ? size.X : size.Y; }
    FORCE_INLINE float Opposite(Float2 size) const { return _orientation == FudgetScrollBarOrientation::Horizontal ? size.Y : size.X; }
    FORCE_INLINE float &RelevantRef(Float2 &size) const { return _orientation == FudgetScrollBarOrientation::Horizontal ? size.X : size.Y; }
    FORCE_INLINE float &OppositeRef(Float2 &size) const { return _orientation == FudgetScrollBarOrientation::Horizontal ? size.Y : size.X; }

    int _width;
    bool _thumb_size_fixed;
    int _thumb_size;
    int _min_thumb_size;

    FudgetScrollBarOrientation _orientation;

    FudgetDrawable *_bg_draw;
    FudgetDrawable *_frame_draw;
    FudgetDrawable *_thumb_draw;
    FudgetDrawable *_track_draw;
    FudgetDrawable *_before_track_draw;
    FudgetDrawable *_after_track_draw;

    int _before_btn_count;
    int _after_btn_count;
    FudgetDrawable *_btn_draw[20];

    int _before_track_role;
    int _after_track_role;
    int _btn_size[20] = { 0 };
    int _btn_role[20] = { 0 };
};
