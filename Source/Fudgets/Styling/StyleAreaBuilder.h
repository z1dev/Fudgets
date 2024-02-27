#pragma once

#include "StyleStructs.h"


API_CLASS(Static)
class FUDGETS_API FudgetStyleAreaBuilder
{
    DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetStyleAreaBuilder);
public:
    API_FUNCTION() static void Begin();
    API_FUNCTION() static void End();

    API_PROPERTY() static FudgetStyleAreaList* GetArea();

    API_FUNCTION() static void AddColor(Color color);

    API_FUNCTION() static void AddPadding(FudgetPadding padding);

    API_FUNCTION() static void AddBlur(float blur_strength);

    API_FUNCTION() static void AddDrawArea(FudgetDrawArea area);

    API_FUNCTION() static void BeginSubData();
    API_FUNCTION() static void EndSubData();
private:
    static bool _building;
    static FudgetStyleAreaList *_data;
    static Array<FudgetStyleAreaList*> _sub_data;
};