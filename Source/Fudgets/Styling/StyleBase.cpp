#include "StyleBase.h"

FudgetStyleBase::FudgetStyleBase(FudgetThemeBase *theme, FudgetToken token) : Base(SpawnParams(Guid::New(), TypeInitializer)), _theme(theme), _token(token)
{

}