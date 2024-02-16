#include "Container.h"
#include "Fudget.h"
#include "Layouts/Layout.h"
#include "Engine/Core/Math/Rectangle.h"
#include "Utils/Utils.h"
#include "Engine/Core/Log.h"
#include "Engine/Serialization/JsonTools.h"


FudgetContainer::FudgetContainer(const SpawnParams &params) : Base(params),
    FillColor(1.0f), DrawFilledBackground(false),_layout(nullptr), _changing(false), _hint_width_from_layout(false), _hint_height_from_layout(false),
    _min_width_from_layout(false), _max_width_from_layout(false), _min_height_from_layout(false), _max_height_from_layout(false)
{

}

FudgetContainer::~FudgetContainer()
{
    DeleteAll();
    if (_layout != nullptr)
        Delete(_layout);
}

FUDGET_FACTORY(FudgetLayout, layout);

int FudgetContainer::AddChild(FudgetControl *control, int index)
{
    if (control == nullptr || control->GetParent() != nullptr)
        return -1;

    if (_changing)
    {
        if (control->GetParent() == this)
            return ChildIndex(control);
        return -1;
    }

    if (control->GetParent() == this)
    {
        int result = control->_index;
        if (result == -1)
            result = ChildIndex(control);
        if (result != -1)
            return result;
    }

    _changing = true;

    if (index >= 0 && index < _children.Count())
    {
        _children.Insert(index, control);
        for (int ix = index + 1, siz = _children.Count(); ix < siz; ++ix)
            _children[ix]->_index = ix;
    }
    else
    {
        index = _children.Count();
        _children.Add(control);
    }

    control->_parent = this;
    control->_index = index;
    control->_guiRoot = GetGUIRoot();
    control->SetState(FudgetControlState::ParentDisabled, !VirtuallyEnabled());
    control->InitializeFlags();

    control->RegisterToUpdate(control->HasAnyFlag(FudgetControlFlags::RegisterToUpdates));

    if (_layout != nullptr)
        _layout->ChildAdded(control, index);

    if (_guiRoot != nullptr && !control->HasAnyState(FudgetControlState::Initialized))
    {
        control->Initialize();
        control->SetState(FudgetControlState::Initialized, true);
    }

    _changing = false;

    return index;
}

int FudgetContainer::RemoveChild(FudgetControl *control)
{
    if (control == nullptr || control->GetParent() != this || _changing)
        return -1;

    _changing = true;

    int index = control->_index;
    _children.RemoveAtKeepOrder(index);
    control->_parent = nullptr;
    control->_index = -1;
    control->_guiRoot = nullptr;
    control->SetState(FudgetControlState::ParentDisabled, false);
    if (!control->_changing)
        control->RegisterToUpdate(false);

    for (int ix = index, siz = _children.Count(); ix < siz; ++ix)
        _children[ix]->_index = ix;

    if (_layout != nullptr)
        _layout->ChildRemoved(index);

    _changing = false;

    return index;
}

FudgetControl* FudgetContainer::RemoveChild(int index)
{
    if (index < 0 || _children.Count() <= index || _changing)
        return nullptr;
    FudgetControl *control = _children[index];
    RemoveChild(control);
    return control;
}

bool FudgetContainer::MoveChildToIndex(int from, int to)
{
    if (_changing)
        return true;

    if (from < 0 || to < 0 || from >= _children.Count() || to >= _children.Count())
        return false;

    if (from == to)
        return true;

    _changing = true;

    FudgetControl *control = _children[from];
    MoveInArray(_children, from, to);

    for (int ix = Math::Min(from, to), siz = Math::Max(from, to) + 1; ix < siz; ++ix)
        _children[ix]->_index = ix;

    if (_layout != nullptr)
        _layout->ChildMoved(from, to);

    _changing = false;

    return true;
}

FudgetControl* FudgetContainer::ChildAt(int index) const
{
    if (index < 0 || _children.Count() <= index)
        return nullptr;
    return _children[index];
}

int FudgetContainer::GetChildCount() const
{
    return _children.Count();
}

int FudgetContainer::ChildIndex(FudgetControl *control) const
{
    if (control->GetParent() != this)
        return -1;
    int o = control->GetIndexInParent();
    if (o >= 0 && o < _children.Count() && _children[o] == control)
        return o;

    for (int ix = 0, siz = _children.Count(); ix < siz; ++ix)
    {
        if (_children[ix] == control)
            return ix;
    }
    return -1;
}

