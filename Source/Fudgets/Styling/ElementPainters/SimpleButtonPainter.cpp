#include "SimpleButtonPainter.h"
#include "../../Control.h"
#include "../Themes.h"
#include "../Style.h"
#include "../StyleStructs.h"
#include "../../MarginStructs.h"

#include "Engine/Core/Math/Vector2.h"

static const FudgetToken hover_token = FudgetThemes::RegisterToken(TEXT("SimpleButtonHoverTime"));

const FudgetToken FudgetSimpleButtonPainter::simple_btn_bg_token = FudgetThemes::RegisterToken(TEXT("SimpleButtonBackground"));
const FudgetToken FudgetSimpleButtonPainter::simple_btn_border_token = FudgetThemes::RegisterToken(TEXT("SimpleButtonBorderImage"));


FudgetButtonBackgroundPainter::FudgetButtonBackgroundPainter(const SpawnParams &params) : Base(params)
{
}

void FudgetButtonBackgroundPainter::Draw(FudgetControl *control, FudgetPainterPropertyProvider *provider)
{
	Variant padding_var;
	FudgetPadding padding = FudgetPadding();
	if (control->GetStyleValue(FudgetThemes::ButtonBackgroundPaddingToken, padding_var))
		padding = *padding_var.AsStructure<FudgetPadding>();

	FudgetFillAreaSettings Dark(Color::Gray);
	FudgetFillAreaSettings Light(Color::White);
	FudgetFillAreaSettings Normal(Color::LightGray);

	control->GetStyleFillSettings(FudgetThemes::ButtonBackgroundPressedToken, Dark);
	control->GetStyleFillSettings(FudgetThemes::ButtonBackgroundHoverToken, Light);
	control->GetStyleFillSettings(FudgetThemes::ButtonBackgroundNormalToken, Normal);

	//control->GetStyleColor(FudgetThemes::ButtonBackgroundPressedToken, Dark.Color);
	//control->GetStyleColor(FudgetThemes::ButtonBackgroundHoverToken, Light.Color);

	float TimeToAnimate = 0.0f;
	control->GetStyleFloat(FudgetThemes::ButtonHoverAnimationTimeToken, TimeToAnimate);

	// Fetching values from the control

	bool mouse_is_over = false;
	provider->GetElementBoolProperty(FudgetThemes::MouseHoverToken, mouse_is_over);
	bool mouse_is_pressed = false;
	provider->GetElementBoolProperty(FudgetThemes::LeftButtonPressedToken, mouse_is_pressed);

	// Custom stored time used to animate the button color

	float hover_time = 0.0f;
	provider->GetStoredFloat(hover_token, hover_time);

	Rectangle draw_rect = Rectangle(Float2(padding.Left, padding.Top), control->GetSize() - Float2(padding.Width(), padding.Height()));

	if (hover_time == 0.0f)
	{
		if (mouse_is_over && mouse_is_pressed)
			control->DrawFillArea(Dark, draw_rect);
		else if (mouse_is_over)
			control->DrawFillArea(Light, draw_rect);
		else
			control->DrawFillArea(Normal, draw_rect);
		return;
	}

	float light_time = (hover_time) / TimeToAnimate;
	if (Math::NearEqual(light_time, 1.0f))
	{
		control->DrawFillArea(Light, draw_rect);
		return;
	}

	control->DrawFillArea(Normal, draw_rect);
	Light.Color.A = (hover_time) / TimeToAnimate;
	control->DrawFillArea(Light, draw_rect);
}

FudgetButtonBorderPainter::FudgetButtonBorderPainter(const SpawnParams &params) : Base(params)
{
}

void FudgetButtonBorderPainter::Draw(FudgetControl *control, FudgetPainterPropertyProvider *provider)
{
	FudgetFillAreaSettings Dark(Color::Gray);
	FudgetFillAreaSettings Light(Color::White);
	FudgetFillAreaSettings Normal(Color::LightGray);

	control->GetStyleFillSettings(FudgetThemes::ButtonBorderPressedImageToken, Dark);
	control->GetStyleFillSettings(FudgetThemes::ButtonBorderHoverImageToken, Light);
	control->GetStyleFillSettings(FudgetThemes::ButtonBorderNormalImageToken, Normal);

	float TimeToAnimate = 0.0f;
	control->GetStyleFloat(FudgetThemes::ButtonHoverAnimationTimeToken, TimeToAnimate);

	// Fetching values from the control

	bool mouse_is_over = false;
	provider->GetElementBoolProperty(FudgetThemes::MouseHoverToken, mouse_is_over);
	bool mouse_is_pressed = false;
	provider->GetElementBoolProperty(FudgetThemes::LeftButtonPressedToken, mouse_is_pressed);

	// Custom stored time used to animate the button color

	float hover_time = 0.0f;
	provider->GetStoredFloat(hover_token, hover_time);

	Rectangle draw_rect = Rectangle(Float2(0.f), control->GetSize());

	if (hover_time == 0.0f)
	{
		if (mouse_is_over && mouse_is_pressed)
			control->DrawFillArea(Dark, draw_rect);
		else if (mouse_is_over)
			control->DrawFillArea(Light, draw_rect);
		else
			control->DrawFillArea(Normal, draw_rect);
		return;
	}

	float light_time = (hover_time) / TimeToAnimate;
	if (Math::NearEqual(light_time, 1.0f))
	{
		control->DrawFillArea(Light, draw_rect);
		return;
	}

	control->DrawFillArea(Normal, draw_rect);
	Light.Color.A = (hover_time) / TimeToAnimate;
	control->DrawFillArea(Light, draw_rect);
}


FudgetSimpleButtonPainter::FudgetSimpleButtonPainter(const SpawnParams &params) : Base(params)
{
}

void FudgetSimpleButtonPainter::Draw(FudgetControl *control, FudgetPainterPropertyProvider *provider)
{
	float hover_time = 0.0f;
	provider->GetStoredFloat(hover_token, hover_time);

	// Calculating hover time that will be used to set the color of the button.

	float TimeToAnimate = 0.0f;
	control->GetStyleFloat(FudgetThemes::ButtonHoverAnimationTimeToken, TimeToAnimate);

	bool mouse_is_over = false;
	provider->GetElementBoolProperty(FudgetThemes::MouseHoverToken, mouse_is_over);
	bool mouse_is_pressed = false;
	provider->GetElementBoolProperty(FudgetThemes::LeftButtonPressedToken, mouse_is_pressed);
	float delta_time = provider->GetDeltaTime();

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
	provider->SetStoredFloat(hover_token, hover_time);

	DrawWithPainter(control, provider, simple_btn_bg_token);
	DrawWithPainter(control, provider, simple_btn_border_token);


	if (control->GetFocused())
	{
		Float2 made_up_padding = control->GetSize() * 0.15;
		Color BorderColor = Color::Black;
		control->GetStyleColor(FudgetThemes::ButtonFocusRectangleColorToken, BorderColor);
		float borderWidth = 1.0f;
		control->GetStyleFloat(FudgetThemes::ButtonFocusRectangleWidthToken, borderWidth);
		control->DrawRectangle(made_up_padding, control->GetSize() - (made_up_padding * 2.f), BorderColor, borderWidth);
	}
}

