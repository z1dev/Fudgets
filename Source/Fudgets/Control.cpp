#include "Control.h"
#include "Container.h"
#include "GUIRoot.h"
#include "Fudget.h"
#include "MarginStructs.h"
#include "Styling/Themes.h"
#include "Styling/StyleStructs.h"
#include "Styling/DrawableBuilder.h"
#include "Styling/PartPainterIds.h"
#include "Styling/Painters/PartPainters.h"
#include "Styling/Painters/DrawablePainter.h"
#include "Layouts/Layout.h"


#include "Engine/Render2D/Render2D.h"
#include "Engine/Render2D/TextLayoutOptions.h"
#include "Engine/Core/Math/Rectangle.h"
#include "Engine/Scripting/Scripting.h"
#include "Engine/Core/Log.h"
#include "Engine/Scripting/ManagedCLR/MClass.h"
#include "Engine/Scripting/Types.h"
#include "Engine/Render2D/SpriteAtlas.h"


FudgetControl::FudgetControl(const SpawnParams &params) : ScriptingObject(params),
    _guiRoot(nullptr), _parent(nullptr), _index(-1), _flags(FudgetControlFlag::ResetFlags), _cursor(CursorType::Default),
    _pos(0), _size(0), _hint_size(120, 60), _min_size(0), _max_size(MAX_int32),
    _state_flags(FudgetControlState::Enabled), _visual_state(0), _cached_global_to_local_translation(0.f), _clipping_count(0), _changing(false),
    _style(nullptr), _cached_style(nullptr), _theme(nullptr), _cached_theme(nullptr)
{
    _data_proxy = New<FudgetControlDataConsumerProxy>();
    _data_proxy->_owner = this;
}

