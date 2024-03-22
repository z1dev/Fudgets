#include "ScrollBar.h"
#include "../Control.h"
#include "../Styling/PartPainterIds.h"
#include "../Styling/Painters/ScrollBarPainter.h"

#include "Engine/Core/Log.h"

FudgetScrollBarComponent::FudgetScrollBarComponent(const SpawnParams &params) : Base(params), _owner(nullptr), _event_owner(nullptr),
    _painter(nullptr), _orientation(FudgetScrollBarOrientation::Horizontal), style_inited(false), _bounds(Rectangle::Empty), _rects_dirty(true),
    _track_rect(Rectangle::Empty), _before_track_rect(Rectangle::Empty), _after_track_rect(Rectangle::Empty), _thumb_rect(Rectangle::Empty),
    _range_min(0), _range_max(0), _scroll_pos(0), _page_size(0), _line_size(1), _visible(false), _mouse_capture(MouseCapture::None), _old_mouse_pos(-1.f), _thumb_mouse_pos(-1.f),
    _visibility_mode(FudgetScrollBarVisibilityMode::Visible)
{
    for (int ix = 0; ix < 20; ++ix)
        _btn_rects[ix] = Rectangle::Empty;
}

FudgetScrollBarComponent::~FudgetScrollBarComponent()
{
    if (_painter != nullptr)
        Delete(_painter);
}


void FudgetScrollBarComponent::Initialize(FudgetControl *owner, IFudgetScollBarOwner *event_owner, FudgetScrollBarOrientation orientation)
{
    if (_owner != nullptr)
    {
        LOG(Error, "Trying to initialize a FudgetScrollBarComponent that is already initialized.");
        return;
    }
    _owner = owner;
    _event_owner = event_owner;
    _orientation = orientation;
}

void FudgetScrollBarComponent::StyleInitialize()
{
    _painter = _owner->CreateStylePainter<FudgetScrollBarPainter>(_painter,
        _orientation == FudgetScrollBarOrientation::Horizontal ? (int)FudgetScrollBarPartIds::HorzPainter :
                                                                 (int)FudgetScrollBarPartIds::VertPainter);
    UpdateVisibility();
}

void FudgetScrollBarComponent::SetOrientation(FudgetScrollBarOrientation value)
{
    if (value == _orientation)
        return;
    _orientation = value;
    if (style_inited)
        StyleInitialize();
}

