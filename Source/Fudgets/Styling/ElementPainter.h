#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Types/Variant.h"

#include "Token.h"

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
	FudgetElementPainter();

	/// <summary>
	/// Draws an element or full control completely, using the data from the provider to know what state to draw.
	/// </summary>
	/// <param name="provider"></param>
	/// <returns></returns>
	API_FUNCTION() virtual void Draw(FudgetPainterPropertyProvider *provider) = 0;
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
	FudgetPainterPropertyProvider(FudgetControl *_source_control);

	/// <summary>
	/// Returns the control associated with this property provider to provide properties for.
	/// </summary>
	API_PROPERTY() FudgetControl* GetSourceControl() const { return _source_control; }

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
	/// to the token if it has been stored.
	/// The property provider might not support storage for value types required by a painter, so the painter has
	/// to make sure to use sane defaults.
	/// </summary>
	/// <param name="token">Token to the value to get</param>
	/// <param name="result">The result on success</param>
	/// <returns>Whether the call was successful and result was updated to the requested value</returns>
	API_FUNCTION() virtual bool GetStoredVariant(FudgetToken token, API_PARAM(Out) Variant &result) const { return false; }

	/// <summary>
	/// Allows element painters to store custom values in the control they need to draw. Sets or updates a value
	/// associated to the token.
	/// The property provider might not support storage for value types required by a painter, so the painter has
	/// to make sure to use sane defaults.
	/// </summary>
	/// <param name="token">Token to the value to set</param>
	/// <param name="value">The updated value</param>
	API_FUNCTION() virtual void SetStoredVariant(FudgetToken token, Variant value) {}

	/// <summary>
	/// Allows element painters to store custom values in the control they need to draw. Returns the value associated
	/// to the token if it has been stored.
	/// The property provider might not support storage for value types required by a painter, so the painter has
	/// to make sure to use sane defaults.
	/// </summary>
	/// <param name="token">Token to the value to get</param>
	/// <param name="result">The result on success</param>
	/// <returns>Whether the call was successful and result was updated to the requested value</returns>
	API_FUNCTION() virtual bool GetStoredFloat(FudgetToken token, API_PARAM(Out) float &result) const { return false; }

	/// <summary>
	/// Allows element painters to store custom values in the control they need to draw. Sets or updates a value
	/// associated to the token.
	/// The property provider might not support storage for value types required by a painter, so the painter has
	/// to make sure to use sane defaults.
	/// </summary>
	/// <param name="token">Token to the value to set</param>
	/// <param name="value">The updated value</param>
	API_FUNCTION() virtual void SetStoredFloat(FudgetToken token, float value) {}


protected:
	// The control we provide properties about.
	FudgetControl *_source_control;

	// Time since this painter was last passed to the property provider. Mostly used by controls that need to animate to draw.
	// Automatically increased in a control's OnUpdate() if updating is enabled. It should be negative when not used.
	float _delta_time;
};

