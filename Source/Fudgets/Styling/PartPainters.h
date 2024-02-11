#pragma once

#include "Engine/Scripting/ScriptingObject.h"

#include "../MarginStructs.h"
#include "StyleStructs.h"

class FudgetTheme;
class FudgetStyle;
class FudgetControl;

/// <summary>
/// Base class for objects that paint the standard controls' parts.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetPartPainter : public ScriptingObject
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE(FudgetPartPainter);
public:
    ~FudgetPartPainter();

    /// <summary>
    /// Initializes the painter, caching the resources it will draw with
    /// </summary>
    /// <param name="theme">Theme used for drawing</param>
    /// <param name="style">Style providing the resources for the drawing</param>
    API_FUNCTION() virtual void Initialize(FudgetTheme *theme, FudgetStyle *style);

    // TODO: Change this to not require a control for drawing, but instead separate the control paint functions
    // and state to be passed around.

    ///// <summary>
    ///// Draws a part of a control
    ///// </summary>
    ///// <param name="control">Control to draw</param>
    //API_FUNCTION() virtual void Draw(FudgetControl *control) {}

    /// <summary>
    /// Updates the drawing state of the painter. Called by controls in OnUpdate if they registered to get updates
    /// </summary>
    /// <param name="delta_time">Time passed since last update</param>
    API_FUNCTION() virtual void Update(float delta_time) {}

    /// <summary>
    /// The theme used for drawing 
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetTheme* GetTheme() const { return _theme; }

    /// <summary>
    /// The style used for drawing. In derived classes requres GetStyleToken to return a valid style token
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetStyle* GetStyle() const { return _style != nullptr ? _style : GetDefaultStyle(); }
protected:
    /// <summary>
    /// Tries to find a style that can be used to draw with this painter when a valid style wasn't provided on initialization
    /// </summary>
    /// <returns>A style appropriate for the painter</returns>
    API_PROPERTY() FudgetStyle* GetDefaultStyle() const;
private:
    FudgetTheme *_theme;
    FudgetStyle *_style;
};

/// <summary>
/// Supported states for framed field drawing
/// </summary>
API_ENUM(Attributes="Flags")
enum class FudgetFramedFieldState
{
    Normal = 0,
    Pressed = 1 << 0,
    Down = 1 << 1,
    Focused = 1 << 2,
    Disabled = 1 << 3,
    Hovered = 1 << 4,
};
DECLARE_ENUM_OPERATORS(FudgetFramedFieldState);

API_STRUCT()
struct FUDGETS_API FudgetPainterStateHelper
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetPainterStateHelper);
public:
    void SetState(FudgetFramedFieldState value, bool set = true);
    FORCE_INLINE bool HasState(FudgetFramedFieldState value) const { return (State & value) == value; }

    FORCE_INLINE bool Enabled() const { return !HasState(FudgetFramedFieldState::Disabled); }
    FORCE_INLINE bool Hovered() const { return HasState(FudgetFramedFieldState::Hovered); }
    FORCE_INLINE bool Pressed() const { return HasState(FudgetFramedFieldState::Pressed); }
    FORCE_INLINE bool Down() const { return HasState(FudgetFramedFieldState::Down); }
    FORCE_INLINE bool Focused() const { return HasState(FudgetFramedFieldState::Focused); }

    API_FIELD() FudgetFramedFieldState State;
};


API_CLASS()
class FUDGETS_API FudgetStatePainter : public FudgetPartPainter
{
    using Base = FudgetPartPainter;
    DECLARE_SCRIPTING_TYPE(FudgetStatePainter);
public:
    /// <summary>
    /// Draws the standard control part based on the control's current state.
    /// </summary>
    /// <param name="control">Control to draw</param>
    /// <param name="state_object">State to paint</param>
    API_FUNCTION() virtual void Draw(FudgetControl *control, const FudgetPainterStateHelper &state_object) {}
};

// TODO: these alignments might be unified somewhere if there are multiple

