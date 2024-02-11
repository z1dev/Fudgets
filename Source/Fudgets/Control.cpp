#include "Control.h"
#include "Container.h"
#include "GUIRoot.h"
#include "Fudget.h"
#include "MarginStructs.h"
#include "Styling/Themes.h"
#include "Styling/StyleStructs.h"
#include "Styling/PartPainters.h"

#include "Engine/Render2D/Render2D.h"
#include "Engine/Render2D/TextLayoutOptions.h"
#include "Engine/Core/Math/Rectangle.h"
#include "Engine/Scripting/Scripting.h"
#include "Engine/Core/Log.h"
#include "Engine/Scripting/ManagedCLR/MClass.h"
#include "Engine/Scripting/Types.h"
#include "Engine/Render2D/SpriteAtlas.h"


FudgetControl::FudgetControl(const SpawnParams &params) : ScriptingObject(params),
    _guiRoot(nullptr), _parent(nullptr), _index(-1), _flags(FudgetControlFlags::ResetFlags), _pos(0.f), _size(0.0f),
    _hint_size(120.f, 60.0f), _min_size(30.f, 30.f), _max_size(-1.f, -1.f), _state_flags(FudgetControlState::Enabled),
    _cached_global_to_local_translation(0.f), _clipping_count(0), _changing(false), _style(nullptr), _cached_style(nullptr),
    _theme_id(FudgetToken::Invalid), _cached_theme(nullptr)
{
}

FudgetControl::~FudgetControl()
{
    RegisterToUpdate(false);
    _painters.ClearDelete();
}

void FudgetControl::SetParent(FudgetContainer *value)
{
    SetParent(value, -1);
}

void FudgetControl::SetParent(FudgetContainer *value, int order)
{
    if (_parent == value || _changing)
        return;

    _changing = true;
    if (_parent != nullptr)
        _parent->RemoveChild(_index);
    if (value != nullptr)
        value->AddChild(this, order);
    if (_parent == nullptr)
        RegisterToUpdate(false);
    _changing = false;
}

void FudgetControl::SetIndexInParent(int value)
{
    if (_changing || _parent == nullptr || value == _index || value < 0 || value >= _parent->GetChildCount())
        return;
    _changing = true;
    _parent->MoveChildToIndex(_index, value);
    _changing = false;
}

void FudgetControl::SetName(String value)
{
    // TODO: event on name change?
    _name = value;
}

bool FudgetControl::IsInRunningGame() const
{
    FudgetGUIRoot *gui_root = GetGUIRoot();
    if (gui_root == nullptr)
        return false;
    Fudget *root = gui_root->GetRoot();
    if (root == nullptr)
        return false;
    return root->IsInRunningGame();
}

void FudgetControl::SetHintSize(Float2 value)
{
    if (Float2::NearEqual(_hint_size, value))
        return;
    _hint_size = value;
    SizeOrPosModified(FudgetDirtType::Size);
}

void FudgetControl::SetMinSize(Float2 value)
{
    if (Float2::NearEqual(_min_size, value))
        return;
    _min_size = value;
    SizeOrPosModified(FudgetDirtType::Size);
}

void FudgetControl::SetMaxSize(Float2 value)
{
    if (Float2::NearEqual(_max_size, value))
        return;
    _max_size = value;
    SizeOrPosModified(FudgetDirtType::Size);
}

Float2 FudgetControl::GetRequestedSize(FudgetSizeType type) const
{
    switch (type)
    {
        case FudgetSizeType::Hint:
            return GetHintSize();
        case FudgetSizeType::Min:
            return GetMinSize();
        case FudgetSizeType::Max:
            return GetMaxSize();
        default:
            return Float2(0.f);
    }
}

Float2 FudgetControl::GetSize() const
{
    if (_parent == nullptr)
        return _hint_size;
    _parent->RequestLayout();
    return _size;
}

Float2 FudgetControl::GetPosition() const
{
    if (_parent != nullptr)
        _parent->RequestLayout();
    return _pos;
}

void FudgetControl::SetPosition(Float2 value)
{
    if (Float2::NearEqual(_pos, value) || !IsPositionChangePermitted())
        return;

    _pos = value;
    SizeOrPosModified(FudgetDirtType::Position);
}

void FudgetControl::RegisterToUpdate(bool value)
{
    if (value == HasAnyState(FudgetControlState::Updating) || (_parent == nullptr && value ))
        return;

    auto gui = GetGUIRoot();
    if (gui == nullptr)
        return;

    if (gui->RegisterControlUpdate(this, value))
        SetState(FudgetControlState::Updating, value);
}

Float2 FudgetControl::LocalToGlobal(Float2 local) const
{
    FudgetContainer *parent = _parent;
    local += GetPosition();

    while (parent != nullptr)
    {
        local += parent->GetPosition();
        parent = parent->_parent;
    }

    return local;
}

Float2 FudgetControl::GlobalToLocal(Float2 global) const
{
    FudgetContainer *parent = _parent;
    global -= GetPosition();

    while (parent != nullptr)
    {
        global -= parent->GetPosition();
        parent = parent->_parent;
    }

    return global;
}

Rectangle FudgetControl::LocalToGlobal(const Rectangle &local) const
{
    FudgetContainer *parent = _parent;
    Float2 pos = local.Location;
    pos += GetPosition();

    while (parent != nullptr)
    {
        pos += parent->GetPosition();
        parent = parent->_parent;
    }

    return Rectangle(pos, local.Size);
}

Rectangle FudgetControl::GlobalToLocal(const Rectangle &global) const
{
    FudgetContainer *parent = _parent;
    Float2 pos = global.Location;
    pos -= GetPosition();

    while (parent != nullptr)
    {
        pos -= parent->GetPosition();
        parent = parent->_parent;
    }

    return Rectangle(pos, global.Size);
}

void FudgetControl::CacheGlobalToLocal()
{
    if (HasAnyState(FudgetControlState::Global2LocalCached))
        return;
    SetState(FudgetControlState::Global2LocalCached, true);
    _cached_global_to_local_translation = GlobalToLocal(Float2(0.f));
}

