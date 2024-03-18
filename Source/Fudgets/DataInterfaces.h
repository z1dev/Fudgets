#pragma once

//#include "../Styling/Painters/PartPainters.h"

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Collections/Array.h"


class IFudgetDataConsumer;
class FudgetControl;

/// <summary>
/// Interface class to provide data to list controls. Only the relevant getter and setter
/// needs to return valid values.
/// 
/// IFudgetDataConsumer derived classes can be registered with the interface class to get
/// notifications about changes. For proper notifications, any change must be started by
/// calling IFudgetDataConsumer::DataChangeStarted and when all changes are over it
/// must call IFudgetDataConsumer::DataChangeEnded. Multiple calls to the consumer's
/// notifications can be called in-between.
/// </summary>
API_INTERFACE()
class FUDGETS_API IFudgetDataProvider
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(IFudgetDataProvider);
public:
    virtual ~IFudgetDataProvider() = default;


    /// <summary>
    /// Must be called before changing any data. EndChange should be called after the
    /// changes are over. It's invalid to set, add or remove data while no change
    /// is taking place.
    /// Only data reset and clear can happen without these calls.
    /// </summary>
    API_FUNCTION() virtual void BeginChange() = 0;

    /// <summary>
    /// Must be called after finishing the changes after StartChange was called. It's
    /// invalid to set, add or remove data while no change is taking place.
    /// Only data reset and clear can happen without these calls.
    /// </summary>
    API_FUNCTION() virtual void EndChange() = 0;

    /// <summary>
    /// Can be called in place of BeginChange when the specific changes are not important. This will result in data
    /// provider controls to lose their selection state as they won't get notification of the individual changes, and
    /// have to rebuild their items state from scratch.
    /// Call EndDataReset after the data was changed.
    /// </summary>
    API_FUNCTION() virtual void BeginDataReset() = 0;
    /// <summary>
    /// Call after BeginDataReset once the new data is ready.
    /// </summary>
    API_FUNCTION() virtual void EndDataReset() = 0;

    /// <summary>
    /// Number of data items.
    /// </summary>
    API_FUNCTION() virtual int GetCount() const = 0;

    /// <summary>
    /// Clears all the data.
    /// </summary>
    API_FUNCTION() virtual void Clear() = 0;

    /// <summary>
    /// Looks up the value for the data item at the given index.
    /// </summary>
    /// <param name="index">Index of the data item</param>
    /// <returns>Value at index</returns>
    API_FUNCTION() virtual Variant GetValue(int index) = 0;
    /// <summary>
    /// Changes the value for the data item at the given index.
    /// BeginChange or BeginDataReset() should be called before.
    /// </summary>
    /// <param name="index">Index of the data item</param>
    /// <param name="value">Value to set</param>
    API_FUNCTION() virtual void SetValue(int index, Variant value) = 0;

    /// <summary>
    /// Looks up the text representation for the data item at the given index.
    /// </summary>
    /// <param name="index">Index of the data item</param>
    /// <returns>Text for the item at index</returns>
    API_FUNCTION() virtual String GetText(int index) = 0;
    /// <summary>
    /// Changes the text representation for the data item at the given index.
    /// BeginChange or BeginDataReset() should be called before.
    /// </summary>
    /// <param name="index">Index of the data item</param>
    /// <param name="value">String to set</param>
    API_FUNCTION() virtual void SetText(int index, const StringView &value) = 0;

    /// <summary>
    /// Looks up the int value for the data item at the given index.
    /// </summary>
    /// <param name="index">Index of the data item</param>
    /// <returns>Int value at index</returns>
    API_FUNCTION() virtual int GetInt(int index) = 0;
    /// <summary>
    /// Changes the int value for the data item at the given index.
    /// BeginChange or BeginDataReset() should be called before.
    /// </summary>
    /// <param name="index">Index of the data item</param>
    /// <param name="value">Value to set</param>
    API_FUNCTION() virtual void SetInt(int index, int value) = 0;

    /// <summary>
    /// Registers an object implementing IFudgetDataConsumer that will be notified about data changes.
    /// </summary>
    /// <param name="consumer">The consumer object to be notified</param>
    API_FUNCTION() virtual void RegisterDataConsumer(IFudgetDataConsumer *consumer) = 0;

    /// <summary>
    /// Unregisters an object that was previously registered with RegisterDataConsumer to stop
    /// sending it notifications.
    /// </summary>
    /// <param name="consumer">The consumer object to be notified</param>
    API_FUNCTION() virtual void UnregisterDataConsumer(IFudgetDataConsumer *consumer) = 0;
};


