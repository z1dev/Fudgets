#include "Container.h"
#include "Fudget.h"
#include "Layouts/Layout.h"
#include "Layouts/ContainerLayout.h"
#include "Engine/Core/Math/Rectangle.h"
#include "Utils/Utils.h"
#include "Engine/Core/Log.h"
#include "Engine/Serialization/JsonTools.h"


FudgetContainer::FudgetContainer(const SpawnParams &params) : Base(params),
    FillColor(1.0f), DrawFilledBackground(false), _layout(nullptr), _dummy_layout(true), _size_overrides(FudgetSizeOverride::AllUnrestricted), _changing(false)
{
    CreateDummyLayout();
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

    FudgetContainer *old_parent = control->_parent;

    //if (control->GetGUIRoot() != GetGUIRoot())
    //    control->RegisterToUpdate(false);

    control->_parent = this;
    control->_index = index;
    //control->SetState(FudgetControlState::ParentDisabled, !VirtuallyEnabled());
    //control->InitializeFlags();

    if (_layout != nullptr)
        _layout->ChildAdded(control, index);

    control->DoParentChanged(old_parent);

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

    //control->SetGUIRootInternal(nullptr);
    control->SetState(FudgetControlState::ParentDisabled, false);

    //if (!control->_changing)
    //    control->RegisterToUpdate(false);

    for (int ix = index, siz = _children.Count(); ix < siz; ++ix)
        _children[ix]->_index = ix;

    if (_layout != nullptr)
        _layout->ChildRemoved(index);

    control->DoParentChanged(nullptr);

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

Float2 FudgetContainer::GetLayoutHintSize() const
{
    bool self_width = !HasSizeOverride(FudgetSizeOverride::LayoutHintWidth) || _layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideHintSizeWidth)
        || (HasSizeOverride(FudgetSizeOverride::UnrestrictedHintWidth) && IsDirectSizeChangePermitted());
    bool self_height = !HasSizeOverride(FudgetSizeOverride::LayoutHintHeight) || _layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideHintSizeHeight)
        || (HasSizeOverride(FudgetSizeOverride::UnrestrictedHintHeight) && IsDirectSizeChangePermitted());

    Float2 min_size = GetLayoutMinSize();
    Float2 max_size = GetLayoutMaxSize();

    if (!self_height && !self_width)
        return Float2::Min(Float2::Max(min_size, _layout->GetHintSize()), max_size);
    if (self_height && self_width)
        return Float2::Min(Float2::Max(min_size, GetHintSize()), max_size);

    Float2 value = Float2::Min(Float2::Max(min_size, _layout->GetHintSize()), max_size);
    Float2 base = Float2::Min(Float2::Max(min_size, GetHintSize()), max_size);

    if (self_width)
        value.X = base.X;
    if (self_height)
        value.Y = base.Y;

    return value;
}

Float2 FudgetContainer::GetLayoutMinSize() const
{
    bool self_width = !HasSizeOverride(FudgetSizeOverride::LayoutMinWidth) || _layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMinSizeWidth)
        || (HasSizeOverride(FudgetSizeOverride::UnrestrictedMinWidth) && IsDirectSizeChangePermitted());
    bool self_height = !HasSizeOverride(FudgetSizeOverride::LayoutMinHeight) || _layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMinSizeHeight)
        || (HasSizeOverride(FudgetSizeOverride::UnrestrictedMinHeight) && IsDirectSizeChangePermitted());



    if (!self_height && !self_width)
        return Float2::Max(Float2::Zero, _layout->GetMinSize());
    if (self_height && self_width)
        return Float2::Max(Float2::Zero, GetMinSize());

    Float2 value = Float2::Max(Float2::Zero, _layout->GetMinSize());
    Float2 base = Float2::Max(Float2::Zero, GetMinSize());

    if (self_width)
        value.X = base.X;
    if (self_height)
        value.Y = base.Y;

    return value;
}

