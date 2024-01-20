#pragma once

#include "../Control.h"
#include "Engine/Core/Math/Color.h"

/// <summary>
/// Test control that fills its background with color
/// </summary>
API_CLASS()
class FUDGETS_API FudgetSimpleButton : public FudgetControl
{
	using Base = FudgetControl;
	DECLARE_SCRIPTING_TYPE(FudgetSimpleButton);
public:
	//FudgetSimpleButton();
	~FudgetSimpleButton();

	/// <summary>
	/// Gets the neutral background color used for filling the control.
	/// </summary>
	/// <returns>Background color</returns>
	API_PROPERTY() Color GetColor() const { return _color; }
	/// <summary>
	/// Sets the neutral background color used for filling the control.
	/// </summary>
	/// <returns>Background color</returns>
	API_PROPERTY() void SetColor(Color value) { _color = value; }

	/// <summary>
	/// Background color when the button is down and the mouse is positioned
	/// over its area.
	/// </summary>
	API_FIELD() Color Dark;

	/// <summary>
	/// Background color when the button is not down and the mouse is positioned
	/// over its area.
	/// </summary>
	API_FIELD() Color Light;

	void Draw() override;

	/// <inheritdoc />
	void OnMouseEnter(Float2 pos) override;

	/// <inheritdoc />
	void OnMouseLeave() override;

	/// <inheritdoc />
	void OnMouseMove(Float2 pos) override;

	/// <inheritdoc />
	bool OnMouseDown(Float2 pos, MouseButton button) override;

	/// <inheritdoc />
	bool OnMouseUp(Float2 pos, MouseButton button) override;
private:
	Color _color;
	
	bool _down;
	bool _over;
};
