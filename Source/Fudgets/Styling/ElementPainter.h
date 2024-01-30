#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Types/Variant.h"

#include "Token.h"

#include <map>

class FudgetControl;
class FudgetPainterPropertyProvider;

/// <summary>
/// Base type for classes that can draw elements or a full control. Derived classes can draw something simple
/// like a line of text or the background of a button, or can draw every part of a complex control.
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetElementPainter : public ScriptingObject
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetElementPainter);
public:
	FudgetElementPainter(String name);
	FudgetElementPainter(FudgetToken name_token);
	~FudgetElementPainter();

	/// <summary>
	/// Draws an element of a control or complete control, using the data from the provider for the state to draw.
	/// </summary>
	/// <param name="control">Control whose theme and style is used for drawing</param>
	/// <param name="provider">Property provider that can supply the painter with data about the state to draw</param>
	API_FUNCTION() virtual void Draw(FudgetControl *control, FudgetPainterPropertyProvider *provider) = 0;

	/// <summary>
	/// Retrieves an element painter from the control's active style with the passed token and calls its Draw function.
	/// </summary>
	/// <param name="control">Control whose theme and style is used for drawing</param>
	/// <param name="provider">Object providing state values for the painter</param>
	/// <param name="painter_token">Token of painter overriden in the style</param>
	API_FUNCTION() void DrawWithPainter(FudgetControl *control, FudgetPainterPropertyProvider *provider, FudgetToken painter_token);
private:
	FudgetToken _painter_name;
};

/// <summary>
/// Base type for property providers, whose task is to return values about a control and to hold draw state data.
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetPainterPropertyProvider : public ScriptingObject
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetPainterPropertyProvider);
public:
	// TODO: add property getters as necessary

	/// <summary>
	/// Creates the property provider with the specified control as the source for the values that painters can request.
	/// </summary>
	/// <param name="_source_control">The control that holds the properties needed for drawing</param>
	FudgetPainterPropertyProvider();

	/// <summary>
	/// Called by ElementPainter derived objects when drawing to get generic properties needed for showing the state of the control, that
	/// don't fit in any of the type specific getters.
	/// The property provider might not support every property required by a painter, so the painter has to make sure to use sane defaults.
	/// </summary>
	/// <param name="token">The requested property's token. TODO: make a default set of tokens as static values in FudgetToken </param>
	/// <param name="result">The property's value</param>
	/// <returns>Whether the control supports a specific property or not. If false, the result is ignored</returns>
	API_FUNCTION() virtual bool GetElementGenericProperty(FudgetToken token, API_PARAM(Out) Variant &result) { return false; }

	/// <summary>
	/// Called by ElementPainter derived objects when drawing, to get bool properties needed for showing the state of the control.
	/// The property provider might not support every property required by a painter, so the painter has to make sure to use sane defaults.
	/// </summary>
	/// <param name="token">The requested property's token. TODO: make a default set of tokens as static values in FudgetToken </param>
	/// <param name="result">The property's value</param>
	/// <returns>Whether the control supports a specific property or not. If false, the result is ignored</returns>
	API_FUNCTION() virtual bool GetElementBoolProperty(FudgetToken token, API_PARAM(Out) bool &result) { return false; }

	// TODO: add more property getters. GetElementGenericProperty should work for everything, but using variants in c++ is more work.

	/// <summary>
	/// Returns the delta time, which is increased in Control on OnUpdate. It's reset to 0 after drawing is done.
	/// </summary>
	/// <returns>Delta time since the last draw call</returns>
	API_PROPERTY() virtual float GetDeltaTime() const { return _delta_time; }

	/// <summary>
	/// Resets the delta time to 0.
	/// </summary>
	API_FUNCTION() virtual void ResetDeltaTime() { _delta_time = 0.f; }

	/// <summary>
	/// Increases the delta time by a set value.
	/// </summary>
	/// <param name="value">How much to increase delta time</param>
	API_FUNCTION() virtual void AddDeltaTime(float value);

	// TODO: find a better way to store custom values in this property provider. The current implementation
	// takes up extra space for every control that creates this object. This could be moved to the control somehow,
	// so at least only those controls would take memory that actually need it.

	// TODO: add more values types

	/// <summary>
	/// Allows element painters to store custom values in the control they need to draw. Returns the value associated
	/// with the token if it has been stored.
	/// </summary>
	/// <param name="token">Token to the value to get</param>
	/// <param name="result">The result on success</param>
	/// <returns>Whether the call was successful and result was updated to the requested value</returns>
	API_FUNCTION() virtual bool GetStoredValue(FudgetToken token, API_PARAM(Out) Variant &result) const;

	/// <summary>
	/// Allows element painters to store custom values in the control they need to draw. Sets or updates a value
	/// associated with the token.
	/// </summary>
	/// <param name="token">Token to the value to set</param>
	/// <param name="value">The updated value</param>
	API_FUNCTION() virtual void SetStoredValue(FudgetToken token, const Variant &value);

	/// <summary>
	/// Allows element painters to store custom values in the control they need to draw. Returns the value associated
	/// to the token if it has been stored.
	/// </summary>
	/// <param name="token">Token to the value to get</param>
	/// <param name="result">The result on success</param>
	/// <returns>Whether the call was successful and result was updated to the requested value</returns>
	API_FUNCTION() virtual bool GetStoredFloat(FudgetToken token, API_PARAM(Out) float &result) const;

	/// <summary>
	/// Allows element painters to store custom values in the control they need to draw. Sets or updates a value
	/// associated to the token.
	/// </summary>
	/// <param name="token">Token to the value to set</param>
	/// <param name="value">The updated value</param>
	API_FUNCTION() virtual void SetStoredFloat(FudgetToken token, float value);


protected:
	std::map<FudgetToken, Variant> _stored_values;

	// Time since this painter was last passed to the property provider. Mostly used by controls that need to animate to draw.
	// Automatically increased in a control's OnUpdate() if updating is enabled. It should be negative when not used.
	float _delta_time;
};

