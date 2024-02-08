#include "FilledBox.h"

FudgetFilledBox::FudgetFilledBox(const SpawnParams &params) : Base(params), DrawBorder(false), BorderColor(0.5f, 0.5f, 0.5f, 1.0f), _color(1.0f)
{
}

FudgetFilledBox::~FudgetFilledBox()
{
}

void FudgetFilledBox::OnDraw()
{
	FillRectangle(Float2(0.f), GetSize(), _color);

	if (DrawBorder)
		DrawRectangle(Float2(0.f), GetSize(), BorderColor);

	//Render2D::FillRectangle(Rectangle(GetPosition(), GetSize()), _color);
}

void FudgetFilledBox::Serialize(SerializeStream& stream, const void* otherObj)
{
	Base::Serialize(stream, otherObj);
	SERIALIZE_GET_OTHER_OBJ(FudgetFilledBox);
	SERIALIZE_MEMBER(Color, _color);
	SERIALIZE_MEMBER(BorderColor, BorderColor);
}

void FudgetFilledBox::Deserialize(DeserializeStream& stream, ISerializeModifier* modifier)
{
	Base::Deserialize(stream, modifier);
	DESERIALIZE_MEMBER(Color, _color);
	DESERIALIZE_MEMBER(BorderColor, BorderColor);
}

