#pragma once

#include "StyleStructs.h"
#include "Engine/Scripting/ScriptingObject.h"
#include "../Utils/Utils.h"

#include <vector>

class FudgetControl;
class FudgetTheme;
class FudgetStyle;
class FudgetPartPainter;
enum class FudgetVisualControlState : uint64;


enum class FudgetDrawInstructionType
{
    Blur,
    DrawArea,
    DrawBorder,
    Resource,
    FillColor,
    Padding,
    InstructionList
};

struct FudgetDrawInstruction
{
    FudgetDrawInstruction(FudgetDrawInstructionType type) : _type(type) {}
    virtual ~FudgetDrawInstruction() {}
    FudgetDrawInstructionType _type;
};

struct FudgetDrawInstructionList : public FudgetDrawInstruction
{
    using Base = FudgetDrawInstruction;
    FudgetDrawInstructionList() : Base(FudgetDrawInstructionType::InstructionList) {}
    ~FudgetDrawInstructionList();
    std::vector<FudgetDrawInstruction*> _list;
};

struct FudgetDrawInstructionResource : public FudgetDrawInstruction
{
    using Base = FudgetDrawInstruction;
    FudgetDrawInstructionResource() : Base(FudgetDrawInstructionType::Resource), _id(-1) {}
    FudgetDrawInstructionResource(int id) : Base(FudgetDrawInstructionType::Resource), _id(id) {}
    int _id;
};

struct FudgetDrawInstructionColor : public FudgetDrawInstruction
{
    using Base = FudgetDrawInstruction;
    FudgetDrawInstructionColor() : Base(FudgetDrawInstructionType::FillColor), _color(Color::White) {}
    FudgetDrawInstructionColor(Color color) : Base(FudgetDrawInstructionType::FillColor), _color(color) {}
    Color _color;
};

struct FudgetDrawInstructionFloat : public FudgetDrawInstruction
{
    using Base = FudgetDrawInstruction;
    FudgetDrawInstructionFloat(FudgetDrawInstructionType type) : Base(type), _value(0.f) {}
    FudgetDrawInstructionFloat(FudgetDrawInstructionType type, float value) : Base(type), _value(value) {}
    float _value;
};

struct FudgetDrawInstructionInt : public FudgetDrawInstruction
{
    using Base = FudgetDrawInstruction;
    FudgetDrawInstructionInt(FudgetDrawInstructionType type) : Base(type), _value(0) {}
    FudgetDrawInstructionInt(FudgetDrawInstructionType type, int value) : Base(type), _value(value) {}
    int _value;
};

struct FudgetDrawInstructionPadding : public FudgetDrawInstruction
{
    using Base = FudgetDrawInstruction;
    FudgetDrawInstructionPadding() : Base(FudgetDrawInstructionType::Padding), _padding() {}
    FudgetDrawInstructionPadding(const FudgetPadding &padding) : Base(FudgetDrawInstructionType::Padding), _padding(padding) {}

    FudgetPadding _padding;
};

struct FudgetDrawInstructionDrawArea : public FudgetDrawInstruction
{
    using Base = FudgetDrawInstruction;
    FudgetDrawInstructionDrawArea() : Base(FudgetDrawInstructionType::DrawArea) {}
    FudgetDrawInstructionDrawArea(const FudgetDrawArea &draw_area) : Base(FudgetDrawInstructionType::DrawArea), _draw_area(draw_area) {}
    FudgetDrawArea _draw_area;
};

struct FudgetDrawInstructionDrawBorder : public FudgetDrawInstruction
{
    using Base = FudgetDrawInstruction;
    FudgetDrawInstructionDrawBorder() : Base(FudgetDrawInstructionType::DrawBorder) {}
    FudgetDrawInstructionDrawBorder(const FudgetDrawBorder &draw_border) : Base(FudgetDrawInstructionType::DrawBorder), _draw_border(draw_border) {}
    FudgetDrawBorder _draw_border;
};
API_CLASS()
class FUDGETS_API FudgetDrawable : public ScriptingObject
{
    using Base = ScriptingObject;

    DECLARE_SCRIPTING_TYPE(FudgetDrawable);
public:
    ~FudgetDrawable();

