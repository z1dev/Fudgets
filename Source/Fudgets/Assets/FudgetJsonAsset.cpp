#include "FudgetJsonAsset.h"
#include "Engine/Serialization/JsonWriters.h"
#include "Engine/Content/Factories/JsonAssetFactory.h"
#include "Engine/Serialization/ISerializeModifier.h"
#include "Engine/Core/Log.h"

REGISTER_JSON_ASSET(FudgetJsonAsset, "Fudgets.FudgetJsonAsset", true);

FudgetJsonAsset::FudgetJsonAsset(const SpawnParams& params, const AssetInfo* info)
    : JsonAssetBase(params, info)
    , WidgetData(nullptr)
{
    DataTypeName = TypeName;
}

String FudgetJsonAsset::GetSerialized()
{
    rapidjson_flax::StringBuffer output;
    OnGetData(output);

    return String(output.GetString());
}

Asset::LoadResult FudgetJsonAsset::loadAsset()
{
    // Base
    auto result = JsonAssetBase::loadAsset();
    if (result != LoadResult::Ok || IsInternalType())
        return result;

    WidgetData = New<FudgetGUIRoot>();
    WidgetData->Deserialize(*Data, new ISerializeModifier());
    WidgetData->Deleted.Bind<FudgetJsonAsset, &FudgetJsonAsset::ItemDeleted>(this);
    for (int i = 0; i < WidgetData->GetChildCount(); i++)
    {
        LOG(Info, "Control Name: {0}", WidgetData->ChildAt(i)->GetName());
    }

    return result;
}

void FudgetJsonAsset::ItemDeleted(ScriptingObject* item)
{
    WidgetData = nullptr;
}

void FudgetJsonAsset::unload(bool isReloading)
{
    // Base
    JsonAssetBase::unload(isReloading);

    // TODO: Actually unload. (and test if this works)
    SAFE_DELETE(WidgetData);
}

void FudgetJsonAsset::OnGetData(rapidjson_flax::StringBuffer& buffer) const
{
    PrettyJsonWriter writerObj(buffer);
    JsonWriter& writer = writerObj;
    writer.StartObject();
    {
        WidgetData->Serialize(writer, nullptr);
    }
    writer.EndObject();
}
