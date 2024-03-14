#include "DrawableBuilder.h"
#include "Themes.h"
#include "Style.h"
#include "Painters/PartPainters.h"

#include "Engine/Core/Log.h"


// FudgetDrawInstructionList


FudgetDrawInstructionList::~FudgetDrawInstructionList()
{
    for (auto area : _list)
        delete area;
    _list.clear();
}


// FudgetDrawableBuilder


bool FudgetDrawableBuilder::_building = false;
FudgetDrawInstructionList *FudgetDrawableBuilder::_data = nullptr;
std::vector<FudgetDrawInstructionList*> FudgetDrawableBuilder::_sub_data;


bool FudgetDrawableBuilder::Begin()
{
    if (_building)
    {
        LOG(Warning, "Begin: Already building a drawable. Ending it and starting a new one.");
        End(true);
    }

    _building = true;
    _data = new FudgetDrawInstructionList();
    return true;
}

FudgetDrawableIndex FudgetDrawableBuilder::End(bool abort)
{
    FudgetDrawableIndex result;
    result.Index = -1;
    if (!_building)
    {
        LOG(Error, "End: Not building a drawable.");
        return result;
    }

    while (!_sub_data.empty())
        EndSubData();
    _building = false;
    if (abort)
        delete _data;
    else
        result.Index = FudgetThemes::RegisterDrawInstructionList(_data);
    return result;
}

void FudgetDrawableBuilder::AddResource(int id)
{
    if (!_building)
    {
        LOG(Error, "AddColor: Not building a drawable.");
        return;
    }

    FudgetDrawInstructionList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetDrawInstructionResource(id));
}

void FudgetDrawableBuilder::AddColor(Color color)
{
    if (!_building)
    {
        LOG(Error, "AddColor: Not building a drawable.");
        return;
    }

    FudgetDrawInstructionList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetDrawInstructionColor(color));
}

void FudgetDrawableBuilder::AddPadding(FudgetPadding padding)
{
    if (!_building)
    {
        LOG(Error, "AddPadding: Not building a drawable.");
        return;
    }

    FudgetDrawInstructionList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetDrawInstructionPadding(padding));
}

void FudgetDrawableBuilder::AddBlur(float blur_strength)
{
    if (!_building)
    {
        LOG(Error, "AddBlur: Not building a drawable.");
        return;
    }

    FudgetDrawInstructionList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetDrawInstructionFloat(FudgetDrawInstructionType::Blur, blur_strength));
}

void FudgetDrawableBuilder::AddDrawArea(FudgetDrawArea area)
{
    if (!_building)
    {
        LOG(Error, "AddDrawArea: Not building a drawable.");
        return;
    }

    FudgetDrawInstructionList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetDrawInstructionDrawArea(area));
}

void FudgetDrawableBuilder::BeginSubData()
{
    if (!_building)
    {
        LOG(Error, "BeginSubData: Not building a drawable.");
        return;
    }

    FudgetDrawInstructionList *data = _sub_data.empty() ? _data : _sub_data.back();
    FudgetDrawInstructionList *new_list = new FudgetDrawInstructionList();
    data->_list.push_back(new_list);
    _sub_data.push_back(new_list);
}

void FudgetDrawableBuilder::EndSubData()
{
    if (!_building)
    {
        LOG(Error, "EndSubData: Not building a drawable.");
        return;
    }
    if (_sub_data.empty())
    {
        LOG(Error, "EndSubData: Not building a drawable sub-data.");
        return;
    }

    _sub_data.pop_back();
}


// FudgetDrawable


FudgetDrawable* FudgetDrawable::Empty = CreateEmpty();

FudgetDrawable::FudgetDrawable(const SpawnParams &params) : Base(params), _list(nullptr), _owned(false)
{
}

FudgetDrawable::~FudgetDrawable()
{
    if (_owned)
        delete _list;
}

FudgetDrawable* FudgetDrawable::FromColor(FudgetPartPainter *owner, Color color)
{
    if (owner == nullptr)
        return nullptr;

    FudgetDrawable *result = Create(owner, nullptr);
    result->_list->_list.push_back(new FudgetDrawInstructionColor(color));
    return result;
}

FudgetDrawable* FudgetDrawable::FromDrawArea(FudgetPartPainter *owner, const FudgetDrawArea &area)
{
    if (owner == nullptr)
        return nullptr;

    FudgetDrawable *result = Create(owner, nullptr);
    result->_list->_list.push_back(new FudgetDrawInstructionDrawArea(area));
    return result;
}