    API_FIELD() static FudgetDrawable*Empty;

    /// <summary>
    /// Returns true when the drawable has no instructions for drawing and does nothing when drawn.
    /// </summary>
    API_PROPERTY() bool IsEmpty() const;

    /// <summary>
    /// Returns the index of a state within the drawable that matches the passed state flags. The index of the first
    /// state that fully matches is returned. The 0 state in the drawable matches any combination of state flags.
    /// </summary>
    /// <param name="states">State flags to match</param>
    /// <returns>Index to the first matching state in the drawable or -1 on failure</returns>
    API_FUNCTION() int FindMatchingState(uint64 states) const;

    /// <summary>
    /// Initializes a rectangular drawable from a color.
    /// </summary>
    /// <param name="control_owner">Control responsible for destroying the drawable. If null, painter_owner must be set.</param>
    /// <param name="painter_owner">Painter tied to the drawable whose control is responsible for destroying it. If null, control_owner must be set.</param>
    /// <param name="color">Color to draw</param>
    /// <returns>The created drawable</returns>
    API_FUNCTION() static FudgetDrawable* FromColor(FudgetControl *control_owner, FudgetPartPainter *painter_owner, Color color);
    /// <summary>
    /// Initializes a rectangular drawable from a draw area.
    /// </summary>
    /// <param name="control_owner">Control responsible for destroying the drawable. If null, painter_owner must be set.</param>
    /// <param name="painter_owner">Painter tied to the drawable whose control is responsible for destroying it. If null, control_owner must be set.</param>
    /// <param name="area">The draw area</param>
    /// <returns>The created drawable</returns>
    API_FUNCTION() static FudgetDrawable* FromDrawArea(FudgetControl *control_owner, FudgetPartPainter *painter_owner, const FudgetDrawArea &area);
    /// <summary>
    /// Initializes a rectangular drawable from a draw border.
    /// </summary>
    /// <param name="control_owner">Control responsible for destroying the drawable. If null, painter_owner must be set.</param>
    /// <param name="painter_owner">Painter tied to the drawable whose control is responsible for destroying it. If null, control_owner must be set.</param>
    /// <param name="border">The draw border</param>
    /// <returns>The created drawable</returns>
    API_FUNCTION() static FudgetDrawable* FromDrawBorder(FudgetControl *control_owner, FudgetPartPainter *painter_owner, const FudgetDrawBorder &border);
    /// <summary>
    /// Initializes a rectangular drawable from a draw colors list.
    /// </summary>
    /// <param name="control_owner">Control responsible for destroying the drawable. If null, painter_owner must be set.</param>
    /// <param name="painter_owner">Painter tied to the drawable whose control is responsible for destroying it. If null, control_owner must be set.</param>
    /// <param name="colors">The draw colors</param>
    /// <returns>The created drawable</returns>
    API_FUNCTION() static FudgetDrawable* FromDrawColors(FudgetControl *control_owner, FudgetPartPainter *painter_owner, const FudgetDrawColors &colors);
private:
    /// <summary>
    /// Initializes a rectangular drawable from a draw instruction list. The list can only come from FudgetStyle
    /// by finding it in FudgetThemes.
    /// </summary>
    /// <param name="control_owner">Control responsible for destroying the drawable. If null, painter_owner must be set.</param>
    /// <param name="painter_owner">Painter tied to the drawable whose control is responsible for destroying it. If null, control_owner must be set.</param>
    /// <param name="drawlist">The draw instruction list</param>
    /// <returns>The created drawable</returns>
    static FudgetDrawable* FromDrawInstructionList(FudgetControl *control_owner, FudgetPartPainter *painter_owner, FudgetStyle *style, FudgetTheme *theme, const std::vector<uint64> &states, const std::vector<FudgetDrawInstructionList*> &lists);

    /// <summary>
    /// Checks if there are any int ids of fudget resources in the list, which would force the drawable to create a new
    /// list and take ownership over it.
    /// </summary>
    /// <param name="drawlist">The draw instruction list</param>
    /// <returns>Whether there are no int ids in the list, and then it can be used without taking ownership.</returns>
    static bool NoExternalResources(FudgetDrawInstructionList* drawlist);