Float2 FudgetContainer::GetLayoutMaxSize() const
{
    bool self_width = !HasSizeOverride(FudgetSizeOverride::LayoutMaxWidth) || _layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMaxSizeWidth)
        || (HasSizeOverride(FudgetSizeOverride::UnrestrictedMaxWidth) && IsDirectSizeChangePermitted());
    bool self_height = !HasSizeOverride(FudgetSizeOverride::LayoutMaxHeight) || _layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMaxSizeHeight)
        || (HasSizeOverride(FudgetSizeOverride::UnrestrictedMaxHeight) && IsDirectSizeChangePermitted());
    

    if (!self_height && !self_width)
        return _layout->GetMaxSize();
    if (self_height && self_width)
        return GetMaxSize();

    Float2 value = _layout->GetMaxSize();
    Float2 base = GetMaxSize();

    if (self_width)
        value.X = base.X;
    if (self_height)
        value.Y = base.Y;

    return value;
}

Float2 FudgetContainer::LayoutSpace() const
{
    return GetSize();
}

void FudgetContainer::MarkLayoutDirty(FudgetLayoutDirtyReason dirt_flags, FudgetControl *control)
{
    if (control != nullptr && control->GetParent() != this)
        return;

    bool self = (dirt_flags & FudgetLayoutDirtyReason::Container) == FudgetLayoutDirtyReason::Container;
    bool size_change = _layout->MarkDirty(dirt_flags) && !IgnoresLayoutSizes();
    if (control != nullptr && (dirt_flags & FudgetLayoutDirtyReason::Size) == FudgetLayoutDirtyReason::Size)
        _layout->MarkControlSizesDirty(control->GetIndexInParent());

    if (_parent == nullptr)
        return;

    if (size_change)
        dirt_flags = FudgetLayoutDirtyReason::Size;

    if (!self && !size_change)
        return;

    dirt_flags &= ~FudgetLayoutDirtyReason::Container;

    if ((int)dirt_flags != 0)
        _parent->MarkLayoutDirty(dirt_flags, this);
}

void FudgetContainer::EnsureLayout()
{
    if (_layout == nullptr)
        return;

    if (_parent == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::LayoutOnContainerChange))
    {
        if (_layout->IsLayoutDirty())
            RequestLayout();
        return;
    }

    _parent->EnsureLayoutParent(_layout->IsLayoutDirty() ? this : nullptr);
}

void FudgetContainer::OnDraw()
{
    if (DrawFilledBackground)
    {
        FillRectangle(Float2(0.0f), GetSize(), FillColor);
    }
}

bool FudgetContainer::OnMeasure(Float2 available, API_PARAM(Out) Float2 &wanted, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size)
{
    bool result = false;
    if (!IgnoresLayoutSizes())
    {
        _layout->DoLayoutChildren(available);
        if (_layout->HasAllFlags(FudgetLayoutFlag::CanProvideSizes))
            result = _layout->SizeDependsOnSpace();
        else if (!_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideSizes))
            result = SizeDependsOnSpace();
        else
            result = _layout->SizeDependsOnSpace() || SizeDependsOnSpace();
    }

    wanted = GetLayoutHintSize();
    min_size = GetLayoutMinSize();
    max_size = GetLayoutMaxSize();

    return result;
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
                if (c->HasAnyFlag(FudgetControlFlag::ContainerControl))
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

FudgetLayout* FudgetContainer::SetLayout(FudgetLayout *value)
{
    if (_layout == value || (value == nullptr && _dummy_layout) || _changing)
        return nullptr;
    FudgetLayout *old_layout = _layout;

    _changing = true;

    if (old_layout != nullptr)
    {
        old_layout->SetOwnerInternal(nullptr);
        if (_dummy_layout)
        {
            _dummy_layout = false;
            Delete(old_layout);
            old_layout = nullptr;
        }
    }

    _layout = value;
    if (_layout != nullptr)
        _layout->SetOwnerInternal(this);
    else
        CreateDummyLayout();

    _changing = false;

    return old_layout;
}

