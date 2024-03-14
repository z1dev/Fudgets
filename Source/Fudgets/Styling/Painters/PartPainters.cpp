#include "PartPainters.h"
#include "../Style.h"
#include "../Themes.h"
#include "../../MarginStructs.h"
#include "../StyleStructs.h"
#include "../../Control.h"
#include "../DrawableBuilder.h"
#include "../../Control.h"

#include "../StateOrderBuilder.h"


// FudgetPartPainter


FudgetPartPainter::FudgetPartPainter(const SpawnParams &params) : Base(params), _state_order(nullptr)
{

}

FudgetPartPainter::~FudgetPartPainter()
{
}

FudgetStyle* FudgetPartPainter::GetDefaultStyle() const
{
    FudgetStyle *style = FudgetThemes::GetStyle(GetType().Fullname.ToString());
    if (style == nullptr)
        style = FudgetThemes::CreateStyle(GetType().Fullname.ToString());
    return style;
}

void FudgetPartPainter::GetMappedStateOrder(int theme_order_index, FudgetStateOrder* &order) const
{
    order = FudgetThemes::GetStateOrder(theme_order_index);
}

bool FudgetPartPainter::GetMappedStyle(FudgetControl *control, int mapping_id, API_PARAM(Out) FudgetStyle* &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = nullptr;
    return control->GetStyleStyle(mapping_id, result);
}

bool FudgetPartPainter::GetMappedTexture(FudgetControl *control,  int mapping_id, AssetReference<TextureBase> &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;

    TextureBase *tex = nullptr;
    control->GetStyleTexture(mapping_id, tex);
    result = tex;
    return tex != nullptr;
}

bool FudgetPartPainter::GetMappedTexture(FudgetControl *control, int mapping_id, API_PARAM(Out) TextureBase* &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;

    control->GetStyleTexture(mapping_id, result);
    return result != nullptr;
}

bool FudgetPartPainter::GetMappedColor(FudgetControl *control, int mapping_id, API_PARAM(Out) Color &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = Color::White;
    return control->GetStyleColor(mapping_id, result);
}

bool FudgetPartPainter::GetMappedString(FudgetControl *control, int mapping_id, API_PARAM(Out) String &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = String();
    return control->GetStyleString(mapping_id, result);
}

bool FudgetPartPainter::GetMappedFloat(FudgetControl *control, int mapping_id, API_PARAM(Out) float &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = 0.f;
    return control->GetStyleFloat(mapping_id, result);
}

bool FudgetPartPainter::GetMappedFloat2(FudgetControl *control, int mapping_id, API_PARAM(Out) Float2 &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = Float2::Zero;
    return control->GetStyleFloat2(mapping_id, result);
}

bool FudgetPartPainter::GetMappedFloat3(FudgetControl *control, int mapping_id, API_PARAM(Out) Float3 &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = Float3::Zero;
    return control->GetStyleFloat3(mapping_id, result);
}

bool FudgetPartPainter::GetMappedFloat4(FudgetControl *control, int mapping_id, API_PARAM(Out) Float4 &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = Float4::Zero;
    return control->GetStyleFloat4(mapping_id, result);
}

bool FudgetPartPainter::GetMappedInt(FudgetControl *control, int mapping_id, API_PARAM(Out) int &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = 0;
    return control->GetStyleInt(mapping_id, result);
}

bool FudgetPartPainter::GetMappedInt2(FudgetControl *control, int mapping_id, API_PARAM(Out) Int2 &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = Int2::Zero;
    return control->GetStyleInt2(mapping_id, result);

}

bool FudgetPartPainter::GetMappedInt3(FudgetControl *control, int mapping_id, API_PARAM(Out) Int3 &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = Int3::Zero;
    return control->GetStyleInt3(mapping_id, result);
}

bool FudgetPartPainter::GetMappedInt4(FudgetControl *control, int mapping_id, API_PARAM(Out) Int4 &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = Int4::Zero;
    return control->GetStyleInt4(mapping_id, result);
}

bool FudgetPartPainter::GetMappedPadding(FudgetControl *control, int mapping_id, API_PARAM(Out) FudgetPadding &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = FudgetPadding(0);
    return control->GetStylePadding(mapping_id, result);
}

bool FudgetPartPainter::GetMappedBorder(FudgetControl *control, int mapping_id, API_PARAM(Out) FudgetBorder &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = FudgetBorder(0.f);
    return control->GetStyleBorder(mapping_id, result);
}

bool FudgetPartPainter::CreateMappedDrawable(FudgetControl *control, int mapping_id, API_PARAM(Out) FudgetDrawable* &result)
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = nullptr;
    return control->GetStyleDrawable(mapping_id, this, result);
}

bool FudgetPartPainter::GetMappedDrawArea(FudgetControl *control, int mapping_id, API_PARAM(Out) FudgetDrawArea &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = FudgetDrawArea();
    return control->GetStyleDrawArea(mapping_id, result);
}

bool FudgetPartPainter::GetMappedFont(FudgetControl *control, int mapping_id, API_PARAM(Out) FudgetFont &result) const
{
    if (mapping_id <= 0 || control == nullptr)
        return false;
    result = FudgetFont();
    return control->GetStyleFont(mapping_id, result);
}

void FudgetPartPainter::RegisterDrawable(FudgetDrawable *drawable)
{
    _owner->RegisterDrawable(this, drawable);
}

void FudgetPartPainter::DoInitialize(FudgetControl *control, API_PARAM(Ref) const FudgetPartPainterMapping &resource_mapping)
{
    GetMappedStateOrder(resource_mapping.StateOrderIndex, _state_order);
    Initialize(control, resource_mapping.Mapping);
}

// FudgetStatePainter


FudgetStatePainter::FudgetStatePainter(const SpawnParams &params) : Base(params)
{
}


// FudgetTextPainter


FudgetSingleLineTextPainter::FudgetSingleLineTextPainter(const SpawnParams &params) : Base(params)
{
}


// FudgetListItemPainter


FudgetListItemPainter::FudgetListItemPainter(const SpawnParams &params) : Base(params)
{

}

