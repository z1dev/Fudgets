#pragma once

#include "TextBoxBase.h"
#include "TextBoxPainter.h"

/// <summary>
/// Text box sizing and auto sizing options determine if the text box size is automatically adjusted to fit its
/// contents, or entirely relies on the user set values and the layout
/// </summary>
API_ENUM()
enum class FudgetTextBoxSizingMode
{
    /// <summary>
    /// The size of the textbox is determined by the user set sizes and the layout.
    /// </summary>
    Normal,
    /// <summary>
    /// The textbox is resized to fit its contents, ignoring the user set sizes, but not the layout. Adding
    /// new line characters in the text is the only way to influence the size of the control. Word wrap settings
    /// are respected if the text box is forced to be smaller than what its contents require.
    /// </summary>
    AutoSize,
    /// <summary>
    /// The width of the textbox is determined by the user set sizes and the layout. The height is adjusted to fit
    /// the text, respecting new line characters and the word wrap setting, if the layout allows it
    /// </summary>
    AutoHeight
};


API_ENUM()
enum class FudgetTextBoxIds
{
    First = 6000,

    FramePainter = First,
    FrameStyle,

    FrameDraw,
    DisabledFrameDraw,
    FocusedFrameDraw,

    ContentPadding,

    TextPainter,
    TextStyle,

    TextSelBg,
    DisabledTextSelBg,

    TextColor,
    DisabledTextColor,
    SelectedTextColor,
    DisabledSelectedTextColor,

    Font,

    CaretDraw,
    CaretBlinkTime,
    CaretWidth,
    CaretScrollCount,
    BeamCursor,
    SnapTopLine,
};

/// <summary>
/// Multiline input box for unformatted basic text. Text can span over multiple lines either
/// when lines are too long and word wrapping is enabled, or if there are newline characters
/// in the text.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetTextBox : public FudgetTextBoxBase
{
    using Base = FudgetTextBoxBase;
    DECLARE_SCRIPTING_TYPE(FudgetTextBox);
    API_AUTO_SERIALIZATION();

public:
    /// <inheritdoc />
    void OnInitialize() override;

    /// <inheritdoc />
    void OnStyleInitialize() override;

    /// <inheritdoc />
    StringView GetText() const override { return _text; }

    /// <inheritdoc />
    int GetTextLength() const override { return _text.Length(); }

    /// <inheritdoc />
    void OnDraw() override;

    /// <inheritdoc />
    void OnSizeChanged() override;

    /// <inheritdoc />
    int CharIndexAt(Float2 pos) override;

    /// <inheritdoc />
    void DoPositionChanged(int old_caret_pos, int old_sel_pos) override;

    /// <inheritdoc />
    void DoTextEdited(int old_caret_pos, int old_sel_pos) override;

    /// <inheritdoc />
    void OnUpdate(float delta_time) override;

    ///// <inheritdoc />
    //void OnFocusChanged(bool focused, FudgetControl *other) override;

    ///// <inheritdoc />
    //void OnVirtuallyEnabledChanged() override;

    /// <inheritdoc />
    void OnMouseMove(Float2 pos, Float2 global_pos) override;

    /// <inheritdoc />
    FudgetInputResult OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click) override;

    /// <inheritdoc />
    bool OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button) override;

    /// <inheritdoc />
    CursorType GetContextCursor() const override;

    /// <inheritdoc />
    Float2 GetLayoutHintSize() const override;

    /// <inheritdoc />
    Float2 GetLayoutMinSize() const override;

    /// <inheritdoc />
    Float2 GetLayoutMaxSize() const override;

    /// <inheritdoc />
    bool OnMeasure(Float2 available, API_PARAM(Out) Float2 &wanted, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size) override;

    /// <summary>
    /// Whether the lines of a text should wrap to the new line if they are too long.
    /// </summary>
    API_PROPERTY() bool GetLineWrap() const { return _line_wrap; }
    /// <summary>
    /// Whether the lines of a text should wrap to the new line if they are too long.
    /// </summary>
    API_PROPERTY() void SetLineWrap(bool value);

    /// <summary>
    /// Method used to wrap long lines to the next line
    /// </summary>
    API_PROPERTY() FudgetLineWrapMode GetLineWrapMode() const { return _wrap_mode; }

    /// <summary>
    /// Method used to wrap long lines to the next line
    /// </summary>
    API_PROPERTY() void SetLineWrapMode(FudgetLineWrapMode value);

    /// <summary>
    /// Snapping makes the top line in the text box snap to the top edge of the control when scrolling. When not snapping,
    /// the contents of the text box scroll free. False by default
    /// </summary>
    /// <returns>Whether the top line is snapping to the top edge, or the contents scroll free instead</returns>
    API_PROPERTY() bool GetSnapTopLine() const { return _snap_top_line; }

    /// <summary>
    /// Snapping makes the top line in the text box snap to the top edge of the control when scrolling. When not snapping,
    /// the contents of the text box scroll free.
    /// </summary>
    /// <param name="value">Whether the top line is snapping to the top edge, or the contents scroll free instead</param>
    API_PROPERTY() void SetSnapTopLine(bool value);

    /// <inheritdocs />
    bool SizeDependsOnSpace() const override;

    /// <summary>
    /// Which, if any of the hint size dimensions are calculated by the text box based on its text contents
    /// </summary>
    API_PROPERTY() FudgetAutoSizing GetAutoSize() const { return _auto_size; }
    /// <summary>
    /// Which, if any of the hint size dimensions are calculated by the text box based on its text contents
    /// </summary>
    /// <param name="value">The new auto size setting</param>
    API_PROPERTY() void SetAutoSize(FudgetAutoSizing value);