/// <summary>
/// Horizontal image alignment for FudgetAlignedImagePainter
/// </summary>
API_ENUM()
enum class FudgetImageHorzAlign
{
    Left,
    Right,
    Center,
    Stretch
};

/// <summary>
/// Vertical image alignment for FudgetAlignedImagePainter
/// </summary>
API_ENUM()
enum class FudgetImageVertAlign
{
    Top,
    Bottom,
    Center,
    Stretch
};


/// <summary>
/// Painter for controls that have a background and a frame around a field. For text editors
/// list boxes, drop down menus.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetAlignedImagePainter : public FudgetStatePainter
{
    using Base = FudgetStatePainter;
    DECLARE_SCRIPTING_TYPE(FudgetAlignedImagePainter);
public:
    /// <summary>
    /// Creates the default style for drawing, filling the resources to use that will be looked up in the current theme.
    /// </summary>
    API_FUNCTION() static void CreateStyle();

    /// <inheritdoc />
    void Initialize(FudgetTheme *theme, FudgetStyle *style) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const FudgetPainterStateHelper &state_object) override;

    API_FIELD(ReadOnly) static FudgetToken SelfToken;

    API_FIELD(ReadOnly) static FudgetToken ImageToken;
    API_FIELD(ReadOnly) static FudgetToken HoveredImageToken;
    API_FIELD(ReadOnly) static FudgetToken FocusedImageToken;
    API_FIELD(ReadOnly) static FudgetToken PressedImageToken;
    API_FIELD(ReadOnly) static FudgetToken DownImageToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledImageToken;

    API_FIELD(ReadOnly) static FudgetToken ImageTintToken;
    API_FIELD(ReadOnly) static FudgetToken HoveredImageTintToken;
    API_FIELD(ReadOnly) static FudgetToken FocusedImageTintToken;
    API_FIELD(ReadOnly) static FudgetToken PressedImageTintToken;
    API_FIELD(ReadOnly) static FudgetToken DownImageTintToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledImageTintToken;

    API_FIELD(ReadOnly) static FudgetToken ImageOffsetToken;
    API_FIELD(ReadOnly) static FudgetToken HoveredImageOffsetToken;
    API_FIELD(ReadOnly) static FudgetToken FocusedImageOffsetToken;
    API_FIELD(ReadOnly) static FudgetToken PressedImageOffsetToken;
    API_FIELD(ReadOnly) static FudgetToken DownImageOffsetToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledImageOffsetToken;

    API_FIELD(ReadOnly) static FudgetToken ImagePaddingToken;
    API_FIELD(ReadOnly) static FudgetToken HoveredImagePaddingToken;
    API_FIELD(ReadOnly) static FudgetToken PressedImagePaddingToken;
    API_FIELD(ReadOnly) static FudgetToken DownImagePaddingToken;
    API_FIELD(ReadOnly) static FudgetToken FocusedImagePaddingToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledImagePaddingToken;

    API_FIELD(ReadOnly) static FudgetToken HorzAlignToken;
    API_FIELD(ReadOnly) static FudgetToken VertAlignToken;
private:
    AssetReference<TextureBase> _image;
    AssetReference<TextureBase> _hovered_image;
    AssetReference<TextureBase> _pressed_image;
    AssetReference<TextureBase> _down_image;
    AssetReference<TextureBase> _focused_image;
    AssetReference<TextureBase> _disabled_image;

    Color _image_tint;
    Color _hovered_image_tint;
    Color _pressed_image_tint;
    Color _down_image_tint;
    Color _focused_image_tint;
    Color _disabled_image_tint;

    Float2 _image_offset;
    Float2 _hovered_image_offset;
    Float2 _pressed_image_offset;
    Float2 _down_image_offset;
    Float2 _focused_image_offset;
    Float2 _disabled_image_offset;

    FudgetPadding _image_padding;
    FudgetPadding _hovered_image_padding;
    FudgetPadding _pressed_image_padding;
    FudgetPadding _down_image_padding;
    FudgetPadding _focused_image_padding;
    FudgetPadding _disabled_image_padding;

    FudgetImageHorzAlign _horz_align;
    FudgetImageVertAlign _vert_align;
};