void FudgetControl::InvalidateGlobalToLocalCache()
{
    SetState(FudgetControlState::Global2LocalCached, false);
}

Float2 FudgetControl::CachedLocalToGlobal(Float2 local) const
{
    return local - _cached_global_to_local_translation;
}

Float2 FudgetControl::CachedGlobalToLocal(Float2 global) const
{
    return global + _cached_global_to_local_translation;
}

Rectangle FudgetControl::CachedLocalToGlobal(const Rectangle &local) const
{
    return Rectangle(local.Location - _cached_global_to_local_translation, local.Size);
}

Rectangle FudgetControl::CachedGlobalToLocal(const Rectangle &global) const
{
    return Rectangle(global.Location + _cached_global_to_local_translation, global.Size);
}

FudgetControlFlags FudgetControl::GetControlFlags() const
{
    return _flags;
}

void FudgetControl::SetControlFlags(FudgetControlFlags flags)
{
    FudgetControlFlags tmp = _flags;
    _flags = flags;

    if ((_flags & FudgetControlFlags::RegisterToUpdates) != (tmp & FudgetControlFlags::RegisterToUpdates))
    {
        RegisterToUpdate(_parent != nullptr && ((_flags & FudgetControlFlags::RegisterToUpdates) == FudgetControlFlags::RegisterToUpdates));
    }

    if (_parent == _guiRoot && _parent != nullptr && (_flags & FudgetControlFlags::AlwaysOnTop) != (tmp & FudgetControlFlags::AlwaysOnTop))
    {
        if (_guiRoot->ChangeControlAlwaysOnTop(this, (_flags & FudgetControlFlags::AlwaysOnTop) == FudgetControlFlags::AlwaysOnTop) == -1)
            _flags |= (tmp & FudgetControlFlags::AlwaysOnTop);
    }

    // Might not need recalculation but we can't be sure.
    SizeOrPosModified(FudgetDirtType::All);
}

bool FudgetControl::HasAllFlags(FudgetControlFlags flags) const
{
    return (flags & _flags) == flags;
}

bool FudgetControl::HasAnyFlag(FudgetControlFlags flags) const
{
    return (int)(flags & _flags) != 0;
}

void FudgetControl::SizeOrPosModified(FudgetDirtType dirt_flags)
{
    if (_parent != nullptr)
        _parent->MarkLayoutDirty(dirt_flags, true);
}

bool FudgetControl::GetAlwaysOnTop() const
{
    return _parent == _guiRoot && _parent != nullptr && (_flags & FudgetControlFlags::AlwaysOnTop) == FudgetControlFlags::AlwaysOnTop;
}

void FudgetControl::SetAlwaysOnTop(bool value)
{
    if (_parent != _guiRoot || _parent == nullptr || GetAlwaysOnTop() == value)
        return;
    _changing = true;
    _guiRoot->ChangeControlAlwaysOnTop(this, value);
    _changing = false;
}

void FudgetControl::CaptureMouseInput()
{
    auto root = GetGUIRoot();
    if (root == nullptr)
        return;

    root->StartMouseCapture(this);
}

void FudgetControl::ReleaseMouseInput()
{
    auto root = GetGUIRoot();
    if (root == nullptr || root->GetMouseCaptureControl() != this)
        return;

    root->ReleaseMouseCapture();
}

bool FudgetControl::MouseIsCaptured() const
{
    return HasAnyState(FudgetControlState::MouseIsCaptured);
}

bool FudgetControl::GetFocused() const
{
    auto root = GetGUIRoot();
    if (root == nullptr)
        return false;
    return root->GetFocusedControl() == this;
}

bool FudgetControl::VirtuallyFocused() const
{
    return HasAnyState(FudgetControlState::ShowFocused);
}

void FudgetControl::SetFocused(bool value)
{
    auto root = GetGUIRoot();
    if (root == nullptr)
        return;
    if (value)
        root->SetFocusedControl(this);
    if (!value && root->GetFocusedControl() == this)
        root->SetFocusedControl(nullptr);
}

bool FudgetControl::GetHovered() const
{
    auto root = GetGUIRoot();
    if (root == nullptr)
        return false;
    return root->GetHoveredControl() == this;
}

bool FudgetControl::VirtuallyHovered() const
{
    return HasAnyState(FudgetControlState::ShowHovered);
}

bool FudgetControl::GetEnabled() const
{
    return (_state_flags & FudgetControlState::Enabled) == FudgetControlState::Enabled;
}

void FudgetControl::SetEnabled(bool value)
{
    SetState(FudgetControlState::Enabled, value);
}

bool FudgetControl::VirtuallyEnabled() const
{
    return ((_state_flags & (FudgetControlState::Enabled | FudgetControlState::ParentDisabled)) == FudgetControlState::Enabled);
}

void FudgetControl::FillRectangle(Float2 pos, Float2 size, Color color)
{
    CacheGlobalToLocal();
    pos = CachedLocalToGlobal(pos);

    Render2D::FillRectangle(Rectangle(pos, size), color);
}

void FudgetControl::FillRectangle(const Rectangle &rect, Color color)
{
    CacheGlobalToLocal();
    Render2D::FillRectangle(CachedLocalToGlobal(rect), color);
}

void FudgetControl::FillRectangle(const Rectangle& rect, const Color& color1, const Color& color2, const Color& color3, const Color& color4)
{
    CacheGlobalToLocal();
    Render2D::FillRectangle(CachedLocalToGlobal(rect), color1, color2, color3, color4);
}

void FudgetControl::DrawRectangle(Float2 pos, Float2 size, Color color, float thickness)
{
    CacheGlobalToLocal();
    pos = CachedLocalToGlobal(pos);

    Render2D::DrawRectangle(Rectangle(pos, size), color, color, color, color, thickness);
}

void FudgetControl::DrawRectangle(const Rectangle &rect, Color color, float thickness)
{
    CacheGlobalToLocal();
    Render2D::DrawRectangle(LocalToGlobal(rect), color, color, color, color, thickness);
}

