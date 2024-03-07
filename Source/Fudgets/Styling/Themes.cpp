#include "Themes.h"
#include "../MarginStructs.h"
#include "StyleStructs.h"
#include "PartPainters.h"
#include "DrawableBuilder.h"
#include "../Controls/Button.h"

#include "Engine/Core/Math/Color.h"
#include "Engine/Content/Content.h"
#include "Engine/Content/Assets/Texture.h"
#include "Engine/Core/Log.h"
#include "Engine/Scripting/Scripting.h"
#include "Engine/Scripting/ScriptingObject.h"

#if USE_EDITOR
bool FudgetThemes::_runtime_use = false;
FudgetThemes::Data* FudgetThemes::_edittime_data = nullptr;
FudgetThemes::Data* FudgetThemes::_runtime_data = nullptr;
bool FudgetThemes::_edittime_initialized = false;
#endif
bool FudgetThemes::_initialized = false;
FudgetThemes::Data* FudgetThemes::_data = nullptr;


const String FudgetThemes::MainThemeName = TEXT("MainTheme");

// FudgetTheme


FudgetTheme::FudgetTheme() : Base(SpawnParams(Guid::New(), TypeInitializer))
{

}

FudgetTheme::FudgetTheme(const FudgetTheme &ori) : FudgetTheme()
{
	_resources = ori._resources;
}

bool FudgetTheme::GetResource(int res_id, API_PARAM(Out) Variant &result) const
{
	auto it = _resources.find(res_id);
	if (it == _resources.end())
	{
		result = Variant();
		return false;
	}

	if (it->second.Type.Type == VariantType::Structure)
	{
		HashSet<int> found;
		while (it->second.Type.Type == VariantType::Structure)
		{
			const FudgetResourceId *id = it->second.AsStructure<FudgetResourceId>();
			if (id == nullptr)
				break;

			if (found.Find(id->Id) != found.End())
			{
				result = Variant();
				return false;
			}
			int next = id->Id;
			found.Add(next);
			it = _resources.find(next);
			if (it == _resources.end())
			{
				result = Variant();
				return false;
			}
		}
	}

	result = it->second;


	return true;
}

void FudgetTheme::SetResource(int res_id, Variant value)
{
	_resources[res_id] = value;
}

void FudgetTheme::SetForwarding(int res_id, int forward_id)
{
	_resources[res_id] = StructToVariant(FudgetResourceId(forward_id));
}

FudgetTheme* FudgetTheme::Duplicate() const
{
	FudgetTheme *result = New<FudgetTheme>(*this);
	return result;
}


// FudgetThemes


void FudgetThemes::Initialize(bool in_game)
{
	if (in_game)
	{
		if (_initialized)
		{
			LOG(Error, "Initializing FudgetThemes twice.");
			return;
		}

		_initialized = true;
		_data = new Data;
#if USE_EDITOR
		_runtime_use = true;
		_runtime_data = _data;
#endif
	}

#if USE_EDITOR
	if (!in_game)
	{
		if (_edittime_initialized)
		{
			LOG(Error, "Initializing FudgetThemes twice in editor.");
			return;
		}
		_edittime_initialized = true;
		_runtime_use = false;
		_data = _edittime_data = new Data;
	}
#endif
}