/// <summary>
/// Interface class that can be registered with IFudgetDataProvider classes to receive notifications
/// about data changes.
/// </summary>
API_INTERFACE()
class FUDGETS_API IFudgetDataConsumer
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(IFudgetDataConsumer);
public:
    virtual ~IFudgetDataConsumer() = default;
    /// <summary>
    /// Called at the start of a group of data changes. DataChangeEnded is called afterwards.
    /// </summary>
    API_FUNCTION() virtual void DataChangeBegin() = 0;
    /// <summary>
    /// Called at the end of a group of data changes that started with DataChangeStarted.
    /// </summary>
    /// <param name="changed">Whether there </param>
    API_FUNCTION() virtual void DataChangeEnd(bool changed) = 0;

    /// <summary>
    /// Called before the data is changed in a way that can't be described by individual changes. This
    /// should be regarded like the data is erased and the data provider is filled with unrelated items.
    /// DataChangeBegin might not be called before this.
    /// </summary>
    API_FUNCTION() virtual void DataToBeReset() = 0;
    /// <summary>
    /// Called after the data changed in a way that can't be described by individual changes. This
    /// should be regarded like the data was erased and the data provider was filled with unrelated items.
    /// DataChangeBegin might not be called before this.
    /// </summary>
    API_FUNCTION() virtual void DataReset() = 0;

    /// <summary>
    /// Called before all the data is removed.
    /// DataChangeBegin might not be called before this.
    /// </summary>
    API_FUNCTION() virtual void DataToBeCleared() = 0;
    /// <summary>
    /// Called after all the data is removed.
    /// DataChangeBegin might not be called before this.
    /// </summary>
    API_FUNCTION() virtual void DataCleared() = 0;

    /// <summary>
    /// Called before data is updated.
    /// </summary>
    /// <param name="index">Index of the data to be updated</param>
    API_FUNCTION() virtual void DataToBeUpdated(int index) = 0;
    /// <summary>
    /// Called after data was updated.
    /// </summary>
    /// <param name="index">Index of the updated data</param>
    API_FUNCTION() virtual void DataUpdated(int index) = 0;

    /// <summary>
    /// Called before data is added.
    /// </summary>
    /// <param name="index">Index of the data to be added</param>
    API_FUNCTION() virtual void DataToBeAdded(int count) = 0;
    /// <summary>
    /// Called after data was added.
    /// </summary>
    /// <param name="index">Index of the data that was added</param>
    API_FUNCTION() virtual void DataAdded(int count) = 0;

    /// <summary>
    /// Called before data is removed.
    /// </summary>
    /// <param name="index">Index of the data to be removed</param>
    API_FUNCTION() virtual void DataToBeRemoved(int index, int count) = 0;
    /// <summary>
    /// Called after data was removed.
    /// </summary>
    /// <param name="index">Index of the data that was removed</param>
    API_FUNCTION() virtual void DataRemoved(int index, int count) = 0;

    /// <summary>
    /// Called before data is removed.
    /// </summary>
    /// <param name="index">Index of the data to be removed</param>
    API_FUNCTION() virtual void DataToBeInserted(int index, int count) = 0;
    /// <summary>
    /// Called after data was removed.
    /// </summary>
    /// <param name="index">Index of the data that was removed</param>
    API_FUNCTION() virtual void DataInserted(int index, int count) = 0;

    friend class FudgetDataConsumerRegistry;
};


/// <summary>
/// Helper class that IFudgetDataProvider derived classes can use to keep track of registered
/// consumers and calling their notification functions.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetDataConsumerRegistry : public ScriptingObject
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE(FudgetDataConsumerRegistry);
public:
    /// <summary>
    /// Registers an object implementing IFudgetDataConsumer that will be notified about data changes.
    /// </summary>
    /// <param name="consumer">The consumer object to be notified</param>
    API_FUNCTION() void RegisterDataConsumer(IFudgetDataConsumer *consumer);
    /// <summary>
    /// Unregisters an object that was previously registered with RegisterDataConsumer to stop
    /// sending it notifications.
    /// </summary>
    /// <param name="consumer">The consumer object to be notified</param>
    API_FUNCTION() void UnregisterDataConsumer(IFudgetDataConsumer *consumer);

    /// <summary>
    /// Equivalent of BeginChange in IFudgetDataProvider.
    /// </summary>
    API_FUNCTION() void BeginChange();
    /// <summary>
    /// Equivalent of EndChange in IFudgetDataProvider.
    /// </summary>
    API_FUNCTION() void EndChange();

    /// <summary>
    /// Equivalent of BeginDataReset in IFudgetDataProvider.
    /// </summary>
    API_FUNCTION() void BeginDataReset();
    /// <summary>
    /// Equivalent of EndDataReset in IFudgetDataProvider.
    /// </summary>
    API_FUNCTION() void EndDataReset();

    /// <summary>
    /// Call before clearing the data.
    /// </summary>
    API_FUNCTION() void ClearBegin();
    /// <summary>
    /// Call after clearing the data.
    /// </summary>
    API_FUNCTION() void ClearEnd();

    /// <summary>
    /// Call before changing a data.
    /// </summary>
    API_FUNCTION() bool SetBegin(int index);
    /// <summary>
    /// Call after changing a data.
    /// </summary>
    API_FUNCTION() void SetEnd(int index);

    /// <summary>
    /// Call before adding new data items.
    /// </summary>
    API_FUNCTION() bool AddBegin(int count);
    /// <summary>
    /// Call after adding new data items.
    /// </summary>
    API_FUNCTION() void AddEnd(int count);

    /// <summary>
    /// Call before removing data items.
    /// </summary>
    API_FUNCTION() bool RemoveBegin(int index, int count);
    /// <summary>
    /// Call after removing data items.
    /// </summary>
    API_FUNCTION() void RemoveEnd(int index, int count);

    /// <summary>
    /// Call before inserting data items.
    /// </summary>
    API_FUNCTION() bool InsertBegin(int index, int count);
    /// <summary>
    /// Call After inserting data items.
    /// </summary>
    API_FUNCTION() void InsertEnd(int index, int count);
private:
    void ScriptingObjectConsumerDestroyed(ScriptingObject *obj);

    enum class Status
    {
        Idle,
        Changing,
        Reseting,
        Clearing,
    };

    enum class Function
    {
        None,
        Set,
        Add,
        Remove,
        Insert,
        Move
    };

    Status _status;
    Function _func;
    bool _changed;

    Array<IFudgetDataConsumer*> _consumers;
};

