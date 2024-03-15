#include "AlignedImagePainter.h"
#include "../../Control.h"



FudgetAlignedImagePainter::FudgetAlignedImagePainter(const SpawnParams &params) : Base(params),
    _horz_align(FudgetImageHorzAlign::Stretch), _vert_align(FudgetImageVertAlign::Stretch)
{
}

void FudgetAlignedImagePainter::Initialize(FudgetControl *control,  const Variant &mapping)
{
    Mapping res = *mapping.AsStructure<Mapping>();

    if (!GetMappedTexture(control, res.Image, _image))
        _image = nullptr;
    if (!GetMappedDrawColors(control, res.Tint, _tint))
        _tint = FudgetDrawColors();
    if (!GetMappedFloat2(control, res.Tint, _offset))
        _offset = Float2::Zero;
    if (!GetMappedPadding(control, res.Padding, _padding))
        _padding = FudgetPadding(0);

    if (!GetMappedEnum<FudgetImageHorzAlign>(control, res.HorzAlign, _horz_align))
        _horz_align = FudgetImageHorzAlign::Stretch;
    if (!GetMappedEnum<FudgetImageVertAlign>(control, res.VertAlign, _vert_align))
        _vert_align = FudgetImageVertAlign::Stretch;
}

void FudgetAlignedImagePainter::Draw(FudgetControl *control, const Rectangle &bounds, uint64 states)
{

    TextureBase *t = _image.Get();
    if (t == nullptr)
        return;

    Rectangle r = _padding.Padded(bounds);
    r.Location += _offset;

    if (_horz_align != FudgetImageHorzAlign::Stretch)
    {
        float w = (float)t->Width();
        if (_horz_align == FudgetImageHorzAlign::Right)
            r.Location.X = r.Location.X + (r.Size.X - w);
        else if (_horz_align == FudgetImageHorzAlign::Center)
            r.Location.X = r.Location.X + (r.Size.X - w) * 0.5f;
        r.Size.X = w;
    }

    if (_vert_align != FudgetImageVertAlign::Stretch)
    {
        float h = (float)t->Height();
        if (_vert_align == FudgetImageVertAlign::Bottom)
            r.Location.Y = r.Location.Y + (r.Size.Y - h);
        else if (_vert_align == FudgetImageVertAlign::Center)
            r.Location.Y = r.Location.Y + (r.Size.Y - h) * 0.5f;
        r.Size.Y = h;
    }

    control->DrawTexture(t, r, _tint.FindMatchingColor(states));

}