FudgetDrawable* FudgetDrawable::FromDrawInstructionList(FudgetPartPainter *owner, FudgetStyle *style, FudgetTheme *theme, FudgetDrawInstructionList *arealist)
{
    if (owner == nullptr)
        return nullptr;

    // Check first if the area list contains any ids that must be looked up. If no ids are found,
    // the area list can be used as-is and the ownership stays with FudgetThemes.

    if (NoExternalResources(arealist))
    {
        FudgetDrawable *result = Create(owner, arealist);
        return result;
    }
   
    FudgetDrawable *result = Create(owner, nullptr);
    for (auto item : arealist->_list)
    {
        FudgetDrawInstruction *cloned = CloneDrawInstructionListItem(style, theme, item);
        if (cloned != nullptr)
            result->_list->_list.push_back(cloned);
    }
    return result;
}

bool FudgetDrawable::NoExternalResources(FudgetDrawInstructionList *arealist)
{
    for (auto item : arealist->_list)
    {
        if (item->_type == FudgetDrawInstructionType::Resource)
            return false;
        if (item->_type == FudgetDrawInstructionType::AreaList)
        {
            if (!NoExternalResources((FudgetDrawInstructionList*)item))
                return false;
        }
    }
    return true;
}

FudgetDrawInstruction* FudgetDrawable::CloneDrawInstructionListItem(FudgetStyle *style, FudgetTheme *theme, FudgetDrawInstruction *item)
{
    FudgetDrawInstruction *result = nullptr;
    switch (item->_type)
    {
        case FudgetDrawInstructionType::AreaList:
            result = new FudgetDrawInstructionList();
            for (auto areaitem : ((FudgetDrawInstructionList*)item)->_list)
            {
                FudgetDrawInstruction *cloned = CloneDrawInstructionListItem(style, theme, areaitem);
                if (cloned != nullptr)
                    ((FudgetDrawInstructionList*)result)->_list.push_back(cloned);
            }
            break;
        case FudgetDrawInstructionType::Blur:
            result = new FudgetDrawInstructionFloat(FudgetDrawInstructionType::Blur,((FudgetDrawInstructionFloat*)item)->_value);
            break;
        case FudgetDrawInstructionType::DrawArea:
            result = new FudgetDrawInstructionDrawArea(((FudgetDrawInstructionDrawArea*)item)->_draw_area);
            break;
        case FudgetDrawInstructionType::FillColor:
            result = new FudgetDrawInstructionColor(((FudgetDrawInstructionColor*)item)->_color);
            break;
        case FudgetDrawInstructionType::Padding:
            result = new FudgetDrawInstructionPadding(((FudgetDrawInstructionPadding*)item)->_padding);
            break;
        case FudgetDrawInstructionType::Resource:
        {
            int id = ((FudgetDrawInstructionResource*)item)->_id;
            float f;
            if (FudgetStyle::GetFloatResource(style, theme, id, true, f))
            {
                result = new FudgetDrawInstructionFloat(FudgetDrawInstructionType::Blur, f);
                break;
            }
            Color color;
            if (FudgetStyle::GetColorResource(style, theme, id, true, color))
            {
                result = new FudgetDrawInstructionColor(color);
                break;
            }
            FudgetPadding padding;
            if (FudgetStyle::GetPaddingResource(style, theme, id, true, padding))
            {
                result = new FudgetDrawInstructionPadding(padding);
                break;
            }
            FudgetDrawArea area;
            if (FudgetStyle::GetDrawAreaResource(style, theme, id, true, area))
            {
                result = new FudgetDrawInstructionDrawArea(area);
                break;
            }

            // GetDrawableResource is not supported to avoid circular referencing (?)

            break;
        }
    }
    return result;
}

FudgetDrawable* FudgetDrawable::Create(FudgetPartPainter *owner, FudgetDrawInstructionList *list)
{
    if (owner == nullptr)
        return nullptr;

    FudgetDrawable *result = New<FudgetDrawable>(SpawnParams(Guid::New(), FudgetDrawable::TypeInitializer));
    result->_owned = list == nullptr;
    result->_list = list == nullptr ? new FudgetDrawInstructionList() : list;
    owner->RegisterDrawable(result);
    return result;
}

FudgetDrawable* FudgetDrawable::CreateEmpty()
{
    FudgetDrawable *result = New<FudgetDrawable>(SpawnParams(Guid::New(), FudgetDrawable::TypeInitializer));
    result->_owned = true;
    result->_list = new FudgetDrawInstructionList();
    return result;
}

