#pragma once

#include "Engine/Scripting/ScriptingObject.h"

#include "PartPainters.h"


/// <summary>
/// Word wrapping mode used in Fudget text controls. 
/// </summary>
API_ENUM()
enum class FudgetLineWrapMode
{
    /// <summary>
    /// Wrap word at whitespace boundary only. The whitespace is omitted when drawing, but any whitespace
    /// following is displayed on the next line.
    /// </summary>
    Whitespace,
    /// <summary>
    /// Same as normal Whitespace wrapping, but also wraps when a word is too long to fit on a line.
    /// </summary>
    WhitespaceLongWord,
    /// <summary>
    /// Wraps on any character that wouldn't fit the line. No character is omitted while drawing.
    /// </summary>
    Anywhere,
    /// <summary>
    /// Using a custom object that decides where to break text and if anything needs to be omitted.
    /// </summary>
    Custom
};

/// <summary>
/// Data about a single line measured by a FudgetMultiLineTextPainter
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetLineMeasurements
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetLineMeasurements);

    FudgetLineMeasurements() : StartIndex(0), EndIndex(0), Size(0), Location(0) {}

    /// <summary>
    /// Index of first character to display in the line
    /// </summary>
    API_FIELD() int StartIndex;

    /// <summary>
    /// Index of the character after last to display in the line
    /// </summary>
    API_FIELD() int EndIndex;

    /// <summary>
    /// Line dimensions
    /// </summary>
    API_FIELD() Int2 Size;

    /// <summary>
    /// Top left corner of the line
    /// </summary>
    API_FIELD() Int2 Location;
};

template<>
struct TIsPODType<FudgetLineMeasurements>
{
    enum { Value = true };
};

/// <summary>
/// Data about lines of a string after measured by a FudgetMultiLineTextPainter
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetMultilineTextMeasurements
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetMultilineTextMeasurements);

    FudgetMultilineTextMeasurements() : Scale(1.f), Lines(), Size(0) {}

    /// <summary>
    /// The text that was measured. The original text buffer or string must persist while this measurement is used.
    /// </summary>
    API_FIELD() StringView Text;

    /// <summary>
    /// Scale that was used to measure the text
    /// </summary>
    API_FIELD() float Scale;

    /// <summary>
    /// Line bounds and character indexes
    /// </summary>
    API_FIELD() Array<FudgetLineMeasurements> Lines;

    /// <summary>
    /// Size of all the measured lines
    /// </summary>
    API_FIELD() Int2 Size;
};

template<>
struct TIsPODType<FudgetMultilineTextMeasurements>
{
    enum { Value = true };
};

/// <summary>
/// Options that determine how a string of text should be measured and displayed by a FudgetMultiLineTextPainter
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetMultiLineTextOptions
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetMultiLineTextOptions);

    FudgetMultiLineTextOptions() : Wrapping(false), WrapMode(FudgetLineWrapMode::Whitespace), Spans() {}

    /// <summary>
    /// When true, wrap the text inside some bounds
    /// </summary>
    API_FIELD() bool Wrapping;

    /// <summary>
    /// Word wrapping mode
    /// </summary>
    API_FIELD() FudgetLineWrapMode WrapMode;

    /// <summary>
    /// Ranges for drawing text in different ways. It depends on the painter how it uses them. For example
    /// a text box painter could take the first item in the spans array and use it to show selections. In a
    /// different painter the second index would be for spans of bold text. The ranges refer to the whole Text,
    /// and not just the part specified by Range.
    /// </summary>
    API_FIELD() Span<FudgetTextRangeSpan> Spans;
};

template<>
struct TIsPODType<FudgetMultiLineTextOptions>
{
    enum { Value = true };
};


/// <summary>
/// Base class for text painters of multi-line text.
/// </summary>
API_CLASS(Abstract)
class FUDGETS_API FudgetMultiLineTextPainter : public FudgetPartPainter
{
    using Base = FudgetPartPainter;
    DECLARE_SCRIPTING_TYPE(FudgetMultiLineTextPainter);
public:
    /// <summary>
    /// Draws multi-line text based on previous measurements and the passed options
    /// </summary>
    /// <param name="control">Control to paint in</param>
    /// <param name="bounds">Bounds to draw in. Used for clipping</param>
    /// <param name="offset">Added to the upper left of bounds, determines where the topmost line would be drawn</param>
    /// <param name="state">State of the control</param>
    /// <param name="options">Options for text, like scale or wrapping mode</param>
    /// <param name="measurements">Line measurements calculated previously with Measure</param>
    API_FUNCTION() virtual void Draw(FudgetControl *control, const Rectangle &bounds, const Int2 &offset, FudgetVisualControlState states,
        const FudgetMultiLineTextOptions &options, const FudgetMultilineTextMeasurements &measurements) {}

