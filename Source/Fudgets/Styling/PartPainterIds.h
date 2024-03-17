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

