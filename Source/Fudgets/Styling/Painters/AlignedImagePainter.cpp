#include "AlignedImagePainter.h"
#include "../../Control.h"



FudgetAlignedImagePainter::FudgetAlignedImagePainter(const SpawnParams &params) : Base(params),
    _horz_align(FudgetImageHorzAlign::Stretch), _vert_align(FudgetImageVertAlign::Stretch)
{
}

void FudgetAlignedImagePainter::Initialize(FudgetControl *control,  const Variant &mapping)
{
    Mapping res = *mapping.AsStructure<Mapping>();

    Array<DrawMapping> mappings;
    HashSet<uint64> states;
    for (const FudgetAlignedImageMapping &mapping : res.Mappings)
    {
        DrawMapping new_mapping;

        // Duplicate states are not allowed
        if (states.Contains(mapping.State))
            continue;
        states.Add(mapping.State);
        new_mapping._state = mapping.State;

        if (!GetMappedTexture(control, mapping.Image, new_mapping._image))
            new_mapping._image = nullptr;
        if (!GetMappedColor(control, mapping.Tint, new_mapping._tint))
            new_mapping._tint = Color::White;
        if (!GetMappedFloat2(control, mapping.Tint, new_mapping._offset))
            new_mapping._offset = Float2::Zero;
        if (!GetMappedPadding(control, mapping.Padding, new_mapping._padding))
            new_mapping._padding = FudgetPadding(0);
        mappings.Add(new_mapping);
    }
    _mappings.Add(mappings);

    if (!GetMappedEnum<FudgetImageHorzAlign>(control, res.HorzAlign, _horz_align))
        _horz_align = FudgetImageHorzAlign::Stretch;
    if (!GetMappedEnum<FudgetImageVertAlign>(control, res.VertAlign, _vert_align))
        _vert_align = FudgetImageVertAlign::Stretch;
}

void FudgetAlignedImagePainter::Draw(FudgetControl *control, const Rectangle &bounds, FudgetVisualControlState states)
{
    uint64 matched_state = GetMatchingState(states); 

    bool failed = false;
    for (auto &mapping : _mappings)
    {
        if (mapping._state == 0 || (!failed && matched_state != 0 && (mapping._state & matched_state) == matched_state))
        {
            TextureBase *t = mapping._image.Get();
            if (t != nullptr)
            {
                Rectangle r = mapping._padding.Padded(bounds);
                r.Location += mapping._offset;

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

                control->DrawTexture(t, r, mapping._tint);
                break;
            }
            else
            {
                if (mapping._state == 0)
                    break;

                failed = true;
            }
        }
    }

}