FudgetControl::~FudgetControl()
{
    RegisterToUpdate(false);

    for (auto p : _painters)
        p->_owner = nullptr;
    _painters.ClearDelete();

    for (const auto &p : _drawables)
        Delete(p.Key);
    _drawables.Clear();

    Delete(_data_proxy);
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
        _parent->RemoveChild(this);
    if (value != nullptr)
        value->AddChild(this, order);
    //if (_parent == nullptr)
    //    RegisterToUpdate(false);
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

void FudgetControl::SetHintSize(Int2 value)
{
    if (_hint_size == value)
        return;
    Int2 old_size = GetHintSize();
    _hint_size = value;
    if (/*!Int2::NearEqual(value, _size) &&*/ IsDirectSizeChangePermitted())
    {
        LayoutUpdate(_pos, _hint_size);
        //_size = _hint_size;
        //SetState(FudgetControlState::SizeUpdated, true);
    }
    if (GetHintSize() != old_size)
        SizeModified();
}

void FudgetControl::SetMinSize(Int2 value)
{
    if (_min_size == value)
        return;
    Int2 old_size = GetMinSize();
    _min_size = value;
    if (GetMinSize() != old_size)
        SizeModified();
}

void FudgetControl::SetMaxSize(Int2 value)
{
    if (value.X < 0)
        value.X = MAX_int32;
    if (value.Y < 0)
        value.Y = MAX_int32;

    if (_max_size == value)
        return;
    Int2 old_size = GetMaxSize();
    _max_size = value;
    if (GetMaxSize() != old_size)
        SizeModified();
}

Int2 FudgetControl::GetSize() const
{
    if (_parent == nullptr)
        return _hint_size;
    _parent->EnsureLayout();
    return _size;
}

Int2 FudgetControl::GetPosition() const
{
    if (_parent != nullptr)
        _parent->EnsureLayout();
    return _pos;
}

void FudgetControl::SetPosition(Int2 value)
{
    if (/*Int2::NearEqual(_pos, value) || */ !IsPositionChangePermitted())
        return;

    LayoutUpdate(value, _size);

    //_pos = value;
    //PositionModified();
    //OnPositionChanged();
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
    if (_parent != nullptr)
        GetGUIRoot()->DoLayout();

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
    if (_parent != nullptr)
        GetGUIRoot()->DoLayout();

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
    if (_parent != nullptr)
        GetGUIRoot()->DoLayout();

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
    if (_parent != nullptr)
        GetGUIRoot()->DoLayout();

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
    _cached_global_to_local_translation = GlobalToLocal(Int2(0));
}

void FudgetControl::InvalidateGlobalToLocalCache()
{
    SetState(FudgetControlState::Global2LocalCached, false);
}

Float2 FudgetControl::CachedLocalToGlobal(Float2 local, Float2 offset) const
{
    return local - _cached_global_to_local_translation + offset;
}

Float2 FudgetControl::CachedGlobalToLocal(Float2 global, Float2 offset) const
{
    return global + _cached_global_to_local_translation + offset;
}

Rectangle FudgetControl::CachedLocalToGlobal(const Rectangle &local, Float2 offset) const
{
    return Rectangle(local.Location - _cached_global_to_local_translation + offset, local.Size);
}

Rectangle FudgetControl::CachedGlobalToLocal(const Rectangle &global, Float2 offset) const
{
    return Rectangle(global.Location + _cached_global_to_local_translation + offset, global.Size);
}

FudgetControlFlag FudgetControl::GetControlFlags() const
{
    return _flags;
}

void FudgetControl::SetControlFlags(FudgetControlFlag flags)
{
    FudgetControlFlag tmp = _flags;
    _flags = flags;

    if ((_flags & FudgetControlFlag::RegisterToUpdates) != (tmp & FudgetControlFlag::RegisterToUpdates))
    {
        RegisterToUpdate(_parent != nullptr && ((_flags & FudgetControlFlag::RegisterToUpdates) == FudgetControlFlag::RegisterToUpdates));
    }

    if (_parent != _guiRoot && (_flags & FudgetControlFlag::AlwaysOnTop) == FudgetControlFlag::AlwaysOnTop)
        _flags &= ~FudgetControlFlag::AlwaysOnTop;
    else if (_parent == _guiRoot && _parent != nullptr && (_flags & FudgetControlFlag::AlwaysOnTop) != (tmp & FudgetControlFlag::AlwaysOnTop))
    {
        if (_guiRoot->ChangeControlAlwaysOnTop(this, (_flags & FudgetControlFlag::AlwaysOnTop) == FudgetControlFlag::AlwaysOnTop) == -1)
            _flags |= (tmp & FudgetControlFlag::AlwaysOnTop);
    }

    if ((tmp & FudgetControlFlag::Framed) != (_flags & FudgetControlFlag::Framed))
    {
        _flags ^= FudgetControlFlag::Framed;
        SetFramed((_flags & FudgetControlFlag::Framed) != FudgetControlFlag::Framed);
    }

    // Might not need recalculation but we can't be sure.
    SizeOrPosModified(FudgetLayoutDirtyReason::All);
}

bool FudgetControl::HasAllFlags(FudgetControlFlag flags) const
{
    return (flags & _flags) == flags;
}

bool FudgetControl::HasAnyFlag(FudgetControlFlag flags) const
{
    return (int)(flags & _flags) != 0;
}

void FudgetControl::SetFramed(bool value)
{
    bool had_frame = (_flags & FudgetControlFlag::Framed) == FudgetControlFlag::Framed;
    if (had_frame == value)
        return;

    if (value)
        _flags |= FudgetControlFlag::Framed;
    else
        _flags &= ~FudgetControlFlag::Framed;

    ClearStyleCache();
    SizeModified();
}

bool FudgetControl::OnMeasure(Int2 available, API_PARAM(Out) Int2 &wanted, API_PARAM(Out) Int2 &min_size, API_PARAM(Out) Int2 &max_size)
{
    wanted = GetLayoutHintSize();
    min_size = GetLayoutMinSize();
    max_size = GetLayoutMaxSize();
    return SizeDependsOnSpace();
}

void FudgetControl::SizeModified()
{
    SizeOrPosModified(FudgetLayoutDirtyReason::Size);
}

void FudgetControl::PositionModified()
{
    SizeOrPosModified(FudgetLayoutDirtyReason::Position);
}

bool FudgetControl::GetAlwaysOnTop() const
{
    return _parent == _guiRoot && _parent != nullptr && (_flags & FudgetControlFlag::AlwaysOnTop) == FudgetControlFlag::AlwaysOnTop;
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
    return (_visual_state & (uint64)FudgetVisualControlState::Focused) == (uint64)FudgetVisualControlState::Focused;
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
    return (_visual_state & (uint64)FudgetVisualControlState::Hovered) == (uint64)FudgetVisualControlState::Hovered;
}

bool FudgetControl::GetEnabled() const
{
    return (_state_flags & FudgetControlState::Enabled) == FudgetControlState::Enabled;
}

void FudgetControl::SetEnabled(bool value)
{
    if (((_state_flags & FudgetControlState::Enabled) == FudgetControlState::Enabled) == value)
        return;
    SetState(FudgetControlState::Enabled, value);
    DoEnabledChanged();
    if ((_state_flags & FudgetControlState::ParentDisabled) != FudgetControlState::ParentDisabled)
        DoVirtuallyEnabledChanged();

}

bool FudgetControl::VirtuallyEnabled() const
{
    return ((_state_flags & (FudgetControlState::Enabled | FudgetControlState::ParentDisabled)) == FudgetControlState::Enabled);
}

bool FudgetControl::IsVisible() const
{
    return (int)(_state_flags & (FudgetControlState::Invisible | FudgetControlState::Hidden | FudgetControlState::ParentHidden)) == 0;
}

FudgetControlVisibility FudgetControl::GetVisibility() const
{
    if (HasAnyState(FudgetControlState::Invisible))
        return FudgetControlVisibility::Invisible;
    if (HasAnyState(FudgetControlState::Hidden))
        return FudgetControlVisibility::Hidden;
    return FudgetControlVisibility::Visible;
}

void FudgetControl::SetVisibility(FudgetControlVisibility value)
{
    if (value == FudgetControlVisibility::Visible)
    {
        SetVisible(true);
    }
    else if (value == FudgetControlVisibility::Invisible)
    {
        if (HasAnyState(FudgetControlState::Invisible))
            return;

        SetVisible(true);
        MakeInvisible();
    }
    else
    {
        SetVisible(false);
    }
}

void FudgetControl::SetVisible(bool value)
{
    if (!HasAnyState(FudgetControlState::Invisible) && (HasAnyState(FudgetControlState::Hidden) != value))
        return;

    bool old_visible = IsVisible();

    if (value)
    {
        SetState(FudgetControlState::Hidden | FudgetControlState::Invisible, false);
        if (!old_visible && !HasAnyState(FudgetControlState::ParentHidden))
            DoShow();
    }
    else
    {
        bool invisible = HasAnyState(FudgetControlState::Invisible);
        SetState(FudgetControlState::Invisible, false);
        SetState(FudgetControlState::Hidden, true);
        if (old_visible)
            DoHide();
        else if (invisible)
            VisibilityModified();
    }
}

void FudgetControl::MakeInvisible()
{
    if (HasAnyState(FudgetControlState::Invisible | FudgetControlState::Hidden))
        return;

    SetState(FudgetControlState::Invisible, true);
    DoHide();
}

void FudgetControl::FillRectangle(Int2 pos, Int2 size, Color color)
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
    pos = CachedLocalToGlobal(pos) + Float2(0.5f);

    Render2D::DrawRectangle(Rectangle(pos, size), color, color, color, color, thickness);
}

void FudgetControl::DrawRectangle(const Rectangle &rect, Color color, float thickness)
{
    CacheGlobalToLocal();
    Render2D::DrawRectangle(CachedLocalToGlobal(rect, Float2(0.5f)), color, color, color, color, thickness);
}

