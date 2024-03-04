#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Render2D/Font.h"

#include "../MarginStructs.h"
#include "StyleStructs.h"

class FudgetTheme;
class FudgetStyle;
class FudgetControl;
class FudgetDrawable;


API_ENUM()
enum class FudgetPainterIds
{
    First = 1000000,

    AlignedImagePainter = First,
    FramedFieldPainter,
    LineEditTextPainter,
    TextBoxPainter,
    ListBoxItemPainter,
};


/// <summary>
/// Supported states for framed field drawing
/// </summary>
API_ENUM(Attributes = "Flags")
enum class FudgetVisualControlState
{
    /// <summary>
    /// Normal state that means no other state is set
    /// </summary>
    Normal = 0,
    /// <summary>
    /// Pressed appearance of a control, usually for buttons
    /// </summary>
    Pressed = 1 << 0,
    /// <summary>
    /// Down appearance of a control, usually for buttons
    /// </summary>
    Down = 1 << 1,
    /// <summary>
    /// Focused appearance of a control that can have the keyboard focus
    /// </summary>
    Focused = 1 << 2,
    /// <summary>
    /// Disabled appearance of a control that means the users can't interact with it
    /// </summary>
    Disabled = 1 << 3,
    /// <summary>
    /// Hovered appearance of a control, usually when the mouse pointer is over it
    /// </summary>
    Hovered = 1 << 4,
};
DECLARE_ENUM_OPERATORS(FudgetVisualControlState);


/// <summary>
/// Base class for objects that paint the standard controls' parts.
/// </summary>
API_CLASS(Abstract)
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
    /// The style used for drawing. In derived classes requres GetStyle to return a valid style.
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetStyle* GetStyle() const { return _style != nullptr ? _style : GetDefaultStyle(); }
protected:
    /// <summary>
    /// Tries to find a style that can be used to draw with this painter when a valid style wasn't provided on initialization
    /// </summary>
    /// <returns>A style appropriate for the painter</returns>
    API_PROPERTY() FudgetStyle* GetDefaultStyle() const;

    API_FUNCTION() bool HasState(FudgetVisualControlState states, FudgetVisualControlState to_check) const;

    API_FUNCTION() bool IsEnabled(FudgetVisualControlState states) const;
    API_FUNCTION() bool IsDisabled(FudgetVisualControlState states) const;
    API_FUNCTION() bool IsHovered(FudgetVisualControlState states) const;
    API_FUNCTION() bool IsFocused(FudgetVisualControlState states) const;
    API_FUNCTION() bool IsDown(FudgetVisualControlState states) const;
    API_FUNCTION() bool IsPressed(FudgetVisualControlState states) const;
private:
    FudgetTheme *_theme;
    FudgetStyle *_style;
};

/// <summary>
/// Base class for painter objects that paint controls whose appearance and behavior might depend on their state
/// </summary>
API_CLASS(Abstract)
class FUDGETS_API FudgetStatePainter : public FudgetPartPainter
{
    using Base = FudgetPartPainter;
    DECLARE_SCRIPTING_TYPE(FudgetStatePainter);
public:
    /// <summary>
    /// Draws the standard control part based on the control's current state.
    /// </summary>
    /// <param name="control">Control to draw</param>
    /// <param name="bounds">Bounds to draw inside. This is usually the result of padding on the control's bounds.</param>
    /// <param name="state">State to paint</param>
    API_FUNCTION() virtual void Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState state) {}
};

// TODO: these alignments might be unified somewhere if there are multiple

/// <summary>
/// Horizontal image alignment for FudgetAlignedImagePainter
/// </summary>
API_ENUM()
enum class FudgetImageHorzAlign
{
    /// <summary>
    /// Image is aligned to the left
    /// </summary>
    Left,
    /// <summary>
    /// Image is aligned to the right
    /// </summary>
    Right,
    /// <summary>
    /// Image is aligned to be at the center horizontally
    /// </summary>
    Center,
    /// <summary>
    /// Stretch image to fill the horizontal space
    /// </summary>
    Stretch
};

