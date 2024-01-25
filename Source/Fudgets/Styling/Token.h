#pragma once

#include "Engine/Scripting/ScriptingObject.h"

// TODO: make the C# equivalent of this struct and all the functions using it.

/// <summary>
/// A simple structure that can be used to identify things in the fudget theming classes or to find values
/// associated with a string.
/// </summary>
struct FudgetToken
{
	/// <summary>
	/// A token that can be used as a result for failed operations.
	/// </summary>
	static const FudgetToken Invalid;

	FudgetToken();
	FudgetToken(int32);
	FudgetToken(const FudgetToken &other);
	FudgetToken(FudgetToken &&other) noexcept;

	FudgetToken& operator=(int32);
	FudgetToken& operator=(const FudgetToken &other);
	FudgetToken& operator=(FudgetToken &&other) noexcept;

	bool operator==(int32 val) const;
	bool operator==(const FudgetToken &other)  const;
	bool operator<(const FudgetToken &other) const;
	bool operator<=(const FudgetToken &other) const;
	bool operator>(const FudgetToken &other) const;
	bool operator>=(const FudgetToken &other) const;
	bool operator!=(const FudgetToken &other) const;

	operator int32() const { return Token; }

	/// <summary>
	/// The inner data used as the identifier of this token
	/// </summary>
	int32 Token;
};