void FudgetControl::DrawRectangle(const Rectangle& rect, const Color& color1, const Color& color2, const Color& color3, const Color& color4, float thickness)
{
    CacheGlobalToLocal();
    Render2D::DrawRectangle(CachedLocalToGlobal(rect, Float2(0.5f)), color1, color2, color3, color4, thickness);
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

void FudgetControl::Draw9SlicingPrecalculatedTexture(TextureBase *t, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color, FudgetImageAlignment alignment)
{
    Draw9SlicingPrecalculatedInner(t, SpriteHandle(), rect, borderWidths, color, alignment, false);
}

void FudgetControl::Draw9SlicingPrecalculatedTexturePoint(TextureBase *t, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color, FudgetImageAlignment alignment)
{
    Draw9SlicingPrecalculatedInner(t, SpriteHandle(), rect, borderWidths, color, alignment, true);
}

void FudgetControl::Draw9SlicingPrecalculatedSprite(const SpriteHandle& spriteHandle, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color, FudgetImageAlignment alignment)
{
    Draw9SlicingPrecalculatedInner(nullptr, spriteHandle, rect, borderWidths, color, alignment, false);
}

void FudgetControl::Draw9SlicingPrecalculatedSpritePoint(const SpriteHandle& spriteHandle, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color, FudgetImageAlignment alignment)
{
    Draw9SlicingPrecalculatedInner(nullptr, spriteHandle, rect, borderWidths, color, alignment, true);
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

void FudgetControl::DrawText(Font* font, const StringView& text, const Color& color, const Int2& location, MaterialBase* customMaterial)
{
    CacheGlobalToLocal();
    Render2D::DrawText(font, text, color, CachedLocalToGlobal(location), customMaterial);
}

void FudgetControl::DrawText(Font* font, const StringView& text, API_PARAM(Ref) const TextRange& textRange, const Color& color, const Int2& location, MaterialBase* customMaterial)
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

void FudgetControl::DrawText(Font* font, const StringView& text, API_PARAM(Ref) const TextRange& textRange, const Color& color, API_PARAM(Ref) TextLayoutOptions& layout, MaterialBase* customMaterial)
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

void FudgetControl::DrawArea(const FudgetDrawArea &area, const Rectangle &rect, const Color &tint)
{
    if (area.FillType == FudgetFillType::None)
        return;

    CacheGlobalToLocal();

    FudgetPadding slices = FudgetPadding::Max(area.Slices, 0);
    
    bool point = (area.FillType & FudgetFillType::Point) == FudgetFillType::Point;

    if ((area.FillType & FudgetFillType::Sliced) != FudgetFillType::Sliced)
    {
        DrawTextureInner(area.Texture, area.SpriteHandle.ToHandle(), area.Scale, area.Offset, rect, tint, area.ImageAlignment, point);
        return;
    }

    if (area.Texture != nullptr)
    {
        if (!point)
        {
            Draw9SlicingPrecalculatedTexture(area.Texture, rect, slices, area.Tint * tint, area.ImageAlignment);
            return;
        }
        Draw9SlicingPrecalculatedTexturePoint(area.Texture, rect, slices, area.Tint * tint, area.ImageAlignment);
        return;
    }

    if (!point)
    {
        Draw9SlicingPrecalculatedSprite(area.SpriteHandle.ToHandle(), rect, slices, area.Tint * tint, area.ImageAlignment);
        return;
    }

    Draw9SlicingPrecalculatedSpritePoint(area.SpriteHandle.ToHandle(), rect, slices, area.Tint * tint, area.ImageAlignment);
}

void FudgetControl::DrawArea(const FudgetDrawArea &area, Float2 pos, Float2 size, const Color &tint)
{
    DrawArea(area, Rectangle(pos, size), tint);
}

void FudgetControl::DrawBorder(const FudgetDrawBorder &border, const Rectangle &rect, const Color &tint)
{
    if (border.BorderType == FudgetBorderType::None || (border.Borders.Left <= 0 && border.Borders.Top <= 0 && border.Borders.Right <= 0 && border.Borders.Bottom <= 0))
        return;

    CacheGlobalToLocal();

    FudgetBorder borders = FudgetBorder::Max(border.Borders, 0.f);

    FudgetImageAlignment alignment = (border.BorderType & FudgetBorderType::Stretch) == FudgetBorderType::Stretch ? FudgetImageAlignment::Fit : (FudgetImageAlignment::Tiled | FudgetImageAlignment::Fit);
    bool point = (border.BorderType & FudgetBorderType::Point) == FudgetBorderType::Point;

    if ((border.BorderType & FudgetBorderType::Color) == FudgetBorderType::Color)
    {
        Rectangle r;
        if ((border.BorderPlacement & FudgetBorderPlacement::Centered) == FudgetBorderPlacement::Inside)
        {
            r = Rectangle(Float2(rect.GetLeft() + borders.Left * 0.5f, rect.GetTop() + borders.Top * 0.5f), Float2(rect.GetWidth() - (borders.Left + borders.Right) * 0.5f, rect.GetHeight() - (borders.Top + borders.Bottom) * 0.5f));
        }
        else if ((border.BorderPlacement & FudgetBorderPlacement::Centered) == FudgetBorderPlacement::Outside)
        {
            r = Rectangle(Float2(rect.GetLeft() - borders.Left * 0.5f, rect.GetTop() - borders.Top * 0.5f), Float2(rect.GetWidth() + (borders.Left + borders.Right) * 0.5f, rect.GetHeight() + (borders.Top + borders.Bottom) * 0.5f));
        }
        else
            r = rect;

        if (r.GetWidth() < 0 || r.GetHeight() < 0)
            return;
        if (borders.Left == borders.Right && borders.Left == borders.Top && borders.Top == borders.Bottom)
        {
            DrawRectangle(r, border.Tint * tint, (float)border.Borders.Left);
            return;
        }

        // Draw borders normally with lines
        if (borders.Left > 0)
            DrawLine(r.GetUpperLeft() - (float)borders.Top * 0.5f, r.GetBottomLeft() + (float)borders.Bottom * 0.5f, border.Tint * tint, (float)borders.Left);
        if (borders.Right > 0)
            DrawLine(r.GetUpperRight() - (float)borders.Top * 0.5f, r.GetBottomRight() + (float)borders.Bottom * 0.5f, border.Tint * tint, (float)borders.Right);
        if (borders.Top > 0)
            DrawLine(r.GetUpperLeft() - (float)borders.Left * 0.5f, r.GetUpperRight() + (float)borders.Right * 0.5f, border.Tint * tint, (float)borders.Top);
        if (borders.Bottom > 0)
            DrawLine(r.GetBottomLeft() - (float)borders.Left * 0.5f, r.GetBottomRight() + (float)borders.Right * 0.5f, border.Tint * tint, (float)borders.Bottom);
        return;
    }

    Rectangle r;
    if ((border.BorderPlacement & FudgetBorderPlacement::Centered) == FudgetBorderPlacement::Inside)
    {
        r = rect;
    }
    else if ((border.BorderPlacement & FudgetBorderPlacement::Centered) == FudgetBorderPlacement::Outside)
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
            DrawTextureInner(border.Texture, border.SpriteHandle.ToHandle(), border.Scale, border.Offset, Rectangle(r.GetUpperLeft(), Float2(r.GetWidth(), (float)borders.Top)), border.Tint * tint, alignment, point);
        if (borders.Left > 0.0f)
            DrawTextureInner(border.Texture, border.SpriteHandle.ToHandle(), border.Scale, border.Offset - Float2(0.0f, (float)borders.Top), Rectangle(Float2(r.GetLeft(), r.GetTop() + (float)borders.Top), Float2((float)borders.Left, r.GetHeight() - (float)borders.Height())), border.Tint * tint, alignment, point);
        if (borders.Right > 0.0f)
            DrawTextureInner(border.Texture, border.SpriteHandle.ToHandle(), border.Scale, border.Offset - Float2(r.GetWidth() - (float)borders.Right, (float)borders.Top), Rectangle(Float2(r.GetRight() - (float)borders.Right, r.GetTop() + (float)borders.Top), Float2((float)borders.Right, r.GetHeight() - (float)borders.Height())), border.Tint * tint, alignment, point);
        if (borders.Bottom > 0.0f)
            DrawTextureInner(border.Texture, border.SpriteHandle.ToHandle(), border.Scale, border.Offset - Float2(0.0f, r.GetHeight() - (float)borders.Bottom), Rectangle(Float2(0.0f, r.GetBottom() - (float)borders.Bottom), Float2(r.GetWidth(), (float)borders.Bottom)), border.Tint * tint, alignment, point);
        return;
    }

    DrawTextureInner(border.Texture, border.SpriteHandle.ToHandle(), border.Scale, border.Offset, r, border.Tint * tint, alignment, point);
}

void FudgetControl::DrawBorder(const FudgetDrawBorder &area, Float2 pos, Float2 size, const Color &tint)
{
    DrawBorder(area, Rectangle(pos, size), tint);
}

void FudgetControl::DrawDrawable(FudgetDrawable *drawable, int stateindex, const Rectangle &rect, const Color &tint)
{
    if (stateindex < 0 || stateindex >= drawable->_lists.size())
        return;
    DrawDrawableInstructions(*drawable->_lists[stateindex], rect, tint);
}

void FudgetControl::DrawDrawable(FudgetDrawable *drawable, int stateindex, Float2 pos, Float2 size, const Color &tint)
{
    if (stateindex < 0 || stateindex >= drawable->_lists.size())
        return;
    DrawDrawableInstructions(*drawable->_lists[stateindex], Rectangle(pos, size), tint);
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

bool FudgetControl::ClearStyleCache(bool forced)
{
    bool style_changed = forced || _cached_style != nullptr || _cached_theme != nullptr;
    _cached_theme = nullptr;
    _cached_style = nullptr;
    style_changed |= ResetCreatedFonts();

    SizeModified();

    SetState(FudgetControlState::StyleInitialized, false);

    return style_changed;
}

FudgetStyle* FudgetControl::GetStyle()
{
    if (/*_guiRoot != nullptr &&*/ !HasAnyState(FudgetControlState::StyleInitialized) && (_parent == nullptr || _parent->HasAnyState(FudgetControlState::StyleInitialized)))
        DoStyleInitialize();

    return _style;
}

void FudgetControl::SetStyle(FudgetStyle *value)
{
    if (_style == value)
        return;
    _style = value;
    ClearStyleCache(true);
}

void FudgetControl::SetStyleName(const String &value)
{
    if (_style_name == value)
        return;
    _style_name = value;
    ClearStyleCache(true);
}

FudgetStyle* FudgetControl::GetClassStyle()
{
    //if (!HasAnyState(FudgetControlState::Initialized))
    //    return nullptr;

    if (_cached_style != nullptr)
        return _cached_style;

    // No style was found, let's resolve one and save it to cached.

    CreateClassNames();
#if USE_EDITOR
    FudgetThemes::SetRuntimeUse(IsInRunningGame());
#endif
    if (!_style_name.IsEmpty())
        _cached_style = FudgetThemes::GetStyle(_style_name);
    if (_cached_style == nullptr)
        _cached_style = FudgetThemes::FindMatchingStyle(GetActiveTheme(), _class_names);

    if (_cached_style != nullptr)
        SizeModified();

    if (/*_guiRoot != nullptr &&*/ !HasAnyState(FudgetControlState::StyleInitialized) && (_parent == nullptr || _parent->HasAnyState(FudgetControlState::StyleInitialized)))
        DoStyleInitialize();

    return _cached_style;
}

void FudgetControl::SetTheme(FudgetTheme *value)
{
#if USE_EDITOR
    FudgetThemes::SetRuntimeUse(IsInRunningGame());
#endif
    if (_theme == value)
        return;

    _theme = value;
    ClearStyleCache(true);
}

FudgetTheme* FudgetControl::GetActiveTheme()
{
    if (_cached_theme != nullptr)
        return _cached_theme;

#if USE_EDITOR
    FudgetThemes::SetRuntimeUse(IsInRunningGame());
#endif
    if (_theme != nullptr)
        _cached_theme = _theme;
    else if (_parent != nullptr)
        _cached_theme = _parent->GetActiveTheme();
    if (_cached_theme == nullptr)
        _cached_theme = FudgetThemes::GetTheme(FudgetThemes::MAIN_THEME);

    return _cached_theme;
}

bool FudgetControl::GetStyleValue(int id, API_PARAM(Out) Variant &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
    {
        if (!FudgetStyle::GetResourceValue(style, GetActiveTheme(), id, false, result))
        {
            // Out parameters start uninitialized in C# and they need to be initialized. This is done
            // in other style resource getter functions on fail but not in this one. Initializing to
            // empty variant saves us a crash.
            result = Variant();
            return false;
        }
        return true;
    }

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr && FudgetStyle::GetResourceValue(class_style, GetActiveTheme(), id, false, result))
        return true;

    // Initializing on fail for same reason as above.
    result = Variant();
    return false;
}

bool FudgetControl::GetStyleStyle(int id, API_PARAM(Out) FudgetStyle* &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetStyleResource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetStyleResource(class_style, GetActiveTheme(), id, false, result);

    result = nullptr;
    return false;
}

bool FudgetControl::GetStylePainterMapping(int id, API_PARAM(Out) FudgetPartPainterMapping &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetPainterMappingResource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetPainterMappingResource(class_style, GetActiveTheme(), id, false, result);

    result = FudgetPartPainterMapping{};
    return false;
}

bool FudgetControl::GetStyleString(int id, API_PARAM(Out) String &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetStringResource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetStringResource(class_style, GetActiveTheme(), id, false, result);

    result = String();
    return false;
}

bool FudgetControl::GetStyleColor(int id, API_PARAM(Out) Color &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetColorResource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetColorResource(class_style, GetActiveTheme(), id, false, result);

    result = Color();
    return false;
}

bool FudgetControl::GetStyleDrawColors(int id, API_PARAM(Out) FudgetDrawColors &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetDrawColorsResource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetDrawColorsResource(class_style, GetActiveTheme(), id, false, result);

    result = FudgetDrawColors();
    return false;
}

bool FudgetControl::GetStyleBool(int id, API_PARAM(Out) bool &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetBoolResource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetBoolResource(class_style, GetActiveTheme(), id, false, result);

    result = false;
    return false;
}

bool FudgetControl::GetStyleFloat(int id, API_PARAM(Out) float &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetFloatResource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetFloatResource(class_style, GetActiveTheme(), id, false, result);

    result = 0.f;
    return false;
}

bool FudgetControl::GetStyleFloat2(int id, API_PARAM(Out) Float2 &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetFloat2Resource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetFloat2Resource(class_style, GetActiveTheme(), id, false, result);

    result = Float2::Zero;
    return false;
}

bool FudgetControl::GetStyleFloat3(int id, API_PARAM(Out) Float3 &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetFloat3Resource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetFloat3Resource(class_style, GetActiveTheme(), id, false, result);

    result = Float3::Zero;
    return false;
}

bool FudgetControl::GetStyleFloat4(int id, API_PARAM(Out) Float4 &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetFloat4Resource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetFloat4Resource(class_style, GetActiveTheme(), id, false, result);

    result = Float4::Zero;
    return false;
}

bool FudgetControl::GetStyleInt(int id, API_PARAM(Out) int &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetIntResource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetIntResource(class_style, GetActiveTheme(), id, false, result);

    result = 0;
    return false;
}

bool FudgetControl::GetStyleInt2(int id, API_PARAM(Out) Int2 &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetInt2Resource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetInt2Resource(class_style, GetActiveTheme(), id, false, result);

    result = Int2::Zero;
    return false;
}

bool FudgetControl::GetStyleInt3(int id, API_PARAM(Out) Int3 &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetInt3Resource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetInt3Resource(class_style, GetActiveTheme(), id, false, result);

        result = Int3::Zero;
    return false;
}

bool FudgetControl::GetStyleInt4(int id, API_PARAM(Out) Int4 &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetInt4Resource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetInt4Resource(class_style, GetActiveTheme(), id, false, result);

    result = Int4::Zero;
    return false;
}

bool FudgetControl::GetStylePadding(int id, API_PARAM(Out) FudgetPadding &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetPaddingResource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetPaddingResource(class_style, GetActiveTheme(), id, false, result);

    result = FudgetPadding();
    return false;
}

bool FudgetControl::GetStyleBorder(int id, API_PARAM(Out) FudgetBorder &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetBorderResource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetBorderResource(class_style, GetActiveTheme(), id, false, result);

    result = FudgetBorder();
    return false;
}

bool FudgetControl::GetStyleFontSettings(int id, API_PARAM(Out) FudgetFontSettings &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetFontSettingsResource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetFontSettingsResource(class_style, GetActiveTheme(), id, false, result);

    result = FudgetFontSettings();
    return false;
}

bool FudgetControl::GetStyleFont(int id, API_PARAM(Out) FudgetFont &result)
{
    if (id < 0)
    {
        result = FudgetFont();
        return false;
    }

    auto it = _cached_fonts.find(id);
    if (it != _cached_fonts.end())
    {
        result = it->second;
        return true;
    }

#if USE_EDITOR
    FudgetThemes::SetRuntimeUse(IsInRunningGame());
#endif

    FudgetStyle *style = GetStyle();
    if (style != nullptr)
    {
        if (FudgetStyle::GetFontResource(style, GetActiveTheme(), id, false, result))
        {
            _cached_fonts[id] = result;
            return true;
        }
        result = FudgetFont();
        return false;
    }

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr && FudgetStyle::GetFontResource(class_style, GetActiveTheme(), id, true, result))
    {
        _cached_fonts[id] = result;
        return true;
    }

    result = FudgetFont();
    return false;
}

