#include "ElementPainter.h"
#include "Style.h"
#include "../Control.h"
#include "Themes.h"

#include "Engine/Core/Log.h"

//FudgetElementPainter::FudgetElementPainter(String name) : FudgetElementPainter(FudgetThemes::RegisterToken(name))
//{
//}
//
//FudgetElementPainter::FudgetElementPainter(FudgetToken name_token) : Base(SpawnParams(Guid::New(), TypeInitializer)),
//	_painter_name(name_token)
//{
//	InitializeInternal(_painter_name);
//}
//
//void FudgetElementPainter::InitializeInternal(FudgetToken name_token)
//{
//	ASSERT(name_token == _painter_name);
//
//	if (!FudgetThemes::RegisterElementPainter(_painter_name, this))
//		LOG(Error, "Invalid token for FudgetElementPainter. Element painter wasn't registered.");
//}

FudgetElementPainter::FudgetElementPainter(const SpawnParams &params) : Base(params)
{
}

FudgetElementPainter::~FudgetElementPainter()
{
}

void FudgetElementPainter::DrawWithPainter(FudgetControl *control, FudgetPainterPropertyProvider *provider, FudgetToken painter_token)
{
	FudgetTheme *theme = control->GetActiveTheme();
	FudgetStyle *style = control->GetActiveStyle();
	if (theme != nullptr && style != nullptr)
	{
		FudgetElementPainter *painter = style->GetElementPainter(control->GetActiveTheme(), painter_token);
		if (painter != nullptr)
			painter->Draw(control, provider);
	}
}

FudgetPainterPropertyProvider::FudgetPainterPropertyProvider(const SpawnParams &params) : Base(params), _delta_time(-1.0f)
{

}

void FudgetPainterPropertyProvider::AddDeltaTime(float value)
{
	_delta_time += value;
}


bool FudgetPainterPropertyProvider::GetStoredValue(FudgetToken token, API_PARAM(Out) Variant &result) const
{
	auto it = _stored_values.find(token);
	if (it == _stored_values.end())
		return false;
	result = it->second;
	return true;
}

void FudgetPainterPropertyProvider::SetStoredValue(FudgetToken token, const Variant &value)
{
	_stored_values[token] = value;
}

bool FudgetPainterPropertyProvider::GetStoredFloat(FudgetToken token, API_PARAM(Out) float &result) const
{
	auto it = _stored_values.find(token);
	if (it == _stored_values.end())
		return false;
	if (it->second.Type.Type != VariantType::Float)
		return false;
	result = it->second.AsFloat;
	return true;
}

void FudgetPainterPropertyProvider::SetStoredFloat(FudgetToken token, float value)
{
	_stored_values[token] = value;
}
