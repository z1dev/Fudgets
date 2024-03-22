#include "ScrollingControl.h"

FudgetScrollingControl::FudgetScrollingControl(const SpawnParams &params) : Base(params), _scrollbars(FudgetScrollBars::None),
    _h_scrollbar(nullptr), _v_scrollbar(nullptr), _dirty_extents(true)
{
}

FudgetScrollingControl::~FudgetScrollingControl()
{
    if (_v_scrollbar != nullptr)
        Delete(_v_scrollbar);
    if (_h_scrollbar != nullptr)
        Delete(_h_scrollbar);
}

void FudgetScrollingControl::OnStyleInitialize()
{
    if (_v_scrollbar != nullptr)
        _v_scrollbar->StyleInitialize();
    if (_h_scrollbar != nullptr)
        _h_scrollbar->StyleInitialize();
}

void FudgetScrollingControl::OnDraw()
{
    if (_dirty_extents)
        RequestLayout();

    Base::OnDraw();

    if (_v_scrollbar != nullptr)
        _v_scrollbar->Draw();
    if (_h_scrollbar != nullptr)
        _h_scrollbar->Draw();
}

FudgetInputResult FudgetScrollingControl::DoMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
    if (_v_scrollbar != nullptr && _v_scrollbar->MouseDown(pos, global_pos, button, double_click))
        return FudgetInputResult::Consume;
    if (_h_scrollbar != nullptr && _h_scrollbar->MouseDown(pos, global_pos, button, double_click))
        return FudgetInputResult::Consume;
    return Base::DoMouseDown(pos, global_pos, button, double_click);
}

bool FudgetScrollingControl::DoMouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
    if (_v_scrollbar != nullptr && _v_scrollbar->MouseUp(pos, global_pos, button))
        return true;
    if (_h_scrollbar != nullptr && _h_scrollbar->MouseUp(pos, global_pos, button))
        return true;

    return Base::DoMouseUp(pos, global_pos, button);
}

void FudgetScrollingControl::DoMouseMove(Float2 pos, Float2 global_pos)
{
    if (_v_scrollbar != nullptr && _v_scrollbar->MouseMove(pos, global_pos))
        return;
    if (_h_scrollbar != nullptr && _h_scrollbar->MouseMove(pos, global_pos))
        return;

    Base::DoMouseMove(pos, global_pos);
}

void FudgetScrollingControl::DoMouseLeave()
{
    if (_v_scrollbar != nullptr && _v_scrollbar->MouseLeave())
        return;
    if (_h_scrollbar != nullptr && _h_scrollbar->MouseLeave())
        return;
    Base::DoMouseLeave();
}

void FudgetScrollingControl::SetScrollBars(FudgetScrollBars value)
{
    if (_scrollbars == value)
        return;

    if (_v_scrollbar != nullptr && value != FudgetScrollBars::Vertical && value != FudgetScrollBars::Both)
    {
        Delete(_v_scrollbar);
        _v_scrollbar = nullptr;
    }
    if (_h_scrollbar != nullptr && value != FudgetScrollBars::Horizontal && value != FudgetScrollBars::Both)
    {
        Delete(_h_scrollbar);
        _h_scrollbar = nullptr;
    }
    _scrollbars = value;
    ClearStyleCache();
    if (_v_scrollbar == nullptr && (_scrollbars == FudgetScrollBars::Both || _scrollbars == FudgetScrollBars::Vertical))
    {
        _v_scrollbar = New<FudgetScrollBarComponent>(SpawnParams(Guid::New(), FudgetScrollBarComponent::TypeInitializer));
        _v_scrollbar->Initialize(this, this, FudgetScrollBarOrientation::Vertical);
    }
    if (_h_scrollbar == nullptr && (_scrollbars == FudgetScrollBars::Both || _scrollbars == FudgetScrollBars::Horizontal))
    {
        _h_scrollbar = New<FudgetScrollBarComponent>(SpawnParams(Guid::New(), FudgetScrollBarComponent::TypeInitializer));
        _h_scrollbar->Initialize(this, this, FudgetScrollBarOrientation::Horizontal);
    }
}

