#include "ElementPainter.h"
#include "../Control.h"

FudgetElementPainter::FudgetElementPainter() : Base(SpawnParams(Guid::New(), TypeInitializer))
{

}

FudgetPainterPropertyProvider::FudgetPainterPropertyProvider(FudgetControl *source_control) : Base(SpawnParams(Guid::New(), TypeInitializer)),
	_source_control(source_control), _delta_time(-1.0f)
{

}

void FudgetPainterPropertyProvider::AddDeltaTime(float value)
{
	_delta_time += value;
}