    static FudgetDrawInstruction* CloneDrawInstructionListItem(FudgetStyle *style, FudgetTheme *theme, FudgetDrawInstruction *item, uint64 states);

    // Creates a new drawable from instructions. If the instructions don't contain referenced ids then the
    // ownership of the lists stays with the caller. Otherwise a copy is made and the drawable is responsible for deleting it.
    // Passing in at least the control owner is necessary for destroying the drawable.
    static FudgetDrawable* Create(FudgetControl *control_owner, FudgetPartPainter *painter_owner, const std::vector<uint64> &states, const std::vector<FudgetDrawInstructionList*> &lists);
    // Creates a new drawable with a single state and list allocated for an item. The list is deleted when the drawable is destroyed.
    // Use this when calling from one of the public From*** functions that need to add a single instruction.
    // Passing in at least the control owner is necessary for destroying the drawable.
    static FudgetDrawable* Create(FudgetControl *control_owner, FudgetPartPainter *painter_owner, uint64 state);
    // Creates a new empty drawable with ownership staying with the drawable for any list added to it. Called when cloning an
    // instruction list that might create lists for multiple states.
    // Passing in at least the control owner is necessary for destroying the drawable.
    static FudgetDrawable* Create(FudgetControl *control_owner, FudgetPartPainter *painter_owner);
    // Creates the empty drawable. Don't call this.
    static FudgetDrawable* CreateEmpty();

    bool _owned;

    std::vector<uint64> _states;
    std::vector<FudgetDrawInstructionList*> _lists;

    friend class FudgetStyle;
    friend class FudgetControl;
};

/// <summary>
/// Stores an index that identifies a drawable's instruction list in the main themes.
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetDrawableIndex
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetDrawableIndex);

    /// <summary>
    /// Index of the draw instructions for drawables in FudgetThemes.
    /// </summary>
    API_FIELD() int Index;
};

/// <summary>
/// Static type that can be used to make a list of instructions that make up a drawable object.
/// </summary>
API_CLASS(Static)
class FUDGETS_API FudgetDrawableBuilder
{
    DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetDrawableBuilder);
