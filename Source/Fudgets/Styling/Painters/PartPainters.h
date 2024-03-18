#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Render2D/Font.h"

#include "../../MarginStructs.h"
#include "../StyleStructs.h"
#include "../Style.h"
#include "../../Utils/Utils.h"

#include <map>

class FudgetTheme;
class FudgetStyle;
class FudgetControl;
class FudgetDrawable;


/// <summary>
/// Supported states for framed field drawing
/// </summary>
API_ENUM(Attributes = "Flags")
enum class FudgetVisualControlState : uint64
{
    /// <summary>
    /// Normal state that means no other state is set.
    /// </summary>
    Normal = 0,

    /// <summary>
    /// Pressed appearance of a part, usually for buttons.
    /// </summary>
    Pressed = 1 << 0,

    /// <summary>
    /// Down appearance of a part, usually for buttons.
    /// </summary>
    Down = 1 << 1,

    /// <summary>
    /// Focused appearance of a part that means it's the current part for keyboard interaction.
    /// </summary>
    Focused = 1 << 2,

    /// <summary>
    /// Disabled appearance of a part that means the users can't interact with it
    /// </summary>
    Disabled = 1 << 3,

    /// <summary>
    /// Hovered appearance of a part, usually when the mouse pointer is over it
    /// </summary>
    Hovered = 1 << 4,

    /// <summary>
    /// Selected appearance of a part, usually for items in lists.
    /// </summary>
    Selected = 1 << 5,

    /// <summary>
    /// Expanded appearance of a part, usually for items in lists.
    /// </summary>
    Expanded = 1 << 6,

    /// <summary>
    /// Opened appearance of a part, usually for items in lists.
    /// </summary>
    Opened = 1 << 7,

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

    template<typename T>
    static FudgetPartPainterMapping InitializeMapping(const typename T::Mapping &resource_mapping)
    {
        FudgetPartPainterMapping result;
        result.PainterType = T::TypeInitializer.GetType().Fullname;
        result.Mapping = StructToVariant(resource_mapping);
        return result;
    }

    /// <summary>
    /// Initializes the painter, caching the resources it will draw with.
    /// </summary>
    /// <param name="control">Control that holds the theme and style to use for fetching resources</param>
    /// <param name="style_override">A style, that when not null, is used for looking up resources before the control is checked.</param>
    /// <param name="mapping">Mapping from id to id for the specific painter. Each painter expects a different mapping structure.</param>
    API_FUNCTION() virtual void Initialize(FudgetControl *control,  const Variant &mapping) { }

    /// <summary>
    /// Updates the drawing state of the painter. Called by controls in OnUpdate if they registered to get updates
    /// </summary>
    /// <param name="delta_time">Time passed since last update</param>
    API_FUNCTION() virtual void Update(float delta_time) {}
protected:
    /// <summary>
    /// Tries to find a style that can be used to draw with this painter when a valid style wasn't provided on initialization
    /// </summary>
    /// <returns>A style appropriate for the painter</returns>
    API_PROPERTY() FudgetStyle* GetDefaultStyle() const;

    /// <summary>
    /// Checks the control's styles for a style with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedStyle(FudgetControl *control, int mapping_id, API_PARAM(Out) FudgetStyle* &result) const;

    /// <summary>
    /// Checks the control's styles for a texture with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    bool GetMappedTexture(FudgetControl *control, int mapping_id, AssetReference<TextureBase> &result) const;

    /// <summary>
    /// Checks the control's styles for a texture with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedTexture(FudgetControl *control, int mapping_id, API_PARAM(Out) TextureBase* &result) const;

    /// <summary>
    /// Checks the control's styles for a color with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedColor(FudgetControl *control, int mapping_id, API_PARAM(Out) Color &result) const;

    /// <summary>
    /// Checks the control's styles for a draw color list with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedDrawColors(FudgetControl *control, int mapping_id, API_PARAM(Out) FudgetDrawColors &result) const;

    /// <summary>
    /// Checks the control's styles for a string with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedString(FudgetControl *control, int mapping_id, API_PARAM(Out) String &result) const;

    /// <summary>
    /// Checks the control's styles for a bool with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedBool(FudgetControl *control, int mapping_id, API_PARAM(Out) bool &result) const;

    /// <summary>
    /// Checks the control's styles for a float with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedFloat(FudgetControl *control, int mapping_id, API_PARAM(Out) float &result) const;

    /// <summary>
    /// Checks the control's styles for a Float2 with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedFloat2(FudgetControl *control, int mapping_id, API_PARAM(Out) Float2 &result) const;

    /// <summary>
    /// Checks the control's styles for a Float3 with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedFloat3(FudgetControl *control, int mapping_id, API_PARAM(Out) Float3 &result) const;

    /// <summary>
    /// Checks the control's styles for a Float4 with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedFloat4(FudgetControl *control, int mapping_id, API_PARAM(Out) Float4 &result) const;

    /// <summary>
    /// Checks the control's styles for an integer with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedInt(FudgetControl *control, int mapping_id, API_PARAM(Out) int &result) const;

    /// <summary>
    /// Checks the control's styles for an Int2 with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedInt2(FudgetControl *control, int mapping_id, API_PARAM(Out) Int2 &result) const;

