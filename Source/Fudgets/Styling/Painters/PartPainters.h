#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Render2D/Font.h"

#include "../../MarginStructs.h"
#include "../StyleStructs.h"
#include "../Style.h"

#include <map>

class FudgetStateOrder;


class FudgetTheme;
class FudgetStyle;
class FudgetControl;
class FudgetDrawable;


/// <summary>
/// Supported states for framed field drawing
/// </summary>
API_ENUM(Attributes = "Flags")
enum class FudgetVisualControlState
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
    static FudgetPartPainterMapping InitializeMapping(const typename T::ResourceMapping &resource_mapping)
    {
        FudgetPartPainterMapping value;
        value.PainterType = T::TypeInitializer.GetType().Fullname;
        VariantType t;
        t.Type = VariantType::Structure;
        t.SetTypeName(T::ResourceMapping::TypeInitializer.GetType().Fullname);
        value.ResourceMapping = Variant::Structure(std::move(t), resource_mapping);
        return value;
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
    /// Checks the themes database for a draw order list object at the given index. If the index is not found, the default draw order
    /// is added which does not correspond to any state.
    /// The draw order list can tell a part painter which resources to use to draw the part in. For example if a control's state
    /// state is both focused and selected, the painter looks for either of them in the list. Whether focused or selected is coming
    /// first, its item is further checked if there is a focused or selected item inside it. When the last such item is found,
    /// the value in it can be used to pick the appropriate resources for drawing.
    /// </summary>
    /// <param name="theme_order_index">Index of the draw order list in FudgetThemes</param>
    /// <param name="order">The variable that receives the list. It's set to null if the index was not found.</param>
    void GetMappedStateOrder(int theme_order_index, FudgetStateOrder* &order) const;


    /// <summary>
    /// Checks style_override then the control's styles for a texture. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedStyle(FudgetControl *control,  int mapping_id, API_PARAM(Out) FudgetStyle* &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a texture. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    bool GetMappedTexture(FudgetControl *control,  int mapping_id, AssetReference<TextureBase> &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a texture. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedTexture(FudgetControl *control,  int mapping_id, API_PARAM(Out) TextureBase* &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a color. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedColor(FudgetControl *control,  int mapping_id, API_PARAM(Out) Color &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a string. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedString(FudgetControl *control,  int mapping_id, API_PARAM(Out) String &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a float. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedFloat(FudgetControl *control,  int mapping_id, API_PARAM(Out) float &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a Float2. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedFloat2(FudgetControl *control,  int mapping_id, API_PARAM(Out) Float2 &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a Float2. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedFloat3(FudgetControl *control,  int mapping_id, API_PARAM(Out) Float3 &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a Float2. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedFloat4(FudgetControl *control,  int mapping_id, API_PARAM(Out) Float4 &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a int. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedInt(FudgetControl *control,  int mapping_id, API_PARAM(Out) int &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a Int2. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedInt2(FudgetControl *control,  int mapping_id, API_PARAM(Out) Int2 &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a Int2. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedInt3(FudgetControl *control,  int mapping_id, API_PARAM(Out) Int3 &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a Int2. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedInt4(FudgetControl *control,  int mapping_id, API_PARAM(Out) Int4 &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a padding. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedPadding(FudgetControl *control,  int mapping_id, API_PARAM(Out) FudgetPadding &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a border. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedBorder(FudgetControl *control,  int mapping_id, API_PARAM(Out) FudgetBorder &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a drawable resource. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value. The control is also responsible for
    /// destroying the resulting drawable.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool CreateMappedDrawable(FudgetControl *control, int mapping_id, API_PARAM(Out) FudgetDrawable* &result);

    /// <summary>
    /// Checks style_override then the control's styles for a draw area resource. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedDrawArea(FudgetControl *control,  int mapping_id, API_PARAM(Out) FudgetDrawArea &result) const;

    /// <summary>
    /// Checks style_override then the control's styles for a font resource. First mapping_id is tested and then painter_id.
    /// Returns whether the result was set to the value.
    /// </summary>
    /// <param name="control">The control to use for looking up the resource if style_override doesn't have the value.</param>
    /// <param name="style_override">The style used for looking up the resource first.</param>
    /// <param name="painter_id">A resource id. Usually this is the id provided for the painter.</param>
    /// <param name="mapping_id">A resource id. Usually an id in the owner control's style that is used for looking up values instead of the painter's own id.</param>
    /// <param name="result">The variable to receive the result</param>
    /// <returns>Whether the mapped id or the painter id was referring to an enum of the correct type</returns>
    API_FUNCTION() bool GetMappedFont(FudgetControl *control,  int mapping_id, API_PARAM(Out) FudgetFont &result) const;

    template<typename E>
    bool GetMappedEnum(FudgetControl *control,  int mapping_id, API_PARAM(Out) E &result) const
    {
        FudgetTheme *theme = control->GetActiveTheme();
        result = (E)0;
        return control->GetStyleEnum<E>(mapping_id, result);
    }

    /// <summary>
    /// Checks if the passed state flags contain a given state.
    /// </summary>
    /// <param name="states">The state flags</param>
    /// <param name="to_check">The state to check</param>
    /// <returns>Whether the checked state was found in the flags</returns>
    template<typename T>
    bool HasState(T states, T to_check) const
    {
        return ((uint64)states & (uint64)to_check) == (uint64)to_check;
    }

    /// <summary>
    /// Checks if the passed state flag matches the pressed state.
    /// </summary>
    API_FUNCTION() FORCE_INLINE bool DrawPressed(uint64 state) const
    {
        return (state & (uint64)FudgetVisualControlState::Pressed) == (uint64)FudgetVisualControlState::Pressed;
    }
    /// <summary>
    /// Checks if the passed state flag matches the down state.
    /// </summary>
    API_FUNCTION() FORCE_INLINE bool DrawDown(uint64 state) const
    {
        return (state & (uint64)FudgetVisualControlState::Down) == (uint64)FudgetVisualControlState::Down;
    }
    /// <summary>
    /// Checks if the passed state flag matches the focused state.
    /// </summary>
    API_FUNCTION() FORCE_INLINE bool DrawFocused(uint64 state) const
    {
        return (state & (uint64)FudgetVisualControlState::Focused) == (uint64)FudgetVisualControlState::Focused;
    }
    /// <summary>
    /// Checks if the passed state flag matches the disabled state.
    /// </summary>
    API_FUNCTION() FORCE_INLINE bool DrawDisabled(uint64 state) const
    {
        return (state & (uint64)FudgetVisualControlState::Disabled) == (uint64)FudgetVisualControlState::Disabled;
    }
    /// <summary>
    /// Checks if the passed state flag matches the hovered state.
    /// </summary>
    API_FUNCTION() FORCE_INLINE bool DrawHovered(uint64 state) const
    {
        return (state & (uint64)FudgetVisualControlState::Hovered) == (uint64)FudgetVisualControlState::Hovered;
    }

    API_FUNCTION() FORCE_INLINE bool DrawSelected(uint64 state) const
    {
        return (state & (uint64)FudgetVisualControlState::Selected) == (uint64)FudgetVisualControlState::Selected;
    }

    API_FUNCTION() FORCE_INLINE bool DrawExpanded(uint64 state) const
    {
        return (state & (uint64)FudgetVisualControlState::Expanded) == (uint64)FudgetVisualControlState::Expanded;
    }
    /// <summary>
    /// Checks if the passed state flag matches the opened state.
    /// </summary>
    API_FUNCTION() FORCE_INLINE bool DrawOpened(uint64 state) const
    {
        return (state & (uint64)FudgetVisualControlState::Opened) == (uint64)FudgetVisualControlState::Opened;
    }

    /// <summary>
    /// Stores the drawable in the drawables list and destroys it when the part painter is destroyed.
    /// </summary>
    void RegisterDrawable(FudgetDrawable *drawable);

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
    API_FUNCTION() virtual void Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState states) {}
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
    API_FUNCTION() virtual Int2 Measure(FudgetControl *control, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state, const FudgetSingleLineTextOptions &text_options) { return Float2::Zero;  }

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
    API_FUNCTION() virtual int HitTest(FudgetControl *control, const Rectangle &bounds, const StringView &text, const FudgetTextRange &range, FudgetVisualControlState state, const FudgetSingleLineTextOptions &text_options, const Int2 &point) { return 0; }

    /// <summary>
    /// Returns the height of the font used by the painter.
    /// </summary>
    API_FUNCTION() virtual int GetFontHeight() const { return 0; }
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
    API_FUNCTION() virtual void Draw(FudgetControl *control, const Rectangle &bounds, Int2 offset, int item_index, IListDataProvider *data, FudgetVisualControlState state) {}

    /// <summary>
    /// Measures the list item at index.
    /// </summary>
    /// <param name="control">Control used for measuring</param>
    /// <param name="item_index">Index of item in data</param>
    /// <param name="data">Source of item data</param>
    /// <param name="state">State of control</param>
    /// <returns>Dimensions of the requested item</returns>
    API_FUNCTION() virtual Int2 Measure(FudgetControl *control, int item_index, IListDataProvider *data, FudgetVisualControlState state) { return Int2::Zero; }
};

