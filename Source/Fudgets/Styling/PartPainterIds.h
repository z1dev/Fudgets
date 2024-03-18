#pragma once

#include "Engine/Scripting/ScriptingObject.h"



/// <summary>
/// Resource ids in the main theme that can be used to set some global defaults. Painters are encouraged to use these
/// as fallback for their values, to have defaults that can be easily set by users.
/// </summary>
API_ENUM()
enum class FudgetThemePartIds
{
    First = 1,

    /// <summary>
    /// Frame edge drawable or color, used for standard framed controls
    /// </summary>
    FrameDraw,
    /// <summary>
    /// Color to multiply every pixel with when drawing the frame of standard framed controls
    /// </summary>
    FrameTint,
    /// <summary>
    /// Margin outside of a frame of standard framed controls
    /// </summary>
    FrameMargin,

    /// <summary>
    /// Background drawable for input fields in controls
    /// </summary>
    FieldBackground = First,
    /// <summary>
    /// Margin to limit the bounds when drawing the background of input fields in controls
    /// </summary>
    FieldBackgroundMargin,
    /// <summary>
    /// Color to multiply every pixel with when drawing the background of input fields in controls
    /// </summary>
    FieldBackgroundTint,

    /// <summary>
    /// Frame edge drawable or color, used for input fields in controls
    /// </summary>
    FieldFrameDraw,
    /// <summary>
    /// Color to multiply every pixel with when drawing the frame of input fields in controls
    /// </summary>
    FieldFrameTint,
    /// <summary>
    /// Margin to limit the bounds when drawing the frame of input fields in controls
    /// </summary>
    FieldFrameMargin,

    /// <summary>
    /// Visual thickness of the frame of input fields.
    /// </summary>
    FieldFrameThickness,
    /// <summary>
    /// Padding inside the frame of input fields that includes extra padding for contents.
    /// </summary>
    FieldFramePadding,

    /// <summary>
    /// Default width of the vertical scroll bar or height of the horizontal scroll bar.
    /// </summary>
    ScrollBarWidth,
    /// <summary>
    /// Whether the draggable thumb button is of a fixed size instead of representing the real size of a page
    /// relative to the range.
    /// </summary>
    ScrollBarIsThumbSizeFixed,
    /// <summary>
    /// Size of the draggable thumb button if it has a fixed size. This value is ignored for not fixed thumb sizes.
    /// </summary>
    ScrollBarThumbSize,
    /// <summary>
    /// Minimum size of the draggable thumb button when the button size depends on the visible page size.
    /// </summary>
    ScrollBarMinThumbSize,

    /// <summary>
    /// Drawable of the background of a horizontal scrollbar, which is drawn over the full bounds, including the position of buttons.
    /// </summary>
    ScrollBarHorizontalBackground,
    /// <summary>
    /// Drawable of the background of a vertical scrollbar, which is drawn over the full bounds, including the position of buttons.
    /// </summary>
    ScrollBarVerticalBackground,
    /// <summary>
    /// Frame drawable around a scrollbar and its buttons that is drawn last.
    /// </summary>
    ScrollBarFrame,
    /// <summary>
    /// Drawable of the draggable thumb button of a horizontal scroll bar.
    /// </summary>
    ScrollBarHorizontalThumb,
    /// <summary>
    /// Drawable of the draggable thumb button of a vertical scroll bar.
    /// </summary>
    ScrollBarVerticalThumb,
    /// <summary>
    /// Drawable background of a horizontal scroll bar that serves as the track for the thumb button.
    /// </summary>
    ScrollBarHorizontalTrack,
    /// <summary>
    /// Drawable on the horizontal scrollbar track in front of the thumb button.
    /// </summary>
    ScrollBarHorizontalBeforeTrack,
    /// <summary>
    /// Drawable on the horizontal scrollbar track after of the thumb button.
    /// </summary>
    ScrollBarHorizontalAfterTrack,
    /// <summary>
    /// Drawable background of a vertical scroll bar that serves as the track for the thumb button.
    /// </summary>
    ScrollBarVerticalTrack,
    /// <summary>
    /// Drawable on the vertical scrollbar track in front of the thumb button.
    /// </summary>
    ScrollBarVerticalBeforeTrack,
    /// <summary>
    /// Drawable on the vertical scrollbar track after of the thumb button.
    /// </summary>
    ScrollBarVerticalAfterTrack,

