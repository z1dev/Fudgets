#pragma once

#include "StyleStructs.h"
#include "Engine/Scripting/ScriptingObject.h"

#include <vector>

class FudgetTheme;
class FudgetStyle;

enum class FudgetStyleAreaType
{
    Blur,
    DrawArea,
    Resource,
    FillColor,
    Padding,
    AreaList
};

struct FudgetStyleArea
{
    FudgetStyleArea(FudgetStyleAreaType type) : _type(type) {}
    virtual ~FudgetStyleArea() {}
    FudgetStyleAreaType _type;
};

struct FudgetStyleAreaList : public FudgetStyleArea
{
    using Base = FudgetStyleArea;
    FudgetStyleAreaList() : Base(FudgetStyleAreaType::AreaList) {}
    ~FudgetStyleAreaList();
    std::vector<FudgetStyleArea*> _list;
};

struct FudgetStyleAreaResource : public FudgetStyleArea
{
    using Base = FudgetStyleArea;
    FudgetStyleAreaResource() : Base(FudgetStyleAreaType::FillColor), _token(-1) {}
    FudgetStyleAreaResource(FudgetToken token) : Base(FudgetStyleAreaType::Resource), _token(token) {}
    FudgetToken _token;
};

struct FudgetStyleAreaColor : public FudgetStyleArea
{
    using Base = FudgetStyleArea;
    FudgetStyleAreaColor() : Base(FudgetStyleAreaType::FillColor), _color(Color::White) {}
    FudgetStyleAreaColor(Color color) : Base(FudgetStyleAreaType::FillColor), _color(color) {}
    Color _color;
};

struct FudgetStyleAreaFloat : public FudgetStyleArea
{
    using Base = FudgetStyleArea;
    FudgetStyleAreaFloat(FudgetStyleAreaType type) : Base(type), _value(0.f) {}
    FudgetStyleAreaFloat(FudgetStyleAreaType type, float value) : Base(type), _value(value) {}
    float _value;
};

struct FudgetStyleAreaPadding : public FudgetStyleArea
{
    using Base = FudgetStyleArea;
    FudgetStyleAreaPadding() : Base(FudgetStyleAreaType::Padding), _padding() {}
    FudgetStyleAreaPadding(const FudgetPadding &padding) : Base(FudgetStyleAreaType::Padding), _padding(padding) {}

    FudgetPadding _padding;
};

struct FudgetStyleAreaDrawArea : public FudgetStyleArea
{
    using Base = FudgetStyleArea;
    FudgetStyleAreaDrawArea() : Base(FudgetStyleAreaType::DrawArea) {}
    FudgetStyleAreaDrawArea(const FudgetDrawArea &draw_area) : Base(FudgetStyleAreaType::DrawArea), _draw_area(draw_area) {}
    FudgetDrawArea _draw_area;
};

//API_STRUCT()
//struct FUDGETS_API FudgetDrawableToken
//{
//    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetDrawableToken)
//
//    API_FIELD() int id;
//
//    FudgetDrawableToken() : id(-1) {}
//    FudgetDrawableToken(const FudgetDrawableToken &other) : id(other.id) {}
//    FudgetDrawableToken(FudgetDrawableToken &&other) noexcept : id(other.id) {}
//    FudgetDrawableToken(int id) : id(id) {}
//
//    FudgetDrawableToken& operator=(const FudgetDrawableToken &other) { id = other.id; return *this; }
//    FudgetDrawableToken& operator=(FudgetDrawableToken &&other) noexcept { id = other.id; return *this; }
//
//
//    bool operator>(const FudgetDrawableToken &other) const { return id > other.id; }
//    bool operator>=(const FudgetDrawableToken &other) const { return id >= other.id; }
//    bool operator<(const FudgetDrawableToken &other) const { return id < other.id; }
//    bool operator<=(const FudgetDrawableToken &other) const { return id <= other.id; }
//    bool operator==(const FudgetDrawableToken &other) const { return id == other.id; }
//    bool operator!=(const FudgetDrawableToken &other) const { return id == other.id; }
//};
//
//template<>
//struct TIsPODType<FudgetDrawableToken>
//{
//    enum { Value = true };
//};

API_CLASS()
class FUDGETS_API FudgetDrawable : public ScriptingObject
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE(FudgetDrawable);
public:
    ~FudgetDrawable();

    /// <summary>
    /// Initializes a rectangular drawable from a color
    /// </summary>
    /// <param name="color">Color to draw</param>
    /// <returns>The created drawable</returns>
    API_FUNCTION() static FudgetDrawable* FromColor(Color color);
    /// <summary>
    /// Initializes a rectangular drawable from a draw area
    /// </summary>
    /// <param name="area">The draw area</param>
    /// <returns>The created drawable</returns>
    API_FUNCTION() static FudgetDrawable* FromDrawArea(const FudgetDrawArea &area);
private:
    /// <summary>
    /// Initializes a rectangular drawable from a style area list. The arealist can only come from FudgetStyle
    /// by finding it in FudgetThemes.
    /// </summary>
    /// <param name="arealist">The style area list</param>
    /// <returns>The created drawable</returns>
    static FudgetDrawable* FromStyleAreaList(FudgetStyle *style, FudgetTheme *theme, FudgetStyleAreaList *arealist);

    /// <summary>
    /// Checks if there are any fudget token resources in the list, which would force the drawable to create a new
    /// list and take ownership over it.
    /// </summary>
    /// <param name="arealist">The style area list</param>
    /// <returns>Whether there are no tokens in the list, and then it can be used without taking ownership.</returns>
    static bool NoExternalResources(FudgetStyleAreaList *arealist);

    static FudgetStyleArea* CloneStyleAreaListItem(FudgetStyle *style, FudgetTheme *theme, FudgetStyleArea *item);

    FORCE_INLINE static FudgetDrawable* Create()
    {
        FudgetDrawable *result = New<FudgetDrawable>(SpawnParams(Guid::New(), FudgetDrawable::TypeInitializer));
        result->_owned = true;
        result->_list = new FudgetStyleAreaList();
        return result;
    }

    FudgetStyleAreaList *_list;
    bool _owned;

    friend class FudgetStyle;
    friend class FudgetControl;
};

API_CLASS(Static)
class FUDGETS_API FudgetDrawableBuilder
{
    DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetDrawableBuilder);
public:
    API_FUNCTION() static bool Begin(FudgetToken token);
    API_FUNCTION() static FudgetToken Begin(String token_name);
    API_FUNCTION() static void End(bool abort = false);

    //API_PROPERTY() static FudgetDrawableToken GetToken();

    API_FUNCTION() static void AddResource(FudgetToken token);

    API_FUNCTION() static void AddColor(Color color);

    API_FUNCTION() static void AddPadding(FudgetPadding padding);

    API_FUNCTION() static void AddBlur(float blur_strength);

    API_FUNCTION() static void AddDrawArea(FudgetDrawArea area);

    API_FUNCTION() static void BeginSubData();
    API_FUNCTION() static void EndSubData();
private:
    static bool _building;
    static FudgetToken _token;
    static FudgetStyleAreaList *_data;
    static std::vector<FudgetStyleAreaList*> _sub_data;
};
