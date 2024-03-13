#pragma once

#include "Engine/Scripting/ScriptingObject.h"

#include <vector>


struct FudgetStateOrderItem
{
    FudgetStateOrderItem(uint64 state, bool has_sub_items = false) : _state(state), _has_sub_items(has_sub_items) {}
    virtual ~FudgetStateOrderItem() {}
    uint64 _state;
    bool _has_sub_items;
};

struct FudgetStateOrderItemList : public FudgetStateOrderItem
{
    using Base = FudgetStateOrderItem;

    FudgetStateOrderItemList() : Base(0, false) {}
    FudgetStateOrderItemList(uint64 item) : Base(item, true) {}
    ~FudgetStateOrderItemList();
    std::vector<FudgetStateOrderItem*> _list;
};

/// <summary>
/// Helper object for part painters to determine what resources correspond to the current state when drawing a part. 
/// Use FudgetStateOrderBuilder to determine the priority of each state for drawing. See the (comments) documentation of
/// FudgetStateOrderBuilder to learn more.
/// The result of FudgetStateOrderBuilder is an index, that should be stored in the StateOrderIndex member, when filling
/// the ***Resources structure of each painter.
/// The index should be cached and reused as much as possible instead of building new state priority data. FudgetStates
/// contains a few ***_STATE_ORDER values for pre-built state priority data as well.
/// 
/// Check the FudgetVisualControlState enum in PartPainters.h for a partial list of possible states. The enum values
/// are not used, they are converted to uint64, so new states can be included.
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetStateOrder : public ScriptingObject
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetStateOrder);
public:
    FudgetStateOrder(FudgetStateOrderItemList *data) : Base(SpawnParams(Guid::New(), TypeInitializer)), _data(data) {}
    ~FudgetStateOrder() { delete _data; }

    /// <summary>
    /// Finds the state flag(s) that first match in this state order. It's enough for one of the flags to match in any
    /// of the items in the state order to match. For example if an item is specifying both Focused and Selected, and
    /// the state flags only have either Focused or Selected, but not both, it will still match.
    /// To make sure both are required, make a state order with one of the flags first, and a sub-item for the other
    /// flag.
    /// </summary>
    /// <param name="states">States that are matched to the draw order list</param>
    /// <returns>The state flag that first matched the states</returns>
    API_FUNCTION() uint64 GetMatchingState(uint64 states) const;

private:
    FudgetStateOrderItemList *_data;
};

/// <summary>
/// Static type that can be used to make a priority order of states that are used when part painters are drawn.
/// 
/// A control can be in multiple states at the same time. For example it can be focused, hovered and selected simultaneously.
/// When drawing its parts, like the frame or text, the resources used for drawing might only correspond to one of the states. 
/// To decide which texture or color to use for the current state, a priority order of states is necessary.
/// 
/// Start an ordering of states by calling Begin. Add each state with AddState (the value of the more important states first,)
/// to decide their priority. When states need to be combined to use a given resource, the states should be nested by
/// first adding one of them with AddStateWithSubstates, and then the other one with AddState. Call AddStateWithSubstates
/// multiple times if multiple states need to be combined. Using AddStateWithSubstates creates a nesting of states.
/// 
/// If a state can be combined with different states, but those shouldn't be combined together, call AddState after calling
/// AddStateWithSubstates with the states to combine with, in their order of priority.
/// 
/// It's necessary to call EndSubData for each AddStateWithSubstates, once the ordering of sub-states are done for that state. 
/// 
/// It's also possible to call AddState or AddStateWithSubstates with an OR'ed value of multiple states. This means that any
/// of the states will match, but they don't have to all appear together.
/// 
/// Call End when the state ordering is done. This will return an index that can be used to access the ordering. Usually
/// part painters have a ***Resources structure when accessing them, with a StateOrderIndex member. Set this to the ordering's
/// index to make the painter use the corresponding state order. The order can also be accessed by FudgetThemes::GetStateOrder,
/// but it's usually not necessary, unless you are making a new style-aware part painter.
/// 
/// FudgetStates contains a few ***_STATE_ORDER values for pre-built state priority data as well.
/// 
/// For usage of the built state order, check the Initialize function of the included painters that use states, for fetching
/// the state order from the themes, and the first lines of the Draw function.
/// </summary>
API_CLASS(Static)
class FUDGETS_API FudgetStateOrderBuilder
{
    DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetStateOrderBuilder);
public:
    /// <summary>
    /// Starts building a priority ordering of states.
    /// </summary>
    /// <returns></returns>
    API_FUNCTION() static bool Begin();
    /// <summary>
    /// Finishes the priority ordering of states. Only valid to call after a previous call to Begin, but only once.
    /// </summary>
    /// <param name="abort">Whether the built data should be discarded. In this case the result is not a valid index.</param>
    /// <returns>The index of the built state order that can be retrieved from FudgetThemes::GetStateOrder.</returns>
    API_FUNCTION() static int End(bool abort = false);
    /// <summary>
    /// Adds a state to the built state order. Must be called after Begin was called at least once but before calling End.
    /// It can be called multiple times before End. States added first are prioritized by part painters.
    /// Calling this with multiple state values OR'ed together will add a priority that matches either of those states.
    /// </summary>
    /// <param name="state">The state(s) to add</param>
    API_FUNCTION() static void AddState(uint64 state);
    /// <summary>
    /// Adds a state to the built state order and begins a sub-state block for that state. Must be called after Begin was
    /// called at least once but before calling End. It can be called multiple times before End. Calling AddState or
    /// AddStateWithSubstates in this block will combine the newly added states with the original. For example calling it
    /// with FudgetVisualControlState::Focused, and then AddState with FudgetVisualControlState::Selected (both cast to uint64
    /// or ulong in C#) can be used to match these states only when a control has both states at the same time.
    /// On the other hand, calling AddState with FudgetVisualControlState::Focused | FudgetVisualControlState::Selected
    /// will add a state that matches when either of these states are found.
    /// Call EndSubData to end the sub-state block.
    /// </summary>
    /// <param name="state">The state(s) to add, that will be combined with the other states.</param>
    API_FUNCTION() static void AddStateWithSubstates(uint64 state);
    /// <summary>
    /// Call for each AddStateWithSubstates after their sub-state blocks are set up.
    /// </summary>
    API_FUNCTION() static void EndSubData();
private:
    static bool _building;
    static FudgetStateOrderItemList *_data;
    static std::vector<FudgetStateOrderItemList*> _sub_data;
};