/// <summary>
/// Painter for controls that have a background and a frame around a field. For text editors
/// list boxes, drop down menus.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetFramedFieldPainter : public FudgetStatePainter
{
    using Base = FudgetStatePainter;
    DECLARE_SCRIPTING_TYPE(FudgetFramedFieldPainter);
public:
    /// <summary>
    /// Creates the default style for drawing, filling the resources to use that will be looked up in the current theme.
    /// </summary>
    API_FUNCTION() static void CreateStyle();

    /// <inheritdoc />
    void Initialize(FudgetTheme *theme, FudgetStyle *style) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const FudgetPainterStateHelper &state_object) override;

    API_FIELD(ReadOnly) static FudgetToken SelfToken;

    API_FIELD(ReadOnly) static FudgetToken FieldBackgroundToken;
    API_FIELD(ReadOnly) static FudgetToken HoveredFieldBackgroundToken;
    API_FIELD(ReadOnly) static FudgetToken PressedFieldBackgroundToken;
    API_FIELD(ReadOnly) static FudgetToken DownFieldBackgroundToken;
    API_FIELD(ReadOnly) static FudgetToken FocusedFieldBackgroundToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledFieldBackgroundToken;
    API_FIELD(ReadOnly) static FudgetToken FieldPaddingToken;
    API_FIELD(ReadOnly) static FudgetToken HoveredFieldPaddingToken;
    API_FIELD(ReadOnly) static FudgetToken PressedFieldPaddingToken;
    API_FIELD(ReadOnly) static FudgetToken DownFieldPaddingToken;
    API_FIELD(ReadOnly) static FudgetToken FocusedFieldPaddingToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledFieldPaddingToken;

    API_FIELD(ReadOnly) static FudgetToken FrameDrawToken;
    API_FIELD(ReadOnly) static FudgetToken HoveredFrameDrawToken;
    API_FIELD(ReadOnly) static FudgetToken FocusedFrameDrawToken;
    API_FIELD(ReadOnly) static FudgetToken PressedFrameDrawToken;
    API_FIELD(ReadOnly) static FudgetToken DownFrameDrawToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledFrameDrawToken;
    API_FIELD(ReadOnly) static FudgetToken FramePaddingToken;
    API_FIELD(ReadOnly) static FudgetToken HoveredFramePaddingToken;
    API_FIELD(ReadOnly) static FudgetToken PressedFramePaddingToken;
    API_FIELD(ReadOnly) static FudgetToken DownFramePaddingToken;
    API_FIELD(ReadOnly) static FudgetToken FocusedFramePaddingToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledFramePaddingToken;

    API_FIELD(ReadOnly) static FudgetToken ContentPaddingToken;

    API_PROPERTY() FORCE_INLINE FudgetPadding GetContentPadding() const { return _inner_padding; }
private:
    FudgetDrawArea _field_bg;
    FudgetDrawArea _hovered_field_bg;
    FudgetDrawArea _pressed_field_bg;
    FudgetDrawArea _down_field_bg;
    FudgetDrawArea _focused_field_bg;
    FudgetDrawArea _disabled_field_bg;
    FudgetPadding _field_padding;
    FudgetPadding _hovered_field_padding;
    FudgetPadding _pressed_field_padding;
    FudgetPadding _down_field_padding;
    FudgetPadding _focused_field_padding;
    FudgetPadding _disabled_field_padding;

    FudgetDrawArea _frame_area;
    FudgetDrawArea _hovered_frame_area;
    FudgetDrawArea _pressed_frame_area;
    FudgetDrawArea _down_frame_area;
    FudgetDrawArea _focused_frame_area;
    FudgetDrawArea _disabled_frame_area;
    FudgetPadding _frame_padding;
    FudgetPadding _hovered_frame_padding;
    FudgetPadding _pressed_frame_padding;
    FudgetPadding _down_frame_padding;
    FudgetPadding _focused_frame_padding;
    FudgetPadding _disabled_frame_padding;

    FudgetPadding _inner_padding;
};