bool FudgetControl::ResetCreatedFonts()
{
    if (_cached_fonts.empty())
        return false;
    _cached_fonts.clear();
    return true;
}

bool FudgetControl::GetStyleDrawArea(int id, API_PARAM(Out) FudgetDrawArea &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetDrawAreaResource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetDrawAreaResource(class_style, GetActiveTheme(), id, false, result);

    result = FudgetDrawArea();
    return false;
}

bool FudgetControl::GetStyleDrawable(int id, FudgetPartPainter *drawable_owner, API_PARAM(Out) FudgetDrawable* &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetDrawableResource(style, GetActiveTheme(), this, drawable_owner, id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetDrawableResource(class_style, GetActiveTheme(), this, drawable_owner, id, false, result);

    result = nullptr;
    return false;
}

bool FudgetControl::GetStyleTexture(int id, API_PARAM(Out) TextureBase* &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetTextureResource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetTextureResource(class_style, GetActiveTheme(), id, false, result);

    result = nullptr;
    return false;
}

bool FudgetControl::GetStyleTextDrawSettings(int id, API_PARAM(Out) FudgetTextDrawSettings &result)
{
    FudgetStyle *style = GetStyle();
    if (style != nullptr)
        return FudgetStyle::GetTextDrawSettingsResource(style, GetActiveTheme(), id, false, result);

    FudgetStyle *class_style = GetClassStyle();
    if (class_style != nullptr)
        return FudgetStyle::GetTextDrawSettingsResource(class_style, GetActiveTheme(), id, false, result);

    result = FudgetTextDrawSettings();
    return false;
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

    FudgetLayout* layout = _parent ? _parent->GetLayout() : nullptr;
    if (layout)
    {
        stream.JKEY("SlotProperties");
        stream.StartObject();
        // Serialize slot
        layout->GetSlot(GetIndexInParent())->Serialize(stream, nullptr);
        stream.EndObject();
    }
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
    else
    {
        // Can't deserialize layout slot properties without a parent.
        return;
    }

    const auto& layoutSlotMember = stream.FindMember("SlotProperties");
    if (layoutSlotMember != stream.MemberEnd() && layoutSlotMember->value.IsObject())
    {
        DeserializeStream& slotProperties = layoutSlotMember->value;
        FudgetLayout* layout = _parent ? _parent->GetLayout() : nullptr;
        if (layout)
        {
            layout->GetSlot(GetIndexInParent())->Deserialize(slotProperties, modifier);
        }
    }
}

void FudgetControl::DoDraw()
{
    if (!IsVisible())
        return;

    SetState(FudgetControlState::Global2LocalCached, false);

    if (HasAnyState(FudgetControlState::PositionUpdated | FudgetControlState::SizeUpdated))
    {
        bool pos = HasAnyState(FudgetControlState::PositionUpdated);
        bool siz = HasAnyState(FudgetControlState::SizeUpdated);
        SetState(FudgetControlState::PositionUpdated, false);
        SetState(FudgetControlState::SizeUpdated, false);

        if (pos)
            OnPositionChanged();
        if (siz)
            OnSizeChanged();

        OnPositionOrSizeChanged(pos, siz);
    }

    if (HasAnyState(FudgetControlState::FrameCreated))
        ((FudgetDrawablePainter*)_painters[0])->Draw(this, GetBounds(), GetVisualState());
    OnDraw();
}

void FudgetControl::DoUpdate(float delta_time)
{
    for (FudgetPartPainter *p : _painters)
        p->Update(delta_time);
    OnUpdate(delta_time);
}

void FudgetControl::DoEnabledChanged()
{
    OnEnabledChanged();
}

void FudgetControl::DoVirtuallyEnabledChanged()
{
    SetVisualState(FudgetVisualControlState::Disabled, !VirtuallyEnabled());
    OnVirtuallyEnabledChanged();
}

void FudgetControl::DoFocusChanging(bool focused, FudgetControl *other)
{
    OnFocusChanging(focused, other);
}

void FudgetControl::DoFocusChanged(bool focused, FudgetControl *other)
{
    SetVisualState(FudgetVisualControlState::Focused, focused);
    OnFocusChanged(focused, other);
}

void FudgetControl::DoShow()
{
    VisibilityModified();
    OnShow();
}

void FudgetControl::DoHide()
{
    VisibilityModified();
    OnHide();
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

FudgetInputResult FudgetControl::DoMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
    return OnMouseDown(pos, global_pos, button, double_click);
}

bool FudgetControl::DoMouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
    return OnMouseUp(pos, global_pos, button);
}

