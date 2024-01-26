#pragma once

#include "Engine/Scripting/ScriptingObject.h"

/// <summary>
/// A simple structure that can be used to identify things in the fudget theming classes or to find values
/// associated with a string.
/// </summary>
API_STRUCT() struct FUDGETS_API FudgetToken
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetToken)
	//static FUDGETS_API struct ScriptingTypeInitializer TypeInitializer;

	FORCE_INLINE FudgetToken(int32 val)
	{
		Token = val;
	}

	// All of these need to be default or Flax won't generate the required code in virtual functions.

	FudgetToken() = default;
	FudgetToken(const FudgetToken &other) = default;
	FudgetToken(FudgetToken &&other) noexcept = default;

	FudgetToken& operator=(int32 val) { Token = val; return *this; }
	FudgetToken& operator=(const FudgetToken &other) = default;
	FudgetToken& operator=(FudgetToken &&other) noexcept = default;

	bool operator==(int32 val) const;
	bool operator==(const FudgetToken &other)  const;
	bool operator<(const FudgetToken &other) const;
	bool operator<=(const FudgetToken &other) const;
	bool operator>(const FudgetToken &other) const;
	bool operator>=(const FudgetToken &other) const;
	bool operator!=(const FudgetToken &other) const;

	operator int32() const { return Token; }

	/// <summary>
	/// A token that can be used as a result for failed operations.
	/// </summary>
	API_FIELD(ReadOnly) static const FudgetToken Invalid;

	/// <summary>
	/// The inner data used as the identifier of this token
	/// </summary>
	API_FIELD() int32 Token;

};