void FudgetControl::DrawRectangle(const Rectangle& rect, const Color& color1, const Color& color2, const Color& color3, const Color& color4, float thickness)
{
    CacheGlobalToLocal();
    Render2D::DrawRectangle(CachedLocalToGlobal(rect), color1, color2, color3, color4, thickness);
}

void FudgetControl::Draw9SlicingTexture(TextureBase *t, const Rectangle &rect, const Float4 &border, const Float4 &borderUVs, const Color &color)
{
    CacheGlobalToLocal();
    Render2D::Draw9SlicingTexture(t, CachedLocalToGlobal(rect), border, borderUVs, color);
}

void FudgetControl::Draw9SlicingTexturePoint(TextureBase *t, const Rectangle &rect, const Float4 &border, const Float4 &borderUVs, const Color &color)
{
    CacheGlobalToLocal();
    Render2D::Draw9SlicingTexturePoint(t, CachedLocalToGlobal(rect), border, borderUVs, color);
}

void FudgetControl::Draw9SlicingSprite(const SpriteHandle& spriteHandle, const Rectangle &rect, const Float4 &border, const Float4 &borderUVs, const Color &color)
{
    CacheGlobalToLocal();
    Render2D::Draw9SlicingSprite(spriteHandle, CachedLocalToGlobal(rect), border, borderUVs, color);
}

void FudgetControl::Draw9SlicingSpritePoint(const SpriteHandle& spriteHandle, const Rectangle &rect, const Float4 &border, const Float4 &borderUVs, const Color &color)
{
    CacheGlobalToLocal();
    Render2D::Draw9SlicingSpritePoint(spriteHandle, CachedLocalToGlobal(rect), border, borderUVs, color);
}

void FudgetControl::Draw9SlicingPrecalculatedTexture(TextureBase *t, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color)
{
    if (t == nullptr)
        return;
    Float4 border;
    Float4 borderUV;
    Float2 siz = t->Size();
    border = Float4(borderWidths.Left, borderWidths.Right, borderWidths.Top, borderWidths.Bottom);
    borderUV = Float4(border.X / siz.X, border.Y / siz.X, border.Z / siz.Y, border.W / siz.Y);

    Draw9SlicingTexture(t, rect, border, borderUV, color);
}

void FudgetControl::Draw9SlicingPrecalculatedTexturePoint(TextureBase *t, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color)
{
    if (t == nullptr)
        return;
    Float4 border;
    Float4 borderUV;
    Float2 siz = t->Size();
    border = Float4(borderWidths.Left, borderWidths.Right, borderWidths.Top, borderWidths.Bottom);
    borderUV = Float4(border.X / siz.X, border.Y / siz.X, border.Z / siz.Y, border.W / siz.Y);

    Draw9SlicingTexturePoint(t, rect, border, borderUV, color);
}

void FudgetControl::Draw9SlicingPrecalculatedSprite(const SpriteHandle& spriteHandle, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color)
{
    if (!spriteHandle.IsValid())
        return;
    Float4 border;
    Float4 borderUV;
    Float2 siz = spriteHandle.Atlas->GetSprite(spriteHandle.Index).Area.Size;
    border = Float4(borderWidths.Left, borderWidths.Right, borderWidths.Top, borderWidths.Bottom);
    borderUV = Float4(border.X / siz.X, border.Y / siz.X, border.Z / siz.Y, border.W / siz.Y);

    Draw9SlicingSprite(spriteHandle, rect, border, borderUV, color);
}

void FudgetControl::Draw9SlicingPrecalculatedSpritePoint(const SpriteHandle& spriteHandle, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color)
{
    if (!spriteHandle.IsValid())
        return;
    Float4 border;
    Float4 borderUV;
    Float2 siz = spriteHandle.Atlas->GetSprite(spriteHandle.Index).Area.Size;
    border = Float4(borderWidths.Left, borderWidths.Right, borderWidths.Top, borderWidths.Bottom);
    borderUV = Float4(border.X / siz.X, border.Y / siz.X, border.Z / siz.Y, border.W / siz.Y);

    Draw9SlicingSpritePoint(spriteHandle, rect, border, borderUV, color);
}

void FudgetControl::DrawBezier(const Float2& p1, const Float2& p2, const Float2& p3, const Float2& p4, const Color& color, float thickness)
{
    CacheGlobalToLocal();
    Render2D::DrawBezier(CachedLocalToGlobal(p1), CachedLocalToGlobal(p2), CachedLocalToGlobal(p3), CachedLocalToGlobal(p4), color, thickness);
}

void FudgetControl::DrawBlur(const Rectangle& rect, float blurStrength)
{
    CacheGlobalToLocal();
    Render2D::DrawBlur(CachedLocalToGlobal(rect), blurStrength);
}

void FudgetControl::DrawLine(const Float2& p1, const Float2& p2, const Color& color, float thickness)
{
    CacheGlobalToLocal();
    Render2D::DrawLine(CachedLocalToGlobal(p1), CachedLocalToGlobal(p2), color, color, thickness);
}

void FudgetControl::DrawLine(const Float2& p1, const Float2& p2, const Color& color1, const Color& color2, float thickness)
{
    CacheGlobalToLocal();
    Render2D::DrawLine(CachedLocalToGlobal(p1), CachedLocalToGlobal(p2), color1, color2, thickness);
}

