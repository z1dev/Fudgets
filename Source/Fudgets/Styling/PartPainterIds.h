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
    /// Background drawable or color, used for standard framed controls
    /// </summary>
    Background = First,
    /// <summary>
    /// Hovered background drawable or color, used for standard framed controls
    /// </summary>
    HoveredBackground,
    /// <summary>
    /// Disabled background drawable or color, used for standard framed controls
    /// </summary>
    DisabledBackground,
    /// <summary>
    /// Focused background drawable or color, used for standard framed controls
    /// </summary>
    FocusedBackground,
    /// <summary>
    /// Selected background drawable or color, used for standard framed controls
    /// </summary>
    SelectedBackground,

    /// <summary>
    /// Padding to limit the bounds when drawing the background of standard framed controls
    /// </summary>
    BackgroundPadding,
    /// <summary>
    /// Padding to limit the bounds when drawing the background of hovered standard framed controls
    /// </summary>
    HoveredBackgroundPadding,
    /// <summary>
    /// Padding to limit the bounds when drawing the background of disabled standard framed controls
    /// </summary>
    DisabledBackgroundPadding,
    /// <summary>
    /// Padding to limit the bounds when drawing the background of focused standard framed controls
    /// </summary>
    FocusedBackgroundPadding,
    /// <summary>
    /// Padding to limit the bounds when drawing the selected background of standard framed controls
    /// </summary>
    SelectedBackgroundPadding,

    /// <summary>
    /// Color to multiply every pixel with when drawing the background of standard framed controls
    /// </summary>
    BackgroundTint,
    /// <summary>
    /// Color to multiply every pixel with when drawing the background of hovered standard framed controls
    /// </summary>
    HoveredBackgroundTint,
    /// <summary>
    /// Color to multiply every pixel with when drawing the background of disabled standard framed controls
    /// </summary>
    DisabledBackgroundTint,
    /// <summary>
    /// Color to multiply every pixel with when drawing the background of focused standard framed controls
    /// </summary>
    FocusedBackgroundTint,
    /// <summary>
    /// Color to multiply every pixel with when drawing the selected background of standard framed controls
    /// </summary>
    SelectedBackgroundTint,

    /// <summary>
    /// Frame edge drawable or color, used for standard framed controls
    /// </summary>
    FrameDraw,
    /// <summary>
    /// Hovered frame edge drawable or color, used for standard framed controls
    /// </summary>
    HoveredFrameDraw,
    /// <summary>
    /// Disabled frame edge drawable or color, used for standard framed controls
    /// </summary>
    DisabledFrameDraw,
    /// <summary>
    /// Focused frame edge drawable or color, used for standard framed controls
    /// </summary>
    FocusedFrameDraw,
    /// <summary>
    /// Selected frame edge drawable or color, used for standard framed controls
    /// </summary>
    SelectedFrameDraw,

    /// <summary>
    /// Color to multiply every pixel with when drawing the frame of standard framed controls
    /// </summary>
    FrameTint,
    /// <summary>
    /// Color to multiply every pixel with when drawing the frame of hovered standard framed controls
    /// </summary>
    HoveredFrameTint,
    /// <summary>
    /// Color to multiply every pixel with when drawing the frame of disabled standard framed controls
    /// </summary>
    DisabledFrameTint,
    /// <summary>
    /// Color to multiply every pixel with when drawing the frame of focused standard framed controls
    /// </summary>
    FocusedFrameTint,
    /// <summary>
    /// Color to multiply every pixel with when drawing the frame of selected standard framed controls
    /// </summary>
    SelectedFrameTint,

    /// <summary>
    /// Padding to limit the bounds when drawing the background of standard framed controls
    /// </summary>
    FramePadding,
    /// <summary>
    /// Padding to limit the bounds when drawing the background of hovered standard framed controls
    /// </summary>
    HoveredFramePadding,
    /// <summary>
    /// Padding to limit the bounds when drawing the background of disabled standard framed controls
    /// </summary>
    DisabledFramePadding,
    /// <summary>
    /// Padding to limit the bounds when drawing the background of focused standard framed controls
    /// </summary>
    FocusedFramePadding,
    /// <summary>
    /// Padding to limit the bounds when drawing the background of selected standard framed controls
    /// </summary>
    SelectedFramePadding,

    /// <summary>
    /// Visual padding inside the frame of a control or drawable for some contents .
    /// </summary>
    VisualPadding,
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
    /// Tint for background of text input field's text selection
    /// </summary>
    SelectedTextBackground,
    /// <summary>
    /// Tint for background of focused text input field's text selection
    /// </summary>
    FocusedSelectedTextBackground,
    /// <summary>
    /// Tint for background of disabled text input field's text selection
    /// </summary>
    DisabledSelectedTextBackground,


    /// <summary>
    /// Background of text input field's text selection
    /// </summary>
    SelectedTextBackgroundTint,
    /// <summary>
    /// Background of focused text input field's text selection
    /// </summary>
    FocusedSelectedTextBackgroundTint,
    /// <summary>
    /// Background of disabled text input field's text selection
    /// </summary>
    DisabledSelectedTextBackgroundTint,

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
    /// Background for selected items in list controls
    /// </summary>
    ListItemSelectedBackground,
    /// <summary>
    /// Background for hovered items in list controls
    /// </summary>
    ListItemHoveredBackground,
    /// <summary>
    /// Background for hovered and selected items in list controls
    /// </summary>
    ListItemHoveredSelectedBackground,
    /// <summary>
    /// Background for focused and selected items in list controls
    /// </summary>
    ListItemFocusedSelectedBackground,
    /// <summary>
    /// Background for disabled and selected items in list controls
    /// </summary>
    ListItemDisabledSelectedBackground,

    /// <summary>
    /// Tint for selected items in list controls
    /// </summary>
    ListItemSelectedBackgroundTint,
    /// <summary>
    /// Tint for hovered items in list controls
    /// </summary>
    ListItemHoveredBackgroundTint,
    /// <summary>
    /// Tint for hovered and selected items in list controls
    /// </summary>
    ListItemHoveredSelectedBackgroundTint,
    /// <summary>
    /// Tint for focused and selected items in list controls
    /// </summary>
    ListItemFocusedSelectedBackgroundTint,
    /// <summary>
    /// Tint for disabled and selected items in list controls
    /// </summary>
    ListItemDisabledSelectedBackgroundTint,

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

    FieldBackground,
    HoveredFieldBackground,
    PressedFieldBackground,
    DownFieldBackground,
    DisabledFieldBackground,
    FocusedFieldBackground,
    SelectedFieldBackground,

    FieldPadding,
    HoveredFieldPadding,
    PressedFieldPadding,
    DownFieldPadding,
    DisabledFieldPadding,
    FocusedFieldPadding,
    SelectedFieldPadding,

    FieldTint,
    HoveredFieldTint,
    PressedFieldTint,
    DownFieldTint,
    DisabledFieldTint,
    FocusedFieldTint,
    SelectedFieldTint,

    FrameDraw,
    HoveredFrameDraw,
    PressedFrameDraw,
    DownFrameDraw,
    DisabledFrameDraw,
    FocusedFrameDraw,
    SelectedFrameDraw,

    FramePadding,
    HoveredFramePadding,
    PressedFramePadding,
    DownFramePadding,
    DisabledFramePadding,
    FocusedFramePadding,
    SelectedFramePadding,

    FrameTint,
    HoveredFrameTint,
    PressedFrameTint,
    DownFrameTint,
    DisabledFrameTint,
    FocusedFrameTint,
    SelectedFrameTint,

    VisualPadding,
    ContentPadding,
};

API_ENUM()
enum class FudgetTextFieldPartIds
{
    First = 2000,

    SelectedTextBackground = First,
    FocusedSelectedTextBackground,
    DisabledSelectedTextBackground,
    SelectedTextBackgroundTint = First,
    FocusedSelectedTextBackgroundTint,
    DisabledSelectedTextBackgroundTint,

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

API_ENUM()
enum class FudgetListBoxPartIds
{
    First = 6000,

    ItemPainter = First,
    TextPainter,

    SelectedBackground,
    HoveredBackground,
    HoveredSelectedBackground,
    FocusedSelectedBackground,
    DisabledSelectedBackground,

    SelectedBackgroundTint,
    HoveredBackgroundTint,
    HoveredSelectedBackgroundTint,
    FocusedSelectedBackgroundTint,
    DisabledSelectedBackgroundTint,

};