    /// <summary>
    /// Number of buttons in front of the scrollbar's track. Buttons that don't fit are not drawn. The default scrollbar supports up to 20 buttons.
    /// </summary>
    ScrollBarBeforeTrackButtonCount,
    /// <summary>
    /// Number of buttons after of the scrollbar's track. Buttons that don't fit are not drawn. The default scrollbar supports up to 20 buttons.
    /// </summary>
    ScrollBarAfterTrackButtonCount,
    /// <summary>
    /// The first button drawable on a horizontal scrollbar. The default scrollbar supports up to 20 buttons.
    /// </summary>
    ScrollBarFirstHorizontalButton,
    /// <summary>
    /// The last button drawable on a horizontal scrollbar. The default scrollbar supports up to 20 buttons.
    /// </summary>
    ScrollBarLastHorizontalButton = ScrollBarFirstHorizontalButton + 19,
    /// <summary>
    /// The first button size on a horizontal scrollbar. The default scrollbar supports up to 20 buttons.
    /// </summary>
    ScrollBarFirstHorizontalButtonSize,
    /// <summary>
    /// The last button size on a horizontal scrollbar. The default scrollbar supports up to 20 buttons.
    /// </summary>
    ScrollBarLastHorizontalButtonSize = ScrollBarFirstHorizontalButtonSize + 19,
    /// <summary>
    /// The first button drawable on a vertical scrollbar. The default scrollbar supports up to 20 buttons.
    /// </summary>
    ScrollBarFirstVerticalButton,
    /// <summary>
    /// The last button drawable on a vertical scrollbar. The default scrollbar supports up to 20 buttons.
    /// </summary>
    ScrollBarLastVerticalButton = ScrollBarFirstVerticalButton + 19,
    /// <summary>
    /// The first button size on a vertical scrollbar. The default scrollbar supports up to 20 buttons.
    /// </summary>
    ScrollBarFirstVerticalButtonSize,
    /// <summary>
    /// The last button size on a vertical scrollbar. The default scrollbar supports up to 20 buttons.
    /// </summary>
    ScrollBarLastVerticalButtonSize = ScrollBarFirstVerticalButtonSize + 19,

    /// <summary>
    /// Button surface drawable or color, used for the standard buttons
    /// </summary>
    ButtonSurface,
    /// <summary>
    /// Offset to move contents visually when the button is pressed.
    /// </summary>
    ButtonContentPressedOffset,
    /// <summary>
    /// Padding inside the frame of a button that includes extra padding for contents.
    /// </summary>
    ButtonContentPadding,

    /// <summary>
    /// Placeholder image for buttons with no images set
    /// </summary>
    ButtonImage,
    /// <summary>
    /// Image tint for buttons with aligned image content
    /// </summary>
    ButtonImageTint,
    /// <summary>
    /// Image offset for buttons with aligned image content
    /// </summary>
    ButtonImageOffset,
    /// <summary>
    /// Image margin for buttons with aligned image content
    /// </summary>
    ButtonImageMargin,
    /// <summary>
    /// Horizontal alignment of contents in a generic button
    /// </summary>
    ButtonContentHorzAlignment,
    /// <summary>
    /// Vertical alignment of contents in a generic button
    /// </summary>
    ButtonContentVertAlignment,

    /// <summary>
    /// Default base font
    /// </summary>
    Font,
    /// <summary>
    /// Color of text in input fields and controls
    /// </summary>
    FieldTextColor,

    /// <summary>
    /// Background of text in a text input field
    /// </summary>
    FieldTextBackground,
    /// <summary>
    /// Tint to color background of text in a text input field
    /// </summary>
    FieldTextBackgroundTint,

    /// <summary>
    /// Color or drawable for drawing the caret in input fields.
    /// </summary>
    CaretDraw,
    /// <summary>
    /// Speed of blinking of a caret in seconds for the invisible and visible parts.
    /// </summary>
    CaretBlinkTime,
    /// <summary>
    /// Width of a caret in pixels
    /// </summary>
    CaretWidth,
    /// <summary>
    /// Number of characters to scroll left or right in a text input field when moving the caret out of view sideways.
    /// </summary>
    CaretScrollCount,
    /// <summary>
    /// Cursor to use over text in an input field.
    /// </summary>
    BeamCursor,


