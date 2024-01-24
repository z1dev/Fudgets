#include "Token.h"

FudgetToken FudgetToken::Invalid = -1;


FudgetToken::FudgetToken() : _id(-1)
{

}

FudgetToken::FudgetToken(int32 val) : FudgetToken()
{
	_id = val;
}

FudgetToken::FudgetToken(const FudgetToken &other) : FudgetToken()
{
	_id = other._id;
}

FudgetToken::FudgetToken(FudgetToken &&other) noexcept : FudgetToken()
{
	_id = other._id;
}

FudgetToken& FudgetToken::operator=(int32 val)
{
	_id = val;
	return *this;
}

FudgetToken& FudgetToken::operator=(const FudgetToken &other)
{
	_id = other._id;
	return *this;
}

FudgetToken& FudgetToken::operator=(FudgetToken &&other)
{
	_id = other._id;
	return *this;
}

bool FudgetToken::operator==(int32 val) const
{
	return _id == val;
}

bool FudgetToken::operator==(const FudgetToken &other) const
{
	return _id == other._id;
}

bool FudgetToken::operator<(const FudgetToken &other) const
{
	return _id < other._id;
}

bool FudgetToken::operator<=(const FudgetToken &other) const
{
	return _id <= other._id;
}

bool FudgetToken::operator>(const FudgetToken &other) const
{
	return _id > other._id;
}

bool FudgetToken::operator>=(const FudgetToken &other) const
{
	return _id >= other._id;
}

bool FudgetToken::operator!=(const FudgetToken &other) const
{
	return _id != other._id;
}