void FudgetScrollBarComponent::Draw()
{
    if (_painter == nullptr)
        return;

    uint64 owner_states = _owner->GetVisualState();
    owner_states &= ~uint64(FudgetVisualControlState::Hovered | FudgetVisualControlState::Pressed | FudgetVisualControlState::Down | FudgetVisualControlState::Selected);

    RecalculateRectangles();
    _painter->DrawBackground(_owner, _bounds, _track_rect, owner_states);
    uint64 before_states = owner_states;
    if ((_mouse_capture == MouseCapture::None || _mouse_capture == MouseCapture::BeforeTrack) && _before_track_rect.Contains(_old_mouse_pos))
        before_states |= _mouse_capture == MouseCapture::None ? (uint64)FudgetVisualControlState::Hovered : (uint64)FudgetVisualControlState::Pressed;
    int role = _painter->GetBeforeTrackRole();
    if (((role == (int)FudgetScrollBarButtonRole::LineUp || role == (int)FudgetScrollBarButtonRole::PageUp ||
        role == (int)FudgetScrollBarButtonRole::PageUpLine || role == (int)FudgetScrollBarButtonRole::JumpToStart) && (_scroll_pos <= _range_min)) ||
        ((role == (int)FudgetScrollBarButtonRole::LineDown || role == (int)FudgetScrollBarButtonRole::PageDown ||
        role == (int)FudgetScrollBarButtonRole::PageDownLine || role == (int)FudgetScrollBarButtonRole::JumpToEnd) && (_scroll_pos >= _range_max - _page_size + 1)))
        before_states |= (uint64)FudgetVisualControlState::Disabled;

    uint64 after_states = owner_states;
    role = _painter->GetAfterTrackRole();
    if ((_mouse_capture == MouseCapture::None || _mouse_capture == MouseCapture::AfterTrack) && _after_track_rect.Contains(_old_mouse_pos))
        after_states |= _mouse_capture == MouseCapture::None ? (uint64)FudgetVisualControlState::Hovered : (uint64)FudgetVisualControlState::Pressed;
    if (((role == (int)FudgetScrollBarButtonRole::LineUp || role == (int)FudgetScrollBarButtonRole::PageUp ||
        role == (int)FudgetScrollBarButtonRole::PageUpLine || role == (int)FudgetScrollBarButtonRole::JumpToStart) && (_scroll_pos <= _range_min)) ||
        ((role == (int)FudgetScrollBarButtonRole::LineDown || role == (int)FudgetScrollBarButtonRole::PageDown ||
        role == (int)FudgetScrollBarButtonRole::PageDownLine || role == (int)FudgetScrollBarButtonRole::JumpToEnd) && (_scroll_pos >= _range_max - _page_size + 1)))
        after_states |= (uint64)FudgetVisualControlState::Disabled;
    _painter->DrawTrack(_owner, _before_track_rect, _after_track_rect, before_states, after_states);

    uint64 thumb_states = owner_states;
    if ((_mouse_capture == MouseCapture::None || _mouse_capture == MouseCapture::Thumb) && _thumb_rect.Contains(_old_mouse_pos))
        thumb_states |= _mouse_capture == MouseCapture::None ? (uint64)FudgetVisualControlState::Hovered : (uint64)FudgetVisualControlState::Pressed;
    if (_range_max - _range_min + 1 <= _page_size)
        thumb_states |= (uint64)FudgetVisualControlState::Disabled;
    _painter->DrawThumb(_owner, _thumb_rect, thumb_states);

    int btn_cnt = _painter->GetButtonCount();

    for (int ix = 0; ix < btn_cnt; ++ix)
    {
        uint64 button_states = owner_states;
        if ((_mouse_capture == MouseCapture::None || (int)_mouse_capture == (int)MouseCapture::Button1 + ix) && _btn_rects[ix].Contains(_old_mouse_pos))
        {
            button_states |= _mouse_capture == MouseCapture::None ? (uint64)FudgetVisualControlState::Hovered : (uint64)FudgetVisualControlState::Pressed;
        }
        role = _painter->GetButtonRole(ix);
        if (((role == (int)FudgetScrollBarButtonRole::LineUp || role == (int)FudgetScrollBarButtonRole::PageUp ||
            role == (int)FudgetScrollBarButtonRole::PageUpLine || role == (int)FudgetScrollBarButtonRole::JumpToStart) && (_scroll_pos <= _range_min)) ||
            ((role == (int)FudgetScrollBarButtonRole::LineDown || role == (int)FudgetScrollBarButtonRole::PageDown ||
            role == (int)FudgetScrollBarButtonRole::PageDownLine || role == (int)FudgetScrollBarButtonRole::JumpToEnd) && (_scroll_pos >= _range_max - _page_size + 1)))
            button_states |= (uint64)FudgetVisualControlState::Disabled;

        _painter->DrawButton(_owner, ix, _btn_rects[ix], button_states);
    }
    _painter->DrawFrame(_owner, _bounds, owner_states);
}

bool FudgetScrollBarComponent::GetAssumedVisible(bool visible) const
{
    if (_visibility_mode == FudgetScrollBarVisibilityMode::Automatic)
        return visible;
    return _visible;
}

int FudgetScrollBarComponent::GetWidth() const
{
    if (_painter == nullptr)
        return 0;
    return _painter->GetWidth();
}

void FudgetScrollBarComponent::SetPageSize(int64 value)
{
    if (value < 1)
        value = 1;
    if (_page_size == value)
        return;
    _page_size = value;
    UpdateVisibility();
}

void FudgetScrollBarComponent::SetMinScrollRange(int64 value)
{
    if (_range_min == value)
        return;
    _range_min = value;
    if (_range_max < value)
        _range_max = value;

    int64 old_pos = _scroll_pos;
    if (_scroll_pos < value)
        _scroll_pos = value;
    UpdateVisibility();
    if (old_pos != _scroll_pos && _event_owner != nullptr)
        _event_owner->OnScrollBarScroll(this, old_pos, false);
}

void FudgetScrollBarComponent::SetMaxScrollRange(int64 value)
{
    if (_range_max == value)
        return;
    _range_max = value;
    if (_range_min > value)
        _range_min = value;
    int64 old_pos = _scroll_pos;
    if (_scroll_pos > value)
        _scroll_pos = value;
    UpdateVisibility();
    if (old_pos != _scroll_pos && _event_owner != nullptr)
        _event_owner->OnScrollBarScroll(this, old_pos, false);
}

void FudgetScrollBarComponent::SetScrollRange(int64 value)
{
    if (value < 1)
        value = 1;
    if (_range_min + value == _range_max + 1)
        return;
    _range_max = _range_min + value - 1;
    UpdateVisibility();
}

void FudgetScrollBarComponent::SetScrollPos(int64 value)
{
    value = Math::Clamp(value, _range_min, Math::Max(0LL, _range_max + 1 - _page_size));
    if (_scroll_pos == value)
        return;
    int64 old_pos = _scroll_pos;
    _scroll_pos = value;
    _rects_dirty = true;
    if (old_pos != _scroll_pos && _event_owner != nullptr)
        _event_owner->OnScrollBarScroll(this, old_pos, false);
}