void FudgetControl::DrawMaterial(MaterialBase* material, const Rectangle& rect, const Color& color)
{
    CacheGlobalToLocal();
    Render2D::DrawMaterial(material, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawSprite(const SpriteHandle& spriteHandle, const Rectangle& rect, const Color& color)
{
    CacheGlobalToLocal();
    Render2D::DrawSprite(spriteHandle, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawSpritePoint(const SpriteHandle& spriteHandle, const Rectangle& rect, const Color& color)
{
    CacheGlobalToLocal();
    Render2D::DrawSpritePoint(spriteHandle, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawText(Font* font, const StringView& text, const Color& color, const Float2& location, MaterialBase* customMaterial)
{
    CacheGlobalToLocal();
    Render2D::DrawText(font, text, color, CachedLocalToGlobal(location), customMaterial);
}

void FudgetControl::DrawText(Font* font, const StringView& text, API_PARAM(Ref) TextRange& textRange, const Color& color, const Float2& location, MaterialBase* customMaterial)
{
    CacheGlobalToLocal();
    Render2D::DrawText(font, text, textRange, color, CachedLocalToGlobal(location), customMaterial);
}

void FudgetControl::DrawText(Font* font, const StringView& text, const Color& color, API_PARAM(Ref) TextLayoutOptions& layout, MaterialBase* customMaterial)
{
    CacheGlobalToLocal();
    TextLayoutOptions tmp = layout;
    tmp.Bounds = CachedLocalToGlobal(layout.Bounds);
    Render2D::DrawText(font, text, color, tmp, customMaterial);
}

void FudgetControl::DrawText(Font* font, const StringView& text, API_PARAM(Ref) TextRange& textRange, const Color& color, API_PARAM(Ref) TextLayoutOptions& layout, MaterialBase* customMaterial)
{
    CacheGlobalToLocal();
    TextLayoutOptions tmp = layout;
    tmp.Bounds = CachedLocalToGlobal(layout.Bounds);
    Render2D::DrawText(font, text, textRange, color, tmp, customMaterial);
}

void FudgetControl::DrawTexture(GPUTextureView* rt, const Rectangle& rect, const Color& color)
{
    CacheGlobalToLocal();
    Render2D::DrawTexture(rt, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawTexture(GPUTexture* t, const Rectangle& rect, const Color& color)
{
    CacheGlobalToLocal();
    Render2D::DrawTexture(t, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawTexture(TextureBase* t, const Rectangle& rect, const Color& color)
{
    CacheGlobalToLocal();
    Render2D::DrawTexture(t, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawSpriteTiled(const SpriteHandle& spriteHandle, Float2 size, Float2 offset, const Rectangle& rect, const Color& color)
{
    CacheGlobalToLocal();
    DrawTiled(nullptr, spriteHandle, false, size, offset, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawSpritePointTiled(const SpriteHandle& spriteHandle, Float2 size, Float2 offset, const Rectangle& rect, const Color& color)
{
    CacheGlobalToLocal();
    DrawTiled(nullptr, spriteHandle, true, size, offset, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawTextureTiled(GPUTexture *t, Float2 size, Float2 offset, const Rectangle& rect, const Color& color)
{
    CacheGlobalToLocal();
    DrawTiled(t, SpriteHandle(), false, size, offset, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawTexturePointTiled(GPUTexture *t, Float2 size, Float2 offset, const Rectangle& rect, const Color& color)
{
    CacheGlobalToLocal();
    DrawTiled(t, SpriteHandle(), true, size, offset, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawTexturePoint(GPUTexture* t, const Rectangle& rect, const Color& color)
{
    CacheGlobalToLocal();
    Render2D::DrawTexturePoint(t, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawTexturedTriangles(GPUTexture* t, const Span<Float2>& vertices, const Span<Float2>& uvs)
{
    CacheGlobalToLocal();
    Array<Float2> copy(vertices.Length());
    for (int ix = 0, siz = vertices.Length(); ix < siz; ++ix)
    {
        copy.Add(CachedLocalToGlobal(vertices[ix]));
    }

    Render2D::DrawTexturedTriangles(t, Span<Float2>(copy.Get(), copy.Count()), uvs);
}

void FudgetControl::DrawTexturedTriangles(GPUTexture* t, const Span<Float2>& vertices, const Span<Float2>& uvs, const Color& color)
{
    CacheGlobalToLocal();
    Array<Float2> copy(vertices.Length());
    for (int ix = 0, siz = vertices.Length(); ix < siz; ++ix)
    {
        copy.Add(CachedLocalToGlobal(vertices[ix]));
    }

    Render2D::DrawTexturedTriangles(t, Span<Float2>(copy.Get(), copy.Count()), uvs);
}

void FudgetControl::DrawTexturedTriangles(GPUTexture* t, const Span<Float2>& vertices, const Span<Float2>& uvs, const Span<Color>& colors)
{
    CacheGlobalToLocal();
    Array<Float2> copy(vertices.Length());
    for (int ix = 0, siz = vertices.Length(); ix < siz; ++ix)
    {
        copy.Add(CachedLocalToGlobal(vertices[ix]));
    }

    Render2D::DrawTexturedTriangles(t, Span<Float2>(copy.Get(), copy.Count()), uvs, colors);
}

void FudgetControl::DrawTexturedTriangles(GPUTexture* t, const Span<uint16>& indices, const Span<Float2>& vertices, const Span<Float2>& uvs, const Span<Color>& colors)
{
    CacheGlobalToLocal();
    Array<Float2> copy(vertices.Length());
    for (int ix = 0, siz = vertices.Length(); ix < siz; ++ix)
    {
        copy.Add(CachedLocalToGlobal(vertices[ix]));
    }

    Render2D::DrawTexturedTriangles(t, indices, Span<Float2>(copy.Get(), copy.Count()), uvs, colors);
}

void FudgetControl::FillTriangles(const Span<Float2>& vertices, const Span<Color>& colors, bool useAlpha)
{
    CacheGlobalToLocal();
    Array<Float2> copy(vertices.Length());
    for (int ix = 0, siz = vertices.Length(); ix < siz; ++ix)
    {
        copy.Add(CachedLocalToGlobal(vertices[ix]));
    }

    Render2D::FillTriangles(Span<Float2>(copy.Get(), copy.Count()), colors, useAlpha);
}

void FudgetControl::FillTriangle(const Float2& p0, const Float2& p1, const Float2& p2, const Color& color)
{
    CacheGlobalToLocal();
    Render2D::FillTriangle(CachedLocalToGlobal(p0), CachedLocalToGlobal(p1), CachedLocalToGlobal(p2), color);
}

void FudgetControl::DrawArea(const FudgetDrawArea &area, const Rectangle &rect)
{
    CacheGlobalToLocal();
    bool draw_color = (area.AreaType & FudgetFillType::Color) == FudgetFillType::Color;

    FudgetPadding borders = FudgetPadding(Math::Max(0.0f, area.Borders.Left), Math::Max(0.0f, area.Borders.Right), Math::Max(0.0f, area.Borders.Top), Math::Max(0.0f, area.Borders.Bottom));
    
    bool stretch = (area.AreaType & FudgetFillType::Stretch) == FudgetFillType::Stretch;
    bool point = (area.AreaType & FudgetFillType::Point) == FudgetFillType::Point;

    if ((int)(area.AreaType & FudgetFillType::BorderCentered) != 0)
    {
        // Border drawing.
        if (area.Borders.Left <= 0 && area.Borders.Top <= 0 && area.Borders.Right <= 0 && area.Borders.Bottom <= 0)
            return;

        if (draw_color)
        {
            Rectangle r;
            if ((area.AreaType & FudgetFillType::BorderCentered) == FudgetFillType::BorderInside)
            {
                r = Rectangle(Float2(rect.GetLeft() + borders.Left * 0.5f, rect.GetTop() + borders.Top * 0.5f), Float2(rect.GetWidth() - (borders.Left + borders.Right) * 0.5f, rect.GetHeight() - (borders.Top + borders.Bottom) * 0.5f));
            }
            else if ((area.AreaType & FudgetFillType::BorderCentered) == FudgetFillType::BorderOutside)
            {
                r = Rectangle(Float2(rect.GetLeft() - borders.Left * 0.5f, rect.GetTop() - borders.Top * 0.5f), Float2(rect.GetWidth() + (borders.Left + borders.Right) * 0.5f, rect.GetHeight() + (borders.Top + borders.Bottom) * 0.5f));
            }
            else
                r = rect;

            if (r.GetWidth() < 0 || r.GetHeight() < 0)
                return;
            if (Math::NearEqual(borders.Left, borders.Right) && Math::NearEqual(borders.Left, borders.Top) && Math::NearEqual(borders.Top, borders.Bottom))
            {
                DrawRectangle(r, area.Tint, area.Borders.Left);
                return;
            }

            // Draw borders normally with lines
            if (borders.Left > 0)
                DrawLine(r.GetUpperLeft() - borders.Top * 0.5f, r.GetBottomLeft() + borders.Bottom * 0.5f, area.Tint, borders.Left);
            if (borders.Right > 0)
                DrawLine(r.GetUpperRight() - borders.Top * 0.5f, r.GetBottomRight() + borders.Bottom * 0.5f, area.Tint, borders.Right);
            if (borders.Top > 0)
                DrawLine(r.GetUpperLeft() - borders.Left * 0.5f, r.GetUpperRight() + borders.Right * 0.5f, area.Tint, borders.Top);
            if (borders.Bottom > 0)
                DrawLine(r.GetBottomLeft() - borders.Left * 0.5f, r.GetBottomRight() + borders.Right * 0.5f, area.Tint, borders.Bottom);

            return;
        }

        Rectangle r;
        if ((area.AreaType & FudgetFillType::BorderCentered) == FudgetFillType::BorderInside)
        {
            r = rect;
        }
        else if ((area.AreaType & FudgetFillType::BorderCentered) == FudgetFillType::BorderOutside)
        {
            r = Rectangle(Float2(rect.GetLeft() - borders.Left, rect.GetTop() - borders.Top), Float2(rect.GetWidth() + (borders.Left + borders.Right), rect.GetHeight() + (borders.Top + borders.Bottom)));
        }
        else
            r = Rectangle(Float2(rect.GetLeft() - borders.Left * 0.5f, rect.GetTop() - borders.Top * 0.5f), Float2(rect.GetWidth() + (borders.Left + borders.Right) * 0.5f, rect.GetHeight() + (borders.Top + borders.Bottom) * 0.5f));
        if (r.GetWidth() < 0 || r.GetHeight() < 0)
            return;

        // Empty area to block out during drawing
        if ((borders.Width() < r.GetWidth()) && (borders.Height() < r.GetHeight()))
        {
            Rectangle inner = Rectangle(Float2(r.GetLeft() + borders.Left, r.GetTop() + borders.Top), Float2(r.GetWidth() - borders.Width(), r.GetHeight() - borders.Height()));

            // Draw border in 4 parts, top, left, right, bottom

            if (borders.Top > 0.0f)
                DrawTextureInner(area.Texture, area.SpriteHandle.ToHandle(), area.TextureScale, area.TextureOffset, Rectangle(r.GetUpperLeft(), Float2(r.GetWidth(), borders.Top)), area.Tint, stretch, point);
            if (borders.Left > 0.0f)
                DrawTextureInner(area.Texture, area.SpriteHandle.ToHandle(), area.TextureScale, area.TextureOffset - Float2(0.0f, borders.Top), Rectangle(Float2(r.GetLeft(), r.GetTop() + borders.Top), Float2(borders.Left, r.GetHeight() - borders.Height())), area.Tint, stretch, point);
            if (borders.Right > 0.0f)
                DrawTextureInner(area.Texture, area.SpriteHandle.ToHandle(), area.TextureScale, area.TextureOffset - Float2(r.GetWidth() - borders.Right, borders.Top), Rectangle(Float2(r.GetRight() - borders.Right, r.GetTop() + borders.Top), Float2(borders.Right, r.GetHeight() - borders.Height())), area.Tint, stretch, point);
            if (borders.Bottom > 0.0f)
                DrawTextureInner(area.Texture, area.SpriteHandle.ToHandle(), area.TextureScale, area.TextureOffset - Float2(0.0f, r.GetHeight() - borders.Bottom), Rectangle(Float2(0.0f, r.GetBottom() - borders.Bottom), Float2(r.GetWidth(), borders.Bottom)), area.Tint, stretch, point);
            return;
        }

        DrawTextureInner(area.Texture, area.SpriteHandle.ToHandle(), area.TextureScale, area.TextureOffset, r, area.Tint, stretch, point);
        return;
    }

    if (draw_color)
    {
        FillRectangle(rect, area.Tint);
        return;
    }

    if ((area.AreaType & FudgetFillType::Sliced) != FudgetFillType::Sliced)
    {
        DrawTextureInner(area.Texture, area.SpriteHandle.ToHandle(), area.TextureScale, area.TextureOffset, rect, area.Tint, stretch, point);
        return;
    }

    if (area.Texture != nullptr)
    {
        if (!point)
        {
            Draw9SlicingPrecalculatedTexture(area.Texture, rect, borders, area.Tint);
            return;
        }
        Draw9SlicingPrecalculatedTexturePoint(area.Texture, rect, borders, area.Tint);
        return;
    }

    if (!point)
    {
        Draw9SlicingPrecalculatedSprite(area.SpriteHandle.ToHandle(), rect, borders, area.Tint);
        return;
    }

    Draw9SlicingPrecalculatedSpritePoint(area.SpriteHandle.ToHandle(), rect, borders, area.Tint);
}

void FudgetControl::DrawArea(const FudgetDrawArea &area, Float2 pos, Float2 size)
{
    DrawArea(area, Rectangle(pos, size));
}

void FudgetControl::PushClip(const Rectangle &rect)
{
    CacheGlobalToLocal();
    Render2D::PushClip(CachedLocalToGlobal(rect));

    ++_clipping_count;
}

void FudgetControl::PopClip()
{
    if (_clipping_count == 0)
        return;
    --_clipping_count;
    Render2D::PopClip();
}

void FudgetControl::ClearStyleCache(bool inherited)
{
    _cached_theme = nullptr;
    _cached_style = nullptr;
    ResetCreatedFonts();
}

void FudgetControl::SetStyle(FudgetStyle *value)
{
    if (_style == value)
        return;
    _style = value;
    _cached_style = nullptr;
}

FudgetStyle* FudgetControl::GetActiveStyle()
{
    if (_style != nullptr)
        return _style;
    if (_cached_style != nullptr)
        return _cached_style;

    // No style was found, let's resolve one and save it to cached.

    CreateClassTokens();
#if USE_EDITOR
    FudgetThemes::SetRuntimeUse(IsInRunningGame());
#endif
    _cached_style = FudgetThemes::GetControlStyleOrDefault(_class_token);

    return _cached_style;
}

void FudgetControl::SetThemeId(FudgetToken value)
{
#if USE_EDITOR
    FudgetThemes::SetRuntimeUse(IsInRunningGame());
#endif
    if (_theme_id == value || FudgetThemes::GetTheme(value) == nullptr)
        return;

    _theme_id = value;
    ClearStyleCache(true);
}

FudgetTheme* FudgetControl::GetActiveTheme()
{
    if (_cached_theme != nullptr)
        return _cached_theme;

#if USE_EDITOR
    FudgetThemes::SetRuntimeUse(IsInRunningGame());
#endif
    if (_theme_id.IsValid())
        _cached_theme = FudgetThemes::GetTheme(_theme_id);
    else if (_parent != nullptr)
        _cached_theme = _parent->GetActiveTheme();
    if (_cached_theme == nullptr)
        _cached_theme = FudgetThemes::GetTheme(FudgetThemes::MainThemeToken);

    return _cached_theme;
}

template<typename T>
T* FudgetControl::CreateStylePainter(FudgetToken token, FudgetToken style_token)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
        return nullptr;

    T *painter = style->CreatePainter<T>(GetActiveTheme(), token);

    RegisterStylePainterInternal(painter, style_token);

    return painter;
}

bool FudgetControl::GetStyleValue(FudgetToken token, API_PARAM(Out) Variant &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = Variant();
        return false;
    }

    return style->GetResourceValue(GetActiveTheme(), token, result);
}

bool FudgetControl::GetStyleValue(const Span<FudgetToken> &tokens, API_PARAM(Out) Variant &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = Variant();
        return false;
    }

    return style->GetResourceValue(GetActiveTheme(), tokens, result);
}

bool FudgetControl::GetStyleToken(FudgetToken token, API_PARAM(Out) FudgetToken &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = FudgetToken::Invalid;
        return false;
    }

    return style->GetTokenResource(GetActiveTheme(), token, result);
}

bool FudgetControl::GetStyleToken(const Span<FudgetToken> &tokens, API_PARAM(Out) FudgetToken &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = FudgetToken::Invalid;
        return false;
    }

    return style->GetTokenResource(GetActiveTheme(), tokens, result);
}

bool FudgetControl::GetStyleColor(FudgetToken token, API_PARAM(Out) Color &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = Color();
        return false;
    }

    return style->GetColorResource(GetActiveTheme(), token, result);
}

bool FudgetControl::GetStyleColor(const Span<FudgetToken> &tokens, API_PARAM(Out) Color &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = Color();
        return false;
    }

    return style->GetColorResource(GetActiveTheme(), tokens, result);
}

bool FudgetControl::GetStyleFloat(FudgetToken token, API_PARAM(Out) float &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = 0.0f;
        return false;
    }

    return style->GetFloatResource(GetActiveTheme(), token, result);
}

bool FudgetControl::GetStyleFloat(const Span<FudgetToken> &tokens, API_PARAM(Out) float &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = 0.0f;
        return false;
    }

    return style->GetFloatResource(GetActiveTheme(), tokens, result);
}

bool FudgetControl::GetStyleFloat2(FudgetToken token, API_PARAM(Out) Float2 &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = Float2(0.0f);
        return false;
    }

    return style->GetFloat2Resource(GetActiveTheme(), token, result);
}

bool FudgetControl::GetStyleFloat2(const Span<FudgetToken> &tokens, API_PARAM(Out) Float2 &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = Float2(0.0f);
        return false;
    }

    return style->GetFloat2Resource(GetActiveTheme(), tokens, result);
}

bool FudgetControl::GetStyleFloat3(FudgetToken token, API_PARAM(Out) Float3 &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = Float3(0.0f);
        return false;
    }

    return style->GetFloat3Resource(GetActiveTheme(), token, result);
}

bool FudgetControl::GetStyleFloat3(const Span<FudgetToken> &tokens, API_PARAM(Out) Float3 &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = Float3(0.0f);
        return false;
    }

    return style->GetFloat3Resource(GetActiveTheme(), tokens, result);
}

bool FudgetControl::GetStyleFloat4(FudgetToken token, API_PARAM(Out) Float4 &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = Float4(0.0f);
        return false;
    }

    return style->GetFloat4Resource(GetActiveTheme(), token, result);
}

bool FudgetControl::GetStyleFloat4(const Span<FudgetToken> &tokens, API_PARAM(Out) Float4 &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = Float4(0.0f);
        return false;
    }

    return style->GetFloat4Resource(GetActiveTheme(), tokens, result);
}

bool FudgetControl::GetStyleInt(FudgetToken token, API_PARAM(Out) int &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = 0;
        return false;
    }

    return style->GetIntResource(GetActiveTheme(), token, result);
}

bool FudgetControl::GetStyleInt(const Span<FudgetToken> &tokens, API_PARAM(Out) int &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = 0;
        return false;
    }

    return style->GetIntResource(GetActiveTheme(), tokens, result);
}

bool FudgetControl::GetStylePadding(FudgetToken token, API_PARAM(Out) FudgetPadding &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = FudgetPadding();
        return false;
    }

    return style->GetPaddingResource(GetActiveTheme(), token, result);
}

bool FudgetControl::GetStylePadding(const Span<FudgetToken> &tokens, API_PARAM(Out) FudgetPadding &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = FudgetPadding();
        return false;
    }

    return style->GetPaddingResource(GetActiveTheme(), tokens, result);
}

bool FudgetControl::GetStyleFontSettings(FudgetToken token, API_PARAM(Out) FudgetFontSettings &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = FudgetFontSettings();
        return false;
    }

    return style->GetFontSettingsResource(GetActiveTheme(), token, result);
}

bool FudgetControl::GetStyleFontSettings(const Span<FudgetToken> &tokens, API_PARAM(Out) FudgetFontSettings &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = FudgetFontSettings();
        return false;
    }

    return style->GetFontSettingsResource(GetActiveTheme(), tokens, result);
}

bool FudgetControl::GetStyleFont(FudgetToken token, API_PARAM(Out) FudgetFont &result)
{
    if (!token.IsValid())
    {
        result = FudgetFont();
        return false;
    }

    auto it = _cached_fonts.find(token);
    if (it != _cached_fonts.end())
    {
        result = it->second;
        return true;
    }

    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = FudgetFont();
        return false;
    }

    FudgetThemes::SetRuntimeUse(IsInRunningGame());
    if (style->GetFontResource(GetActiveTheme(), token, result))
    {
        _cached_fonts[token] = result;
        return true;
    }

    result = FudgetFont();
    return false;
}

