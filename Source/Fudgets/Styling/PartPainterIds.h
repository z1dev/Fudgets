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
    /// Background drawable or color, used for the standard framed controls
    /// </summary>
    Background = First,
    /// <summary>
    /// Hovered background drawable or color, used for the standard framed controls
    /// </summary>
    HoveredBackground,
    /// <summary>
    /// Disabled background drawable or color, used for the standard framed controls
    /// </summary>
    DisabledBackground,
    /// <summary>
    /// Frame edge drawable or color, used for the standard framed controls
    /// </summary>
    FrameDraw,
    /// <summary>
    /// Focused frame edge drawable or color, used for the standard framed controls
    /// </summary>
    FocusedFrameDraw,
    /// <summary>
    /// Hovered frame edge drawable or color, used for the standard framed controls
    /// </summary>
    HoveredFrameDraw,
    /// <summary>
    /// Padding inside the frame of a control or drawable.
    /// </summary>
    Padding,
    /// <summary>
    /// Padding inside the frame of a control or drawable that includes extra padding for contents.
    /// </summary>
    ContentPadding,

    /// <summary>
    /// Button surface drawable or color, used for the standard buttons
    /// </summary>
    ButtonSurface,
    /// <summary>
    /// Button hovered surface drawable or color, used for the standard buttons
    /// </summary>
    ButtonHoveredSurface,
    /// <summary>
    /// Button disabled surface drawable or color, used for the standard buttons
    /// </summary>
    ButtonDisabledSurface,
    /// <summary>
    /// Button focused surface drawable or color, used for the standard buttons
    /// </summary>
    ButtonFocusedSurface,
    /// <summary>
    /// Button pressed surface drawable or color, used for the standard buttons
    /// </summary>
    ButtonPressedSurface,
    /// <summary>
    /// Button down surface drawable or color, used for the standard buttons
    /// </summary>
    ButtonDownSurface,
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
    /// Placeholder hovered image for buttons with no images set
    /// </summary>
    ButtonHoveredImage,
    /// <summary>
    /// Placeholder pressed image for buttons with no images set
    /// </summary>
    ButtonPressedImage,
    /// <summary>
    /// Placeholder down image for buttons with no images set
    /// </summary>
    ButtonDownImage,
    /// <summary>
    /// Placeholder focused image for buttons with no images set
    /// </summary>
    ButtonFocusedImage,
    /// <summary>
    /// Placeholder disabled image for buttons with no images set
    /// </summary>
    ButtonDisabledImage,

    /// <summary>
    /// Image tint for buttons with aligned image content
    /// </summary>
    ButtonImageTint,
    /// <summary>
    /// Tint of image for hovered buttons with aligned image content
    /// </summary>
    ButtonHoveredImageTint,
    /// <summary>
    /// Tint of image for pressed buttons with aligned image content
    /// </summary>
    ButtonPressedImageTint,
    /// <summary>
    /// Tint of image for down buttons with aligned image content
    /// </summary>
    ButtonDownImageTint,
    /// <summary>
    /// Tint of image for focused buttons with aligned image content
    /// </summary>
    ButtonFocusedImageTint,
    /// <summary>
    /// Tint of image for disabled buttons with aligned image content
    /// </summary>
    ButtonDisabledImageTint,

    /// <summary>
    /// Image offset for buttons with aligned image content
    /// </summary>
    ButtonImageOffset,
    /// <summary>
    /// Offset of image for hovered buttons with aligned image content
    /// </summary>
    ButtonHoveredImageOffset,
    /// <summary>
    /// Offset of image for pressed buttons with aligned image content
    /// </summary>
    ButtonPressedImageOffset,
    /// <summary>
    /// Offset of image for down buttons with aligned image content
    /// </summary>
    ButtonDownImageOffset,
    /// <summary>
    /// Offset of image for focused buttons with aligned image content
    /// </summary>
    ButtonFocusedImageOffset,
    /// <summary>
    /// Offset of image for disabled buttons with aligned image content
    /// </summary>
    ButtonDisabledImageOffset,

    /// <summary>
    /// Image padding for buttons with aligned image content
    /// </summary>
    ButtonImagePadding,
    /// <summary>
    /// Padding of image for hovered buttons with aligned image content
    /// </summary>
    ButtonHoveredImagePadding,
    /// <summary>
    /// Padding of image for pressed buttons with aligned image content
    /// </summary>
    ButtonPressedImagePadding,
    /// <summary>
    /// Padding of image for down buttons with aligned image content
    /// </summary>
    ButtonDownImagePadding,
    /// <summary>
    /// Padding of image for focused buttons with aligned image content
    /// </summary>
    ButtonFocusedImagePadding,
    /// <summary>
    /// Padding of image for disabled buttons with aligned image content
    /// </summary>
    ButtonDisabledImagePadding,

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
    TextColor,
    /// <summary>
    /// Color of text in focused input fields and controls
    /// </summary>
    FocusedTextColor,
    /// <summary>
    /// Color of text in disabled input fields and controls
    /// </summary>
    DisabledTextColor,
    /// <summary>
    /// Color of selected text in input fields and controls
    /// </summary>
    SelectedTextColor,
    /// <summary>
    /// Color of selected text in focused input fields and controls
    /// </summary>
    FocusedSelectedTextColor,
    /// <summary>
    /// Color of selected text in disabled input fields and controls
    /// </summary>
    DisabledSelectedTextColor,

    /// <summary>
    /// Background of text input field's text selection
    /// </summary>
    TextSelectionBackground,
    /// <summary>
    /// Background of focused text input field's text selection
    /// </summary>
    FocusedTextSelectionBackground,
    /// <summary>
    /// Background of disabled text input field's text selection
    /// </summary>
    DisabledTextSelectionBackground,

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
    /// Buttonhovered surface drawable or color, used for the combobox buttons
    /// </summary>
    ComboBoxButtonHoveredSurface,
    /// <summary>
    /// Buttondisabled surface drawable or color, used for the combobox buttons
    /// </summary>
    ComboBoxButtonDisabledSurface,
    /// <summary>
    /// Buttonfocused surface drawable or color, used for the combobox buttons
    /// </summary>
    ComboBoxButtonFocusedSurface,
    /// <summary>
    /// Buttonpressed surface drawable or color, used for the combobox buttons
    /// </summary>
    ComboBoxButtonPressedSurface,
    /// <summary>
    /// Buttondown surface drawable or color, used for the combobox buttons
    /// </summary>
    ComboBoxButtonDownSurface,
    /// <summary>
    /// Offset to move contents visually when the combobox button is pressed.
    /// </summary>
    ComboBoxButtonContentPressedOffset,

    /// <summary>
    /// Image for combobox buttons with no images set
    /// </summary>
    ComboBoxButtonImage,
    /// <summary>
    /// Hovered image for combobox buttons with no images set
    /// </summary>
    ComboBoxButtonHoveredImage,
    /// <summary>
    /// Pressed image for combobox buttons with no images set
    /// </summary>
    ComboBoxButtonPressedImage,
    /// <summary>
    /// Down image for combobox buttons with no images set
    /// </summary>
    ComboBoxButtonDownImage,
    /// <summary>
    /// Focused image for combobox buttons with no images set
    /// </summary>
    ComboBoxButtonFocusedImage,
    /// <summary>
    /// Disabled image for combobox buttons with no images set
    /// </summary>
    ComboBoxButtonDisabledImage,

    /// <summary>
    /// Image tint for combobox buttons with aligned image content
    /// </summary>
    ComboBoxButtonImageTint,
    /// <summary>
    /// Tint of image for hovered buttons with aligned image content
    /// </summary>
    ComboBoxButtonHoveredImageTint,
    /// <summary>
    /// Tint of image for pressed buttons with aligned image content
    /// </summary>
    ComboBoxButtonPressedImageTint,
    /// <summary>
    /// Tint of image for down buttons with aligned image content
    /// </summary>
    ComboBoxButtonDownImageTint,
    /// <summary>
    /// Tint of image for focused buttons with aligned image content
    /// </summary>
    ComboBoxButtonFocusedImageTint,
    /// <summary>
    /// Tint of image for disabled buttons with aligned image content
    /// </summary>
    ComboBoxButtonDisabledImageTint,

    /// <summary>
    /// Image offset for combobox buttons with aligned image content
    /// </summary>
    ComboBoxButtonImageOffset,
    /// <summary>
    /// Offset of image for hovered buttons with aligned image content
    /// </summary>
    ComboBoxButtonHoveredImageOffset,
    /// <summary>
    /// Offset of image for pressed buttons with aligned image content
    /// </summary>
    ComboBoxButtonPressedImageOffset,
    /// <summary>
    /// Offset of image for down buttons with aligned image content
    /// </summary>
    ComboBoxButtonDownImageOffset,
    /// <summary>
    /// Offset of image for focused buttons with aligned image content
    /// </summary>
    ComboBoxButtonFocusedImageOffset,
    /// <summary>
    /// Offset of image for disabled buttons with aligned image content
    /// </summary>
    ComboBoxButtonDisabledImageOffset,

    /// <summary>
    /// Image padding for combobox buttons with aligned image content
    /// </summary>
    ComboBoxButtonImagePadding,
    /// <summary>
    /// Padding of image for hovered buttons with aligned image content
    /// </summary>
    ComboBoxButtonHoveredImagePadding,
    /// <summary>
    /// Padding of image for pressed buttons with aligned image content
    /// </summary>
    ComboBoxButtonPressedImagePadding,
    /// <summary>
    /// Padding of image for down buttons with aligned image content
    /// </summary>
    ComboBoxButtonDownImagePadding,
    /// <summary>
    /// Padding of image for focused buttons with aligned image content
    /// </summary>
    ComboBoxButtonFocusedImagePadding,
    /// <summary>
    /// Padding of image for disabled buttons with aligned image content
    /// </summary>
    ComboBoxButtonDisabledImagePadding,

    /// <summary>
    /// Horizontal alignment of contents in a combobox button
    /// </summary>
    ComboBoxButtonImageHorzAlignment,
    /// <summary>
    /// Vertical alignment of contents in a combobox button
    /// </summary>
    ComboBoxButtonImageVertAlignment,



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
    /// Painter information for controls with an image content that is aligned within bounds
    /// </summary>
    AlignedImageContentPainter,
};

