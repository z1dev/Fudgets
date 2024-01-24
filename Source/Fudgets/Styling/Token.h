#pragma once

#include "Engine/Scripting/ScriptingObject.h"

API_STRUCT()
struct FUDGETS_API FudgetToken
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetToken);

	static FudgetToken Invalid;

	FudgetToken();
	FudgetToken(int32);
	FudgetToken(const FudgetToken &other);
	FudgetToken(FudgetToken &&other) noexcept;
	FudgetToken& operator=(int32);
	FudgetToken& operator=(const FudgetToken &other);
	FudgetToken& operator=(FudgetToken &&other);
	bool operator==(int32 val) const;
	bool operator==(const FudgetToken &other)  const;
	bool operator<(const FudgetToken &other) const;
	bool operator<=(const FudgetToken &other) const;
	bool operator>(const FudgetToken &other) const;
	bool operator>=(const FudgetToken &other) const;
	bool operator!=(const FudgetToken &other) const;

	int32 _id;
};

