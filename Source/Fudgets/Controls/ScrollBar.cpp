#include "ScrollBar.h"
#include "../Styling/PartPainterIds.h"
#include "../Styling/Painters/ScrollBarPainter.h"

#include "Engine/Core/Log.h"

FudgetScrollBarComponent::FudgetScrollBarComponent(const SpawnParams &params) : Base(params), _owner(nullptr),
    _painter(nullptr), _orientation(FudgetScrollBarOrientation::Horizontal), _bounds(Rectangle::Empty), _rects_dirty(true),
    _track_rect(Rectangle::Empty), _before_track_rect(Rectangle::Empty), _after_track_rect(Rectangle::Empty), _thumb_rect(Rectangle::Empty),
    _range(0), _pos(0), _page_size(0), _visible(false), _visibility_mode(FudgetScrollBarVisibilityMode::Automatic)
{
    for (int ix = 0; ix < 20; ++ix)
        _btn_rects[ix] = Rectangle::Empty;
}

void FudgetScrollBarComponent::Initialize(FudgetControl *owner)
{
    if (_owner != nullptr)
    {
        LOG(Error, "Trying to initialize a FudgetScrollBarComponent that is already initialized.");
        return;
    }
    _owner = owner;
}

void FudgetScrollBarComponent::StyleInitialize()
{
    _painter = _owner->CreateStylePainter<FudgetScrollBarPainter>(_painter, (int)FudgetScrollBarPartIds::Painter);
}

void FudgetScrollBarComponent::Draw()
{
    if (_painter == nullptr)
        return;

    RecalculateRectangles();
    _painter->DrawBackground(_owner, _bounds, _track_rect, _owner->GetVisualState());
    uint64 before_states = _owner->GetVisualState();
    uint64 after_states = _owner->GetVisualState();
    _painter->DrawTrack(_owner, _before_track_rect, _after_track_rect, before_states, after_states);
    uint64 thumb_states = _owner->GetVisualState();
    _painter->DrawThumb(_owner, _thumb_rect, thumb_states);

    int btn_cnt = _painter->GetButtonCount();

    for (int ix = 0; ix < btn_cnt; ++ix)
    {
        uint64 button_states = _owner->GetVisualState();
        _painter->DrawButton(_owner, ix, _btn_rects[ix], button_states);
    }
    _painter->DrawFrame(_owner, _bounds ,_owner->GetVisualState());
}

int FudgetScrollBarComponent::GetWidth() const
{
    if (_painter == nullptr)
        return 0;
    return _painter->GetWidth();
}

void FudgetScrollBarComponent::SetPageSize(int64 value)
{
    if (_page_size == value)
        return;
    _page_size = value;
    UpdateVisibility();
}

void FudgetScrollBarComponent::SetScrollRange(int64 value)
{
    if (_range == value)
        return;
    _range = value;
    UpdateVisibility();
}

void FudgetScrollBarComponent::SetVisibilityMode(FudgetScrollBarVisibilityMode value)
{
    if (_visibility_mode == value)
        return;
    _visibility_mode = value;
    UpdateVisibility();
}

void FudgetScrollBarComponent::UpdateVisibility()
{
    if (_visibility_mode == FudgetScrollBarVisibilityMode::Visible)
    {
        _visible = true;
        return;
    }
    else if (_visibility_mode == FudgetScrollBarVisibilityMode::Hidden)
    {
        _visible = false;
        return;
    }

    _visible = _range > _page_size;
    _rects_dirty = true;
}

void FudgetScrollBarComponent::RecalculateRectangles()
{
    if (_painter == nullptr || _rects_dirty == false)
        return;
    int btn_cnt = _painter->GetButtonCount();
    _painter->GetPartBounds(_owner, _bounds, _range, _page_size, _pos, _track_rect, _before_track_rect, _after_track_rect, _thumb_rect, _btn_rects);

    _rects_dirty = false;
}

