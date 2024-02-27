#include "StyleAreaBuilder.h"

#include "Engine/Core/Log.h"


FudgetStyleAreaList::FudgetStyleAreaList(const SpawnParams &params) : Base(params)
{

}

bool FudgetStyleAreaBuilder::_building = false;
FudgetStyleAreaList *FudgetStyleAreaBuilder::_data = nullptr;
Array<FudgetStyleAreaList*> FudgetStyleAreaBuilder::_sub_data;

void FudgetStyleAreaBuilder::Begin()
{
    if (_building)
    {
        LOG(Warning, "Begin: Already building a style area. Ending it and starting a new one.");
        End();
    }
    //if (_data != nullptr)
    //    Delete(_data);
    _building = true;
    _data = New<FudgetStyleAreaList>(ScriptingObject::SpawnParams(Guid::New(), FudgetStyleAreaList::TypeInitializer));
}

void FudgetStyleAreaBuilder::End()
{
    if (!_building)
    {
        LOG(Error, "End: Not building a style area.");
        return;
    }

    while (!_sub_data.IsEmpty())
        EndSubData();
    _building = false;
}

FudgetStyleAreaList* FudgetStyleAreaBuilder::GetArea()
{
    if (_building)
        End();
    return _data;
}

void FudgetStyleAreaBuilder::AddColor(Color color)
{
    if (!_building)
    {
        LOG(Error, "AddColor: Not building a style area.");
        return;
    }

    FudgetStyleAreaList *data = _sub_data.IsEmpty() ? _data : _sub_data.Last();

    data->Types.Add(FudgetStyleAreaType::FillColor);
    data->Var.Add(Variant(color));
}

void FudgetStyleAreaBuilder::AddPadding(FudgetPadding padding)
{
    if (!_building)
    {
        LOG(Error, "AddPadding: Not building a style area.");
        return;
    }

    FudgetStyleAreaList *data = _sub_data.IsEmpty() ? _data : _sub_data.Last();

    data->Types.Add(FudgetStyleAreaType::Padding);
    VariantType vtype;
    vtype.SetTypeName(FudgetPadding::TypeInitializer.GetType().Fullname);
    vtype.Type = VariantType::Types::Structure;
    data->Var.Add(Variant::Structure<FudgetPadding>(std::move(vtype), padding));
}

void FudgetStyleAreaBuilder::AddBlur(float blur_strength)
{
    if (!_building)
    {
        LOG(Error, "AddBlur: Not building a style area.");
        return;
    }

    FudgetStyleAreaList *data = _sub_data.IsEmpty() ? _data : _sub_data.Last();

    data->Types.Add(FudgetStyleAreaType::Blur);
    data->Var.Add(Variant(blur_strength));
}

void FudgetStyleAreaBuilder::AddDrawArea(FudgetDrawArea area)
{
    if (!_building)
    {
        LOG(Error, "AddDrawArea: Not building a style area.");
        return;
    }

    FudgetStyleAreaList *data = _sub_data.IsEmpty() ? _data : _sub_data.Last();

    data->Types.Add(FudgetStyleAreaType::DrawArea);
    VariantType vtype;
    vtype.SetTypeName(FudgetDrawArea::TypeInitializer.GetType().Fullname);
    vtype.Type = VariantType::Types::Structure;
    data->Var.Add(Variant::Structure<FudgetDrawArea>(std::move(vtype), area));
}

void FudgetStyleAreaBuilder::BeginSubData()
{
    if (!_building)
    {
        LOG(Error, "BeginSubData: Not building a style area.");
        return;
    }

    FudgetStyleAreaList *new_data = New<FudgetStyleAreaList>();
    _sub_data.Add(new_data);
}

void FudgetStyleAreaBuilder::EndSubData()
{
    if (!_building)
    {
        LOG(Error, "EndSubData: Not building a style area.");
        return;
    }
    if (_sub_data.IsEmpty())
    {
        LOG(Error, "EndSubData: Not building sub-data.");
        return;
    }

    FudgetStyleAreaList *last = _sub_data.Last();
    FudgetStyleAreaList *data = _sub_data.Count() == 1 ? _data : _sub_data[_sub_data.Count() - 2];

    data->Types.Add(FudgetStyleAreaType::AreaList);
    //VariantType vtype;
    //vtype.SetTypeName(FudgetStyleAreaList::TypeInitializer.GetType().Fullname);
    //vtype.Type = VariantType::Types::Structure;
    data->Var.Add(last);

    _sub_data.RemoveLast();
}


