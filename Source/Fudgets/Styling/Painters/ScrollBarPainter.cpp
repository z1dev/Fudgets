#include "ScrollBarPainter.h"
#include "../DrawableBuilder.h"
#include "../../Control.h"


// FudgetScrollBarPainter

FudgetScrollBarPainter::FudgetScrollBarPainter(const SpawnParams &params) : Base(params), _orientation(FudgetScrollBarOrientation::Horizontal),
    _width(0), _thumb_size_fixed(false), _thumb_size(0), _min_thumb_size(0), _frame_draw(nullptr), _thumb_draw(nullptr), _track_draw(nullptr),
    _before_track_draw(nullptr), _after_track_draw(nullptr), _before_btn_count(0), _after_btn_count(0)
{
    for (int ix = 0; ix < 20; ++ix)
        _btn_draw[ix] = FudgetDrawable::Empty;
}

void FudgetScrollBarPainter::Initialize(FudgetControl *control, const Variant &mapping)
{
    Mapping res = *mapping.AsStructure<Mapping>();

    _orientation = res.Orientation;
    if (!GetMappedInt(control, res.Width, _width))
        _width = 0;
    if (!GetMappedBool(control, res.IsThumbSizeFixed, _thumb_size_fixed))
        _thumb_size_fixed = false;
    if (!GetMappedInt(control, res.ThumbSize, _thumb_size))
        _thumb_size = 24;
    _thumb_size = Math::Max(1, _thumb_size);
    if (!GetMappedInt(control, res.MinThumbSize, _min_thumb_size))
        _min_thumb_size = 8;
    _min_thumb_size = Math::Max(1, _min_thumb_size);

    if (!CreateMappedDrawable(control, res.BgDraw, _bg_draw))
        _bg_draw = FudgetDrawable::Empty;
    if (!CreateMappedDrawable(control, res.FrameDraw, _frame_draw))
        _frame_draw = FudgetDrawable::Empty;
    if (!CreateMappedDrawable(control, res.ThumbDraw, _thumb_draw))
        _thumb_draw = FudgetDrawable::Empty;

    if (!CreateMappedDrawable(control, res.TrackDraw, _track_draw))
        _track_draw = FudgetDrawable::Empty;
    if (!CreateMappedDrawable(control, res.BeforeTrackDraw, _before_track_draw))
        _before_track_draw = FudgetDrawable::Empty;
    if (!CreateMappedDrawable(control, res.AfterTrackDraw, _after_track_draw))
        _after_track_draw = FudgetDrawable::Empty;

    if (!GetMappedInt(control, res.BeforeTrackButtonCount, _before_btn_count))
        _before_btn_count = 0;
    _before_btn_count = Math::Clamp(_before_btn_count, 0, 20);
    if (!GetMappedInt(control, res.AfterTrackButtonCount, _after_btn_count))
        _after_btn_count = 0;
    _after_btn_count = Math::Clamp(_after_btn_count, 0, 20);

    if (!GetMappedInt(control, res.BeforeTrackRole, _before_track_role))
        _before_track_role = 0;
    if (!GetMappedInt(control, res.AfterTrackRole, _after_track_role))
        _after_track_role = 0;

    for (int ix = 0, siz = _before_btn_count + _after_btn_count; ix < siz; ++ix)
    {
        if (!CreateMappedDrawable(control, res.ButtonDraw[ix], _btn_draw[ix]))
            _btn_draw[ix] = FudgetDrawable::Empty;
        if (!GetMappedInt(control, res.ButtonSize[ix], _btn_size[ix]))
            _btn_size[ix] = 0;
        if (!GetMappedInt(control, res.ButtonRole[ix], _btn_role[ix]))
            _btn_role[ix] = 0;
    }
}

int FudgetScrollBarPainter::GetBeforeTrackRole() const
{
    return _before_track_role;
}

int FudgetScrollBarPainter::GetAfterTrackRole() const
{
    return _after_track_role;
}

int FudgetScrollBarPainter::GetButtonRole(int button_index) const
{
    return _btn_role[button_index];
}

void FudgetScrollBarPainter::GetPartBounds(FudgetControl *control, const Rectangle &bounds, int64 range, int64 page_size, int64 thumb_pos, API_PARAM(Out) Rectangle &track, API_PARAM(Out) Rectangle &before_track, API_PARAM(Out) Rectangle &after_track,
    API_PARAM(Out) Rectangle &thumb_button, API_PARAM(Out) Array<Rectangle> &buttons) const
{
    if (buttons.Count() < _before_btn_count + _after_btn_count)
        buttons.Resize(_before_btn_count + _after_btn_count);
    GetPartBounds(control, bounds, range, page_size, thumb_pos, track, before_track, after_track, thumb_button, buttons.Get());
}

