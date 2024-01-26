#include "Token.h"


const FudgetToken FudgetToken::Invalid = -1;

bool FudgetToken::operator==(int32 val) const
{
	return Token == val;
}

bool FudgetToken::operator==(const FudgetToken &other) const
{
	return Token == other.Token;
}

bool FudgetToken::operator<(const FudgetToken &other) const
{
	return Token < other.Token;
}

bool FudgetToken::operator<=(const FudgetToken &other) const
{
	return Token <= other.Token;
}

bool FudgetToken::operator>(const FudgetToken &other) const
{
	return Token > other.Token;
}

bool FudgetToken::operator>=(const FudgetToken &other) const
{
	return Token >= other.Token;
}

bool FudgetToken::operator!=(const FudgetToken &other) const
{
	return Token != other.Token;
}
