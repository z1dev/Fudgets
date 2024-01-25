#include "SimpleButtonPainter.h"
#include "../../Control.h"
#include "../Theme.h"

#include "Engine/Core/Math/Vector2.h"

const FudgetToken FudgetSimpleButtonPainter::hover_token = FudgetTheme::RegisterToken(TEXT("SimpleButtonHoverTime"));

FudgetSimpleButtonPainter::FudgetSimpleButtonPainter() : Base()
{
	
}

void FudgetSimpleButtonPainter::Draw(FudgetPainterPropertyProvider *provider)
{
	Color Dark = Color(0.6f, 0.6f, 0.6f, 1.0f);
	Color Light = Color(1.0f);
	Color Normal = Color(0.8f);

	float TimeToAnimate = 0.3f;

	bool mouse_is_over = false;
	provider->GetElementBoolProperty(FudgetTheme::MouseHoverToken, mouse_is_over);
	bool mouse_is_pressed = false;
	provider->GetElementBoolProperty(FudgetTheme::LeftButtonPressedToken, mouse_is_pressed);
	float delta_time = provider->GetDeltaTime();

	float hover_time = 0.0f;
	provider->GetStoredFloat(hover_token, hover_time);
	if (mouse_is_over)
	{
		if (!mouse_is_pressed)
		{
			if (delta_time != -1.f)
				hover_time = Math::Min(hover_time + delta_time, TimeToAnimate);
			else
				hover_time = TimeToAnimate;
		}
		else if (hover_time != 0.0f)
			hover_time = 0.0f;
	}
	else if (hover_time != 0.0f)
	{
		if (!mouse_is_pressed && delta_time >= 0.0f)
		{
			hover_time = Math::Max(hover_time - delta_time, 0.0f);
		}
		else
			hover_time = 0.0f;
	}

	Color draw_color = mouse_is_over && mouse_is_pressed ? Dark : mouse_is_over ? Light : Normal;
	if (hover_time != 0.0f)
	{
		draw_color = Normal * (TimeToAnimate - hover_time) / TimeToAnimate + Light * (hover_time) / TimeToAnimate;
	}

	provider->SetStoredFloat(hover_token, hover_time);

	auto control = provider->GetSourceControl();
	control->FillRectangle(Float2(0.f), control->GetSize(), draw_color);

	if (control->GetFocused())
	{
		Color BorderColor = Color(0.4f, 0.7f, 0.5f, 1.0f);
		control->DrawRectangle(Float2(0.f), control->GetSize(), BorderColor, 4.5f);
	}
}