void FudgetScrollBarComponent::SetVisibilityMode(FudgetScrollBarVisibilityMode value)
{
    if (_visibility_mode == value)
        return;
    _visibility_mode = value;
    UpdateVisibility();
}

bool FudgetScrollBarComponent::MouseMove(Float2 pos, Float2 global_pos)
{
    if (!_visible || (!MouseIsCaptured() && (_owner->MouseIsCaptured() || !_bounds.Contains(pos))))
    {
        if (_old_mouse_pos.X != -1.f)
            MouseLeave();
        return false;
    }

    int old_pos = (int)_scroll_pos;
    if (_mouse_capture == MouseCapture::Thumb)
    {
        double track_height = Relevant(_track_rect.Size) - Relevant(_thumb_rect.Size);
        if (track_height <= 0 || Math::NearEqual(track_height, 0))
            _scroll_pos = _range_min;
        else
        {
            double track_pos = Math::Clamp(double(Relevant(pos) - _thumb_mouse_pos) - Relevant(_track_rect.Location), 0.0, track_height);
            _scroll_pos = _range_min + int64((track_pos / track_height) * (_range_max - _range_min + 1 - _page_size));
        }
    }

    _old_mouse_pos = pos;

    if (old_pos != _scroll_pos)
    {
        _rects_dirty = true;
        if (_event_owner != nullptr)
            _event_owner->OnScrollBarScroll(this, old_pos, true);
    }

    if (MouseIsCaptured())
        return true;
    return false;
}

bool FudgetScrollBarComponent::MouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
    if (!_visible || _painter == nullptr || (!MouseIsCaptured() && (_owner->MouseIsCaptured() || !_bounds.Contains(pos))))
        return false;

    _old_mouse_pos = pos;

    MouseCapture old_capture = _mouse_capture;
    if (!MouseIsCaptured() && button == MouseButton::Left)
    {
        if (_before_track_rect.Contains(pos))
            _mouse_capture = MouseCapture::BeforeTrack;
        else if (_after_track_rect.Contains(pos))
            _mouse_capture = MouseCapture::AfterTrack;
        else if (_thumb_rect.Contains(pos))
        {
            _mouse_capture = MouseCapture::Thumb;
            _thumb_mouse_pos = (Relevant(pos) - Relevant(_thumb_rect.Location));
        }
        else
        {
            for (int ix = 0, siz = _painter->GetButtonCount(); ix < siz; ++ix)
            {
                if (_btn_rects[ix].Contains(pos))
                {
                    _mouse_capture = MouseCapture((int)MouseCapture::Button1 + ix);
                    break;
                }
            }
        }
    }
    if (_event_owner != nullptr && old_capture != _mouse_capture)
    {
        if (old_capture >= MouseCapture::Button1 && old_capture <= MouseCapture::Button20)
        {
            int btn_ix = (int)old_capture - (int)MouseCapture::Button1;
            _event_owner->OnScrollBarButtonReleased(this, btn_ix, btn_ix < _painter->GetBeforeButtonCount());
        }
        else if (old_capture == MouseCapture::Thumb)
        {
            _event_owner->OnScrollBarThumbReleased(this);
        }
        else if (old_capture == MouseCapture::BeforeTrack)
        {
            _event_owner->OnScrollBarTrackReleased(this, true);
        }
        else if (old_capture == MouseCapture::AfterTrack)
        {
            _event_owner->OnScrollBarTrackReleased(this, false);
        }

        int role = 0;
        if (_mouse_capture >= MouseCapture::Button1 && _mouse_capture <= MouseCapture::Button20)
        {
            int btn_ix = (int)_mouse_capture - (int)MouseCapture::Button1;
            _event_owner->OnScrollBarButtonPressed(this, btn_ix, btn_ix < _painter->GetBeforeButtonCount(), double_click);
            role = _painter->GetButtonRole(btn_ix);
        }
        else if (_mouse_capture == MouseCapture::Thumb)
        {
            _event_owner->OnScrollBarThumbPressed(this, double_click);
        }
        else if (_mouse_capture == MouseCapture::BeforeTrack)
        {
            _event_owner->OnScrollBarTrackPressed(this, true, double_click);
            role = _painter->GetBeforeTrackRole();
        }
        else if (_mouse_capture == MouseCapture::AfterTrack)
        {
            _event_owner->OnScrollBarTrackPressed(this, false, double_click);
            role = _painter->GetAfterTrackRole();
        }
        if (role != 0)
        {
            if (!_event_owner->OnRole(this, role))
                HandleRole(role);
        }
    }

    if (MouseIsCaptured())
        return true;
    return false;
}