void FudgetContainer::SizeOrPosModified(FudgetLayoutDirtyReason dirt_flags)
{
    MarkLayoutDirty(dirt_flags | FudgetLayoutDirtyReason::Container);
}

void FudgetContainer::SetControlFlags(FudgetControlFlag flags)
{
    flags |= FudgetControlFlag::ContainerControl;
    Base::SetControlFlags(flags);
    //MarkLayoutDirty(FudgetLayoutDirtyReason::All | FudgetLayoutDirtyReason::Container);
    //if (_parent != nullptr)
    //    _parent->MarkLayoutDirty(FudgetLayoutDirtyReason::All);
}

bool FudgetContainer::IsControlPositionChangePermitted(const FudgetControl *control) const
{
    if (_layout == nullptr)
        return true;
    if (control->GetParent() != this)
        return false;
    return _layout->IsControlPositionChangePermitted(control);
}

bool FudgetContainer::IsControlDirectSizeChangePermitted(const FudgetControl *control) const
{
    if (_layout == nullptr)
        return true;
    if (control->GetParent() != this)
        return false;
    return _layout->IsControlDirectSizeChangePermitted(control);
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

void FudgetContainer::ControlsAtPosition(Float2 pos, FudgetControlFlag request, FudgetControlFlag reject, FudgetControlFlag block, API_PARAM(Ref) Array<FudgetControl*> &result)
{
    for (int ix = 0, siz = _children.Count(); ix < siz; ++ix)
    {
        FudgetControl *control = _children[ix];
        if (!control->GetBoundsInParent().Contains(pos))
            continue;

        if ((request == FudgetControlFlag::None || control->HasAnyFlag(request)) && (reject == FudgetControlFlag::None || !control->HasAnyFlag(reject)))
            result.Add(control);
        if (control->HasAnyFlag(FudgetControlFlag::ContainerControl) && (block == FudgetControlFlag::None || !control->HasAnyFlag(block)))
            dynamic_cast<FudgetContainer*>(control)->ControlsAtPosition(pos - control->_pos, request, reject, block, result);
    }
}

bool FudgetContainer::IgnoresLayoutHintSize() const
{
    bool self_width = !HasSizeOverride(FudgetSizeOverride::LayoutHintWidth) || _layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideHintSizeWidth)
        || (HasSizeOverride(FudgetSizeOverride::UnrestrictedHintWidth) && IsDirectSizeChangePermitted());
    bool self_height = !HasSizeOverride(FudgetSizeOverride::LayoutHintHeight) || _layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideHintSizeHeight)
        || (HasSizeOverride(FudgetSizeOverride::UnrestrictedHintHeight) && IsDirectSizeChangePermitted());

    return self_width && self_height;
}

bool FudgetContainer::IgnoresLayoutMinSize() const
{

    bool self_width = !HasSizeOverride(FudgetSizeOverride::LayoutMinWidth) || _layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMinSizeWidth)
        || (HasSizeOverride(FudgetSizeOverride::UnrestrictedMinWidth) && IsDirectSizeChangePermitted());
    bool self_height = !HasSizeOverride(FudgetSizeOverride::LayoutMinHeight) || _layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMinSizeHeight)
        || (HasSizeOverride(FudgetSizeOverride::UnrestrictedMinHeight) && IsDirectSizeChangePermitted());

    return self_width && self_height;
}

bool FudgetContainer::IgnoresLayoutMaxSize() const
{

    bool self_width = !HasSizeOverride(FudgetSizeOverride::LayoutMaxWidth) || _layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMaxSizeWidth)
        || (HasSizeOverride(FudgetSizeOverride::UnrestrictedMaxWidth) && IsDirectSizeChangePermitted());
    bool self_height = !HasSizeOverride(FudgetSizeOverride::LayoutMaxHeight) || _layout == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::CanProvideMaxSizeHeight)
        || (HasSizeOverride(FudgetSizeOverride::UnrestrictedMaxHeight) && IsDirectSizeChangePermitted());

    return self_width && self_height;
}