void FudgetControl::DoMouseMove(Float2 pos, Float2 global_pos)
{
    OnMouseMove(pos, global_pos);
}

void FudgetControl::DoMouseEnter(Float2 pos, Float2 global_pos)
{
    SetVisualState(FudgetVisualControlState::Hovered, true);
    OnMouseEnter(pos, global_pos);
}

void FudgetControl::FudgetControl::DoMouseLeave()
{
    SetVisualState(FudgetVisualControlState::Hovered, false);
    OnMouseLeave();
}

void FudgetControl::SetCursor(CursorType value)
{
    if (value == _cursor)
        return;

    _cursor = value;
    DoCursorChanged();
}

void FudgetControl::DoCursorChanged()
{
    auto root = GetGUIRoot();
    if (root != nullptr)
        root->UpdateCursor(this);

    OnCursorChanged();
}

bool FudgetControl::HasAnyState(FudgetControlState states) const
{
    return (int)(_state_flags & states) != 0;
}

bool FudgetControl::HasAllStates(FudgetControlState states) const
{
    return (_state_flags & states) == states;
}

FudgetPadding FudgetControl::GetFramePadding() const
{
    if (HasAnyState(FudgetControlState::FrameCreated))
        return ((FudgetDrawablePainter*)_painters[0])->GetPadding();
    return FudgetPadding();
}

