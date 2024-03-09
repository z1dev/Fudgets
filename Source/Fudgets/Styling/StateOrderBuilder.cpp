#include "StateOrderBuilder.h"
#include "Themes.h"

#include "Engine/Core/Log.h"


// FudgetDrawInstructionList


FudgetStateOrderItemList::~FudgetStateOrderItemList()
{
    for (auto item : _list)
        delete item;
    _list.clear();
}


// FudgetStateOrder


uint64 FudgetStateOrder::GetMatchingState(uint64 states) const
{
    FudgetStateOrderItemList *pos = _data;
    FudgetStateOrderItem **data = _data->_list.data();
    uint64 result = 0;
    for (int ix = 0, cnt = pos->_list.size(); ix < cnt; ++ix)
    {
        if ((states & data[ix]->_state) != 0 || data[ix]->_state == 0)
        {
            result = data[ix]->_state;
            if (data[ix]->_has_sub_items)
            {
                ix = -1;
                pos = (FudgetStateOrderItemList*)data[ix];
                data = pos->_list.data();
                cnt = pos->_list.size();
            }
            else
            {
                break;
            }
        }
    }
    return result;
}


// FudgetStateOrderBuilder


bool FudgetStateOrderBuilder::_building = false;
FudgetStateOrderItemList *FudgetStateOrderBuilder::_data = nullptr;
std::vector<FudgetStateOrderItemList*> FudgetStateOrderBuilder::_sub_data;

bool FudgetStateOrderBuilder::Begin()
{
    if (_building)
    {
        LOG(Warning, "Begin: Already building a draw order. Ending it and starting a new one.");
        End(true);
    }

    _building = true;
    _data = new FudgetStateOrderItemList(-1);
    return true;
}

int FudgetStateOrderBuilder::End(bool abort)
{
    int result;
    result = -1;
    if (!_building)
    {
        LOG(Error, "End: Not building a draw order.");
        return result;
    }

    while (!_sub_data.empty())
        EndSubData();
    _building = false;
    if (abort)
        delete _data;
    else
        result = FudgetThemes::RegisterStateOrder(new FudgetStateOrder(_data));
    return result;
}

void FudgetStateOrderBuilder::AddState(uint64 state)
{
    if (!_building)
    {
        LOG(Error, "AddOrder: Not building a draw order.");
        return;
    }

    FudgetStateOrderItemList *data = _sub_data.empty() ? _data : _sub_data.back();
    data->_list.push_back(new FudgetStateOrderItem(state));
}

void FudgetStateOrderBuilder::AddStateWithSubstates(uint64 state)
{
    if (!_building)
    {
        LOG(Error, "AddSubOrder: Not building a draw order.");
        return;
    }

    FudgetStateOrderItemList *data = _sub_data.empty() ? _data : _sub_data.back();
    FudgetStateOrderItemList *new_list = new FudgetStateOrderItemList(state);
    data->_list.push_back(new_list);
    _sub_data.push_back(new_list);
}

void FudgetStateOrderBuilder::EndSubData()
{
    if (!_building)
    {
        LOG(Error, "EndSubData: Not building a draw order.");
        return;
    }
    if (_sub_data.empty())
    {
        LOG(Error, "EndSubData: Not building a drawable sub-data.");
        return;
    }

    _sub_data.pop_back();
}