bool FudgetControl::GetStyleFont(const Span<FudgetToken> &tokens, API_PARAM(Out) FudgetFont &result)
{
    for (auto t : tokens)
    {
        if (GetStyleFont(t, result))
            return true;
    }
    return false;
}

void FudgetControl::ResetCreatedFonts()
{
    _cached_fonts.clear();
}

bool FudgetControl::GetStyleDrawArea(FudgetToken token, API_PARAM(Out) FudgetDrawArea &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = FudgetDrawArea();
        return false;
    }

    return style->GetDrawAreaResource(GetActiveTheme(), token, result);
}

bool FudgetControl::GetStyleDrawArea(const Span<FudgetToken> &tokens, API_PARAM(Out) FudgetDrawArea &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = FudgetDrawArea();
        return false;
    }

    return style->GetDrawAreaResource(GetActiveTheme(), tokens, result);
}

bool FudgetControl::GetStyleTextDrawSettings(FudgetToken token, API_PARAM(Out) FudgetTextDrawSettings &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = FudgetTextDrawSettings();
        return false;
    }

    return style->GetTextDrawSettingsResource(GetActiveTheme(), token, result);
}

bool FudgetControl::GetStyleTextDrawSettings(const Span<FudgetToken> &tokens, API_PARAM(Out) FudgetTextDrawSettings &result)
{
    FudgetStyle *style = GetActiveStyle();
    if (style == nullptr)
    {
        result = FudgetTextDrawSettings();
        return false;
    }

    return style->GetTextDrawSettingsResource(GetActiveTheme(), tokens, result);
}