void FudgetThemes::CreateDefaultThemesAndStyles()
{
	FudgetTheme *main_theme = New<FudgetTheme>();
	_data->_theme_map[MainThemeName] = main_theme;

	// Basic

	main_theme->SetResource((int)FudgetBasicPainterIds::Background, Color::White);
	main_theme->SetResource((int)FudgetBasicPainterIds::DisabledBackground, Color(.9f, .9f, .9f, 1.f));
	main_theme->SetResource((int)FudgetBasicPainterIds::FrameEdge, Color(.5f, .5f, .5f, 1.f));
	main_theme->SetResource((int)FudgetBasicPainterIds::FocusedFrameEdge, Color(.5f, .5f, .7f, 1.f));
	main_theme->SetResource((int)FudgetBasicPainterIds::FramePadding, FudgetPadding(3.f));

	main_theme->SetResource((int)FudgetBasicPainterIds::ButtonSurface, Color(.7f, .7f, .7f, 1.f));
	main_theme->SetResource((int)FudgetBasicPainterIds::ButtonHoveredSurface, Color(.85f, .85f, .85f, 1.f));
	main_theme->SetResource((int)FudgetBasicPainterIds::ButtonDisabledSurface, Color(.6f, .6f, .6f, 1.f));
	main_theme->SetResource((int)FudgetBasicPainterIds::ButtonFocusedSurface, Color(.75f, .75f, .85f, 1.f));
	main_theme->SetResource((int)FudgetBasicPainterIds::ButtonPressedSurface, Color(.65f, .65f, .65f, 1.f));
	main_theme->SetResource((int)FudgetBasicPainterIds::ButtonContentPressedOffset, Float2(0.f, 1.f));

	// Button

	/*
	FramePainter,
	FrameStyle,

	ContentPainter,
	ContentStyle,

	ContentImage,
	ContentHoveredImage,
	ContentPressedImage,
	ContentDisabledImage,
	*/
	main_theme->SetForwarding((int)FudgetButtonIds::Background, (int)FudgetBasicPainterIds::ButtonSurface);
	main_theme->SetForwarding((int)FudgetButtonIds::HoveredBackground, (int)FudgetBasicPainterIds::ButtonHoveredSurface);
	main_theme->SetForwarding((int)FudgetButtonIds::PressedBackground, (int)FudgetBasicPainterIds::ButtonPressedSurface);
	main_theme->SetForwarding((int)FudgetButtonIds::DownBackground, (int)FudgetBasicPainterIds::ButtonPressedSurface);
	main_theme->SetForwarding((int)FudgetButtonIds::DisabledBackground, (int)FudgetBasicPainterIds::ButtonDisabledSurface);
	main_theme->SetForwarding((int)FudgetButtonIds::Focusedbackground, (int)FudgetBasicPainterIds::ButtonFocusedSurface);
	main_theme->SetForwarding((int)FudgetButtonIds::ContentPadding, (int)FudgetBasicPainterIds::FramePadding);

	main_theme->SetForwarding((int)FudgetButtonIds::ContentPressedOffset, (int)FudgetBasicPainterIds::ButtonContentPressedOffset);

}


void FudgetThemes::Uninitialize(bool in_game)
{
#if USE_EDITOR
	if (!in_game)
	{
		if (!_edittime_initialized)
		{
			LOG(Error, "Trying to uninitialize FudgetThemes in editor when it wasn't initialized.");
			return;
		}

		_runtime_use = false;
		_data = _edittime_data;
		_edittime_initialized = false;
	}
	else
	{
		_runtime_use = true;
		_data = _runtime_data;
	}
#endif
	if (in_game)
	{
		if (!_initialized)
		{
			LOG(Error, "Trying to uninitialize FudgetThemes when it wasn't initialized.");
			return;
		}
		_initialized = false;
	}


	//for (const auto &st : _data->_style_map)
	//	if (st.Value != nullptr)
	//		Delete(st.Value);
	_data->_style_map.ClearDelete();
	
	//for (auto p : _data->_theme_map)
	//	if (p.Value != nullptr)
	//		Delete(p.Value);
	_data->_theme_map.ClearDelete();

	_data->_font_asset_map.Clear();

	for (auto p : _data->_style_area_list)
		delete p;
	_data->_style_area_list.clear();

#if USE_EDITOR
	if (!in_game)
	{
		delete(_edittime_data);
		_edittime_data = nullptr;
		_runtime_use = true;
		_data = _runtime_data;
	}
	else
	{
		delete(_runtime_data);
		_runtime_data = nullptr;
		_runtime_use = false;
		_data = _edittime_data;
	}
#else
	delete(_data);
	_data = nullptr;
#endif
}


bool FudgetThemes::RegisterFontAsset(int font_id, FontAsset *asset)
{
	if (font_id < 0)
		return false;
	auto it = _data->_font_asset_map.Find(font_id);
	if (it != _data->_font_asset_map.End())
		return false;
	_data->_font_asset_map[font_id] = asset;
	return true;
}

FontAsset* FudgetThemes::GetFontAsset(int font_id)
{
	if (font_id < 0)
		return nullptr;
	auto it = _data->_font_asset_map.Find(font_id);
	if (it == _data->_font_asset_map.End())
		return nullptr;
	return it->Value;
}


FudgetTheme* FudgetThemes::GetTheme(const String &theme_name)
{
	if (theme_name.IsEmpty())
		return nullptr;
	auto it = _data->_theme_map.Find(theme_name);
	if (it == _data->_theme_map.End())
		return nullptr;
	return it->Value;
}

