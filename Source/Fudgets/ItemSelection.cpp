#include "ItemSelection.h"
#include <algorithm>
#include "Engine/Core/Math/Math.h"

FudgetItemSelection::FudgetItemSelection(const SpawnParams &params) : Base(params), _size(0), _count(0)
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

void FudgetItemSelection::Clear()
{
    _blocks.clear();
    _count = 0;
}

void FudgetItemSelection::SetSelected(int index, int count, bool select)
{
    std::vector<FudgetSelectionBlock>::iterator it;
    if (select)
    {
        it = std::lower_bound(_blocks.begin(), _blocks.end(), index, [](const FudgetSelectionBlock &block, int index) {
            return block._end < index;
            });

        if (it == _blocks.end() || it->_start > index + count)
        {
            FudgetSelectionBlock new_block;
            new_block._start = index;
            new_block._end = Math::Min(_size, index + count);
            _blocks.insert(it, new_block);
            _count += count;
            return;
        }

        FudgetSelectionBlock &block = *it;
        _count += (Math::Max(index + count, block._end) - Math::Min(block._start, index)) - (block._end - block._start);
        block._start = Math::Min(index, block._start);
        block._end = Math::Max(index + count, block._end);

        ++it;
        auto it2 = it;
        while (it2 != _blocks.end())
        {
            FudgetSelectionBlock &block2 = *it2;
            if (block2._end >= index + count)
            {
                if (block2._start <= index + count)
                {
                    _count -= index + count - block2._start;

                    block._end = Math::Max(block._end, block2._end);
                    ++it2;
                }
                break;
            }
            _count -= block2._end - block2._start;

            ++it2;
        }

        _blocks.erase(it, it2);
        return;
    }

    it = std::lower_bound(_blocks.begin(), _blocks.end(), index, [](const FudgetSelectionBlock &block, int index) {
        return block._end <= index;
        });

    if (it == _blocks.end())
        return;

    FudgetSelectionBlock &block = *it;
    if (block._start < index)
    {
        ++it;
        if (block._end > index + count)
        {
            _count -= count;
            FudgetSelectionBlock new_block;
            new_block._start = index + count;
            new_block._end = block._end;
            block._end = index;
            _blocks.insert(it, new_block);
            return;
        }
        _count -= block._end - index;
        block._end = index;
    }

    auto it2 = it;
    while (it2 < _blocks.end())
    {
        FudgetSelectionBlock &block2 = *it2;
        if (block2._end > index + count)
        {
            if (block2._start < index + count)
            {
                _count -= index + count - block2._start;
                block2._start = index + count;
            }
            break;
        }
        _count -= block2._end - block2._start;
        ++it2;
    }

    _blocks.erase(it, it2);

}

bool FudgetItemSelection::IsSelected(int index) const
{
    auto it = std::lower_bound(_blocks.cbegin(), _blocks.cend(), index, [](const FudgetSelectionBlock &block, int index) {
        return block._end <= index;
        });

    return it != _blocks.end() && it->_start <= index;
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
    for (auto it2 = block._start >= index ? it : std::next(it); it2 != _blocks.end(); ++it2)
    {
        it2->_start += count;
        it2->_end += count;
    }

    if (block._start >= index)
        return;
    FudgetSelectionBlock new_block;
    new_block._start = index + count;
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

    SetSelected(index, count, false);

    _size = Math::Max(_size - count, index);

    auto it = std::lower_bound(_blocks.begin(), _blocks.end(), index, [](const FudgetSelectionBlock &block, int index) {
        return block._end < index;
        });
    if (it == _blocks.end())
        return;

    FudgetSelectionBlock &block = *it;
    bool remove_block = false;

    if (block._end == index)
    {
        ++it;
        if (it != _blocks.end())
        {
            const FudgetSelectionBlock &block2 = *it;
            if (block2._start == index + count)
            {
                block._end = block2._end - count;
                remove_block = true;
            }
        }
    }

    auto it2 = it;
    while (it2 != _blocks.end())
    {
        it2->_start -= count;
        it2->_end -= count;
        ++it2;
    }

    if (remove_block)
        _blocks.erase(it);
}

