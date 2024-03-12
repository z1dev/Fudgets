#pragma once

#include "StyleStructs.h"
#include "Engine/Scripting/ScriptingObject.h"

#include <vector>

class FudgetTheme;
class FudgetStyle;
class FudgetPartPainter;

enum class FudgetDrawInstructionType
{
    Blur,
    DrawArea,
    Resource,
    FillColor,
    Padding,
    AreaList
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
    FudgetDrawInstructionList() : Base(FudgetDrawInstructionType::AreaList) {}
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

API_CLASS()
class FUDGETS_API FudgetDrawable : public ScriptingObject
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE(FudgetDrawable);
public:
    ~FudgetDrawable();

    API_FUNCTION() static FORCE_INLINE FudgetDrawable* CreateEmpty(FudgetPartPainter *owner)
    {
        return Create(owner, nullptr);
    }

    /// <summary>
    /// Initializes a rectangular drawable from a color
    /// </summary>
    /// <param name="color">Color to draw</param>
    /// <returns>The created drawable</returns>
    API_FUNCTION() static FudgetDrawable* FromColor(FudgetPartPainter *owner, Color color);
    /// <summary>
    /// Initializes a rectangular drawable from a draw area
    /// </summary>
    /// <param name="area">The draw area</param>
    /// <returns>The created drawable</returns>
    API_FUNCTION() static FudgetDrawable* FromDrawArea(FudgetPartPainter *owner, const FudgetDrawArea &area);
private:
    /// <summary>
    /// Initializes a rectangular drawable from a draw instruction list. The list can only come from FudgetStyle
    /// by finding it in FudgetThemes.
    /// </summary>
    /// <param name="drawlist">The draw instruction list</param>
    /// <returns>The created drawable</returns>
    static FudgetDrawable* FromDrawInstructionList(FudgetPartPainter *owner, FudgetStyle *style, FudgetTheme *theme, FudgetDrawInstructionList *drawlist);

    /// <summary>
    /// Checks if there are any int ids of fudget resources in the list, which would force the drawable to create a new
    /// list and take ownership over it.
    /// </summary>
    /// <param name="drawlist">The draw instruction list</param>
    /// <returns>Whether there are no int ids in the list, and then it can be used without taking ownership.</returns>
    static bool NoExternalResources(FudgetDrawInstructionList *drawlist);

    static FudgetDrawInstruction* CloneDrawInstructionListItem(FudgetStyle *style, FudgetTheme *theme, FudgetDrawInstruction *item);

    static FudgetDrawable* Create(FudgetPartPainter *owner, FudgetDrawInstructionList *list);

    FudgetDrawInstructionList *_list;
    bool _owned;

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
    API_FUNCTION() static bool Begin();
    API_FUNCTION() static FudgetDrawableIndex End(bool abort = false);


    API_FUNCTION() static void AddResource(int id);

    // Not allowed to avoid circular reference
    //API_FUNCTION() static void AddAreaList(const String &drawlist_name);

    API_FUNCTION() static void AddColor(Color color);

    API_FUNCTION() static void AddPadding(FudgetPadding padding);

    API_FUNCTION() static void AddBlur(float blur_strength);

    API_FUNCTION() static void AddDrawArea(FudgetDrawArea area);

    API_FUNCTION() static void BeginSubData();
    API_FUNCTION() static void EndSubData();
private:
    static bool _building;
    static FudgetDrawInstructionList *_data;
    static std::vector<FudgetDrawInstructionList*> _sub_data;
};