FudgetTheme* FudgetThemes::DuplicateTheme(const String &src_name, const String &dest_name)
{
	if (src_name.IsEmpty() || dest_name.IsEmpty())
		return nullptr;
	auto it_src = _data->_theme_map.Find(src_name);
	auto it_dst = _data->_theme_map.Find(dest_name);
	if (it_src == _data->_theme_map.End() || it_dst != _data->_theme_map.End())
		return nullptr;

	FudgetTheme *theme = it_src->Value->Duplicate();
	_data->_theme_map[dest_name] = theme;
	return theme;
}

FudgetStyle* FudgetThemes::CreateStyle(const String &style_name)
{
	if (style_name.IsEmpty() || GetStyle(style_name) != nullptr)
		return nullptr;

	FudgetStyle *style = New<FudgetStyle>();
	style->_name = style_name;
	style->_owned = false;
	_data->_style_map[style_name] = style;
	return style;
}

FudgetStyle* FudgetThemes::CreateOrGetStyle(const String &style_name)
{
	if (style_name.IsEmpty())
		return nullptr;

	FudgetStyle *style = CreateStyle(style_name);
	if (style != nullptr)
		return style;

	return GetStyle(style_name);
}

FudgetStyle* FudgetThemes::GetStyle(const String &style_name)
{
	if (style_name.IsEmpty())
		return nullptr;
	auto it = _data->_style_map.Find(style_name);
	if (it == _data->_style_map.end())
		return nullptr;
	return it->Value;
}

FudgetStyle* FudgetThemes::FindMatchingStyle(const Array<String> &class_names, const String &styling_name)
{
	FudgetStyle *first_class_style = nullptr;

	Array<String> names;
	styling_name.Split(Char('/'), names);

	for (int ix = 0, siz = class_names.Count(); ix < siz; ++ix)
	{
		FudgetStyle *style = GetStyle(class_names[ix]);
		if (style != nullptr)
		{
			if (first_class_style == nullptr)
				first_class_style = style;
			if (!styling_name.IsEmpty() && !names.IsEmpty())
			{
				style = style->GetOwnedStyle(names[0]);
				for (int ix = 1, cnt = names.Count(); style != nullptr && ix < cnt; ++ix)
					style = style->GetOwnedStyle(names[ix]);
			}
			if (style != nullptr)
				return style;
		}
	}
	if (!styling_name.IsEmpty() && !names.IsEmpty())
	{
		FudgetStyle *style = GetStyle(names[0]);
		for (int ix = 1, cnt = names.Count(); style != nullptr && ix < cnt; ++ix)
			style = style->GetOwnedStyle(names[ix]);
		if (style != nullptr)
			return style;
	}
	if (first_class_style != nullptr)
		return first_class_style;
	return nullptr;
}

FudgetPartPainter* FudgetThemes::CreatePainter(const StringAnsi &painter_name)
{
	if (painter_name.IsEmpty())
		return nullptr;

	const ScriptingTypeHandle type = Scripting::FindScriptingType(painter_name);
	if (!type)
		return nullptr;

	if (!FudgetPartPainter::TypeInitializer.IsAssignableFrom(type))
		return nullptr;

	return (FudgetPartPainter*)type.GetType().Script.Spawn(ScriptingObjectSpawnParams(Guid::New(), type));
}

int FudgetThemes::RegisterDrawInstructionList(FudgetDrawInstructionList *style_area)
{
	if (style_area == nullptr || IsDrawInstructionListRegistered(style_area))
		return -1;

	_data->_style_area_list.push_back(style_area);
	return (int)_data->_style_area_list.size() - 1;
}

bool FudgetThemes::IsDrawInstructionListRegistered(FudgetDrawInstructionList *arealist)
{
	return std::find(_data->_style_area_list.begin(), _data->_style_area_list.end(), arealist) != _data->_style_area_list.end();
}

FudgetDrawInstructionList* FudgetThemes::GetDrawInstructionList(int arealist_index)
{
	if (arealist_index < 0 || arealist_index >= _data->_style_area_list.size())
		return nullptr;
	return _data->_style_area_list[arealist_index];
}

#if USE_EDITOR

void FudgetThemes::SetRuntimeUse(bool value)
{
	if (value)
		_data = _runtime_data;
	else
		_data = _edittime_data;
	_runtime_use = value;
}

#endif

