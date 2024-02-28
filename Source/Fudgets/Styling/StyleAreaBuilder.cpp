#include "StyleAreaBuilder.h"
#include "Themes.h"
#include "Style.h"

#include "Engine/Core/Log.h"


// FudgetStyleAreaList


FudgetStyleAreaList::~FudgetStyleAreaList()
{
    for (auto area : _list)
        delete area;
    _list.clear();
}

bool FudgetDrawableBuilder::_building = false;
FudgetToken FudgetDrawableBuilder::_token = -1;
FudgetStyleAreaList *FudgetDrawableBuilder::_data = nullptr;
std::vector<FudgetStyleAreaList*> FudgetDrawableBuilder::_sub_data;


// FudgetDrawableBuilder

FudgetToken FudgetDrawableBuilder::Begin(String token_name)
{
    FudgetToken token = FudgetThemes::GetToken(token_name);
    if (token == FudgetToken::Invalid)
        token = FudgetThemes::RegisterToken(token_name);
    if (Begin(token))
        return token;
    return FudgetToken::Invalid;
}

bool FudgetDrawableBuilder::Begin(FudgetToken token)
{
    if (_building)
    {
        LOG(Warning, "Begin: Already building a style area. Ending it and starting a new one.");
        End(true);
    }
    if (FudgetThemes::IsStyleAreaListRegistered(token))
    {
        LOG(Error, "Begin: Drawable with token already exists.");
        return false;
    }

    _token = token;
    _building = true;
    _data = new FudgetStyleAreaList();
    return true;
}

void FudgetDrawableBuilder::End(bool abort)
{
    if (!_building)
    {
        LOG(Error, "End: Not building a style area.");
        return;
    }

    while (!_sub_data.empty())
        EndSubData();
    _building = false;
    if (abort)
        delete _data;
    else
        FudgetThemes::RegisterStyleAreaList(_token, _data);
}

void FudgetDrawableBuilder::AddResource(FudgetToken token)
{
    if (!_building)
    {
        LOG(Error, "AddColor: Not building a style area.");
        return;
    }

    FudgetStyleAreaList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetStyleAreaResource(token));
}

void FudgetDrawableBuilder::AddColor(Color color)
{
    if (!_building)
    {
        LOG(Error, "AddColor: Not building a style area.");
        return;
    }

    FudgetStyleAreaList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetStyleAreaColor(color));
}

void FudgetDrawableBuilder::AddPadding(FudgetPadding padding)
{
    if (!_building)
    {
        LOG(Error, "AddPadding: Not building a style area.");
        return;
    }

    FudgetStyleAreaList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetStyleAreaPadding(padding));
}

void FudgetDrawableBuilder::AddBlur(float blur_strength)
{
    if (!_building)
    {
        LOG(Error, "AddBlur: Not building a style area.");
        return;
    }

    FudgetStyleAreaList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetStyleAreaFloat(FudgetStyleAreaType::Blur, blur_strength));
}

void FudgetDrawableBuilder::AddDrawArea(FudgetDrawArea area)
{
    if (!_building)
    {
        LOG(Error, "AddDrawArea: Not building a style area.");
        return;
    }

    FudgetStyleAreaList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetStyleAreaDrawArea(area));
}

void FudgetDrawableBuilder::BeginSubData()
{
    if (!_building)
    {
        LOG(Error, "BeginSubData: Not building a style area.");
        return;
    }

    FudgetStyleAreaList *data = _sub_data.empty() ? _data : _sub_data.back();
    FudgetStyleAreaList *new_list = new FudgetStyleAreaList();
    data->_list.push_back(new_list);
    _sub_data.push_back(new_list);
}

void FudgetDrawableBuilder::EndSubData()
{
    if (!_building)
    {
        LOG(Error, "EndSubData: Not building a style area.");
        return;
    }
    if (_sub_data.empty())
    {
        LOG(Error, "EndSubData: Not building sub-data.");
        return;
    }

    _sub_data.pop_back();
}


// FudgetDrawable


FudgetDrawable::FudgetDrawable(const SpawnParams &params) : Base(params), _list(nullptr), _owned(false)
{
}