public:
    /// <summary>
    /// Calling the first time starts building instructions that make up a new drawable object. Follow by any of
    /// the Add*** functions or by BeginSubData to build the drawable.
    /// This function can be called again with a different state value to start a new set of instructions. Calling
    /// with an existing state as before will do nothing, beside printing a warning message.
    /// Call End once all drawable states are built, to get an index to the drawable. This index should be set
    /// as a resource in a theme, or stored in a style as value override.
    /// The states can hold multiple state flags (See FudgetVisualControlState enum for the default states). When
    /// the built drawable is drawn, it checks the states in order. The first drawable that matches the requested
    /// drawing state will be drawn. Make sure states with more flags are added first and that the states are
    /// added in draw priority.
    /// The 0 state will match everything, so it should be added last.
    /// </summary>
    /// <param name="states">State flags of the new list of instructions.</param>
    API_FUNCTION() static void Begin(uint64 states = 0);

    /// <summary>
    /// Calling the first time starts building instructions that make up a new drawable object. Follow by any of
    /// the Add*** functions or by BeginSubData to build the drawable.
    /// This function can be called again with a different state value to start a new set of instructions. Calling
    /// with an existing state as before will do nothing, beside printing a warning message.
    /// Call End once all drawable states are built, to get an index to the drawable. This index should be set
    /// as a resource in a theme, or stored in a style as value override.
    /// The states can hold multiple state flags (See FudgetVisualControlState enum for the default states). When
    /// the built drawable is drawn, it checks the states in order. The first drawable that matches the requested
    /// drawing state will be drawn. Make sure states with more flags are added first and that the states are
    /// added in draw priority.
    /// The 0 state will match everything, so it should be added last.
    /// </summary>
    /// <param name="states">State flags of the new list of instructions.</param>
    API_FUNCTION() FORCE_INLINE static void Begin(FudgetVisualControlState states) { return Begin((uint64)states); }

    /// <summary>
    /// Finishes building instructions for a drawable object, and returns an index for it in the main theme database
    /// that can be set as a resource in a theme or stored as value override in a style.
    /// Must be called after a Begin call or the result will be invalid.
    /// </summary>
    /// <param name="abort">If this is true, the drawable instructions are freed and the returned index will be invalid.</param>
    /// <returns>An index object to the built drawable instructions in the main theme database. Store as theme resource or style value override.</returns>
    API_FUNCTION() static FudgetDrawableIndex End(bool abort = false);

    /// <summary>
    /// Adds a resource id to the drawable's list of instructions. If the id refers to a resource that can be drawn
    /// by the drawable, it'll be used for drawing. 
    /// </summary>
    /// <param name="id">Id of a resource in a theme</param>
    API_FUNCTION() static void AddResource(int id);

    template<typename T>
    static typename std::enable_if<Fudget_is_resource_id<T>(), void>::type AddResource(T id)
    {
        AddResource((int)id);
    }

    /// <summary>
    /// Adds a color instruction that will be used to fill the area available to the drawable when drawing.
    /// </summary>
    /// <param name="color">The color value used for filling the drawable's area</param>
    API_FUNCTION() static void AddColor(Color color);
    /// <summary>
    /// Adds a padding instruction that will affect the drawing bounds of the instructions that follow on the same
    /// main or sub data.
    /// </summary>
    /// <param name="padding">The padding value for padding the drawable bounds.</param>
    API_FUNCTION() static void AddPadding(FudgetPadding padding);
    /// <summary>
    /// Adds a blur instruction to draw a blur rectangle over the area available to the drawable when drawing.
    /// </summary>
    /// <param name="blur_strength">The strength of the blur. Higher values can impact performance.</param>
    API_FUNCTION() static void AddBlur(float blur_strength);
    /// <summary>
    /// Adds an instruction to draw a draw area object in the area available to the drawable when drawing.
    /// </summary>
    /// <param name="area">The draw area to draw.</param>
    API_FUNCTION() static void AddDrawArea(FudgetDrawArea area);
    /// <summary>
    /// Adds an instruction to draw a draw border object in the area available to the drawable when drawing.
    /// </summary>
    /// <param name="area">The draw area to draw.</param>
    API_FUNCTION() static void AddDrawBorder(FudgetDrawBorder border);

    /// <summary>
    /// Starts a nested list of instructions for drawing for the same drawable state. When adding instructions like
    /// padding, any drawing following that in the same nested list will be restricted by it. Call EndSubData to finish
    /// the nested list and continue adding instructions in the original list. Multiple levels of nested instructions can
    /// be added. Make sure to call EndSubData for each of them until the builder is back at the desired nested level.
    /// </summary>
    /// <returns></returns>
    API_FUNCTION() static void BeginSubData();
    /// <summary>
    /// Call after a BeginSubData to stop adding instructions to that nested list. Calling at the root level does nothing.
    /// It's not necessary to call before End to complete a drawable.
    /// </summary>
    API_FUNCTION() static void EndSubData();

    /// <summary>
    /// Starts building a draw color structure if one is not already being built. The state determines when this color
    /// is used by a part painter or control for drawing. See Begin for description on how states work on drawables.
    /// </summary>
    /// <param name="states">The state to add the color for</param>
    API_FUNCTION() static void MakeDrawColors(Color color, uint64 states = 0);

    template<typename T>
    static typename std::enable_if<Fudget_is_enum_and_not_type2<T, uint64>(), void>::type MakeDrawColors(Color color, T states)
    {
        MakeDrawColors(color, (uint64)states);
    }

    /// <summary>
    /// Finishes building a draw color object. The result can be added as resource to the theme. The resource will
    /// then be used either for picking a color based on a state, or to make a drawable from it. Returns an empty
    /// object if no colors were added or a different object is being built.
    /// </summary>
    /// <returns>The finished structure with colors.</returns>
    API_FUNCTION() static FudgetDrawColors EndDrawColors();
private:
    enum class BuildMode
    {
        None,
        Drawable,
        Colors
    };


    static BuildMode _mode;

    static std::vector<uint64> _states;
    static std::vector<FudgetDrawInstructionList*> _state_list;
    static FudgetDrawInstructionList *_data;
    static std::vector<FudgetDrawInstructionList*> _sub_data;

    static FudgetDrawColors _colors;
};