    /// <summary>
    /// Checks the control's styles for an Int3 with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedInt3(FudgetControl *control, int mapping_id, API_PARAM(Out) Int3 &result) const;

    /// <summary>
    /// Checks the control's styles for an Int4 with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedInt4(FudgetControl *control, int mapping_id, API_PARAM(Out) Int4 &result) const;

    /// <summary>
    /// Checks the control's styles for a padding with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedPadding(FudgetControl *control, int mapping_id, API_PARAM(Out) FudgetPadding &result) const;

    /// <summary>
    /// Checks the control's styles for border values with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedBorder(FudgetControl *control, int mapping_id, API_PARAM(Out) FudgetBorder &result) const;

    /// <summary>
    /// Checks the control's styles for a list of instructions to create a drawable from with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource. It is also responsible for
    /// destroying the resulting drawable.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool CreateMappedDrawable(FudgetControl *control, int mapping_id, API_PARAM(Out) FudgetDrawable* &result);

    /// <summary>
    /// Checks the control's styles for a draw area with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedDrawArea(FudgetControl *control, int mapping_id, API_PARAM(Out) FudgetDrawArea &result) const;

    /// <summary>
    /// Checks the control's styles for a font with mapping_id and returns true if it was successful stored in result.
    /// </summary>
    /// <param name="control">The control that provides the style and theme for looking up the resource.</param>
    /// <param name="mapping_id">The resource id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id was a valid id in the control's styles and matched the requested type or was converted to it.</returns>
    API_FUNCTION() bool GetMappedFont(FudgetControl *control, int mapping_id, API_PARAM(Out) FudgetFont &result) const;

    template<typename E>
    bool GetMappedEnum(FudgetControl *control, int mapping_id, API_PARAM(Out) E &result) const
    {
        FudgetTheme *theme = control->GetActiveTheme();
        result = (E)0;
        return control->GetStyleEnum<E>(mapping_id, result);
    }

    ///// <summary>
    ///// Checks if the passed state flags contain a given state.
    ///// </summary>
    ///// <param name="states">The state flags</param>
    ///// <param name="to_check">The stte to check</param>
    ///// <returns>Whether the checked state was found in the flags</returns>
    //template<typename T>
    //bool HasState(T states, T to_check) const
    //{
    //    return ((uint64)states & (uint64)to_check) == (uint64)to_check;
    //}

    /// <summary>
    /// Stores the drawable in the drawables list and destroys it when the part painter is destroyed.
    /// </summary>
    void RegisterDrawable(FudgetDrawable *drawable);


private:

    /// <summary>
    /// Calls initialize after some internal setup and calls Initialize.
    /// </summary>
    /// <param name="control">The control invoking the initialization</param>
    /// <param name="resource_mapping">Mapping of style ids that a painter can look up in the style of its control when drawing.</param>
    API_FUNCTION(Internal) void DoInitialize(FudgetControl *control, API_PARAM(Ref) const FudgetPartPainterMapping &resource_mapping);

    FudgetControl *_owner;

    friend class FudgetControl;
    friend class FudgetDrawable;
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
    /// <param name="states">State flags of the part to paint</param>
    API_FUNCTION() virtual void Draw(FudgetControl *control, const Rectangle &bounds, uint64 states) {}

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
    API_FUNCTION() virtual void Draw(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, uint64 state, const FudgetSingleLineTextOptions &text_options) {}

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
    API_FUNCTION() virtual Int2 Measure(FudgetControl *control, const StringView &text, const FudgetTextRange &range, uint64 state, const FudgetSingleLineTextOptions &text_options) { return Float2::Zero;  }

    /// <summary>
    /// Returns the kerning distance between two characters using the font of the painter. If no font is set, the result is 0
    /// </summary>
    /// <param name="a">First character</param>
    /// <param name="b">Other character</param>
    /// <param name="scale">Scale of the control used for measurements and drawing</param>
    /// <returns>Kerning distance between characters</returns>
    API_FUNCTION() virtual int GetKerning(Char a, Char b, float scale) const { return 0; }

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
    API_FUNCTION() virtual int HitTest(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, uint64 state, const FudgetSingleLineTextOptions &text_options, const Int2 &point) { return 0; }

    /// <summary>
    /// Returns the height of the font used by the painter.
    /// </summary>
    API_FUNCTION() virtual int GetFontHeight() const { return 0; }
};



class IFudgetDataProvider;

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
    API_FUNCTION() virtual void Draw(FudgetControl *control, const Rectangle &bounds, Int2 offset, int item_index, IFudgetDataProvider *data, uint64 state) {}

    /// <summary>
    /// Measures the list item at index.
    /// </summary>
    /// <param name="control">Control used for measuring</param>
    /// <param name="item_index">Index of item in data</param>
    /// <param name="data">Source of item data</param>
    /// <param name="state">State of control</param>
    /// <returns>Dimensions of the requested item</returns>
    API_FUNCTION() virtual Int2 Measure(FudgetControl *control, int item_index, IFudgetDataProvider *data, uint64 state) { return Int2::Zero; }
};