API_ENUM()
enum class FudgetFramedControlPartIds
{
    First = 1000,

    FramePainter = First,

    FieldBackground,
    HoveredFieldBackground,
    PressedFieldBackground,
    DownFieldBackground,
    DisabledFieldBackground,
    FocusedFieldBackground,
    FieldDrawPadding,
    HoveredFieldDrawPadding,
    PressedFieldDrawPadding,
    DownFieldDrawPadding,
    DisabledFieldDrawPadding,
    FocusedFieldDrawPadding,

    FrameDraw,
    HoveredFrameDraw,
    PressedFrameDraw,
    DownFrameDraw,
    FocusedFrameDraw,
    DisabledFrameDraw,
    FrameDrawPadding,
    HoveredFrameDrawPadding,
    PressedFrameDrawPadding,
    DownFrameDrawPadding,
    FocusedFrameDrawPadding,
    DisabledFrameDrawPadding,

    Padding,
    ContentPadding,
};

API_ENUM()
enum class FudgetTextFieldPartIds
{
    First = 2000,

    TextSelectionBackground = First,
    FocusedTextSelectionBackground,
    DisabledTextSelectionBackground,

    TextColor,
    FocusedTextColor,
    DisabledTextColor,
    SelectedTextColor,
    FocusedSelectedTextColor,
    DisabledSelectedTextColor,

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

    ContentPainter = First,
    ContentPressedOffset,
};

API_ENUM()
enum class FudgetAlignedImagePartIds
{
    First = 4000,

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


    HorzImageAlign,
    VertImageAlign,
};

API_ENUM()
enum class FudgetComboBoxPartIds
{
    First = 5000,

    ButtonWidth = First,
};

