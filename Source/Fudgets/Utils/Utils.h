#pragma once

#include "Engine/Core/Collections/Array.h"

// Moves an item at index from to index to in the array arr. Only use for fast copiable items, like pointers.
template<typename T>
void MoveInArray(Array<T> &arr, int from, int to)
{
	auto item = arr[from];
	if (from > to)
	{
		for (int ix = from; ix > to; --ix)
		{
			arr[ix] = arr[ix - 1];
		}
		arr[to] = item;
	}
	else
	{
		for (int ix = from; ix < to; ++ix)
		{
			arr[ix] = arr[ix + 1];
		}
		arr[to] = item;
	}
}

//extern const float MaximumFloatLimit;

float AddBigValues(float a, float b);
float AddBigValues(float a, float b, float c);
float AddBigValues(float a, float b, float c, float d);
Float2 AddBigValues(Float2 a, Float2 b);
Float2 AddBigValues(Float2 a, Float2 b, Float2 c);
Float2 AddBigValues(Float2 a, Float2 b, Float2 c, Float2 d);

int AddBigValues(int a, int b);
int AddBigValues(int a, int b, int c);
int AddBigValues(int a, int b, int c, int d);
Int2 AddBigValues(Int2 a, Int2 b);
Int2 AddBigValues(Int2 a, Int2 b, Int2 c);
Int2 AddBigValues(Int2 a, Int2 b, Int2 c, Int2 d);

#define STRING_SUBSTEP(A) #A
#define STRING(A) STRING_SUBSTEP(A)

#define FUDGET_FACTORY(factoryType, name) \
namespace \
{ \
	factoryType* factoryType##Factory(ISerializable::DeserializeStream& stream, ISerializeModifier* modifier) \
	{ \
		Guid id = JsonTools::GetGuid(stream, "ID"); \
		modifier->IdsMapping.TryGet(id, id); \
		if (!id.IsValid()) \
		{ \
			LOG(Warning, "Invalid object id."); \
			return nullptr; \
		} \
		factoryType* obj = nullptr; \
\
		const auto typeNameMember = stream.FindMember("TypeName"); \
		if (typeNameMember != stream.MemberEnd()) \
		{ \
			if (!typeNameMember->value.IsString()) \
			{ \
				LOG(Warning, "Invalid object type (TypeName must be an object type full name string)."); \
				return nullptr; \
			} \
			const StringAnsiView typeName(typeNameMember->value.GetStringAnsiView()); \
			const ScriptingTypeHandle type = Scripting::FindScriptingType(typeName); \
			if (type) \
			{ \
				if (!factoryType::TypeInitializer.IsAssignableFrom(type)) \
				{ \
					LOG(Warning, "Invalid " #name " type {0} (inherits from: {1}).", type.ToString(true), type.GetType().GetBaseType().ToString()); \
					return nullptr; \
				} \
				const ScriptingObjectSpawnParams params(id, type); \
				obj = (factoryType*)type.GetType().Script.Spawn(params); \
\
				if (obj == nullptr) \
				{ \
					LOG(Warning, "Failed to spawn object of type {0}.", type.ToString(true)); \
					return nullptr; \
				} \
			} \
			else \
			{ \
				LOG(Warning, "Unknown object type '{0}', ID: {1}", String(typeName.Get(), typeName.Length()), id.ToString()); \
				return nullptr; \
			} \
		} \
		else \
		{ \
			LOG(Warning, "Invalid object type."); \
		} \
		return obj; \
	} \
}