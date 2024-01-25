#pragma once

#include "Engine/Content/JsonAsset.h"
#include "Engine/Core/Collections/Array.h"
#include "Engine/Core/Collections/Dictionary.h"
#include "Engine/Scripting/SoftObjectReference.h"
#include "Fudgets/GUIRoot.h"

/// <summary>
/// Contains a Fudget widget.
/// </summary>
/// <seealso cref="JsonAssetBase" />
API_CLASS(NoSpawn) class FUDGETS_API FudgetJsonAsset : public JsonAssetBase
{
    DECLARE_ASSET_HEADER(FudgetJsonAsset);

public:
    /// <summary>
    /// TODO: write docs
    /// </summary>
    API_FIELD() FudgetGUIRoot* WidgetData;

    /// <summary>
    /// TODO: write docs
    /// </summary>
    API_FUNCTION() String GetSerialized();

protected:
    // [JsonAssetBase]
    LoadResult loadAsset() override;
    void unload(bool isReloading) override;
    void OnGetData(rapidjson_flax::StringBuffer& buffer) const override;
};
