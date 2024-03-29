#include "ItemSelection.h"
#include <algorithm>
#include "Engine/Core/Math/Math.h"

FudgetItemSelection::FudgetItemSelection(const SpawnParams &params) : Base(params), _size(0)
{

}

void FudgetItemSelection::SetSize(int value)
{
    if (value < 0)
        value = 0;
    if (value < _size)
        ItemsRemoved(value, _size - value);
    else if (value > _size)
        ItemsInserted(_size, value - _size);
}

void FudgetItemSelection::SetSelected(int index, int count, bool select)
{
    std::vector<FudgetSelectionBlock>::iterator it;
    if (select)
    {
        it = std::lower_bound(_blocks.begin(), _blocks.end(), index, [](const FudgetSelectionBlock &block, int index) {
            return block._end < index;
            });

        if (it == _blocks.end() || it->_index > index + count)
        {
            FudgetSelectionBlock new_block;
            new_block._index = index;
            new_block._end = Math::Min(_size, index + count);
            _blocks.insert(it, new_block);
            return;
        }

        FudgetSelectionBlock &block = *it;
        block._index = Math::Min(index, block._index);
        block._end = Math::Max(index + count, block._end);

        ++it;
        auto it2 = it;
        while (it2 != _blocks.end())
        {
            if (it2->_end >= index + count)
            {
                if (it2->_index <= index + count)
                {
                    block._end = Math::Max(block._end, it2->_end);
                    ++it2;
                }
                break;
            }
            ++it2;
        }

        _blocks.erase(it, it2);
        return;
    }

    it = std::lower_bound(_blocks.begin(), _blocks.end(), index, [](const FudgetSelectionBlock &block, int index) {
        return block._end <= index;
        });

    FudgetSelectionBlock &block = *it;
    if (block._index < index)
    {
        ++it;
        if (block._end > index + count)
        {
            FudgetSelectionBlock new_block;
            new_block._index = index + count;
            new_block._end = block._end;
            block._end = index;
            _blocks.insert(it, new_block);
            return;
        }
        block._end = index;
    }

    auto it2 = it;
    while (it2 < _blocks.end())
    {
        if (it2->_end > index + count)
        {
            if (it2->_index < index + count)
                it2->_index = index + count;
            break;
        }
        ++it2;
    }

    _blocks.erase(it, it2);

}

bool FudgetItemSelection::IsSelected(int index) const
{
    auto it = std::lower_bound(_blocks.cbegin(), _blocks.cend(), index, [](const FudgetSelectionBlock &block, int index) {
        return block._end <= index;
        });

    return it != _blocks.end() && it->_index <= index;
}

void FudgetItemSelection::ItemsInserted(int index, int count)
{
    _size = Math::Max(_size + count, index + count);
    auto it = std::lower_bound(_blocks.begin(), _blocks.end(), index, [](const FudgetSelectionBlock &block, int index) {
        return block._end <= index;
        });
    if (it == _blocks.end())
        return;

    FudgetSelectionBlock &block = *it;
    for (auto it2 = block._index >= index ? it : std::next(it); it2 != _blocks.end(); ++it2)
    {
        it2->_index += count;
        it2->_end += count;
    }

    if (block._index >= index)
        return;
    FudgetSelectionBlock new_block;
    new_block._index = index + count;
    new_block._end = block._end + count;
    block._end = index;
    _blocks.insert(std::next(it), new_block);
}

void FudgetItemSelection::ItemsRemoved(int index, int count)
{
    if (index < 0)
    {
        count += index;
        index = 0;
    }
    if (count <= 0)
        return;
    _size = Math::Max(_size - count, index);

    auto it = std::lower_bound(_blocks.begin(), _blocks.end(), index, [](const FudgetSelectionBlock &block, int index) {
        return block._end < index;
        });
    if (it == _blocks.end())
        return;

    FudgetSelectionBlock &block = *it;
    if (block._index < index)
    {
        if (block._end > index + count)
            block._end -= count;
        else
            block._end = index;
        it = std::next(it);
    }
    auto it2 = it;
    while (it2 != _blocks.end())
    {
        FudgetSelectionBlock &block2 = *it2;
        if (block2._end <= index + count)
        {
            ++it2;
            continue;
        }

        if (block2._index <= index + count)
        {
            if (block._index < index)
            {
                ++it2;
                block._end = block2._end - count;
            }
            else
            {
                block2._index = index + count;
            }
        }
        break;
    }
    for (auto it3 = it2; it3 != _blocks.end(); ++it3)
    {
        it3->_index -= count;
        it3->_end -= count;
    }

    if (it != it2)
        _blocks.erase(it, it2);
}

