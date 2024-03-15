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

FudgetDrawableBuilder::BuildMode FudgetDrawableBuilder::_mode = FudgetDrawableBuilder::BuildMode::None;
std::vector<uint64> FudgetDrawableBuilder::_states;
std::vector<FudgetDrawInstructionList*> FudgetDrawableBuilder::_state_list;
FudgetDrawInstructionList *FudgetDrawableBuilder::_data = nullptr;
std::vector<FudgetDrawInstructionList*> FudgetDrawableBuilder::_sub_data;
FudgetDrawColors FudgetDrawableBuilder::_colors;


void FudgetDrawableBuilder::Begin(uint64 states)
{
    if (_mode != BuildMode::Drawable && _mode != BuildMode::None)
    {
        LOG(Warning, "Begin: Building a different type of draw object was interrupted.");
        if (_mode == BuildMode::Colors)
            EndDrawColors();
    }

    if (_mode == BuildMode::Drawable)
    {
        for (auto state : _states)
        {
            if (state == states)
            {
                LOG(Warning, "Begin: Already building a drawable with state {1}.", states);
                return;
            }
        }
    }

    _states.push_back(states);
    _data = new FudgetDrawInstructionList();
    _state_list.push_back(_data);
    _mode = BuildMode::Drawable;
}

FudgetDrawableIndex FudgetDrawableBuilder::End(bool abort)
{
    FudgetDrawableIndex result;
    result.Index = -1;
    if (_states.empty() || _mode != BuildMode::Drawable)
    {
        LOG(Error, "End: Not building a drawable.");
        return result;
    }

    while (!_sub_data.empty())
        EndSubData();

    if (!abort)
    {
        result.Index = FudgetThemes::RegisterDrawInstructionList(_states, _state_list);
    }
    if (abort || result.Index == -1)
    {
        for (auto data : _state_list)
            delete data;
    }
        

    _states.clear();
    _state_list.clear();

    _mode = BuildMode::None;

    return result;
}

void FudgetDrawableBuilder::AddResource(int id)
{
    if (_states.empty() || _mode != BuildMode::Drawable)
    {
        LOG(Error, "AddResource: Not building a drawable.");
        return;
    }

    FudgetDrawInstructionList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetDrawInstructionResource(id));
}

void FudgetDrawableBuilder::AddColor(Color color)
{
    if (_states.empty() || _mode != BuildMode::Drawable)
    {
        LOG(Error, "AddColor: Not building a drawable.");
        return;
    }

    FudgetDrawInstructionList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetDrawInstructionColor(color));
}

void FudgetDrawableBuilder::AddPadding(FudgetPadding padding)
{
    if (_states.empty() || _mode != BuildMode::Drawable)
    {
        LOG(Error, "AddPadding: Not building a drawable.");
        return;
    }

    FudgetDrawInstructionList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetDrawInstructionPadding(padding));
}

void FudgetDrawableBuilder::AddBlur(float blur_strength)
{
    if (_states.empty() || _mode != BuildMode::Drawable)
    {
        LOG(Error, "AddBlur: Not building a drawable.");
        return;
    }

    FudgetDrawInstructionList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetDrawInstructionFloat(FudgetDrawInstructionType::Blur, blur_strength));
}

void FudgetDrawableBuilder::AddDrawArea(FudgetDrawArea area)
{
    if (_states.empty() || _mode != BuildMode::Drawable)
    {
        LOG(Error, "AddDrawArea: Not building a drawable.");
        return;
    }

    FudgetDrawInstructionList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetDrawInstructionDrawArea(area));
}

