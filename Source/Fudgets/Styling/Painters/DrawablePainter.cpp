#include "DrawablePainter.h"
#include "../../Control.h"
#include "../DrawableBuilder.h"
#include "../PartPainterIds.h"

// FudgetFrameFieldPainter


FudgetDrawablePainter::FudgetDrawablePainter(const SpawnParams &params) : Base(params), _drawable(nullptr)
{
}

void FudgetDrawablePainter::Initialize(FudgetControl *control, const Variant &mapping)
{
    Mapping res = *mapping.AsStructure<Mapping>();

    if (!CreateMappedDrawable(control, res.Drawable, _drawable))
        _drawable = FudgetDrawable::Empty;
    if (!GetMappedPadding(control, res.Margin, _margin))
        _margin = FudgetPadding(0);
    if (!GetMappedDrawColors(control, res.Tint, _tint))
        _tint = FudgetDrawColors();

    if (!GetMappedPadding(control, res.Padding, _padding))
        _padding = FudgetPadding(0);
}

void FudgetDrawablePainter::Draw(FudgetControl *control, const Rectangle &bounds, uint64 states)
{
    control->DrawDrawable(_drawable, _drawable->FindMatchingState(states), _margin.Padded(bounds), _tint.FindMatchingColor(states));
}

