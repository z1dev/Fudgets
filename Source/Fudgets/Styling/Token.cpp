#include "Token.h"

const FudgetToken FudgetToken::Invalid = -1;


FudgetToken::FudgetToken() : Token(-1)
{

}

FudgetToken::FudgetToken(int32 val) : FudgetToken()
{
	Token = val;
}

FudgetToken::FudgetToken(const FudgetToken &other) : FudgetToken()
{
	Token = other.Token;
}

FudgetToken::FudgetToken(FudgetToken &&other) noexcept : FudgetToken()
{
	Token = other.Token;
}

FudgetToken& FudgetToken::operator=(int32 val)
{
	Token = val;
	return *this;
}

FudgetToken& FudgetToken::operator=(const FudgetToken &other)
{
	Token = other.Token;
	return *this;
}

FudgetToken& FudgetToken::operator=(FudgetToken &&other) noexcept
{
	Token = other.Token;
	return *this;
}

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