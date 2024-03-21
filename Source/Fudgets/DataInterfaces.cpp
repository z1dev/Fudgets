#include "DataInterfaces.h"

#include "Engine/Core/Log.h"


// FudgetDataConsumerRegistry


FudgetDataConsumerRegistry::FudgetDataConsumerRegistry(const SpawnParams &params) : Base(params), _status(Status::Idle), _func(Function::None), _changed(false)
{

}

FudgetDataConsumerRegistry::~FudgetDataConsumerRegistry()
{
    for (IFudgetDataConsumer *consumer : _consumers)
    {
        ScriptingObject *obj = FromInterface(consumer, IFudgetDataConsumer::TypeInitializer);
        if (obj != nullptr)
        {
            obj->Deleted.Unbind<FudgetDataConsumerRegistry, &FudgetDataConsumerRegistry::ScriptingObjectConsumerDestroyed>(this);
        }
    }
    _consumers.Clear();
}

void FudgetDataConsumerRegistry::RegisterDataConsumer(IFudgetDataConsumer *consumer)
{
    if (consumer == nullptr || _consumers.Find(consumer) != -1)
    {
        if (consumer != nullptr)
            LOG(Warning, "FudgetDataConsumerRegistry::RegisterDataConsumer: consumer is already registered");
        else
            LOG(Warning, "FudgetDataConsumerRegistry::RegisterDataConsumer: calling with null");
        return;
    }

    ScriptingObject *obj = FromInterface(consumer, IFudgetDataConsumer::TypeInitializer);
    if (obj != nullptr)
    {
        obj->Deleted.Bind<FudgetDataConsumerRegistry, &FudgetDataConsumerRegistry::ScriptingObjectConsumerDestroyed>(this);
    }
    _consumers.Add(consumer);
}

void FudgetDataConsumerRegistry::UnregisterDataConsumer(IFudgetDataConsumer *consumer)
{
    if (consumer == nullptr)
        return;

    int ix = _consumers.Find(consumer);
    if (ix != -1)
    {
        ScriptingObject *obj = FromInterface(consumer, IFudgetDataConsumer::TypeInitializer);
        if (obj != nullptr)
        {
            obj->Deleted.Unbind<FudgetDataConsumerRegistry, &FudgetDataConsumerRegistry::ScriptingObjectConsumerDestroyed>(this);
        }

        _consumers.RemoveAt(ix);
    }
}

void FudgetDataConsumerRegistry::BeginChange()
{
    if (_status != Status::Idle)
    {
        LOG(Warning, "FudgetDataConsumerRegistry::BeginChange: status is not Idle");
        return;
    }

    _status = Status::Changing;

    for (auto c : _consumers)
        c->DataChangeBegin();
}

void FudgetDataConsumerRegistry::EndChange()
{
    if (_status != Status::Changing)
    {
        LOG(Warning, "FudgetDataConsumerRegistry::EndChange: status is not Changing");
        return;
    }

    for (auto c : _consumers)
        c->DataChangeEnd(_changed);
    _changed = false;

    _status = Status::Idle;
}

void FudgetDataConsumerRegistry::BeginDataReset()
{
    if (_status != Status::Idle)
    {
        LOG(Warning, "FudgetDataConsumerRegistry::BeginDataReset: status is not Idle");
        return;
    }

    _status = Status::Reseting;

    for (auto c : _consumers)
        c->DataToBeReset();
}

void FudgetDataConsumerRegistry::EndDataReset()
{
    if (_status != Status::Reseting)
    {
        LOG(Warning, "FudgetDataConsumerRegistry::EndDataReset: status is not Reset");
        return;
    }

    for (auto c : _consumers)
        c->DataReset();

    _status = Status::Idle;
}

void FudgetDataConsumerRegistry::ClearBegin()
{
    if (_status != Status::Idle)
    {
        LOG(Warning, "FudgetDataConsumerRegistry::ClearBegin: status is not Idle");
        return;
    }

    _status = Status::Clearing;

    for (auto c : _consumers)
        c->DataToBeCleared();
}

void FudgetDataConsumerRegistry::ClearEnd()
{
    if (_status != Status::Clearing)
    {
        LOG(Warning, "FudgetDataConsumerRegistry::ClearEnd: status is not Clearing");
        return;
    }

    for (auto c : _consumers)
        c->DataCleared();

    _status = Status::Idle;
}

