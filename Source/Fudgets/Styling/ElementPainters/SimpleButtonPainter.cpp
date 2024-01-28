#include "SimpleButtonPainter.h"
#include "../../Control.h"
#include "../Themes.h"
#include "../Style.h"
#include "../../MarginStructs.h"

#include "Engine/Core/Math/Vector2.h"

const FudgetToken FudgetButtonBackgroundPainter::hover_token = FudgetThemes::RegisterToken(TEXT("SimpleButtonHoverTime"));

const FudgetToken FudgetSimpleButtonPainter::simple_btn_bg_token = FudgetThemes::RegisterToken(TEXT("SimpleButtonBackground"));


FudgetButtonBackgroundPainter::FudgetButtonBackgroundPainter() : Base()
{

}

void FudgetButtonBackgroundPainter::Draw(FudgetControl *control, FudgetPainterPropertyProvider *provider)
{
	Color Dark = Color::Black;
	control->GetStyleColor(FudgetThemes::ButtonBackgroundPressedToken, Dark);
	Color Light = Color::White;
	control->GetStyleColor(FudgetThemes::ButtonBackgroundHoverToken, Light);

	bool is_area = false;
	FudgetFillAreaSettings area;
	Color Normal = Color::LightGray;
	if (!control->GetStyleColor(FudgetThemes::ButtonBackgroundNormalToken, Normal))
	{
		Variant var;
		if (control->GetStyleValue(FudgetThemes::ButtonBackgroundNormalToken, var))
		{
			area = *var.AsStructure<FudgetFillAreaSettings>();
			if (area.RectType == FudgetRectType::Color)
			{
				Normal = area.Color;
			}
			else
				is_area = true;
		}
	}

	float TimeToAnimate = 0.0f;
	control->GetStyleFloat(FudgetThemes::ButtonHoverAnimationTimeToken, TimeToAnimate);

	// Fetching values from the control

	bool mouse_is_over = false;
	provider->GetElementBoolProperty(FudgetThemes::MouseHoverToken, mouse_is_over);
	bool mouse_is_pressed = false;
	provider->GetElementBoolProperty(FudgetThemes::LeftButtonPressedToken, mouse_is_pressed);
	float delta_time = provider->GetDeltaTime();

	// Custom stored time used to animate the button color

	float hover_time = 0.0f;
	provider->GetStoredFloat(hover_token, hover_time);

	// Calculating hover time that will be used to set the color of the button.

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

	if (!is_area)
	{
		// Set the color to draw the button

		Color draw_color = mouse_is_over && mouse_is_pressed ? Dark : mouse_is_over ? Light : Normal;

		// Changing color if hover time is not 0. It represents the percent of "how much" the mouse is over the button

		if (hover_time != 0.0f)
			draw_color = Normal * (TimeToAnimate - hover_time) / TimeToAnimate + Light * (hover_time) / TimeToAnimate;

		// Storing new value of hover time

		provider->SetStoredFloat(hover_token, hover_time);

		// The drawing part

		control->FillRectangle(Float2(0.f), control->GetSize(), draw_color);
	}
	else
	{
		control->DrawFillArea(area, Float2(0.f), control->GetSize());
	}
}


FudgetSimpleButtonPainter::FudgetSimpleButtonPainter() : Base()
{
	
}

void FudgetSimpleButtonPainter::Draw(FudgetControl *control, FudgetPainterPropertyProvider *provider)
{
	DrawWithPainter(control, provider, simple_btn_bg_token);

	if (control->GetFocused())
	{
		Color BorderColor = Color::Black;
		control->GetStyleColor(FudgetThemes::ButtonFocusRectangleColorToken, BorderColor);
		float borderWidth = 1.0f;
		control->GetStyleFloat(FudgetThemes::ButtonFocusRectangleWidthToken, borderWidth);
		control->DrawRectangle(Float2(0.f), control->GetSize(), BorderColor, borderWidth);
	}
}