bool FudgetScrollBarComponent::MouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
    if (!_visible || (!MouseIsCaptured() && (_owner->MouseIsCaptured() || !_bounds.Contains(pos))))
        return false;
    if (button == MouseButton::Left)
    {
        MouseCapture old_capture = _mouse_capture;

        _mouse_capture = MouseCapture::None;

        if (_event_owner != nullptr && old_capture != _mouse_capture)
        {
            if (old_capture >= MouseCapture::Button1 && old_capture <= MouseCapture::Button20)
            {
                int btn_ix = (int)old_capture - (int)MouseCapture::Button1;
                _event_owner->OnScrollBarButtonReleased(this, btn_ix, btn_ix < _painter->GetBeforeButtonCount());
            }
            else if (old_capture == MouseCapture::Thumb)
            {
                _event_owner->OnScrollBarThumbReleased(this);
            }
            else if (old_capture == MouseCapture::BeforeTrack)
            {
                _event_owner->OnScrollBarTrackReleased(this, true);
            }
            else if (old_capture == MouseCapture::AfterTrack)
            {
                _event_owner->OnScrollBarTrackReleased(this, false);
            }
        }

    }
    return MouseIsCaptured();
}

bool FudgetScrollBarComponent::MouseLeave()
{
    _old_mouse_pos = Float2(-1.f);
    return false;
}

int FudgetScrollBarComponent::GetBeforeTrackRole() const
{
    if (_painter != nullptr)
        return _painter->GetBeforeTrackRole();
    return 0;
}

int FudgetScrollBarComponent::GetAfterTrackRole() const
{
    if (_painter != nullptr)
        return _painter->GetAfterTrackRole();
    return 0;
}

int FudgetScrollBarComponent::GetButtonRole(int button_index) const
{
    if (_painter != nullptr)
        return _painter->GetButtonRole(button_index);
    return 0;
}

void FudgetScrollBarComponent::HandleRole(int role)
{
    if (role <= 0)
        return;
    switch (role)
    {
        case (int)FudgetScrollBarButtonRole::LineUp:
            SetScrollPos(_scroll_pos - _line_size);
            break;
        case (int)FudgetScrollBarButtonRole::LineDown:
            SetScrollPos(_scroll_pos + _line_size);
            break;
        case (int)FudgetScrollBarButtonRole::PageUp:
            SetScrollPos(_scroll_pos - _page_size);
            break;
        case (int)FudgetScrollBarButtonRole::PageDown:
            SetScrollPos(_scroll_pos + _page_size);
            break;
        case (int)FudgetScrollBarButtonRole::PageUpLine:
            SetScrollPos(_scroll_pos - Math::Max(0LL, _page_size - _line_size));
            break;
        case (int)FudgetScrollBarButtonRole::PageDownLine:
            SetScrollPos(_scroll_pos + Math::Max(0LL, _page_size - _line_size));
            break;
        case (int)FudgetScrollBarButtonRole::JumpToStart:
            SetScrollPos(0LL);
            break;
        case (int)FudgetScrollBarButtonRole::JumpToEnd:
            SetScrollPos(_range_max + 1 - _page_size);
            break;
    }
}

void FudgetScrollBarComponent::UpdateVisibility()
{
    if (_visibility_mode == FudgetScrollBarVisibilityMode::Visible)
    {
        if (!_visible)
        {
            _visible = true;
            if (_event_owner != nullptr)
                _event_owner->OnScrollBarShown(this);
            _rects_dirty = true;
        }
        return;
    }
    else if (_visibility_mode == FudgetScrollBarVisibilityMode::Hidden)
    {
        if (_visible)
        {
            _visible = false;
            if (_event_owner != nullptr)
                _event_owner->OnScrollBarHidden(this);
            _rects_dirty = true;
        }
        return;
    }

    bool new_visible = _range_max - _range_min + 1 > _page_size;
    if (new_visible != _visible)
    {
        _visible = new_visible;
        if (_event_owner != nullptr)
        {
            if (_visible)
                _event_owner->OnScrollBarShown(this);
            else
                _event_owner->OnScrollBarHidden(this);
        }
        _rects_dirty = true;
    }
}

void FudgetScrollBarComponent::RecalculateRectangles()
{
    if (_painter == nullptr || _rects_dirty == false)
        return;
    int btn_cnt = _painter->GetButtonCount();
    _painter->GetPartBounds(_owner, _bounds, _range_max - _range_min + 1, _page_size, _scroll_pos, _track_rect, _before_track_rect, _after_track_rect, _thumb_rect, _btn_rects);

    _rects_dirty = false;
}

