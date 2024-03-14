#pragma once

#include "Engine/Core/Collections/Array.h"
#include "Engine/Core/Types/Variant.h"

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

template<typename T>
constexpr bool Fudget_is_class()
{
	using T2 = std::remove_reference_t<std::remove_const_t<std::remove_volatile_t<T>>>;
	return std::is_class<T2>::value && !std::is_enum<T2>::value && !std::is_same<Variant, T2>::value && !std::is_same<String, T2>::value && !std::is_same<StringView, T2>::value &&
		!std::is_same<StringAnsi, T2>::value && !std::is_same<StringAnsiView, T2>::value && !std::is_same<Guid, T2>::value && !std::is_same<Float2, T2>::value &&
		!std::is_same<Float3, T2>::value && !std::is_same<Float4, T2>::value && !std::is_same<Int2, T2>::value && !std::is_same<Int3, T2>::value &&
		!std::is_same<Int4, T2>::value && !std::is_same<Double2, T2>::value && !std::is_same<Double3, T2>::value && !std::is_same<Double4, T2>::value &&
		!std::is_same<Color, T2>::value && !std::is_same<Quaternion, T2>::value && !std::is_same<BoundingSphere, T2>::value &&
		!std::is_same<Rectangle, T2>::value && !std::is_same<BoundingBox, T2>::value && !std::is_same<Transform, T2>::value && !std::is_same<Ray, T2>::value &&
		!std::is_same<Matrix, T2>::value && !std::is_same<Array<Variant, HeapAllocation>, T2>::value && !std::is_same<Dictionary<Variant, Variant, HeapAllocation>, T2>::value &&
		!std::is_same<Span<byte>, T2>::value && !std::is_same<CommonValue, T2>::value && !std::is_same<class AssetReference<T2>, T2>::value;
}

template<typename T>
constexpr bool Fudget_is_resource_id()
{
	using T2 = std::remove_reference_t<std::remove_const_t<std::remove_volatile_t<T>>>;
	return !std::is_same<T2, int>::value && (std::is_enum<T>::value || std::is_convertible<T, int>::value);
}

template<typename T>
constexpr bool Fudget_is_enum_or_int()
{
	return std::is_enum<T>::value || std::is_convertible<T, int>::value;
}

template<typename T>
constexpr bool Fudget_is_enum_or_int64()
{
	return std::is_enum<T>::value || std::is_convertible<T, int64>::value;
}

template<typename T>
static Variant StructToVariant(const T &s)
{
	VariantType t;
	t.Type = VariantType::Structure;
	t.SetTypeName(T::TypeInitializer.GetType().Fullname);
	return Variant::Structure(std::move(t), s);
}

template<typename T>
static Variant EnumToVariant(const T &s)
{
	VariantType t;
	t.Type = VariantType::Enum;
	t.SetTypeName(StaticType<T>().GetType().Fullname);
	return Variant::Enum(std::move(t), s);
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
