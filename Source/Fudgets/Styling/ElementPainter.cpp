#include "ElementPainter.h"
#include "Style.h"
#include "../Control.h"

FudgetElementPainter::FudgetElementPainter() : Base(SpawnParams(Guid::New(), TypeInitializer))
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

FudgetPainterPropertyProvider::FudgetPainterPropertyProvider() : Base(SpawnParams(Guid::New(), TypeInitializer)), _delta_time(-1.0f)
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