/// <summary>
/// Vertical image alignment for FudgetAlignedImagePainter
/// </summary>
API_ENUM()
enum class FudgetImageVertAlign
{
    /// <summary>
    /// Image is aligned to the top
    /// </summary>
    Top,
    /// <summary>
    /// Image is aligned to the bottom
    /// </summary>
    Bottom,
    /// <summary>
    /// Image is aligned to be at the center vertically
    /// </summary>
    Center,
    /// <summary>
    /// Stretch image to fill the vertical space
    /// </summary>
    Stretch
};


API_ENUM()
enum class FudgetAlignedImagePainterIds
{
    First = 8000,

    Image = First,
    HoveredImage,
    PressedImage,
    DownImage,
    FocusedImage,
    DisabledImage,

    ImageTint,
    HoveredImageTint,
    PressedImageTint,
    DownImageTint,
    FocusedImageTint,
    DisabledImageTint,

    ImageOffset,
    HoveredImageOffset,
    PressedImageOffset,
    DownImageOffset,
    FocusedImageOffset,
    DisabledImageOffset,

    ImagePadding,
    HoveredImagePadding,
    PressedImagePadding,
    DownImagePadding,
    FocusedImagePadding,
    DisabledImagePadding,

    HorzAlign,
    VertAlign,
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
    /// <inheritdoc />
    void Initialize(FudgetTheme *theme, FudgetStyle *style) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState state) override;
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


API_ENUM()
enum class FudgetFramedFieldPainterIds
{
    First = 9000,

    FieldBackground = First,
    HoveredFieldBackground,
    PressedFieldBackground,
    DownFieldBackground,
    DisabledFieldBackground,
    FocusedFieldBackground,
    FieldPadding,
    HoveredFieldPadding,
    PressedFieldPadding,
    DownFieldPadding,
    DisabledFieldPadding,
    FocusedFieldPadding,

    FrameDraw,
    HoveredFrameDraw,
    PressedFrameDraw,
    DownFrameDraw,
    FocusedFrameDraw,
    DisabledFrameDraw,
    FramePadding,
    HoveredFramePadding,
    PressedFramePadding,
    DownFramePadding,
    FocusedFramePadding,
    DisabledFramePadding,

    ContentPadding,
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

    /// <inheritdoc />
    void Initialize(FudgetTheme *theme, FudgetStyle *style) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState state) override;


    /// <summary>
    /// Padding inside the rectangle of the painted frame that restricts the size of the contents inside the frame
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetPadding GetContentPadding() const { return _inner_padding; }
private:
    FudgetDrawable *_field_bg;
    FudgetDrawable *_hovered_field_bg;
    FudgetDrawable *_pressed_field_bg;
    FudgetDrawable *_down_field_bg;
    FudgetDrawable *_focused_field_bg;
    FudgetDrawable *_disabled_field_bg;
    FudgetPadding _field_padding;
    FudgetPadding _hovered_field_padding;
    FudgetPadding _pressed_field_padding;
    FudgetPadding _down_field_padding;
    FudgetPadding _focused_field_padding;
    FudgetPadding _disabled_field_padding;

    FudgetDrawable *_frame_area;
    FudgetDrawable *_hovered_frame_area;
    FudgetDrawable *_pressed_frame_area;
    FudgetDrawable *_down_frame_area;
    FudgetDrawable *_focused_frame_area;
    FudgetDrawable *_disabled_frame_area;
    FudgetPadding _frame_padding;
    FudgetPadding _hovered_frame_padding;
    FudgetPadding _pressed_frame_padding;
    FudgetPadding _down_frame_padding;
    FudgetPadding _focused_frame_padding;
    FudgetPadding _disabled_frame_padding;

    FudgetPadding _inner_padding;
};