protected:
    /// <inheritdoc />
    FudgetControlFlag GetInitFlags() const override;

    /// <inheritdoc />
    void SetTextInternal(const StringView &value) override;

    /// <inheritdoc />
    bool IsWhitespace(int index) const override;

    /// <inheritdoc />
    void InsertCharacter(int index, Char ch) override;

    /// <inheritdoc />
    void DeleteCharacters(int start_index, int end_index) override;

    /// <inheritdoc />
    void ReplaceCharacters(int start_index, int end_index, const StringView &with) override;

    /// <inheritdoc />
    void ReplaceCharacters(int start_index, int end_index, Char ch) override;

    /// <inheritdoc />
    int GetCaretPosUp() override;

    /// <inheritdoc />
    int GetCaretPosDown() override;

    /// <inheritdoc />
    int GetCaretPosPageUp() override;

    /// <inheritdoc />
    int GetCaretPosPageDown() override;

    /// <inheritdoc />
    void SizeOrPosModified(FudgetLayoutDirtyReason dirt_flags) override;

    /// <inheritdoc />
    FudgetTextBoxFlags GetTextBoxInitFlags() const override;
private:
    FudgetPadding GetInnerPadding() const;

    void MarkTextDirty();

    void ScrollToPos();
    void FixScrollPos();

    FORCE_INLINE FudgetMultilineTextMeasurements& GetMeasurements() { if (_lines_dirty) Process(); return _text_measurements; }
    void Process();

    void SnapTopLine();



    FudgetFramedFieldPainter *_frame_painter;
    FudgetPartPainterMapping _default_frame_painter_mapping;

    FudgetMultiLineTextPainter *_text_painter;
    FudgetPartPainterMapping _default_text_painter_mapping;

    FudgetMultilineTextMeasurements _text_measurements;

    // Time passed in seconds since caret blink started. The caret is visible when this value is below
    // _caret_blink_time and hidden when it is over it.
    float _blink_passed;

    bool _show_beam_cursor;
    CursorType _beam_cursor;

    // Number of characters to scroll in view when moving in a line to the left or right and the caret leaves the
    // bounds of the control.
    int _character_scroll_count;

    // Make the line snap to the top, letting partial lines show on the bottom when scrolling down.
    bool _snap_top_line;

    // Tytpe of auto sizing or normal sizing
    FudgetTextBoxSizingMode _sizing_mode;

    FudgetDrawArea _caret_draw;
    float _caret_blink_time;
    float _caret_width;

    // Rightmost X position of the caret when moving with the up and down arrows. This is changed to -1 when
    // the caret position changes by a method that should reset it.
    float _caret_updown_x;

    // How much the contents of the text box are scrolled left or up. It is always positive
    Float2 _scroll_pos;
    String _text;
    // There was a change in the text or the control size changed that requires new measurements for the lines.
    // The measurements will take place the next time the data is needed.
    bool _lines_dirty;

    // Wrapping of lines is turned on or off
    bool _line_wrap;
    // How to wrap lines
    FudgetLineWrapMode _wrap_mode;

    FudgetAutoSizing _auto_size;

    // The space used for calculating the cached size.
    float _measure_space;
    // Used for auto sizing
    Float2 _cached_size;
};