FudgetDrawable::~FudgetDrawable()
{
    if (_owned)
        delete _list;
}

FudgetDrawable* FudgetDrawable::FromColor(Color color)
{
    FudgetDrawable *result = Create();
    result->_list->_list.push_back(new FudgetStyleAreaColor(color));
    return result;
}

FudgetDrawable* FudgetDrawable::FromDrawArea(const FudgetDrawArea &area)
{
    FudgetDrawable *result = Create();
    result->_list->_list.push_back(new FudgetStyleAreaDrawArea(area));
    return result;
}

FudgetDrawable* FudgetDrawable::FromStyleAreaList(FudgetStyle *style, FudgetTheme *theme, FudgetStyleAreaList *arealist)
{
    // Check first if the area list contains any tokens that must be looked up. If no tokens are found,
    // the area list can be used as-is and the ownership stays with FudgetThemes.

    if (NoExternalResources(arealist))
    {
        FudgetDrawable *result = New<FudgetDrawable>(SpawnParams(Guid::New(), FudgetDrawable::TypeInitializer));
        result->_owned = false;
        result->_list = arealist;
        return result;
    }
   
    FudgetDrawable *result = Create();
    for (auto item : arealist->_list)
    {
        FudgetStyleArea *cloned = CloneStyleAreaListItem(style, theme, item);
        if (cloned != nullptr)
            result->_list->_list.push_back(cloned);
    }
    return result;
}

bool FudgetDrawable::NoExternalResources(FudgetStyleAreaList *arealist)
{
    for (auto item : arealist->_list)
    {
        if (item->_type == FudgetStyleAreaType::Resource)
            return false;
        if (item->_type == FudgetStyleAreaType::AreaList)
        {
            if (!NoExternalResources((FudgetStyleAreaList*)item))
                return false;
        }
    }
    return true;
}

FudgetStyleArea* FudgetDrawable::CloneStyleAreaListItem(FudgetStyle *style, FudgetTheme *theme, FudgetStyleArea *item)
{
    FudgetStyleArea *result = nullptr;
    switch (item->_type)
    {
        case FudgetStyleAreaType::AreaList:
            result = new FudgetStyleAreaList();
            for (auto areaitem : ((FudgetStyleAreaList*)item)->_list)
            {
                FudgetStyleArea *cloned = CloneStyleAreaListItem(style, theme, areaitem);
                if (cloned != nullptr)
                    ((FudgetStyleAreaList*)result)->_list.push_back(cloned);
            }
            break;
        case FudgetStyleAreaType::Blur:
            result = new FudgetStyleAreaFloat(FudgetStyleAreaType::Blur,((FudgetStyleAreaFloat*)item)->_value);
            break;
        case FudgetStyleAreaType::DrawArea:
            result = new FudgetStyleAreaDrawArea(((FudgetStyleAreaDrawArea*)item)->_draw_area);
            break;
        case FudgetStyleAreaType::FillColor:
            result = new FudgetStyleAreaColor(((FudgetStyleAreaColor*)item)->_color);
            break;
        case FudgetStyleAreaType::Padding:
            result = new FudgetStyleAreaPadding(((FudgetStyleAreaPadding*)item)->_padding);
            break;
        case FudgetStyleAreaType::Resource:
        {
            FudgetToken token = ((FudgetStyleAreaResource*)item)->_token;
            float f;
            if (style->GetFloatResource(theme, token, f))
            {
                result = new FudgetStyleAreaFloat(FudgetStyleAreaType::Blur, f);
                break;
            }
            Color color;
            if (style->GetColorResource(theme, token, color))
            {
                result = new FudgetStyleAreaColor(color);
                break;
            }
            FudgetPadding padding;
            if (style->GetPaddingResource(theme, token, padding))
            {
                result = new FudgetStyleAreaPadding(padding);
                break;
            }
            FudgetDrawArea area;
            if (style->GetDrawAreaResource(theme, token, area))
            {
                result = new FudgetStyleAreaDrawArea(area);
                break;
            }

            // GetDrawableResource is not supported to avoid cyclic referencing

            break;
        }
    }
    return result;
}