#pragma warning(disable:6385)
void FudgetScrollBarPainter::GetPartBounds(FudgetControl *control, const Rectangle &bounds, int64 range, int64 page_size, int64 thumb_pos,
    Rectangle &track, Rectangle &before_track, Rectangle &after_track, Rectangle &thumb_button, Rectangle *buttons) const
{
    thumb_pos = Math::Clamp(thumb_pos, (int64)0, Math::Max(0LL, range - page_size));

    before_track = Rectangle::Empty;
    after_track = Rectangle::Empty; 
    thumb_button = Rectangle::Empty;
    int btn_cnt = _before_btn_count + _after_btn_count;
    for (int ix = 0; ix < btn_cnt; ++ix)
        buttons[ix] = Rectangle::Empty;
    int size = (int)Relevant(bounds.Size);

    if ((!_thumb_size_fixed && size <= _min_thumb_size) || (_thumb_size_fixed && size <= _thumb_size))
    {
        thumb_button = bounds;
        return;
    }

    // Buttons are hidden if they don't fit with the minimum thumb size. A button before and after the track are shown equally. Outer buttons are hidden first.
    int available = Math::Max(0, size - (_thumb_size_fixed ? _thumb_size : _min_thumb_size));

    int cnt_before = 0;
    int cnt_after = 0;
    int pos_before = _before_btn_count - 1;
    int pos_after = _before_btn_count;
    int siz_before = 0;
    int siz_after = 0;

    while (pos_before >= 0 || pos_after < btn_cnt)
    {
        if (pos_before >= 0 && pos_before < 20)
        {
            if (available - _btn_size[pos_before] < 0)
                break;

            ++cnt_before;
            siz_before += _btn_size[pos_before];
            available -= _btn_size[pos_before];
            --pos_before;
        }

        if (pos_after < btn_cnt)
        {
            if (available - _btn_size[pos_after] < 0)
                break;

            ++cnt_after;
            siz_after += _btn_size[pos_after];
            available -= _btn_size[pos_after];
            ++pos_after;
        }
    }

    pos_before = _before_btn_count - cnt_before;
    pos_after = _before_btn_count + cnt_after - 1;

    Float2 bpos = bounds.GetUpperLeft();
    Float2 bsize;
    OppositeRef(bsize) = Opposite(bounds.Size);
    for (; pos_before < cnt_before; ++pos_before)
    {
        RelevantRef(bsize) = (float)_btn_size[pos_before];
        buttons[pos_before] = Rectangle(bpos, bsize);
        RelevantRef(bpos) += _btn_size[pos_before];
    }

    bpos = bounds.GetBottomRight();
    OppositeRef(bpos) -= Opposite(bounds.Size);
    for (; pos_after >= _before_btn_count; --pos_after)
    {
        RelevantRef(bsize) = (float)_btn_size[pos_after];
        RelevantRef(bpos) -= _btn_size[pos_after];
        buttons[pos_after] = Rectangle(bpos, bsize);
    }

    size -= siz_before + siz_after;
    int thumb_siz = Math::Min(size, _thumb_size_fixed ? Math::Min(size, _thumb_size) :
        range * size == 0 ? Math::Max(_min_thumb_size, int(page_size)) : Math::Max(_min_thumb_size, int(page_size / (float)range * size)));

    bpos = bounds.GetUpperLeft();
    RelevantRef(bpos) += siz_before;
    RelevantRef(bsize) = (float)size;
    track = Rectangle(bpos, bsize);
    int before_siz = range <= page_size ? 0 : int((size - thumb_siz) * (thumb_pos / float(range - page_size)));
    RelevantRef(bsize) = (float)before_siz;
    before_track = Rectangle(bpos, bsize);
    RelevantRef(bpos) += before_siz;
    RelevantRef(bsize) = (float)thumb_siz;
    thumb_button = Rectangle(bpos, bsize);
    RelevantRef(bpos) += thumb_siz;
    RelevantRef(bsize) = (float)(size - (thumb_siz + before_siz));
    after_track = Rectangle(bpos, bsize);
}

void FudgetScrollBarPainter::DrawBackground(FudgetControl *control, const Rectangle &bounds, const Rectangle &track_bounds, uint64 states)
{
    control->DrawDrawable(_bg_draw, _bg_draw->FindMatchingState(states), bounds);
    control->DrawDrawable(_track_draw, _track_draw->FindMatchingState(states), track_bounds);
}

void FudgetScrollBarPainter::DrawFrame(FudgetControl *control, const Rectangle &bounds, uint64 states)
{
    control->DrawDrawable(_frame_draw, _frame_draw->FindMatchingState(states), bounds);
}

void FudgetScrollBarPainter::DrawTrack(FudgetControl *control, const Rectangle &before_bounds, const Rectangle &after_bounds, uint64 before_states, uint64 after_states)
{
    control->DrawDrawable(_before_track_draw, _before_track_draw->FindMatchingState(before_states), before_bounds);
    control->DrawDrawable(_after_track_draw, _after_track_draw->FindMatchingState(after_states), before_bounds);
}

void FudgetScrollBarPainter::DrawThumb(FudgetControl *control, const Rectangle &bounds, uint64 states)
{
    control->DrawDrawable(_thumb_draw, _thumb_draw->FindMatchingState(states), bounds);
}

void FudgetScrollBarPainter::DrawButton(FudgetControl *control, int button_index, const Rectangle &bounds, uint64 states)
{
    control->DrawDrawable(_btn_draw[button_index], _btn_draw[button_index]->FindMatchingState(states), bounds);
}
