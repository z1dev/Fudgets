#include "Theme.h"
#include "Token.h"

#include "ElementPainters/SimpleButtonPainter.h"


FudgetTheme::FudgetTheme() : Base(SpawnParams(Guid::New(), TypeInitializer)), highest_token(0)
{
	Init();
}

FudgetToken FudgetTheme::GetToken(String token_name) const
{
	auto it = token_map.find(token_name);
	if (it == token_map.end())
		return FudgetToken::Invalid;

	return it->second;
}

FudgetToken FudgetTheme::RegisterToken(String token_name, bool duplicate_is_error)
{
	auto it = token_map.find(token_name);
	if (it != token_map.end())
	{
		if (duplicate_is_error)
			return FudgetToken::Invalid;
		return it->second;
	}
	token_map[token_name] = ++highest_token;
	return highest_token;
}

//FudgetStyleBase* FudgetTheme::GetStyle(String token_name) const
//{
//	FudgetToken token = GetToken(token_name);
//	return GetStyle(token);
//}
//
//FudgetStyleBase* FudgetTheme::GetStyle(FudgetToken token) const
//{
//	if (token == FudgetToken::Invalid)
//		return nullptr;
//	auto it = style_map.find(token);
//	if (it == style_map.end())
//		return nullptr;
//	return it->second;
//}

void FudgetTheme::Init()
{
	FudgetSimpleButtonPainter *sbdrawer = New<FudgetSimpleButtonPainter>();
	element_map[RegisterToken(TEXT("SimpleButton"))] = sbdrawer;
}