void FudgetControl::Serialize(SerializeStream& stream, const void* otherObj)
{
    stream.JKEY("TypeName");
    stream.String(GetType().Fullname);

    SERIALIZE_GET_OTHER_OBJ(FudgetControl);
    SERIALIZE_MEMBER(Name, _name);
    SERIALIZE_MEMBER(ID, GetID());

    // Special serialization for parent ID.
    const Guid* otherParentID = other ? (other->_parent ? &(other->_parent)->GetID() : nullptr) : nullptr;
    if (Serialization::ShouldSerialize(_parent ? _parent->GetID() : Guid::Empty, otherParentID))
    {
        stream.JKEY("ParentID");
        Serialization::Serialize(stream, _parent ? _parent->GetID() : Guid::Empty, otherParentID);
    }

    SERIALIZE_MEMBER(Position, _pos);
    SERIALIZE_MEMBER(HintSize, _hint_size);
    SERIALIZE_MEMBER(MinSize, _min_size);
    SERIALIZE_MEMBER(MaxSize, _max_size);
}

void FudgetControl::Deserialize(DeserializeStream& stream, ISerializeModifier* modifier)
{
    DESERIALIZE_MEMBER(Name, _name);

    DESERIALIZE_MEMBER(Position, _pos);
    DESERIALIZE_MEMBER(HintSize, _hint_size);
    DESERIALIZE_MEMBER(MinSize, _min_size);
    DESERIALIZE_MEMBER(MaxSize, _max_size);

    Guid id = Guid::Empty;
    DESERIALIZE_MEMBER(ID, id);

    if (GetID() != id)
        ChangeID(id);

    Guid parentId = Guid::Empty;
    DESERIALIZE_MEMBER(ParentID, parentId);

    if (parentId != Guid::Empty)
    {
        FudgetContainer* parent = Scripting::FindObject<FudgetContainer>(parentId);
        if (parent != nullptr)
            SetParent(parent);
    }
}

