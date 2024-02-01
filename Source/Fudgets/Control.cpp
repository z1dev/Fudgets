#include "Control.h"
#include "Container.h"
#include "GUIRoot.h"
#include "Fudget.h"
#include "Styling/Themes.h"
#include "Styling/ElementPainter.h"
#include "MarginStructs.h"

#include "Engine/Render2D/Render2D.h"
#include "Engine/Render2D/TextLayoutOptions.h"
#include "Engine/Core/Math/Rectangle.h"
#include "Engine/Scripting/Scripting.h"
#include "Engine/Core/Log.h"
#include "Engine/Scripting/ManagedCLR/MClass.h"
#include "Engine/Scripting/Types.h"

FudgetGUIRoot* FudgetControl::_guiRoot = nullptr;


FudgetControl::FudgetControl(const SpawnParams &params) : ScriptingObject(params),
	_parent(nullptr), _index(-1), _flags(FudgetControlFlags::None), _pos(0.f), _size(0.0f), _hint_size(120.f, 60.0f), _min_size(30.f, 30.f),
	_max_size(-1.f, -1.f), _cached_global_to_local_translation(0.f), _g2l_was_cached(false), _changing(false), _updating_registered(false),
	/*_element_painter(nullptr),*/ _cached_painter(nullptr), _style(nullptr), _cached_style(nullptr), _theme_id(FudgetToken::Invalid),
	_cached_theme(nullptr)
{
}

FudgetControl::~FudgetControl()
{
	RegisterToUpdate(false);
}

void FudgetControl::Initialize()
{
	_flags = GetCreationFlags();
	if (HasAnyFlag(FudgetControlFlags::RegisterToUpdates))
		RegisterToUpdate(true);
}

void FudgetControl::Draw()
{
	_g2l_was_cached = false;

	if (!_updating_registered)
		return;

	auto provider = GetPainterPropertyProvider();
	if (provider != nullptr)
		provider->ResetDeltaTime();
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
	{
		_parent->RemoveChild(_index);
	}
	_parent = value;
	_index = -1;
	if (_parent != nullptr)
	{
		if (order >= _parent->GetChildCount())
			order = -1;

		_index = _parent->AddChild(this, order);
	}
	_changing = false;
}

void FudgetControl::SetIndexInParent(int value)
{
	if (_changing || _parent == nullptr || value == _index || value < 0 || value >= _parent->GetChildCount())
		return;
	_changing = true;
	if (_parent->MoveChildToIndex(_index, value))
		_index = value;
	_changing = false;
}

