#include "AssetPlaceholder.h"
#include "Utils/Utils.h"

#include "Engine/Scripting/Scripting.h"
#include "Engine/Content/Content.h"

#include "Engine/Core/Log.h"
#include <Engine/Serialization/JsonTools.h>
#include <Engine/Serialization/JsonWriters.h>
#include "Assets/FudgetJsonAsset.h"

FudgetAssetPlaceholder::FudgetAssetPlaceholder(const SpawnParams &params)
	: Base(params)
{
}

void FudgetAssetPlaceholder::Serialize(SerializeStream& stream, const void* otherObj)
{
	stream.JKEY("TypeName");
	stream.String(GetType().Fullname);

	SERIALIZE_GET_OTHER_OBJ(FudgetAssetPlaceholder);
	SERIALIZE_MEMBER(PlaceholderParent, _deserializationParent.GetID());
	SERIALIZE_MEMBER(SourceAsset, _sourceAsset);
}

void FudgetAssetPlaceholder::Deserialize(DeserializeStream& stream, ISerializeModifier* modifier)
{
	Guid parentId = Guid::Empty;
	DESERIALIZE_MEMBER(PlaceholderParent, parentId);

	if (parentId == Guid::Empty)
	{
		LOG(Warning, "Failed to deserialize asset reference. Serialized GUID does not exist.");
		return;
	}

	FudgetContainer* parent = Scripting::FindObject<FudgetContainer>(parentId);
	if (parent == nullptr)
	{
		LOG(Warning, "Failed to deserialize asset reference. Could not find parent object.");
		return;
	}

	DESERIALIZE_MEMBER(SourceAsset, _sourceAsset);
	FudgetJsonAsset* asset = Content::Load<FudgetJsonAsset>(_sourceAsset);
	if (asset == nullptr)
	{
		LOG(Warning, "Failed to deserialize asset reference. Asset could not be loaded.");
		return;
	}


}