bool FudgetContainer::IgnoresLayoutSizes() const
{
    return IgnoresLayoutHintSize() && IgnoresLayoutMinSize() && IgnoresLayoutMaxSize();
}

void FudgetContainer::SetSizeOverrides(FudgetSizeOverride value)
{
    if (_size_overrides == value)
        return;
    _size_overrides = value;
    MarkLayoutDirty(FudgetLayoutDirtyReason::Size | FudgetLayoutDirtyReason::Container);
}

bool FudgetContainer::HasSizeOverride(FudgetSizeOverride value) const
{
    return (_size_overrides & value) == value;
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

FudgetControlFlag FudgetContainer::GetInitFlags() const
{
    return Base::GetInitFlags() | FudgetControlFlag::ContainerControl | FudgetControlFlag::BlockMouseEvents;
}

void FudgetContainer::RequestLayout()
{
    _layout->RequestLayoutChildren(false);
    for (FudgetControl *c : _children)
        c->RequestLayout();
}

void FudgetContainer::SetParentDisabled(bool value)
{
    bool enabled = VirtuallyEnabled();
    Base::SetParentDisabled(value);
    if (enabled != value)
        SetParentDisabledRecursive();
}

void FudgetContainer::DoRootChanging(FudgetGUIRoot *new_root)
{
    Base::DoRootChanging(new_root);
    for (FudgetControl *c : _children)
        c->DoRootChanging(new_root);
}

void FudgetContainer::DoRootChanged(FudgetGUIRoot *old_root)
{
    Base::DoRootChanged(old_root);
    for (FudgetControl *c : _children)
    {
        c->_guiRoot = GetGUIRoot();
        c->DoRootChanged(old_root);
    }
}

void FudgetContainer::SetParentDisabledRecursive()
{
    for (FudgetControl *c : _children)
        c->SetParentDisabled(!VirtuallyEnabled());
}

void FudgetContainer::EnsureLayoutParent(FudgetContainer *last_dirty)
{
    if (_parent == nullptr || !_layout->HasAnyFlag(FudgetLayoutFlag::LayoutOnContainerChange))
    {
        if (_layout->IsLayoutDirty())
            RequestLayout();
        else if (last_dirty != nullptr)
            last_dirty->RequestLayout();
        return;
    }

    if (_layout->IsLayoutDirty())
    {
        if (_parent != nullptr)
            _parent->EnsureLayoutParent(this);
        else
            RequestLayout();
        return;
    }

    _parent->EnsureLayoutParent(last_dirty);
}

void FudgetContainer::LayoutUpdate(Float2 pos, Float2 size)
{
    bool pos_changed = !Float2::NearEqual(_pos, pos);
    bool size_changed = !Float2::NearEqual(_size, size);

    Base::LayoutUpdate(pos, size);

    if (_layout != nullptr && (pos_changed || size_changed))
    {
        FudgetLayoutDirtyReason type = (FudgetLayoutDirtyReason)((pos_changed ? (int)FudgetLayoutDirtyReason::Position : 0) | (size_changed ? (int)FudgetLayoutDirtyReason::Size : 0));

        _layout->MarkDirtyOnLayoutUpdate(type);
    }
}

void FudgetContainer::CreateDummyLayout()
{
    if (_layout != nullptr)
    {
        LOG(Error, "Setting a dummy layout when there is already a layout in this container");
        return;
    }
    _dummy_layout = true;
    _layout = New<FudgetContainerLayout>(SpawnParams(Guid::New(), FudgetContainerLayout::TypeInitializer));
    _layout->SetOwnerInternal(this);
}