void FudgetControl::SetState(FudgetControlState states, bool value)
{
    if (value)
        _state_flags |= states;
    else
        _state_flags &= ~states;
}

void FudgetControl::SetVisualState(FudgetVisualControlState states, bool value)
{
    if (value)
        _visual_state |= (uint64)states;
    else
        _visual_state &= ~(uint64)states;
}

void FudgetControl::SetVisualState(uint64 states, bool value)
{
    if (value)
        _visual_state |= states;
    else
        _visual_state &= ~states;
}

void FudgetControl::DoInitialize()
{
    //if (_guiRoot == nullptr)
    if (HasAnyState(FudgetControlState::Initialized))
        return;
#if USE_EDITOR
    // Making sure the control uses the correct theme data for style initializations.
    FudgetThemes::SetRuntimeUse(IsInRunningGame());
#endif
    OnInitialize();
    SetState(FudgetControlState::Initialized, true);
}

void FudgetControl::DoStyleInitialize()
{
    if (_guiRoot == nullptr || HasAnyState(FudgetControlState::StyleInitialized) || (_parent != nullptr && !_parent->HasAnyState(FudgetControlState::StyleInitialized)))
        return;
#if USE_EDITOR
    // Making sure the control uses the correct theme data for style initializations.
    FudgetThemes::SetRuntimeUse(IsInRunningGame());
#endif
    // Set the initialized flag early because this is required to access the current style.
    SetState(FudgetControlState::StyleInitialized, true);

    if (HasAnyState(FudgetControlState::FrameCreated) && !_painters.IsEmpty())
    {
        Delete(_painters[0]);
        SetState(FudgetControlState::FrameCreated, false);
    }

    if (HasAnyFlag(FudgetControlFlag::Framed))
    {
        FudgetDrawablePainter *frame_painter = CreateStylePainter<FudgetDrawablePainter>(nullptr, (int)FudgetFramedControlPartIds::FramePainter);
        if (frame_painter == nullptr)
        {
            frame_painter = New<FudgetDrawablePainter>(SpawnParams(Guid::New(), FudgetDrawablePainter::TypeInitializer));
            FudgetDrawablePainter::Mapping map;
            RegisterStylePainterInternal(frame_painter, FudgetPartPainter::InitializeMapping<FudgetDrawablePainter>(map));
        }

        _painters.Remove(frame_painter);
        _painters.Insert(0, frame_painter);
        SetState(FudgetControlState::FrameCreated, true);
    }

    OnStyleInitialize();
    SizeModified();
}