void FudgetContainer::DeleteAll()
{
    for (int ix = _children.Count() - 1; ix > -1; --ix)
    {
        // TODO: lock the layout to not change until this loop is over, if it's noticable on performance

        FudgetControl *c = _children[ix];
        c->SetParent(nullptr);
        Delete(c);
    }
    if (_layout != nullptr)
        _layout->AllDeleted();
}

void FudgetContainer::SetEnabled(bool value)
{
    Base::SetEnabled(value);
    if (!HasAnyState(FudgetControlState::ParentDisabled))
        SetParentDisabledRecursive();
    // TODO: when implementing events, make sure the enabled event is only sent out after this recursive call
}

Float2 FudgetContainer::GetHintSize() const
{
    bool self_width = _layout == nullptr || !_hint_width_from_layout || _layout->HasAnyFlag(FudgetLayoutFlag::CanProvideHintSizeWidth);
    bool self_height = _layout == nullptr || !_hint_height_from_layout || _layout->HasAnyFlag(FudgetLayoutFlag::CanProvideHintSizeHeight);

    if (!self_width || !self_height)
    {
        Float2 value = _layout->GetHintSize();
        if (!self_width && !self_height)
            return value;

        Float2 base = Base::GetHintSize();
        if (self_width)
            value.X = base.X;
        if (self_height)
            value.Y = base.Y;
        return value;
    }
    return Base::GetHintSize();
}

void FudgetContainer::SetHintSize(Float2 value)
{
    if (Float2::NearEqual(_hint_size, value))
        return;
    Base::SetHintSize(value);

    if (_layout == nullptr || (_hint_width_from_layout && _layout->HasAnyFlag(FudgetLayoutFlag::CanProvideHintSizeWidth) &&
        _hint_height_from_layout && _layout->HasAnyFlag(FudgetLayoutFlag::CanProvideHintSizeHeight)))
        return;

    MarkLayoutDirty(FudgetDirtType::Size);
}

Float2 FudgetContainer::GetMinSize() const
{
    bool self_width = _layout == nullptr || !_min_width_from_layout || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMinSizeWidth);
    bool self_height = _layout == nullptr || !_min_height_from_layout || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMinSizeHeight);

    if (!self_width || !self_height)
    {
        Float2 value = _layout->GetMinSize();
        if (!self_width && !self_height)
            return Float2::Max(Float2::Zero, value);

        Float2 base = Base::GetMinSize();
        if (self_width)
            value.X = base.X;
        if (self_height)
            value.Y = base.Y;
        return Float2::Max(Float2::Zero, value);
    }
    return Base::GetMinSize();
}

void FudgetContainer::SetMinSize(Float2 value)
{
    if (Float2::NearEqual(_min_size, value))
        return;
    Base::SetMinSize(value);

    if (_layout == nullptr || (_min_width_from_layout && _layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMinSizeWidth) &&
        _min_height_from_layout && _layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMinSizeHeight)))
        return;

    MarkLayoutDirty(FudgetDirtType::Size);
}

Float2 FudgetContainer::GetMaxSize() const
{
    bool self_width = _layout == nullptr || !_max_width_from_layout || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMaxSizeWidth);
    bool self_height = _layout == nullptr || !_max_height_from_layout || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMaxSizeHeight);

    if (!self_width || !self_height)
    {
        Float2 value = _layout->GetMaxSize();
        if (!self_width && !self_height)
            return value;

        Float2 base = Base::GetMaxSize();
        if (self_width)
            value.X = base.X;
        if (self_height)
            value.Y = base.Y;
        return value;
    }
    return Base::GetMaxSize();

}