void FudgetDrawableBuilder::BeginSubData()
{
    if (_states.empty() || _mode != BuildMode::Drawable)
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
    if (_states.empty() || _mode != BuildMode::Drawable)
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

void FudgetDrawableBuilder::MakeDrawColors(Color color, uint64 states)
{
    if (_mode != BuildMode::None && _mode != BuildMode::Colors)
    {
        LOG(Error, "MakeDrawColors: Not building colors.");
        return;
    }

    if (_mode == BuildMode::Colors)
    {
        for (int ix = 0, siz = _colors._states.Count(); ix < siz; ++ix)
            if (_colors._states[ix] == states)
            {
                LOG(Error, "MakeDrawColors: this state was already added.");
                return;
            }
    }

    _mode = BuildMode::Colors;
    _colors._states.Add(states);
    _colors._colors.Add(color);
}

FudgetDrawColors FudgetDrawableBuilder::EndDrawColors()
{
    if (_mode != BuildMode::None && _mode != BuildMode::Colors)
    {
        LOG(Error, "MakeDrawColors: Not building colors.");
        return FudgetDrawColors();
    }
    FudgetDrawColors tmp = _colors;

    _colors._states.Clear();
    _colors._colors.Clear();

    _mode = BuildMode::None;
    return tmp;
}


// FudgetDrawable


FudgetDrawable* FudgetDrawable::Empty = CreateEmpty();

FudgetDrawable::FudgetDrawable(const SpawnParams &params) : Base(params), _owned(false)
{
}

FudgetDrawable::~FudgetDrawable()
{
    if (_owned)
    {
        for (auto d : _lists)
            delete d;
    }
}

bool FudgetDrawable::IsEmpty() const
{
    if (_states.empty())
        return true;
    for (const auto l : _lists)
        if (l->_list.empty())
            return true;
    return false;
}

int FudgetDrawable::FindMatchingState(uint64 states) const
{
    for (int ix = 0, siz = (int)_states.size(); ix < siz; ++ix)
    {
        uint64 state = _states[ix];
        if ((states & state) == state)
            return ix;
    }
    return -1;
}

FudgetDrawable* FudgetDrawable::FromColor(FudgetPartPainter *owner, Color color)
{
    if (owner == nullptr)
        return nullptr;

    FudgetDrawable *result = Create(owner, 0);
    result->_lists.back()->_list.push_back(new FudgetDrawInstructionColor(color));

    return result;
}

FudgetDrawable* FudgetDrawable::FromDrawArea(FudgetPartPainter *owner, const FudgetDrawArea &area)
{
    if (owner == nullptr)
        return nullptr;

    FudgetDrawable *result = Create(owner, 0);
    result->_lists.back()->_list.push_back(new FudgetDrawInstructionDrawArea(area));
    return result;
}

FudgetDrawable* FudgetDrawable::FromDrawColors(FudgetPartPainter *owner, const FudgetDrawColors &colors)
{
    if (owner == nullptr)
        return nullptr;

    if (colors._states.IsEmpty())
        return FudgetDrawable::Empty;

    FudgetDrawable *result = Create(owner);
    for (int ix = 0, siz = colors._states.Count(); ix < siz; ++ix)
    {
        FudgetDrawInstructionList *new_list = new FudgetDrawInstructionList;
        result->_lists.push_back(new_list);
        result->_states.push_back(colors._states[ix]);
        new_list->_list.push_back(new FudgetDrawInstructionColor(colors._colors[ix]));
    }
    return result;
}

FudgetDrawable* FudgetDrawable::FromDrawInstructionList(FudgetPartPainter *owner, FudgetStyle *style, FudgetTheme *theme, const std::vector<uint64> &states, const std::vector<FudgetDrawInstructionList*> &lists)
{
    if (owner == nullptr || states.size() != lists.size())
        return nullptr;

    // Check first if the instruction list contains any ids that must be looked up. If no ids are found,
    // the instruction list can be used as-is and the ownership stays with FudgetThemes. Otherwise the list
    // is copied and the resources are looked up.

    bool has_external = false;
    for (auto list : lists)
    {
        has_external = !NoExternalResources(list);
        if (has_external)
            break;
    }
    if (!has_external)
    {
        FudgetDrawable *result = Create(owner, states, lists);
        return result;
    }
   
    FudgetDrawable *result = Create(owner);

    
    for (int ix = 0, siz = (int)states.size(); ix < siz; ++ix)
    {
        FudgetDrawInstructionList *new_list = new FudgetDrawInstructionList;
        result->_lists.push_back(new_list);

        uint64 list_states = states[ix];
        result->_states.push_back(list_states);
        FudgetDrawInstructionList *list = lists[ix];
        for (int iy = 0, sizy = (int)list->_list.size(); iy < sizy; ++iy)
        {
            FudgetDrawInstruction *cloned = CloneDrawInstructionListItem(style, theme, list->_list[iy], list_states);
            if (cloned != nullptr)
                new_list->_list.push_back(cloned);
        }
    }
    return result;
}

bool FudgetDrawable::NoExternalResources(FudgetDrawInstructionList* drawlist)
{
    for (auto item : drawlist->_list)
    {
        if (item->_type == FudgetDrawInstructionType::Resource)
            return false;
        if (item->_type == FudgetDrawInstructionType::InstructionList)
        {
            if (!NoExternalResources((FudgetDrawInstructionList*)item))
                return false;
        }
    }
    return true;
}

FudgetDrawInstruction* FudgetDrawable::CloneDrawInstructionListItem(FudgetStyle *style, FudgetTheme *theme, FudgetDrawInstruction *item, uint64 states)
{
    FudgetDrawInstruction *result = nullptr;
    switch (item->_type)
    {
        case FudgetDrawInstructionType::InstructionList:
            result = new FudgetDrawInstructionList();
            for (auto areaitem : ((FudgetDrawInstructionList*)item)->_list)
            {
                FudgetDrawInstruction *cloned = CloneDrawInstructionListItem(style, theme, areaitem, states);
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
            {
                float f;
                if (FudgetStyle::GetFloatResource(style, theme, id, true, f))
                {
                    result = new FudgetDrawInstructionFloat(FudgetDrawInstructionType::Blur, f);
                    break;
                }
            }

            {
                Color color;
                if (FudgetStyle::GetColorResource(style, theme, id, true, color))
                {
                    result = new FudgetDrawInstructionColor(color);
                    break;
                }
            }

            {
                FudgetPadding padding;
                if (FudgetStyle::GetPaddingResource(style, theme, id, true, padding))
                {
                    result = new FudgetDrawInstructionPadding(padding);
                    break;
                }
            }

            {
                FudgetDrawArea area;
                if (FudgetStyle::GetDrawAreaResource(style, theme, id, true, area))
                {
                    result = new FudgetDrawInstructionDrawArea(area);
                    break;
                }
            }

            FudgetDrawColors colors;
            {
                FudgetDrawArea area;
                if (FudgetStyle::GetDrawColorsResource(style, theme, id, true, colors))
                {
                    int color_index = colors.FindMatchingState(states);
                    if (color_index >= 0)
                        result = new FudgetDrawInstructionColor(colors._colors[color_index]);
                    break;
                }
            }

            // GetDrawableResource is not supported right now but should be....

            break;
        }
    }
    return result;
}

FudgetDrawable* FudgetDrawable::Create(FudgetPartPainter *owner, const std::vector<uint64> &states, const std::vector<FudgetDrawInstructionList*> &lists)
{
    if (owner == nullptr)
        return nullptr;

    FudgetDrawable *result = New<FudgetDrawable>(SpawnParams(Guid::New(), FudgetDrawable::TypeInitializer));
    result->_owned = false;
    result->_states = states;
    result->_lists = lists;
    owner->RegisterDrawable(result);
    return result;
}

FudgetDrawable* FudgetDrawable::Create(FudgetPartPainter *owner, uint64 state)
{
    if (owner == nullptr)
        return nullptr;

    FudgetDrawable *result = New<FudgetDrawable>(SpawnParams(Guid::New(), FudgetDrawable::TypeInitializer));
    result->_owned = true;
    result->_states.push_back(state);
    FudgetDrawInstructionList *list = new FudgetDrawInstructionList;
    result->_lists.push_back(list);
    owner->RegisterDrawable(result);
    return result;
}

FudgetDrawable* FudgetDrawable::Create(FudgetPartPainter *owner)
{
    if (owner == nullptr)
        return nullptr;

    FudgetDrawable *result = New<FudgetDrawable>(SpawnParams(Guid::New(), FudgetDrawable::TypeInitializer));
    result->_owned = true;
    owner->RegisterDrawable(result);
    return result;
}

FudgetDrawable* FudgetDrawable::CreateEmpty()
{
    FudgetDrawable *result = New<FudgetDrawable>(SpawnParams(Guid::New(), FudgetDrawable::TypeInitializer));
    return result;
}