void FudgetScrollingControl::OnScrollBarScroll(FudgetScrollBarComponent *scrollbar, int64 old_scroll_pos, bool tracking)
{
}

void FudgetScrollingControl::OnScrollBarButtonPressed(FudgetScrollBarComponent *scrollbar, int button_index, bool before_track, bool double_click)
{
}

void FudgetScrollingControl::OnScrollBarButtonReleased(FudgetScrollBarComponent *scrollbar, int button_index, bool before_track)
{

}

void FudgetScrollingControl::OnScrollBarTrackPressed(FudgetScrollBarComponent *scrollbar, bool before_track, bool double_click)
{
    //if (scrollbar == _v_scrollbar)
    //{
    //    if (before_track)
    //        _v_scrollbar->SetScrollPos(_v_scrollbar->GetScrollPos() - _v_scrollbar->GetPageSize());
    //    else
    //        _v_scrollbar->SetScrollPos(_v_scrollbar->GetScrollPos() + _v_scrollbar->GetPageSize());
    //}
    //else if (scrollbar == _h_scrollbar)
    //{
    //    if (before_track)
    //        _h_scrollbar->SetScrollPos(_h_scrollbar->GetScrollPos() - _h_scrollbar->GetPageSize());
    //    else
    //        _h_scrollbar->SetScrollPos(_h_scrollbar->GetScrollPos() + _h_scrollbar->GetPageSize());
    //}
}

void FudgetScrollingControl::OnScrollBarTrackReleased(FudgetScrollBarComponent *scrollbar, bool before_track)
{

}

void FudgetScrollingControl::OnScrollBarThumbPressed(FudgetScrollBarComponent *scrollbar, bool double_click)
{
}

void FudgetScrollingControl::OnScrollBarThumbReleased(FudgetScrollBarComponent *scrollbar)
{

}

bool FudgetScrollingControl::OnRole(FudgetScrollBarComponent *scrollbar, int role)
{
    return false;
}

void FudgetScrollingControl::OnScrollBarShown(FudgetScrollBarComponent *scrollbar)
{
}

void FudgetScrollingControl::OnScrollBarHidden(FudgetScrollBarComponent *scrollbar)
{
}

Int2 FudgetScrollingControl::GetScrollBarWidths() const
{
    return Int2(_v_scrollbar != nullptr && _v_scrollbar->IsVisible() ? _v_scrollbar->GetWidth() : 0,
        _h_scrollbar != nullptr && _h_scrollbar->IsVisible() ? _h_scrollbar->GetWidth() : 0);
}

Int2 FudgetScrollingControl::GetScrollBarWidths(bool show_horizontal, bool show_vertical) const
{
    return Int2(_v_scrollbar != nullptr && _v_scrollbar->GetAssumedVisible(show_vertical) ? _v_scrollbar->GetWidth() : 0,
        _h_scrollbar != nullptr && _h_scrollbar->GetAssumedVisible(show_horizontal) ? _h_scrollbar->GetWidth() : 0);
}

FudgetPadding FudgetScrollingControl::GetFramePadding() const
{
    FudgetPadding result = Base::GetFramePadding();
    Int2 w = GetScrollBarWidths();
    result.Right += w.X;
    result.Bottom += w.Y;
    return result;
}

void FudgetScrollingControl::RequestLayout()
{
    if (!_dirty_extents)
    {
        Base::RequestLayout();
        return;
    }

    RequestScrollExtents();

    Int2 widths = GetScrollBarWidths();

    Rectangle bounds = Base::GetFramePadding().Padded(GetBounds());
    if (_v_scrollbar != nullptr && _v_scrollbar->IsVisible())
    {
        _v_scrollbar->SetBounds(Rectangle(bounds.GetUpperRight() - Float2((float)widths.X, 0.f), Float2((float)widths.X, bounds.Size.Y - (float)widths.Y) ));
    }
    if (_h_scrollbar != nullptr && _h_scrollbar->IsVisible())
    {
        _h_scrollbar->SetBounds(Rectangle(bounds.GetBottomLeft() - Float2(0.f, (float)widths.Y), Float2(bounds.Size.X - (float)widths.X, (float)widths.Y)));
    }

    _dirty_extents = false;
    Base::RequestLayout();
}