    /// <summary>
    /// Buttonsurface drawable or color, used for the combobox buttons
    /// </summary>
    ComboBoxButtonSurface,
    /// <summary>
    /// Offset to move contents visually when the combobox button is pressed.
    /// </summary>
    ComboBoxButtonContentPressedOffset,
    /// <summary>
    /// Image for combobox buttons with no images set
    /// </summary>
    ComboBoxButtonImage,
    /// <summary>
    /// Image tint for combobox buttons with aligned image content
    /// </summary>
    ComboBoxButtonImageTint,
    /// <summary>
    /// Image offset for combobox buttons with aligned image content
    /// </summary>
    ComboBoxButtonImageOffset,
    /// <summary>
    /// Image margin for combobox buttons with aligned image content
    /// </summary>
    ComboBoxButtonImageMargin,

    /// <summary>
    /// Horizontal alignment of contents in a combobox button
    /// </summary>
    ComboBoxButtonImageHorzAlignment,
    /// <summary>
    /// Vertical alignment of contents in a combobox button
    /// </summary>
    ComboBoxButtonImageVertAlignment,

    /// <summary>
    /// Background for items in list controls
    /// </summary>
    ListItemBackground,

    /// <summary>
    /// Tint for item backgrounds in a list controls
    /// </summary>
    ListItemBackgroundTint,

    /// <summary>
    /// Painter information for field controls
    /// </summary>
    FieldFramePainter,
    /// <summary>
    /// Painter information for horizontal scrollbars and scrollbar controls
    /// </summary>
    HorzScrollBarPainter,
    /// <summary>
    /// Painter information for vertical scrollbars and scrollbar controls
    /// </summary>
    VertScrollBarPainter,
    /// <summary>
    /// Painter information for independent button controls
    /// </summary>
    ButtonFramePainter,
    /// <summary>
    /// Painter information for combobox button controls
    /// </summary>
    ComboBoxButtonFramePainter,
    /// <summary>
    /// Painter information for the text of single line text field controls
    /// </summary>
    SingleLineInputTextPainter,
    /// <summary>
    /// Painter information for the text of multi line text field controls
    /// </summary>
    MultiLineInputTextPainter,
    /// <summary>
    /// Painter information for controls with content that can be drawn by a drawable
    /// </summary>
    DrawableContentPainter,
    /// <summary>
    /// Painter information for controls with a list of items
    /// </summary>
    ListItemPainter,
};

API_ENUM()
enum class FudgetFramedControlPartIds
{
    First = 1000,

    FramePainter = First,

    FrameDraw,
    FrameMargin,
    FrameTint,

    VisualPadding,
    ContentPadding,
};

API_ENUM()
enum class FudgetContentPartIds
{
    First = 1200,

    ContentPainter = First,

    Drawable,
    Margin,
    Offset,
    Tint,
};

API_ENUM()
enum class FudgetScrollBarPartIds
{
    First = 1500,

    Painter = First,

    Width,
    IsThumbSizeFixed,
    ThumbSize,
    MinThumbSize,

    HorzBackgroundDraw,
    VertBackgroundDraw,
    FrameDraw,
    HorzThumbDraw,
    VertThumbDraw,
    HorzTrackDraw,
    HorzBeforeTrackDraw,
    HorzAfterTrackDraw,
    VertTrackDraw,
    VertBeforeTrackDraw,
    VertAfterTrackDraw,

    BeforeTrackButtonCount,
    AfterTrackButtonCount,
    FirstHorzButtonDraw,
    LastHorzButtonDraw = FirstHorzButtonDraw + 19,
    FirstHorzButtonSize,
    LastHorzButtonSize = FirstHorzButtonSize + 19,
    FirstVertButtonDraw,
    LastVertButtonDraw = FirstVertButtonDraw + 19,
    FirstVertButtonSize,
    LastVertButtonSize = FirstVertButtonSize + 19,
};


API_ENUM()
enum class FudgetTextFieldPartIds
{
    First = 2000,

    TextBackground = First,
    TextColor,
    TextBackgroundTint,

    Font,

    CaretDraw,
    CaretBlinkTime,
    CaretWidth,
    CaretScrollCount,
    BeamCursor,
};

API_ENUM()
enum class FudgetMultilineTextFieldPartIds
{
    First = 2200,

    TextPainter = First,

    SnapTopLine,
};

API_ENUM()
enum class FudgetSinglelineTextFieldPartIds
{
    First = 2300,

    TextPainter = First,
};

API_ENUM()
enum class FudgetButtonPartIds
{
    First = 3000,

    ContentPressedOffset,
};

API_ENUM()
enum class FudgetComboBoxPartIds
{
    First = 5000,

    ButtonWidth = First,
};

API_ENUM()
enum class FudgetListBoxPartIds
{
    First = 6000,

    ItemPainter = First,
    TextPainter,

    Background,

    BackgroundTint,
};