/// <summary>
/// Workaround for TextRange not having FLAXENGINE_API in their declaration
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetTextRange
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetTextRange);
public:
    FudgetTextRange() : StartIndex(0), EndIndex(0) {}

    FORCE_INLINE FudgetTextRange(const TextRange &range) : StartIndex(range.StartIndex), EndIndex(range.EndIndex) {}
    FORCE_INLINE FudgetTextRange(TextRange &&range) : StartIndex(range.StartIndex), EndIndex(range.EndIndex) {}
    FORCE_INLINE FudgetTextRange& operator=(const TextRange &range) { StartIndex = range.StartIndex; EndIndex = range.EndIndex; return *this; }
    FORCE_INLINE FudgetTextRange& operator=(TextRange &&range) { StartIndex = range.StartIndex; EndIndex = range.EndIndex; return *this; }
    FORCE_INLINE operator TextRange() const { TextRange r; r.StartIndex = StartIndex; r.EndIndex = EndIndex; return r; }

    /// <summary>
    /// Starting index position of the range in a text
    /// </summary>
    API_FIELD() int StartIndex;
    /// <summary>
    /// End index position of the range in a text, which is (usually) exclusive
    /// </summary>
    API_FIELD() int EndIndex;
};

template<>
struct TIsPODType<FudgetTextRange>
{
    enum { Value = true };
};

/// <summary>
/// Workaround for not being able to generate code for array within array. This can be used as a template argument for Span instead.
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetTextRangeSpan
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetSingleLineTextOptions);
public:
    FudgetTextRangeSpan() {}

    /// <summary>
    /// An array of ranges
    /// </summary>
    API_FIELD() Span<FudgetTextRange> RangeSpan;
};

template<>
struct TIsPODType<FudgetTextRangeSpan>
{
    enum { Value = true };
};


/// <summary>
/// Text drawing options passed to FudgetTextPainter for drawing.
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetSingleLineTextOptions
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetSingleLineTextOptions);
public:
    FudgetSingleLineTextOptions() : Scale(1.f), VerticalAlignment(TextAlignment::Center), Spans() {}

    /// <summary>
    /// Scaling of the drawn text
    /// </summary>
    API_FIELD() float Scale;

    /// <summary>
    /// Vertical alignment of the single line text inside its drawing bounds.
    /// </summary>
    API_FIELD() TextAlignment VerticalAlignment;

    /// <summary>
    /// Ranges for drawing text in different ways. It depends on the painter how it uses them. For example
    /// a line text painter could take the first item in the spans array and use it to show selections. In a
    /// different painter the second index would be for spans of bold text. The ranges refer to the whole Text,
    /// and not just the part specified by Range.
    /// </summary>
    API_FIELD() Span<FudgetTextRangeSpan> Spans;
};

template<>
struct TIsPODType<FudgetSingleLineTextOptions>
{
    enum { Value = true };
};

/// <summary>
/// Base class for painters that can draw a single line text and have spans specified for different drawing
/// </summary>
API_CLASS()
class FUDGETS_API FudgetSingleLineTextPainter : public FudgetPartPainter
{
    using Base = FudgetPartPainter;
    DECLARE_SCRIPTING_TYPE(FudgetSingleLineTextPainter);
public:
    /// <summary>
    /// Draws the standard control text based on the control's current state.
    /// </summary>
    /// <param name="control">Control to draw</param>
    /// <param name="bounds">Bounds to paint inside of</param>
    /// <param name="text">Text to draw</param>
    /// <param name="range">Range in the text to limit drawing to</param>
    /// <param name="state">State of control</param>
    /// <param name="text_options">Options for text, like the range or selection spans.</param>
    API_FUNCTION() virtual void Draw(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state, const FudgetSingleLineTextOptions &text_options) {}

    /// <summary>
    /// Measures the text passed to the painter. The Range of text_options can be used to specify the part of text
    /// to measure. It depends on the painter if it wraps the text in the state bounds or not.
    /// </summary>
    /// <param name="control">Control used for measurement</param>
    /// <param name="text">Text to measure</param>
    /// <param name="range">Range in the text to limit measurements to</param>
    /// <param name="state">State of control</param>
    /// <param name="text_options">Options for text, like the range or selection spans.</param>
    /// <returns>Size of the measured text</returns>
    API_FUNCTION() virtual Float2 Measure(FudgetControl *control, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state, const FudgetSingleLineTextOptions &text_options) { return Float2::Zero;  }

    /// <summary>
    /// Returns the kerning distance between two characters using the font of the painter. If no font is set, the result is 0
    /// </summary>
    /// <param name="a">First character</param>
    /// <param name="b">Other character</param>
    /// <param name="scale">Scale of the control used for measurements and drawing</param>
    /// <returns>Kerning distance between characters</returns>
    API_FUNCTION() virtual float GetKerning(Char a, Char b, float scale) const { return 0.f; }

