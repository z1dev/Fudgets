#pragma once

//#include "Engine/Scripting/ScriptingObject.h"
//
///// <summary>
///// A simple structure that can be used to identify things in the fudget theming classes or to find values
///// associated with a string.
///// </summary>
//API_STRUCT() struct FUDGETS_API FudgetToken
//{
//	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetToken)
//
//	FORCE_INLINE FudgetToken()
//	{
//		Token = -1;
//	}
//
//	FORCE_INLINE FudgetToken(int32 val)
//	{
//		Token = val;
//	}
//
//	// All of these need to be default or Flax won't generate the required code in virtual functions.
//
//	FudgetToken(const FudgetToken &other) = default;
//	FudgetToken(FudgetToken &&other) noexcept = default;
//
//	FudgetToken& operator=(int32 val) { Token = val; return *this; }
//	FudgetToken& operator=(const FudgetToken &other) = default;
//	FudgetToken& operator=(FudgetToken &&other) noexcept = default;
//
//	bool operator==(int32 val) const;
//	bool operator==(const FudgetToken &other)  const;
//	bool operator<(const FudgetToken &other) const;
//	bool operator<=(const FudgetToken &other) const;
//	bool operator>(const FudgetToken &other) const;
//	bool operator>=(const FudgetToken &other) const;
//	bool operator!=(const FudgetToken &other) const;
//
//	FORCE_INLINE bool Equals(const FudgetToken &other)
//	{
//		return other.Token == Token;
//	}
//
//	operator int32() const { return Token; }
//
//	bool IsValid() const { return Token != -1; }
//	bool IsNull() const { return Token == -2; }
//
//	/// <summary>
//	/// The inner data used as the identifier of this token
//	/// </summary>
//	API_FIELD() int32 Token;
//
//	/// <summary>
//	/// A token that can be used as a result for failed operations.
//	/// </summary>
//	API_FIELD(ReadOnly) static const FudgetToken Invalid;
//
//	/// <summary>
//	/// A token that is null. Null tokens should be used where a valid token is required, but it must indicate
//	/// that it represents nothing. For example a style resource must be valid. Setting it to invalid would mean
//	/// that the style doesn't override a value. Instead, null might be used to hide a value of an inherited style.
//	/// </summary>
//	API_FIELD(ReadOnly) static const FudgetToken Null;
//};
//
//inline uint32 GetHash(const FudgetToken &token)
//{
//	return token.Token;
//}
//
//template<>
//struct TIsPODType<FudgetToken>
//{
//	enum { Value = true };
//};