void FudgetContainer::SetMaxSize(Float2 value)
{
    if (Float2::NearEqual(_max_size, value))
        return;
    Base::SetMaxSize(value);

    if (_layout == nullptr || (_max_width_from_layout && _layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMaxSizeWidth) &&
        _max_height_from_layout && _layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMaxSizeHeight)))
        return;

    MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::SetUsingLayoutHintWidth(bool value)
{
    if (_hint_width_from_layout == value)
        return;

    _hint_width_from_layout = value;

    if (_layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideHintSizeWidth))
        return;

    MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::SetUsingLayoutHintHeight(bool value)
{
    if (_hint_height_from_layout == value)
        return;

    _hint_height_from_layout = value;

    if (_layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideHintSizeHeight))
        return;

    MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::SetUsingLayoutMinWidth(bool value)
{
    if (_min_width_from_layout == value)
        return;

    _min_width_from_layout = value;

    if (_layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMinSizeWidth))
        return;

    MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::SetUsingLayoutMinHeight(bool value)
{
    if (_min_height_from_layout == value)
        return;

    _min_height_from_layout = value;

    if (_layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMinSizeHeight))
        return;

    MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::SetUsingLayoutMaxWidth(bool value)
{
    if (_max_width_from_layout == value)
        return;

    _max_width_from_layout = value;

    if (_layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMaxSizeWidth))
        return;

    MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::SetUsingLayoutMaxHeight(bool value)
{
    if (_max_height_from_layout == value)
        return;

    _max_height_from_layout = value;

    if (_layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMaxSizeHeight))
        return;

    MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::MarkLayoutDirty(FudgetDirtType dirt_flags, bool content_changed)
{
    if (_layout == nullptr)
        return;

    _layout->MarkDirty(dirt_flags, content_changed);

    if (_parent != nullptr && _layout->IsSizeDirty())
    {
        // Notify the parent only, if the change in the layout influences the container's size as well
        if (GetUsingLayoutHintWidth() || GetUsingLayoutHintHeight() ||
            GetUsingLayoutMinWidth() || GetUsingLayoutMaxWidth() ||
            GetUsingLayoutMinHeight() || GetUsingLayoutMaxHeight())
            _parent->MarkLayoutDirty(dirt_flags, true);
    }
}

void FudgetContainer::RequestLayout()
{
    if (_layout != nullptr)
    {
        _layout->RequestLayoutChildren(false);
        return;
    }

    // TODO: mark sizes dirty and don't do anything when it's not

    // With no layout, every control is positioned wherever they want to be.
    for (int ix = 0, siz = _children.Count(); ix < siz; ++ix)
    {
        FudgetControl *control = _children[ix];
        if (!Math::NearEqual(control->_size, control->_hint_size))
        {
            control->_size = control->_hint_size;
            control->_pos_layout_updated = true;
        }
    }
}

void FudgetContainer::OnDraw()
{
    if (DrawFilledBackground)
    {
        FillRectangle(Float2(0.0f), GetSize(), FillColor);
    }
}

void FudgetContainer::DoFocusChanged(bool focused, FudgetControl *other)
{
    Base::DoFocusChanged(focused, other);

    if (IsCompoundControl())
    {
        Array<int> indexes;
        FudgetContainer *pos = this;
        indexes.Add(0);
        int ix = 0;
        while (pos != nullptr && indexes.Count() != 0)
        {
            ix = indexes.Pop();
            for (int siz = pos->_children.Count(); ix < siz; ++ix)
            {
                FudgetControl *c = pos->_children[ix];
                c->SetState(FudgetControlState::ShowFocused, focused);
                if (c->HasAnyFlag(FudgetControlFlags::ContainerControl))
                {
                    FudgetContainer *container = dynamic_cast<FudgetContainer*>(c);
                    if (container == nullptr || container->GetChildCount() == 0)
                        continue;
                    indexes.Push(ix + 1);
                    pos = container;
                    ix = -1;
                    siz = pos->_children.Count();
                }
            }
            pos = pos->GetParent();
        } 

        return;
    }
}

void FudgetContainer::ClearStyleCache(bool inherited)
{
    Base::ClearStyleCache(inherited);
    if (inherited)
    {
        for (FudgetControl *c : _children)
            c->ClearStyleCache(true);
    }
}

void FudgetContainer::SetLayout(FudgetLayout *value)
{
    if (_layout == value || _changing)
        return;
    _changing = true;
    _layout = value;
    if (_layout != nullptr)
        _layout->SetOwnerInternal(this);
    _changing = false;
}

void FudgetContainer::SizeOrPosModified(FudgetDirtType dirt_flags)
{
    ; // NO-OP
}

void FudgetContainer::SetControlFlags(FudgetControlFlags flags)
{
    flags |= FudgetControlFlags::ContainerControl;
    Base::SetControlFlags(flags);

    if (_parent != nullptr)
        _parent->MarkLayoutDirty(FudgetDirtType::All, true);
}

