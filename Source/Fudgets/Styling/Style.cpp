#include "Style.h"

FudgetStyle::FudgetStyle(FudgetThemeBase *theme, FudgetToken token) : Base(SpawnParams(Guid::New(), TypeInitializer)), _theme(theme), _token(token)
{

}