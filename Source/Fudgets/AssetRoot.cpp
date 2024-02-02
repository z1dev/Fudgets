#include "AssetRoot.h"
#include "Utils/Utils.h"

#include "Engine/Scripting/Scripting.h"

#include "Engine/Core/Log.h"
#include <Engine/Serialization/JsonTools.h>
#include <Engine/Serialization/JsonWriters.h>

FUDGET_FACTORY(FudgetControl, control);



FudgetAssetRoot::FudgetAssetRoot(const SpawnParams &params) : Base(params)
{

}


String FudgetAssetRoot::SerializationTester()
{
	String output;
	rapidjson_flax::StringBuffer buffer;
	PrettyJsonWriter writerObj(buffer);
	JsonWriter& writer = writerObj;
	writer.StartObject();
	Serialize(writer, nullptr);
	writer.EndObject();

	output = buffer.GetString();
	return output;
}

void FudgetAssetRoot::Serialize(SerializeStream& stream, const void* otherObj)
{
	Array<FudgetControl*> allControls;
	GetAllControls(allControls);

	Base::Serialize(stream, otherObj);
	stream.JKEY("Controls");
	stream.StartArray();
	for (auto control : allControls)
	{
		stream.StartObject();
		control->Serialize(stream, nullptr);
		stream.EndObject();
	}
	stream.EndArray();
}

void FudgetAssetRoot::Deserialize(DeserializeStream& stream, ISerializeModifier* modifier)
{
	Base::Deserialize(stream, modifier);
	// Once we have deserialized everything, register our object.
	if (!IsRegistered())
		RegisterObject();
	/*if (!HasManagedInstance())
		CreateManaged();*/

	int32 controlsCount = 0;
	const auto& controlsListMember = stream.FindMember("Controls");
	if (controlsListMember != stream.MemberEnd() && controlsListMember->value.IsArray())
	{
		controlsCount = controlsListMember->value.Size();
	}

	Array<FudgetControl*> controls;
	if (controlsCount)
	{
		const DeserializeStream& items = controlsListMember->value;
		controls.Resize(controlsCount, false);
		for (int32 i = 0; i < controlsCount; i++)
		{
			FudgetControl* control = FudgetControlFactory((DeserializeStream&)items[i], modifier);
			if (control == nullptr)
			{
				// Skip
				continue;
			}

			control->Deserialize((DeserializeStream&)items[i], modifier);

			// Once we have data, register the object.
			if (!control->IsRegistered())
				control->RegisterObject();
			/*if (!control->HasManagedInstance())
				control->CreateManaged();*/
		}
	}

	//MarkLayoutDirty(FudgetDirtType::All);
	//RequestLayout();
}