void FudgetControl::InitializeFlags()
{
    if ((_flags & FudgetControlFlag::ResetFlags) == FudgetControlFlag::ResetFlags)
        _flags = GetInitFlags();

    // The AlwaysOnTop flag only makes sense when the control is a direct child in the root. In that case, the root
    // can get this flag from GetInitFlags.
    _flags &= ~FudgetControlFlag::AlwaysOnTop;
}

void FudgetControl::SizeOrPosModified(FudgetLayoutDirtyReason dirt_flags)
{
    if (_parent != nullptr)
        _parent->MarkLayoutDirty(dirt_flags, this);
}

void FudgetControl::VisibilityModified()
{
    if (_parent != nullptr)
        _parent->MarkLayoutDirty(FudgetLayoutDirtyReason::Container | FudgetLayoutDirtyReason::Size, this);
}

void FudgetControl::SetParentDisabled(bool value)
{
    if (HasAnyState(FudgetControlState::ParentDisabled) == value)
        return;
    SetState(FudgetControlState::ParentDisabled, value);
    if (HasAnyState(FudgetControlState::Enabled))
        DoVirtuallyEnabledChanged();
}

void FudgetControl::SetParentVisibility(bool value)
{
    if (HasAnyState(FudgetControlState::ParentHidden) != value)
        return;
    SetState(FudgetControlState::ParentHidden, !value);
    if (HasAnyState(FudgetControlState::Hidden | FudgetControlState::Invisible) != value)
        return;
    if (value)
        OnShow();
    else
        OnHide();
}

void FudgetControl::DoRootChanging(FudgetGUIRoot *new_root)
{
    RegisterToUpdate(false);
}

void FudgetControl::DoRootChanged(FudgetGUIRoot *old_root)
{
    RegisterToUpdate(HasAnyFlag(FudgetControlFlag::RegisterToUpdates));
}

void FudgetControl::DoParentChanged(FudgetContainer *old_parent)
{
    if (_parent == nullptr)
    {
        RegisterToUpdate(false);
        DoParentStateChanged();
        SetState(FudgetControlState::StyleInitialized, false);
        _guiRoot = nullptr;
        return;
    }

    DoParentStateChanged();
    InitializeFlags();

    bool root_change = old_parent == nullptr || old_parent->GetGUIRoot() != _parent->GetGUIRoot();
    if (root_change)
        DoRootChanging(_parent->GetGUIRoot());

    _guiRoot = _parent->GetGUIRoot();

    if (/*_guiRoot != nullptr &&*/ !HasAnyState(FudgetControlState::Initialized) /*&& (_parent == nullptr || _parent->HasAnyState(FudgetControlState::Initialized))*/)
    {
        DoInitialize();
        SetState(FudgetControlState::Initialized, true);
    }
    if (/*_guiRoot != nullptr &&*/ !HasAnyState(FudgetControlState::StyleInitialized) && (_parent == nullptr || _parent->HasAnyState(FudgetControlState::StyleInitialized)))
        DoStyleInitialize();

    if (root_change)
        DoRootChanged(old_parent != nullptr ? old_parent->GetGUIRoot() : nullptr);

    OnParentChanged(old_parent);
}

void FudgetControl::DoParentStateChanged()
{
    if (_parent == nullptr)
    {
        SetState(FudgetControlState::ParentDisabled, false);
        SetState(FudgetControlState::ParentHidden, false);
        return;
    }

    SetState(FudgetControlState::ParentDisabled, !_parent->VirtuallyEnabled());
    SetState(FudgetControlState::ParentHidden, !_parent->IsVisible());
}

void FudgetControl::DrawDrawableInstructions(const FudgetDrawInstructionList &area, const Rectangle &rect, const Color &tint)
{
    Rectangle r = rect;
    for (const auto item : area._list)
    {
        switch (item->_type)
        {
            case FudgetDrawInstructionType::DrawArea:
                DrawArea(((FudgetDrawInstructionDrawArea*)(item))->_draw_area, r, tint);
                break;
            case FudgetDrawInstructionType::DrawBorder:
                DrawBorder(((FudgetDrawInstructionDrawBorder*)(item))->_draw_border, r, tint);
                break;
            case FudgetDrawInstructionType::Padding:
                r = ((FudgetDrawInstructionPadding*)(item))->_padding.Padded(r);
                break;
            case FudgetDrawInstructionType::FillColor:
                FillRectangle(r, ((FudgetDrawInstructionColor*)(item))->_color * tint);
                break;
            case FudgetDrawInstructionType::Blur:
                DrawBlur(r, ((FudgetDrawInstructionFloat*)(item))->_value);
                break;
            case FudgetDrawInstructionType::InstructionList:
                DrawDrawableInstructions(*((FudgetDrawInstructionList*)(item)), r, tint);
                break;
        }
    }
}

