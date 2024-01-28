#include "SimpleButtonPainter.h"
#include "../../Control.h"
#include "../Themes.h"
#include "../Style.h"

#include "Engine/Core/Math/Vector2.h"

const FudgetToken FudgetSimpleButtonPainter::hover_token = FudgetThemes::RegisterToken(TEXT("SimpleButtonHoverTime"));


FudgetSimpleButtonPainter::FudgetSimpleButtonPainter() : Base()
{
	
}

void FudgetSimpleButtonPainter::Draw(FudgetPainterPropertyProvider *provider)
{
	FudgetControl *control = provider->GetSourceControl();

	Color Dark = control->GetStyleColor(FudgetThemes::ButtonBackgroundPressedToken);
	Color Light = control->GetStyleColor(FudgetThemes::ButtonBackgroundHoverToken);
	Color Normal = control->GetStyleColor(FudgetThemes::ButtonBackgroundNormalToken);

	float TimeToAnimate = control->GetStyleFloat(FudgetThemes::ButtonHoverAnimationTimeToken);

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

	// Set the color to draw the button

	Color draw_color = mouse_is_over && mouse_is_pressed ? Dark : mouse_is_over ? Light : Normal;

	// Changing color if hover time is not 0. It represents the percent of "how much" the mouse is over the button

	if (hover_time != 0.0f)
		draw_color = Normal * (TimeToAnimate - hover_time) / TimeToAnimate + Light * (hover_time) / TimeToAnimate;

	// Storing new value of hover time

	provider->SetStoredFloat(hover_token, hover_time);

	// The drawing part

	control->FillRectangle(Float2(0.f), control->GetSize(), draw_color);

	if (control->GetFocused())
	{
		Color BorderColor = control->GetStyleColor(FudgetThemes::ButtonFocusRectangleColorToken);
		control->DrawRectangle(Float2(0.f), control->GetSize(), BorderColor, control->GetStyleFloat(FudgetThemes::ButtonFocusRectangleWidthToken));
	}
}