void FudgetControl::DoDraw()
{
    SetState(FudgetControlState::Global2LocalCached, false);
    OnDraw();
}

void FudgetControl::DoUpdate(float delta_time)
{
    for (FudgetPartPainter *p : _painters)
        p->Update(delta_time);
    OnUpdate(delta_time);
}

void FudgetControl::DoFocusChanging(bool focused, FudgetControl *other)
{
    OnFocusChanging(focused, other);
}

void FudgetControl::DoFocusChanged(bool focused, FudgetControl *other)
{
    SetState(FudgetControlState::ShowFocused, focused);
    OnFocusChanged(focused, other);
}

void FudgetControl::DoMouseCaptured()
{
    SetState(FudgetControlState::MouseIsCaptured, true);
    OnMouseCaptured();
}

void FudgetControl::DoMouseReleased()
{
    SetState(FudgetControlState::MouseIsCaptured, false);
    OnMouseReleased();
}

void FudgetControl::DoMouseEnter(Float2 pos, Float2 global_pos)
{
    SetState(FudgetControlState::ShowHovered, true);
    OnMouseEnter(pos, global_pos);
}

void FudgetControl::FudgetControl::DoMouseLeave()
{
    SetState(FudgetControlState::ShowHovered, false);
    OnMouseLeave();
}