void FudgetControl::DrawTextureInner(TextureBase *t, SpriteHandle sprite_handle, Float2 scale, Float2 offset, Rectangle rect, Color tint, FudgetImageAlignment alignment, bool point)
{
    Sprite sprite;
    if (t == nullptr && !sprite_handle.GetSprite(&sprite))
        return;

    Int2 siz = (t != nullptr ? t->Size() : sprite.Area.Size) * scale;
    AlignDrawRectangle(siz, alignment, rect);

    if (t != nullptr)
    {
        if ((alignment & FudgetImageAlignment::Tiled) == FudgetImageAlignment::Tiled)
        {
            if (!point)
                DrawTextureTiled(t->GetTexture(), t->Size() * scale, offset, rect, tint);
            else
                DrawTexturePointTiled(t->GetTexture(), t->Size() * scale, offset, rect, tint);
        }
        else
        {
            rect.Location += offset;
            if (!point)
                DrawTexture(t->GetTexture(), rect, tint);
            else
                DrawTexturePoint(t->GetTexture(), rect, tint);
        }
        return;
    }

    if ((alignment & FudgetImageAlignment::Tiled) == FudgetImageAlignment::Tiled)
    {
        if (!point)
            DrawSpriteTiled(sprite_handle, sprite.Area.Size * scale, offset, rect, tint);
        else
            DrawSpritePointTiled(sprite_handle, sprite.Area.Size * scale, offset, rect, tint);
    }
    else
    {
        rect.Location += offset;
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

void FudgetControl::Draw9SlicingPrecalculatedInner(TextureBase *t, SpriteHandle sprite_handle, Rectangle rect, const FudgetPadding &borderWidths, const Color &color, FudgetImageAlignment alignment, bool point)
{
    if (t == nullptr && !sprite_handle.IsValid())
        return;

    Int4 border;
    Float4 borderUV;
    Float2 siz = t != nullptr ? t->Size() : sprite_handle.Atlas->GetSprite(sprite_handle.Index).Area.Size;
    border = borderWidths.AsInt4();
    borderUV = Float4(border.X / siz.X, border.Y / siz.X, border.Z / siz.Y, border.W / siz.Y);

    AlignDrawRectangle(siz, alignment, rect);

    if (t != nullptr)
    {
        if (!point)
            Draw9SlicingTexture(t, rect, border, borderUV, color);
        else
            Draw9SlicingTexturePoint(t, rect, border, borderUV, color);
        return;
    }

    if (!point)
        Draw9SlicingSprite(sprite_handle, rect, border, borderUV, color);
    else
        Draw9SlicingSpritePoint(sprite_handle, rect, border, borderUV, color);

}

void FudgetControl::AlignDrawRectangle(Int2 tex_size, FudgetImageAlignment alignment, Rectangle &rect)
{
    if (tex_size.X == 0 || tex_size.Y == 0)
        return;

    float w = rect.GetWidth();
    float h = rect.GetHeight();
    Float2 ratio = 1.f;
    bool h_stretch = (alignment & FudgetImageAlignment::StretchHorz) == FudgetImageAlignment::StretchHorz;
    bool v_stretch = (alignment & FudgetImageAlignment::StretchVert) == FudgetImageAlignment::StretchVert;
    bool h_shrink = (alignment & FudgetImageAlignment::ShrinkHorz) == FudgetImageAlignment::ShrinkHorz;
    bool v_shrink = (alignment & FudgetImageAlignment::ShrinkVert) == FudgetImageAlignment::ShrinkVert;

    bool x_change = (h_stretch && tex_size.X < w) || (h_shrink && tex_size.X > w);
    bool y_change = (v_stretch && tex_size.Y < h) || (v_shrink && tex_size.Y > h);

    if ((alignment & FudgetImageAlignment::KeepAspectRatio) == FudgetImageAlignment::KeepAspectRatio)
    {
        if (x_change || y_change)
        {
            if (!x_change || !y_change)
            {
                ratio.X = ratio.Y = x_change ? w / (float)tex_size.X : h / (float)tex_size.Y;

                if (!x_change && h_shrink && tex_size.X * ratio.X > w)
                {
                    x_change = true;
                    ratio.X = ratio.Y = w / (tex_size.X * ratio.X);
                }

                if (!y_change && v_shrink && tex_size.Y * ratio.Y > h)
                {
                    y_change = true;
                    ratio.Y = ratio.X = h / (tex_size.Y * ratio.Y);
                }
            }
            else
                ratio.X = ratio.Y = Math::Min(w / (float)tex_size.X, h / (float)tex_size.Y);
        }
    }
    else
    {
        if (x_change)
            ratio.X = w / (float)tex_size.X;
        if (y_change)
            ratio.Y = h / (float)tex_size.Y;
    }

    tex_size = Int2(int(ratio.X * tex_size.X), int(ratio.Y * tex_size.Y));

    if (x_change || int(alignment & FudgetImageAlignment::StretchHorz) == 0)
        rect.Location.X += float((rect.Size.X - tex_size.X) * 0.5);
    else if ((alignment & FudgetImageAlignment::RightAlign) == FudgetImageAlignment::RightAlign)
        rect.Location.X += rect.Size.X - tex_size.X;
    rect.Size.X = (float)tex_size.X;

    if (y_change || int(alignment & FudgetImageAlignment::StretchVert) == 0)
        rect.Location.Y += float((rect.Size.Y - tex_size.Y) * 0.5);
    else if ((alignment & FudgetImageAlignment::BottomAlign) == FudgetImageAlignment::BottomAlign)
        rect.Location.Y += rect.Size.Y - tex_size.Y;
    rect.Size.Y = (float)tex_size.Y;

}

bool FudgetControl::IsPositionChangePermitted() const
{
    if (!_parent)
        return true;
    return _parent->IsControlPositionChangePermitted(this);
}

bool FudgetControl::IsDirectSizeChangePermitted() const
{
    return _parent == nullptr || _parent->IsControlDirectSizeChangePermitted(this);
}

void FudgetControl::LayoutUpdate(Int2 pos, Int2 size)
{
    if (pos != _pos)
    {
        _pos = pos;
        SetState(FudgetControlState::PositionUpdated, true);
    }
    if (size != _size)
    {
        _size = size;
        SetState(FudgetControlState::SizeUpdated, true);
    }
}

void FudgetControl::CreateClassNames()
{
    if (_class_names.Count() > 0)
        return;

    auto thisclass = GetClass();
    StringAnsiView class_name = thisclass->GetFullName();
    while (thisclass != nullptr && class_name != "Object")
    {
#if USE_EDITOR
        FudgetThemes::SetRuntimeUse(IsInRunningGame());
#endif
        _class_names.Add(class_name.ToString());
        thisclass = thisclass->GetBaseClass();
        if (thisclass != nullptr)
            class_name = thisclass->GetName();
    }
}

void FudgetControl::RegisterStylePainterInternal(FudgetPartPainter *painter, API_PARAM(Ref) const FudgetPartPainterMapping &mapping)
{
    painter->_owner = this;
    _painters.Add(painter);
    painter->DoInitialize(this, mapping);
}

void FudgetControl::UnregisterStylePainterInternal(FudgetPartPainter *painter)
{
    if (painter == nullptr)
        return;

    if (painter->GetOwner() != this)
        LOG(Warning, "Trying to delete a painter object not created for this control.");
    else
        _painters.Remove(painter);

    Array<FudgetDrawable*> to_delete;
    for (const auto &p : _drawables)
    {
        if (p.Value == painter)
        {
            to_delete.Add(p.Key);
            Delete(p.Key);
        }
    }

    for (auto d : to_delete)
        _drawables.RemoveValue(painter);
}

void FudgetControl::RegisterDrawable(FudgetPartPainter *drawable_owner, FudgetDrawable *drawable)
{
    _drawables.Add(drawable, drawable_owner);
}


// FudgetControlDataConsumerProxy


FudgetControlDataConsumerProxy::FudgetControlDataConsumerProxy(const SpawnParams &params) : Base(params), _owner(nullptr)
{

}