    /// <summary>
    /// Returns the kerning distance between two characters using the font of the painter. If no font is set, the result is 0
    /// </summary>
    /// <param name="a">First character</param>
    /// <param name="b">Other character</param>
    /// <param name="scale">Scale used for measurement</param>
    /// <returns>Kerning distance between characters</returns>
    API_FUNCTION() virtual int GetKerning(Char a, Char b, float scale) const { return 0; }

    /// <summary>
    /// Finds the index of character in the text at a position. If the position is outside a line, the closest character index is returned.
    /// </summary>
    /// <param name="control">Control used for hit testing</param>
    /// <param name="measurements">Line measurements calculated previously with Measure</param>
    /// <param name="point">The position of the character to look for</param>
    /// <returns>Index of the character at the given position</returns>
    API_FUNCTION() virtual int HitTest(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, const Int2 &point) { return 0; }

    /// <summary>
    /// Finds the index of the line closest to the y position
    /// </summary>
    /// <param name="control">Control used for hit testing</param>
    /// <param name="measurements">Line measurements calculated previously with Measure</param>
    /// <param name="y_position">Y position of line to look for</param>
    /// <returns>Index of line at the given position</returns>
    API_FUNCTION() virtual int LineAtPos(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, int y_position) { return 0; }

    /// <summary>
    /// Finds the index of character in one line of the text at a position. If the position is outside a line, the closest character index is returned.
    /// </summary>
    /// <param name="control">Control used for hit testing</param>
    /// <param name="measurements">Line measurements calculated previously with Measure</param>
    /// <param name="line_index">Index of line to measure in. </param>
    /// <param name="x_position">The position of the character to look for</param>
    /// <returns>Index of the character at the given position</returns>
    API_FUNCTION() virtual int LineHitTest(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, int line_index, int x_position) { return 0; }

    /// <summary>
    /// Finds the upper left corner of the character at an index
    /// </summary>
    /// <param name="control">Control to find the character in</param>
    /// <param name="measurements">Line measurements calculated previously with Measure</param>
    /// <param name="char_index">Index of character to look for</param>
    /// <returns>Position of the upper left corner of the character</returns>
    API_FUNCTION() virtual Int2 GetCharacterPosition(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, int char_index) const { return Int2::Zero; }

    /// <summary>
    /// Measures text using the current font.
    /// </summary>
    /// <param name="control">Control used for measurement</param>
    /// <param name="text">Text to measure</param>
    /// <param name="scale">Text scale</param>
    /// <returns>Dimensions of the measured text</returns>
    API_FUNCTION() virtual Int2 Measure(FudgetControl *control, const StringView &text, float scale) { return Int2::Zero; }

    /// <summary>
    /// Measures the text and separates it to lines. The lines are checked for word wrapping based on the passed options
    /// and newline characters. The result is used in the other functions for drawing and hit testing.
    /// </summary>
    /// <param name="control">Control used for measurement</param>
    /// <param name="bounds_width">Width used for finding word breaks</param>
    /// <param name="text">Text to measure. It should be a persistent string buffer while the measurements are used</param>
    /// <param name="scale">Scale to use for measurements</param>
    /// <param name="options">Options for text, like the offset, selection spans etc.</param>
    /// <param name="result">The measured lines</param>
    API_FUNCTION() virtual void MeasureLines(FudgetControl *control, int bounds_width, const StringView &text, float scale,
        const FudgetMultiLineTextOptions &options, API_PARAM(Ref) FudgetMultilineTextMeasurements &result) {}

    /// <summary>
    /// Finds the line that holds the character at an index. If the character is omitted due to word wrapping, the next line's
    /// index is returned.
    /// </summary>
    /// <param name="measurements">Line measurements calculated previously with Measure on the text</param>
    /// <param name="char_index">Character index to search for</param>
    /// <returns>Index of the line</returns>
    API_FUNCTION() virtual int GetCharacterLine(FudgetMultilineTextMeasurements &measurements, int char_index) const { return -1; }

    /// <summary>
    /// Returns the height of a character in the text at an index position
    /// </summary>
    /// <param name="measurements">Line measurements calculated previously with Measure on the text</param>
    /// <param name="char_index">Index of character to look for</param>
    /// <returns>Height of the line at the character's position</returns>
    API_FUNCTION() virtual int GetCharacterLineHeight(const FudgetMultilineTextMeasurements &measurements, int char_index) const { return 0; }