bool FudgetControl::HasAnyState(FudgetControlState states) const
{
    return (int)(_state_flags & states) != 0;
}

bool FudgetControl::HasAllStates(FudgetControlState states) const
{
    return (_state_flags & states) == states;
}

void FudgetControl::SetState(FudgetControlState states, bool value)
{
    if (value)
        _state_flags |= states;
    else
        _state_flags &= ~states;
}

void FudgetControl::Initialize()
{
    if (_guiRoot == nullptr)
        return;
#if USE_EDITOR
    // Making sure the painters used by the control use the correct style. The painters can't access the control and
    // can't check if it is set up in a running game.
    FudgetThemes::SetRuntimeUse(IsInRunningGame());
#endif
    OnInitialize();
    SetState(FudgetControlState::Initialized, true);
}

void FudgetControl::SetParentDisabled(bool value)
{
    SetState(FudgetControlState::ParentDisabled, value);
}

void FudgetControl::RegisterStylePainterInternal(FudgetPartPainter *painter, FudgetToken style_token)
{
    painter->Initialize(GetActiveTheme(), FudgetThemes::GetStyle(style_token));
    _painters.Add(painter);
}

void FudgetControl::DrawTextureInner(TextureBase *t, SpriteHandle sprite_handle, Float2 scale, Float2 offset, const Rectangle &rect, Color tint, bool stretch, bool point)
{
    if (t != nullptr)
    {
        if (!stretch)
        {
            if (!point)
                DrawTextureTiled(t->GetTexture(), t->Size() * scale, offset, rect, tint);
            else
                DrawTexturePointTiled(t->GetTexture(), t->Size() * scale, offset, rect, tint);
        }
        else
        {
            if (!point)
                DrawTexture(t->GetTexture(), rect, tint);
            else
                DrawTexturePoint(t->GetTexture(), rect, tint);
        }
        return;
    }

    if (!stretch)
    {
        Sprite sprite;
        if (!sprite_handle.GetSprite(&sprite))
            return;

        if (!point)
            DrawSpriteTiled(sprite_handle, sprite.Area.Size * scale, offset, rect, tint);
        else
            DrawSpritePointTiled(sprite_handle, sprite.Area.Size * scale, offset, rect, tint);
    }
    else
    {
        if (!point)
            DrawSprite(sprite_handle, rect, tint);
        else
            DrawSpritePoint(sprite_handle, rect, tint);
    }
}

void FudgetControl::DrawTiled(GPUTexture *t, SpriteHandle sprite_handle, bool point, Float2 size, Float2 offset, const Rectangle& rect, const Color& color)
{
    // Number of textures to draw along the x axis, including the half drawn one
    float cnt_x_f = rect.Size.X / size.X;
    // Number of textures to draw along the y axis, including the half drawn one
    float cnt_y_f = rect.Size.Y / size.Y;

    // Integer number of textures to draw along the x axis
    int cnt_x = (int)Math::Floor(cnt_x_f);
    // Integer number of textures to draw along the y axis
    int cnt_y = (int)Math::Floor(cnt_x_f);

    // Get the actual number of textures

    if (Math::NotNearEqual(cnt_x_f, (float)cnt_x))
        ++cnt_x;
    if (Math::NotNearEqual(cnt_y_f, (float)cnt_y))
        ++cnt_y;

    Render2D::PushClip(rect);

    float posx = rect.Location.X;
    float posy = rect.Location.Y;
    for (int iy = 0; iy < cnt_y; ++iy)
    {
        for (int ix = 0; ix < cnt_x; ++ix)
        {
            if (t != nullptr && !point)
                Render2D::DrawTexture(t, Rectangle(Float2(posx, posy), size), color);
            if (t != nullptr && point)
                Render2D::DrawTexturePoint(t, Rectangle(Float2(posx, posy), size), color);
            if (t == nullptr && !point)
                Render2D::DrawSprite(sprite_handle, Rectangle(Float2(posx, posy), size), color);
            if (t == nullptr && point)
                Render2D::DrawSpritePoint(sprite_handle, Rectangle(Float2(posx, posy), size), color);

            posx += size.X;
        }
        posy += size.Y;
        posx = rect.Location.X;
    }

    Render2D::PopClip();
}

bool FudgetControl::IsPositionChangePermitted() const
{
    if (!_parent)
        return true;
    return _parent->IsControlPositioningPermitted(this);
}

void FudgetControl::LayoutUpdate(Float2 pos, Float2 size)
{
    _pos = pos; 
    _size = size;
}

void FudgetControl::CreateClassTokens()
{
    if (_class_token.Count() > 0)
        return;

    auto thisclass = GetClass();
    StringAnsiView class_name = thisclass->GetName();
    while (thisclass != nullptr && class_name != "Object")
    {
#if USE_EDITOR
        FudgetThemes::SetRuntimeUse(IsInRunningGame());
#endif
        _class_token.Add(FudgetThemes::RegisterToken(class_name.ToString()));
        thisclass = thisclass->GetBaseClass();
        if (thisclass != nullptr)
            class_name = thisclass->GetName();
    }
}

