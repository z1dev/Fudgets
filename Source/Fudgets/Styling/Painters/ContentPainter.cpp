#include "ContentPainter.h"
#include "../../Control.h"
#include "../DrawableBuilder.h"


FudgetContentPainter::FudgetContentPainter(const SpawnParams &params) : Base(params), _drawable(nullptr), _offset(Float2::Zero)
{
}

void FudgetContentPainter::Initialize(FudgetControl *control,  const Variant &mapping)
{
    Mapping res = *mapping.AsStructure<Mapping>();

    if (!CreateMappedDrawable(control, res.Drawable, _drawable))
        _drawable = FudgetDrawable::Empty;
    if (!GetMappedPadding(control, res.Margin, _margin))
        _margin = FudgetPadding(0);
    if (!GetMappedFloat2(control, res.Offset, _offset))
        _offset = Float2::Zero;
    if (!GetMappedDrawColors(control, res.Tint, _tint))
        _tint = FudgetDrawColors();
}

void FudgetContentPainter::Draw(FudgetControl *control, const Rectangle &bounds, uint64 states)
{
    Rectangle r = _margin.Padded(bounds);
    r.Location += _offset;

    control->DrawDrawable(_drawable, _drawable->FindMatchingState(states), r, _tint.FindMatchingColor(states));

}
