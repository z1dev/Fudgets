#include "SimpleButtonPainter.h"
#include "Engine/Core/Math/Vector2.h"
#include "../../Control.h"


FudgetSimpleButtonPainter::FudgetSimpleButtonPainter() : Base()
{

}

void FudgetSimpleButtonPainter::Draw(FudgetControl *control, FudgetSimpleButtonDrawInfo &info)
{
	Color Dark = Color(0.6f, 0.6f, 0.6f, 1.0f);
	Color Light = Color(1.0f);
	Color Normal = Color(0.8f);

	float TimeToAnimate = 0.3f;

	if (info.MouseIsOver)
	{
		if (!info.MousePressed)
		{
			if (info.DeltaTime != -1.f)
				info._hover_time = Math::Min(info._hover_time + info.DeltaTime, TimeToAnimate);
			else
				info._hover_time = TimeToAnimate;
		}
		else if (info._hover_time != 0.0f)
			info._hover_time = 0.0f;
	}
	else if (info._hover_time != 0.0f)
	{
		if (!info.MousePressed && info.DeltaTime != -1.f)
		{
			info._hover_time = Math::Max(info._hover_time - info.DeltaTime, 0.0f);
		}
		else
			info._hover_time = 0.0f;
	}

	Color draw_color = info.MouseIsOver && info.MousePressed ? Dark : info.MouseIsOver ? Light : Normal;
	if (info._hover_time != 0.0f)
	{
		draw_color = Normal * (TimeToAnimate - info._hover_time) / TimeToAnimate + Light * (info._hover_time) / TimeToAnimate;
	}

	control->FillRectangle(Float2(0.f), control->GetSize(), draw_color);

	if (control->GetFocused())
	{
		Color BorderColor = Color(0.4f, 0.7f, 0.5f, 1.0f);
		control->DrawRectangle(Float2(0.f), control->GetSize(), BorderColor, 4.5f);
	}
	info.DeltaTime = 0.0f;
}