bool FudgetContainer::IsControlPositioningPermitted(const FudgetControl *control) const
{
    if (_layout == nullptr)
        return true;
    if (control->GetParent() != this)
        return false;
    return _layout->IsControlPositioningPermitted(control);
}

void FudgetContainer::GetAllControls(const FudgetContainer *container, API_PARAM(Ref) Array<FudgetControl*> &result)
{
    for (int i = 0; i < container->GetChildCount(); i++)
    {
        FudgetControl* child = container->ChildAt(i);
        result.Add(child);

        if (child->Is<FudgetContainer>())
            GetAllControls((FudgetContainer*)child, result);
    }
}

void FudgetContainer::GetAllControls(API_PARAM(Ref) Array<FudgetControl*> &result) const
{
    FudgetContainer::GetAllControls(this, result);
}

void FudgetContainer::ControlsAtPosition(Float2 pos, FudgetControlFlags request, FudgetControlFlags reject, FudgetControlFlags block, API_PARAM(Ref) Array<FudgetControl*> &result)
{
    for (int ix = 0, siz = _children.Count(); ix < siz; ++ix)
    {
        FudgetControl *control = _children[ix];
        if (!control->GetBoundsInParent().Contains(pos))
            continue;

        if ((request == FudgetControlFlags::None || control->HasAnyFlag(request)) && (reject == FudgetControlFlags::None || !control->HasAnyFlag(reject)))
            result.Add(control);
        if (control->HasAnyFlag(FudgetControlFlags::ContainerControl) && (block == FudgetControlFlags::None || !control->HasAnyFlag(block)))
            dynamic_cast<FudgetContainer*>(control)->ControlsAtPosition(pos - control->_pos, request, reject, block, result);
    }
}

void FudgetContainer::Serialize(SerializeStream& stream, const void* otherObj)
{
    Base::Serialize(stream, otherObj);
    if (_layout != nullptr)
    {
        stream.JKEY("Layout");
        stream.StartObject();
        _layout->Serialize(stream, otherObj);
        stream.EndObject();
    }
}

void FudgetContainer::Deserialize(DeserializeStream& stream, ISerializeModifier* modifier)
{
    Base::Deserialize(stream, modifier);

    const auto& layoutMember = stream.FindMember("Layout");
    if (layoutMember != stream.MemberEnd() && layoutMember->value.IsObject())
    {
        FudgetLayout* layout = FudgetLayoutFactory((DeserializeStream&)layoutMember->value, modifier);
        if (layout == nullptr)
        {
            return;
        }

        layout->Deserialize((DeserializeStream&)layoutMember->value, modifier);

        // Once we have data, register the object.
        if (!layout->IsRegistered())
            layout->RegisterObject();

        SetLayout(layout);
    }
}

void FudgetContainer::DoDraw()
{
    RequestLayout();

    Base::DoDraw();

    for (FudgetControl *c : _children)
        c->DoDraw();
}

void FudgetContainer::Initialize()
{
    if (_guiRoot == nullptr)
        return;

    for (FudgetControl *c : _children)
    {
        if (!c->IsInitialized())
        {
            c->Initialize();
            c->SetState(FudgetControlState::Initialized, true);
        }
    }
    Base::Initialize();
}

FudgetControlFlags FudgetContainer::GetInitFlags() const
{
    return Base::GetInitFlags() | FudgetControlFlags::ContainerControl | FudgetControlFlags::BlockMouseEvents;
}

void FudgetContainer::SetParentDisabled(bool value)
{
    bool enabled = VirtuallyEnabled();
    Base::SetParentDisabled(value);
    if (enabled != value)
        SetParentDisabledRecursive();
}

void FudgetContainer::SetParentDisabledRecursive()
{
    for (FudgetControl *c : _children)
        c->SetParentDisabled(!VirtuallyEnabled());
}

void FudgetContainer::LayoutUpdate(Float2 pos, Float2 size)
{
    bool pos_changed = !Float2::NearEqual(_pos, pos);
    bool size_changed = !Float2::NearEqual(_size, size);

    Base::LayoutUpdate(pos, size);

    if (_layout != nullptr && (pos_changed || size_changed))
    {
        FudgetDirtType type = (FudgetDirtType)((pos_changed ? (int)FudgetDirtType::Position : 0) | (size_changed ? (int)FudgetDirtType::Size : 0));

        _layout->MarkDirtyOnLayoutUpdate(type);
    }
}