bool FudgetDataConsumerRegistry::SetBegin(int index)
{
    if (_status != Status::Changing || _func != Function::None)
    {
        if (_status != Status::Changing)
        {
            LOG(Warning, "FudgetDataConsumerRegistry::SetBegin: status is not Changing");
        }
        else
        {
            LOG(Warning, "FudgetDataConsumerRegistry::SetBegin: already in the middle of another operation");
        }
        return false;
    }

    _func = Function::Set;

    for (auto c : _consumers)
        c->DataToBeUpdated(index);

    return true;
}

void FudgetDataConsumerRegistry::SetEnd(int index)
{
    if (_status != Status::Changing || _func != Function::Set)
    {
        if (_status != Status::Changing)
        {
            LOG(Warning, "FudgetDataConsumerRegistry::SetEnd: status is not Changing");
        }
        else
        {
            LOG(Warning, "FudgetDataConsumerRegistry::SetEnd: SetBegin wasn't called");
        }
        return;
    }

    _changed = true;

    for (auto c : _consumers)
        c->DataUpdated(index);

    _func = Function::None;
}

bool FudgetDataConsumerRegistry::AddBegin(int count)
{
    if (_status != Status::Changing || _func != Function::None)
    {
        if (_status != Status::Changing)
        {
            LOG(Warning, "FudgetDataConsumerRegistry::AddBegin: status is not Changing");
        }
        else
        {
            LOG(Warning, "FudgetDataConsumerRegistry::AddBegin: already in the middle of another operation");
        }
        return false;
    }

    _func = Function::Add;

    for (auto c : _consumers)
        c->DataToBeAdded(count);

    return true;
}

void FudgetDataConsumerRegistry::AddEnd(int count)
{
    if (_status != Status::Changing || _func != Function::Add)
    {
        if (_status != Status::Changing)
        {
            LOG(Warning, "FudgetDataConsumerRegistry::AddEnd: status is not Changing");
        }
        else
        {
            LOG(Warning, "FudgetDataConsumerRegistry::AddEnd: AddBegin wasn't called");
        }
        return;
    }

    _changed = true;

    for (auto c : _consumers)
        c->DataAdded(count);

    _func = Function::None;
}

bool FudgetDataConsumerRegistry::RemoveBegin(int index, int count)
{
    if (_status != Status::Changing || _func != Function::None)
    {
        if (_status != Status::Changing)
        {
            LOG(Warning, "FudgetDataConsumerRegistry::RemoveBegin: status is not Changing");
        }
        else
        {
            LOG(Warning, "FudgetDataConsumerRegistry::RemoveBegin: already in the middle of another operation");
        }
        return false;
    }

    _func = Function::Remove;

    for (auto c : _consumers)
        c->DataToBeRemoved(index, count);

    return true;
}

void FudgetDataConsumerRegistry::RemoveEnd(int index, int count)
{
    if (_status != Status::Changing || _func != Function::Remove)
    {
        if (_status != Status::Changing)
        {
            LOG(Warning, "FudgetDataConsumerRegistry::RemoveEnd: status is not Changing");
        }
        else
        {
            LOG(Warning, "FudgetDataConsumerRegistry::RemoveEnd: RemoveBegin wasn't called");
        }
        return;
    }

    _changed = true;

    for (auto c : _consumers)
        c->DataRemoved(index, count);

    _func = Function::None;
}

bool FudgetDataConsumerRegistry::InsertBegin(int index, int count)
{
    if (_status != Status::Changing || _func != Function::None)
    {
        if (_status != Status::Changing)
        {
            LOG(Warning, "FudgetDataConsumerRegistry::InsertBegin: status is not Changing");
        }
        else
        {
            LOG(Warning, "FudgetDataConsumerRegistry::InsertBegin: already in the middle of another operation");
        }
        return false;
    }

    _func = Function::Insert;

    for (auto c : _consumers)
        c->DataToBeInserted(index, count);

    return true;
}

void FudgetDataConsumerRegistry::InsertEnd(int index, int count)
{
    if (_status != Status::Changing || _func != Function::Insert)
    {
        if (_status != Status::Changing)
        {
            LOG(Warning, "FudgetDataConsumerRegistry::InsertEnd: status is not Changing");
        }
        else
        {
            LOG(Warning, "FudgetDataConsumerRegistry::InsertEnd: InsertBegin wasn't called");
        }
        return;
    }

    _changed = true;

    for (auto c : _consumers)
        c->DataInserted(index, count);

    _func = Function::None;
}

void FudgetDataConsumerRegistry::ScriptingObjectConsumerDestroyed(ScriptingObject *obj)
{
    UnregisterDataConsumer(ToInterface<IFudgetDataConsumer>(obj));
}