    /// <summary>
    /// Returns the height of the font used by the painter.
    /// </summary>
    API_FUNCTION() virtual int GetFontHeight() const { return 0; }

protected:
    /// <summary>
    /// Appends new line measurements to the existing ones.
    /// </summary>
    /// <param name="pos">Location of the line</param>
    /// <param name="start_index">First character's index in the text on this line</param>
    /// <param name="end_index">The next character's index in the text after the end of this line</param>
    /// <param name="line_width">Width of the line in screen units</param>
    /// <param name="line_height">Height of the line in screen units</param>
    /// <param name="result">Measurements to expand</param>
    API_FUNCTION() virtual void AddLine(API_PARAM(Ref) Int2 &pos, int start_index, int end_index, int line_width, int line_height, API_PARAM(Ref) FudgetMultilineTextMeasurements &result);
};


//API_ENUM()
//enum class FudgetTextBoxPainterIds
//{
//    First = 7000,
//
//    SelectionDraw = First,
//    FocusedSelectionDraw,
//    DisabledSelectionDraw,
//    TextColor,
//    FocusedTextColor,
//    DisabledTextColor,
//    SelectedTextColor,
//    FocusedSelectedTextColor,
//    DisabledSelectedTextColor,
//    Font,
//};


API_STRUCT(Attributes = "HideInEditor")
struct FUDGETS_API FudgetTextBoxPainterResources
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetTextBoxPainterResources);

    API_FIELD() int StateOrderIndex = -1;

    API_FIELD() int SelectionDraw = 0;
    API_FIELD() int FocusedSelectionDraw = 0;
    API_FIELD() int DisabledSelectionDraw = 0;
    API_FIELD() int TextColor = 0;
    API_FIELD() int FocusedTextColor = 0;
    API_FIELD() int DisabledTextColor = 0;
    API_FIELD() int SelectedTextColor = 0;
    API_FIELD() int FocusedSelectedTextColor = 0;
    API_FIELD() int DisabledSelectedTextColor = 0;
    API_FIELD() int Font = 0;
};

/// <summary>
/// Text painter for multi-line text boxes with unformatted text.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetTextBoxPainter : public FudgetMultiLineTextPainter
{
    using Base = FudgetMultiLineTextPainter;
    DECLARE_SCRIPTING_TYPE(FudgetTextBoxPainter);
public:
    using ResourceMapping = FudgetTextBoxPainterResources;

    /// <inheritdoc />
    void Initialize(FudgetControl *control, /*FudgetStyle *style_override,*/ const Variant &mapping) override;

    /// <inheritdoc />
    void Draw(FudgetControl *control, const Rectangle &bounds, const Int2 &offset, FudgetVisualControlState states,
        const FudgetMultiLineTextOptions &options, const FudgetMultilineTextMeasurements &measurements) override;

    /// <inheritdoc />
    int GetKerning(Char a, Char b, float scale) const override;

    /// <inheritdoc />
    int HitTest(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, const Int2 &point) override;

    /// <inheritdoc />
    int LineAtPos(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, int y_position) override;

    /// <inheritdoc />
    int LineHitTest(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, int line_index, int x_position) override;

    /// <inheritdoc />
    Int2 GetCharacterPosition(FudgetControl *control, const FudgetMultilineTextMeasurements &measurements, int char_index) const override;

    /// <inheritdoc />
    Int2 Measure(FudgetControl *control, const StringView &text, float scale) override;

    /// <inheritdoc />
    void MeasureLines(FudgetControl *control, int bounds_width, const StringView &text, float scale,
        const FudgetMultiLineTextOptions &options, API_PARAM(Ref) FudgetMultilineTextMeasurements &result) override;

    /// <inheritdoc />
    int GetCharacterLine(FudgetMultilineTextMeasurements &measurements, int char_index) const override;

    /// <inheritdoc />
    int GetCharacterLineHeight(const FudgetMultilineTextMeasurements &measurements, int char_index) const override;

    /// <inheritdoc />
    int GetFontHeight() const override;

private:
    FudgetDrawArea _sel_area;
    FudgetDrawArea _focused_sel_area;
    FudgetDrawArea _disabled_sel_area;

    Color _text_color;
    Color _focused_text_color;
    Color _disabled_text_color;
    Color _selected_text_color;
    Color _focused_selected_text_color;
    Color _disabled_selected_text_color;

    FudgetFont _font;

    FudgetStateOrder *_state_order;
};