void FudgetControl::SetName(String value)
{
	// TODO: event on name change?
	_name = value;
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
	if (value == _updating_registered)
		return;

	auto gui = GetGUIRoot();
	if (gui == nullptr)
		return;

	if (gui->RegisterControlUpdate(this, value))
	{
		_updating_registered = value;

		if (!value)
		{
			auto provider = GetPainterPropertyProvider();
			if (provider != nullptr)
			{
				provider->ResetDeltaTime();
				provider->AddDeltaTime(-1.0f);
			}
		}
		else
		{
			auto provider = GetPainterPropertyProvider();
			if (provider != nullptr)
				provider->ResetDeltaTime();
		}
	}
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

void FudgetControl::CacheGlobalToLocal() const
{
	if (_g2l_was_cached)
		return;
	_g2l_was_cached = true;
	_cached_global_to_local_translation = GlobalToLocal(Float2(0.f));
}

void FudgetControl::InvalidateGlobalToLocalCache() const
{
	_g2l_was_cached = false;
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
		RegisterToUpdate((_flags & FudgetControlFlags::RegisterToUpdates) == FudgetControlFlags::RegisterToUpdates);
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

void FudgetControl::OnUpdate(float delta_time)
{
	auto ppp = GetPainterPropertyProvider();
	if (ppp != 0)
		ppp->AddDeltaTime(delta_time);
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

bool FudgetControl::GetFocused() const
{
	auto root = GetGUIRoot();
	if (root == nullptr)
		return false;
	return root->GetFocusedControl() == this;
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

void FudgetControl::FillRectangle(Float2 pos, Float2 size, Color color) const
{
	CacheGlobalToLocal();
	pos = CachedLocalToGlobal(pos);

	Render2D::FillRectangle(Rectangle(pos, size), color);
}

void FudgetControl::FillRectangle(const Rectangle &rect, Color color) const
{
	CacheGlobalToLocal();
	Render2D::FillRectangle(CachedLocalToGlobal(rect), color);
}

void FudgetControl::FillRectangle(const Rectangle& rect, const Color& color1, const Color& color2, const Color& color3, const Color& color4) const
{
	CacheGlobalToLocal();
	Render2D::FillRectangle(CachedLocalToGlobal(rect), color1, color2, color3, color4);
}

void FudgetControl::DrawRectangle(Float2 pos, Float2 size, Color color, float thickness) const
{
	CacheGlobalToLocal();
	pos = CachedLocalToGlobal(pos);

	Render2D::DrawRectangle(Rectangle(pos, size), color, color, color, color, thickness);
}

void FudgetControl::DrawRectangle(const Rectangle &rect, Color color, float thickness) const
{
	CacheGlobalToLocal();
	Render2D::DrawRectangle(LocalToGlobal(rect), color, color, color, color, thickness);
}

void FudgetControl::DrawRectangle(const Rectangle& rect, const Color& color1, const Color& color2, const Color& color3, const Color& color4, float thickness) const
{
	CacheGlobalToLocal();
	Render2D::DrawRectangle(CachedLocalToGlobal(rect), color1, color2, color3, color4, thickness);
}

void FudgetControl::Draw9SlicingTexture(TextureBase *t, const Rectangle &rect, const Float4 &border, const Float4 &borderUVs, const Color &color) const
{
	CacheGlobalToLocal();
	Render2D::Draw9SlicingTexture(t, CachedLocalToGlobal(rect), border, borderUVs, color);
}

void FudgetControl::Draw9SlicingTexturePoint(TextureBase *t, const Rectangle &rect, const Float4 &border, const Float4 &borderUVs, const Color &color) const
{
	CacheGlobalToLocal();
	Render2D::Draw9SlicingTexturePoint(t, CachedLocalToGlobal(rect), border, borderUVs, color);
}

void FudgetControl::Draw9SlicingSprite(const SpriteHandle& spriteHandle, const Rectangle &rect, const Float4 &border, const Float4 &borderUVs, const Color &color) const
{
	CacheGlobalToLocal();
	Render2D::Draw9SlicingSprite(spriteHandle, CachedLocalToGlobal(rect), border, borderUVs, color);
}

void FudgetControl::Draw9SlicingSpritePoint(const SpriteHandle& spriteHandle, const Rectangle &rect, const Float4 &border, const Float4 &borderUVs, const Color &color) const
{
	CacheGlobalToLocal();
	Render2D::Draw9SlicingSpritePoint(spriteHandle, CachedLocalToGlobal(rect), border, borderUVs, color);
}

void FudgetControl::Draw9SlicingPrecalculatedTexture(TextureBase *t, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color) const
{
	Float4 border;
	Float4 borderUV;
	Float2 siz = t->Size();
	border = Float4(borderWidths.Left, siz.X - borderWidths.Right, borderWidths.Top, siz.Y - borderWidths.Bottom);
	borderUV = Float4(border.X / siz.X, border.Y / siz.X, border.Z / siz.Y, border.W / siz.Y);

	Draw9SlicingTexture(t, rect, border, borderUV);
}

void FudgetControl::Draw9SlicingPrecalculatedTexturePoint(TextureBase *t, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color) const
{
	Float4 border;
	Float4 borderUV;
	Float2 siz = t->Size();
	border = Float4(borderWidths.Left, siz.X - borderWidths.Right, borderWidths.Top, siz.Y - borderWidths.Bottom);
	borderUV = Float4(border.X / siz.X, border.Y / siz.X, border.Z / siz.Y, border.W / siz.Y);

	Draw9SlicingTexturePoint(t, rect, border, borderUV);
}

void FudgetControl::Draw9SlicingPrecalculatedSprite(const SpriteHandle& spriteHandle, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color) const
{
	Float4 border;
	Float4 borderUV;
	Float2 siz = spriteHandle.Atlas->GetSprite(spriteHandle.Index).Area.Size;
	border = Float4(borderWidths.Left, siz.X - borderWidths.Right, borderWidths.Top, siz.Y - borderWidths.Bottom);
	borderUV = Float4(border.X / siz.X, border.Y / siz.X, border.Z / siz.Y, border.W / siz.Y);

	Draw9SlicingSprite(spriteHandle, rect, border, borderUV);
}

void FudgetControl::Draw9SlicingPrecalculatedSpritePoint(const SpriteHandle& spriteHandle, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color) const
{
	Float4 border;
	Float4 borderUV;
	Float2 siz = spriteHandle.Atlas->GetSprite(spriteHandle.Index).Area.Size;
	border = Float4(borderWidths.Left, siz.X - borderWidths.Right, borderWidths.Top, siz.Y - borderWidths.Bottom);
	borderUV = Float4(border.X / siz.X, border.Y / siz.X, border.Z / siz.Y, border.W / siz.Y);

	Draw9SlicingSpritePoint(spriteHandle, rect, border, borderUV);
}

void FudgetControl::DrawBezier(const Float2& p1, const Float2& p2, const Float2& p3, const Float2& p4, const Color& color, float thickness) const
{
	CacheGlobalToLocal();
	Render2D::DrawBezier(CachedLocalToGlobal(p1), CachedLocalToGlobal(p2), CachedLocalToGlobal(p3), CachedLocalToGlobal(p4), color, thickness);
}

void FudgetControl::DrawBlur(const Rectangle& rect, float blurStrength) const
{
	CacheGlobalToLocal();
	Render2D::DrawBlur(CachedLocalToGlobal(rect), blurStrength);
}

void FudgetControl::DrawLine(const Float2& p1, const Float2& p2, const Color& color, float thickness) const
{
	CacheGlobalToLocal();
	Render2D::DrawLine(CachedLocalToGlobal(p1), CachedLocalToGlobal(p2), color, color, thickness);
}

void FudgetControl::DrawLine(const Float2& p1, const Float2& p2, const Color& color1, const Color& color2, float thickness) const
{
	CacheGlobalToLocal();
	Render2D::DrawLine(CachedLocalToGlobal(p1), CachedLocalToGlobal(p2), color1, color2, thickness);
}

void FudgetControl::DrawMaterial(MaterialBase* material, const Rectangle& rect, const Color& color) const
{
	CacheGlobalToLocal();
	Render2D::DrawMaterial(material, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawSprite(const SpriteHandle& spriteHandle, const Rectangle& rect, const Color& color) const
{
	CacheGlobalToLocal();
	Render2D::DrawSprite(spriteHandle, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawSpritePoint(const SpriteHandle& spriteHandle, const Rectangle& rect, const Color& color) const
{
	CacheGlobalToLocal();
	Render2D::DrawSpritePoint(spriteHandle, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawText(Font* font, const StringView& text, const Color& color, const Float2& location, MaterialBase* customMaterial) const
{
	CacheGlobalToLocal();
	Render2D::DrawText(font, text, color, CachedLocalToGlobal(location), customMaterial);
}

void FudgetControl::DrawText(Font* font, const StringView& text, API_PARAM(Ref) const TextRange& textRange, const Color& color, const Float2& location, MaterialBase* customMaterial) const
{
	CacheGlobalToLocal();
	Render2D::DrawText(font, text, textRange, color, CachedLocalToGlobal(location), customMaterial);
}

void FudgetControl::DrawText(Font* font, const StringView& text, const Color& color, API_PARAM(Ref) const TextLayoutOptions& layout, MaterialBase* customMaterial) const
{
	CacheGlobalToLocal();
	TextLayoutOptions tmp = layout;
	tmp.Bounds = CachedLocalToGlobal(layout.Bounds);
	Render2D::DrawText(font, text, color, tmp, customMaterial);
}

void FudgetControl::DrawText(Font* font, const StringView& text, API_PARAM(Ref) const TextRange& textRange, const Color& color, API_PARAM(Ref) const TextLayoutOptions& layout, MaterialBase* customMaterial) const
{
	CacheGlobalToLocal();
	TextLayoutOptions tmp = layout;
	tmp.Bounds = CachedLocalToGlobal(layout.Bounds);
	Render2D::DrawText(font, text, textRange, color, tmp, customMaterial);
}

void FudgetControl::DrawTexture(GPUTextureView* rt, const Rectangle& rect, const Color& color) const
{
	CacheGlobalToLocal();
	Render2D::DrawTexture(rt, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawTexture(GPUTexture* t, const Rectangle& rect, const Color& color) const
{
	CacheGlobalToLocal();
	Render2D::DrawTexture(t, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawTexture(TextureBase* t, const Rectangle& rect, const Color& color) const
{
	CacheGlobalToLocal();
	Render2D::DrawTexture(t, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawSpriteTiled(const SpriteHandle& spriteHandle, Float2 size, const Rectangle& rect, const Color& color) const
{
	DrawTiled(nullptr, spriteHandle, false, size, rect, color);
}

void FudgetControl::DrawSpritePointTiled(const SpriteHandle& spriteHandle, Float2 size, const Rectangle& rect, const Color& color) const
{
	DrawTiled(nullptr, spriteHandle, true, size, rect, color);
}

void FudgetControl::DrawTextureTiled(GPUTexture *t, Float2 size, const Rectangle& rect, const Color& color) const
{
	DrawTiled(t, SpriteHandle(), false, size, rect, color);
}


void FudgetControl::DrawTexturePointTiled(GPUTexture *t, Float2 size, const Rectangle& rect, const Color& color) const
{
	DrawTiled(t, SpriteHandle(), true, size, rect, color);
}

void FudgetControl::DrawTexturePoint(GPUTexture* t, const Rectangle& rect, const Color& color) const
{
	CacheGlobalToLocal();
	Render2D::DrawTexturePoint(t, CachedLocalToGlobal(rect), color);
}

void FudgetControl::DrawTexturedTriangles(GPUTexture* t, const Span<Float2>& vertices, const Span<Float2>& uvs) const
{
	CacheGlobalToLocal();
	Array<Float2> copy(vertices.Length());
	for (int ix = 0, siz = vertices.Length(); ix < siz; ++ix)
	{
		copy.Add(CachedLocalToGlobal(vertices[ix]));
	}

	Render2D::DrawTexturedTriangles(t, Span<Float2>(copy.Get(), copy.Count()), uvs);
}

void FudgetControl::DrawTexturedTriangles(GPUTexture* t, const Span<Float2>& vertices, const Span<Float2>& uvs, const Color& color) const
{
	CacheGlobalToLocal();
	Array<Float2> copy(vertices.Length());
	for (int ix = 0, siz = vertices.Length(); ix < siz; ++ix)
	{
		copy.Add(CachedLocalToGlobal(vertices[ix]));
	}

	Render2D::DrawTexturedTriangles(t, Span<Float2>(copy.Get(), copy.Count()), uvs);
}

void FudgetControl::DrawTexturedTriangles(GPUTexture* t, const Span<Float2>& vertices, const Span<Float2>& uvs, const Span<Color>& colors) const
{
	CacheGlobalToLocal();
	Array<Float2> copy(vertices.Length());
	for (int ix = 0, siz = vertices.Length(); ix < siz; ++ix)
	{
		copy.Add(CachedLocalToGlobal(vertices[ix]));
	}

	Render2D::DrawTexturedTriangles(t, Span<Float2>(copy.Get(), copy.Count()), uvs, colors);
}

void FudgetControl::DrawTexturedTriangles(GPUTexture* t, const Span<uint16>& indices, const Span<Float2>& vertices, const Span<Float2>& uvs, const Span<Color>& colors) const
{
	CacheGlobalToLocal();
	Array<Float2> copy(vertices.Length());
	for (int ix = 0, siz = vertices.Length(); ix < siz; ++ix)
	{
		copy.Add(CachedLocalToGlobal(vertices[ix]));
	}

	Render2D::DrawTexturedTriangles(t, indices, Span<Float2>(copy.Get(), copy.Count()), uvs, colors);
}

void FudgetControl::FillTriangles(const Span<Float2>& vertices, const Span<Color>& colors, bool useAlpha) const
{
	CacheGlobalToLocal();
	Array<Float2> copy(vertices.Length());
	for (int ix = 0, siz = vertices.Length(); ix < siz; ++ix)
	{
		copy.Add(CachedLocalToGlobal(vertices[ix]));
	}

	Render2D::FillTriangles(Span<Float2>(copy.Get(), copy.Count()), colors, useAlpha);
}

void FudgetControl::FillTriangle(const Float2& p0, const Float2& p1, const Float2& p2, const Color& color) const
{
	CacheGlobalToLocal();
	Render2D::FillTriangle(CachedLocalToGlobal(p0), CachedLocalToGlobal(p1), CachedLocalToGlobal(p2), color);
}

void FudgetControl::DrawFillArea(const FudgetFillAreaSettings &area, const Rectangle &rect) const
{
	if (area.AreaType == FudgetFillAreaType::Color)
	{
		FillRectangle(rect, area.Color);
		return;
	}

	//CacheGlobalToLocal();
	//Rectangle r = LocalToGlobal(rect);


	if (area.AreaType == FudgetFillAreaType::Texture)
	{
		DrawTextureTiled(area.Texture->GetTexture(), area.Texture->Size(), rect, area.Color);
		return;
	}
	if (area.AreaType == FudgetFillAreaType::TextureStretch)
	{
		DrawTexture(area.Texture, rect, area.Color);
		return;
	}
	if (area.AreaType == FudgetFillAreaType::TexturePoint)
	{
		DrawTexturePointTiled(area.Texture->GetTexture(), area.Texture->Size(), rect, area.Color);
		return;
	}
	if (area.AreaType == FudgetFillAreaType::TexturePointStretch)
	{
		DrawTexturePoint(area.Texture->GetTexture(), rect, area.Color);
		return;
	}
	if (area.AreaType == FudgetFillAreaType::Sprite)
	{
		Sprite sprite;
		if (area.SpriteHandle.GetSprite(&sprite))
			DrawSpriteTiled(area.SpriteHandle, sprite.Area.Size, rect, area.Color);
		return;
	}
	if (area.AreaType == FudgetFillAreaType::SpriteStretch)
	{
		DrawSprite(area.SpriteHandle, rect, area.Color);
		return;
	}
	if (area.AreaType == FudgetFillAreaType::SpritePoint)
	{
		Sprite sprite;
		if (area.SpriteHandle.GetSprite(&sprite))
			DrawSpritePointTiled(area.SpriteHandle, sprite.Area.Size, rect, area.Color);
		return;
	}
	if (area.AreaType == FudgetFillAreaType::SpritePointStretch)
	{
		DrawSpritePoint(area.SpriteHandle, rect, area.Color);
		return;
	}

	//Float4 border;
	//Float4 borderUV;
	//Float2 siz = 0.f;
	//if (area.Texture.Get() != nullptr)
	//	siz = area.Texture->Size();
	//else
	//	siz = area.SpriteHandle.Atlas->GetSprite(area.SpriteHandle.Index).Area.Size;
	//border = Float4(area.Borders9P.X, siz.X - area.Borders9P.Y, area.Borders9P.Z, siz.Y - area.Borders9P.W);
	//borderUV = Float4(border.X / siz.X, border.Y / siz.X, border.Z / siz.Y, border.W / siz.Y);

	if (area.AreaType == FudgetFillAreaType::Texture9)
	{
		Draw9SlicingPrecalculatedTexture(area.Texture, rect, FudgetPadding(area.Borders9P.X, area.Borders9P.Y, area.Borders9P.Z, area.Borders9P.W));
		//Draw9SlicingTexture(area.Texture, rect, border, borderUV);
		return;
	}
	if (area.AreaType == FudgetFillAreaType::Texture9Point)
	{
		Draw9SlicingPrecalculatedTexturePoint(area.Texture, rect, FudgetPadding(area.Borders9P.X, area.Borders9P.Y, area.Borders9P.Z, area.Borders9P.W));
		//Draw9SlicingTexturePoint(area.Texture, rect, border, borderUV);
		return;
	}

	if (area.AreaType == FudgetFillAreaType::Sprite9)
	{
		Draw9SlicingPrecalculatedSprite(area.SpriteHandle, rect, FudgetPadding(area.Borders9P.X, area.Borders9P.Y, area.Borders9P.Z, area.Borders9P.W));
		//Draw9SlicingSprite(area.SpriteHandle, rect, border, borderUV);
		return;
	}
	if (area.AreaType == FudgetFillAreaType::Texture9Point)
	{
		Draw9SlicingPrecalculatedSpritePoint(area.SpriteHandle, rect, FudgetPadding(area.Borders9P.X, area.Borders9P.Y, area.Borders9P.Z, area.Borders9P.W));
		//Draw9SlicingSpritePoint(area.SpriteHandle, rect, border, borderUV);
		return;
	}
}

void FudgetControl::DrawFillArea(const FudgetFillAreaSettings &area, Float2 pos, Float2 siz) const
{
	DrawFillArea(area, Rectangle(pos, siz));
}


void FudgetControl::ClearStyleCache(bool inherited)
{
	_cached_theme = nullptr;
	_cached_style = nullptr;
	_cached_painter = nullptr;
}

FudgetElementPainter* FudgetControl::GetElementPainter()
{
	//if (_element_painter != nullptr)
	//	return _element_painter;

	if (_cached_painter != nullptr)
		return _cached_painter;

	FudgetStyle *style = GetActiveStyle();
	if (style == nullptr)
		return nullptr;

	CreateClassTokens();
	_cached_painter = style->GetControlPainter(GetActiveTheme(), _class_token);

	return _cached_painter;
}

void FudgetControl::SetPainterPropertyProvider(FudgetPainterPropertyProvider *value)
{
	if (value == nullptr || value == _painter_provider)
		return;

	_painter_provider.reset(value);
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
	_cached_style = FudgetThemes::GetControlStyleOrDefault(_class_token);

	return _cached_style;
}

void FudgetControl::SetThemeId(FudgetToken value)
{
	if (_theme_id == value || FudgetThemes::GetTheme(value) == nullptr)
		return;

	_theme_id = value;
	ClearStyleCache(true);
}

FudgetTheme* FudgetControl::GetActiveTheme()
{
	if (_cached_theme != nullptr)
		return _cached_theme;

	if (_theme_id.IsValid())
		_cached_theme = FudgetThemes::GetTheme(_theme_id);
	else if (_parent != nullptr)
		_cached_theme = _parent->GetActiveTheme();
	if (_cached_theme == nullptr)
		_cached_theme = FudgetThemes::GetTheme(FudgetThemes::MainThemeToken);

	return _cached_theme;
}

bool FudgetControl::GetStyleValue(FudgetToken token, API_PARAM(Out) Variant &result)
{
	FudgetStyle *style = GetActiveStyle();
	if (style == nullptr)
		return false;

	Variant var;
	if (style->GetResourceValue(GetActiveTheme(), token, var))
	{
		result = var;
		return true;
		// TODO: cache if needed
	}

	return false;
}

bool FudgetControl::GetStyleColor(FudgetToken token, API_PARAM(Out) Color &result)
{
	FudgetStyle *style = GetActiveStyle();
	if (style == nullptr)
		return false;
	Variant var;
	if (style->GetResourceValue(GetActiveTheme(), token, var))
	{
		if (var.Type.Type == VariantType::Color)
		{
			result = var.AsColor();
			return true;
		}
		// TODO: cache if needed
	}
	return false;
}

bool FudgetControl::GetStyleFloat(FudgetToken token, API_PARAM(Out) float &result)
{
	FudgetStyle *style = GetActiveStyle();
	if (style == nullptr)
		return false;
	Variant var;
	if (style->GetResourceValue(GetActiveTheme(), token, var))
	{
		if (var.Type.Type == VariantType::Float)
		{
			result = var.AsFloat;
			return true;
		}
		// TODO: cache if needed
	}
	return false;
}

bool FudgetControl::GetStyleFillSettings(FudgetToken token, API_PARAM(Out) FudgetFillAreaSettings &result)
{
	FudgetStyle *style = GetActiveStyle();
	if (style == nullptr)
		return false;
	Variant var;
	if (style->GetResourceValue(GetActiveTheme(), token, var))
	{
		if (var.Type.Type == VariantType::Color)
		{
			result.Color = var.AsColor();
			result.AreaType = FudgetFillAreaType::Color;
			return true;
		}

		const FudgetFillAreaSettings *ptr = var.AsStructure<FudgetFillAreaSettings>();
		if (ptr == nullptr)
			return false;

		result = *ptr;
		return true;
		// TODO: cache if needed
	}
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
		SetParent(parent);
	}
}

void FudgetControl::DrawTiled(GPUTexture *t, SpriteHandle sprite_handle, bool point, Float2 size, const Rectangle& rect, const Color& color) const
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

	Rectangle r = LocalToGlobal(rect);
	Render2D::PushClip(r);

	float posx = r.Location.X;
	float posy = r.Location.Y;
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
		posx = r.Location.X;
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
		_class_token.Add(FudgetThemes::RegisterToken(class_name.ToString()));
		thisclass = thisclass->GetBaseClass();
		if (thisclass != nullptr)
			class_name = thisclass->GetName();
	}
}