    /// <summary>
    /// Finds the index of character in the text at a position.
    /// </summary>
    /// <param name="control">Control used for hit testing</param>
    /// <param name="bounds">Bounds of the text</param>
    /// <param name="text">Text to measure</param>
    /// <param name="range">Range in the text to limit measurements to</param>
    /// <param name="state">State of control</param>
    /// <param name="text_options">Options for text, like the range or selection spans.</param>
    /// <param name="point">The position of the character to look for</param>
    /// <returns>Index of the character at the given position</returns>
    API_FUNCTION() virtual int HitTest(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state, const FudgetSingleLineTextOptions &text_options, const Float2 &point) { return 0; }

    /// <summary>
    /// Returns the height of the font used by the painter.
    /// </summary>
    API_FUNCTION() virtual float GetFontHeight() const { return 0.f; }
};


API_ENUM()
enum class FudgetLineEditTextPainterIds
{
    First = 10000,

    SelectionDraw = First,
    FocusedSelectionDraw,
    DisabledSelectionDraw,
    TextColor,
    DisabledTextColor,
    SelectedTextColor,
    FocusedSelectedTextColor,
    DisabledSelectedTextColor,
    Font,
};

/// <summary>
/// Painter of unformatted text drawn with a single font, that can have spans which should be drawn
/// as selectable
/// </summary>
API_CLASS()
class FUDGETS_API FudgetLineEditTextPainter : public FudgetSingleLineTextPainter
{
    using Base = FudgetSingleLineTextPainter;
    DECLARE_SCRIPTING_TYPE(FudgetLineEditTextPainter);
public:
    /// <inheritdoc />
    void Initialize(FudgetTheme *theme, FudgetStyle *style) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state, const FudgetSingleLineTextOptions &text_options) override;

    /// <inheritdoc />
    Float2 Measure(FudgetControl *control, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state, const FudgetSingleLineTextOptions &text_options) override;

    /// <inheritdoc />
    float GetKerning(Char a, Char b, float scale) const override;

    /// <inheritdoc />
    int HitTest(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state, const FudgetSingleLineTextOptions &text_options, const Float2 &point) override;

    /// <inheritdoc />
    float GetFontHeight() const override;

private:
    FudgetDrawable *_sel_area;
    FudgetDrawable *_focused_sel_area;
    FudgetDrawable *_disabled_sel_area;

    Color _text_color;
    Color _disabled_text_color;
    Color _selected_text_color;
    Color _focused_selected_text_color;
    Color _disabled_selected_text_color;

    FudgetFont _font;
};



class IListDataProvider;

/// <summary>
/// Base class for painter objects that paint items in list controls
/// </summary>
API_CLASS(Abstract)
class FUDGETS_API FudgetListItemPainter : public FudgetPartPainter
{
    using Base = FudgetPartPainter;
    DECLARE_SCRIPTING_TYPE(FudgetListItemPainter);
public:
    /// <summary>
    /// Draws a single list item.
    /// </summary>
    /// <param name="control">Control used for drawing</param>
    /// <param name="bounds">Bounds to draw inside and used for clipping.</param>
    /// <param name="offset">Where to paint, relative to the top left corner of bounds</param>
    /// <param name="item_index">Index of item in data</param>
    /// <param name="data">Source of item data</param>
    /// <param name="state">State of control</param>
    API_FUNCTION() virtual void Draw(FudgetControl *control, const Rectangle &bounds, Float2 offset, int item_index, IListDataProvider *data, FudgetVisualControlState state) {}

    /// <summary>
    /// Measures the list item at index.
    /// </summary>
    /// <param name="control">Control used for measuring</param>
    /// <param name="item_index">Index of item in data</param>
    /// <param name="data">Source of item data</param>
    /// <param name="state">State of control</param>
    /// <returns>Dimensions of the requested item</returns>
    API_FUNCTION() virtual Float2 Measure(FudgetControl *control, int item_index, IListDataProvider *data, FudgetVisualControlState state) { return Float2::Zero; }
};

