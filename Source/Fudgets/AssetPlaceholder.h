#pragma once

#include "Control.h"
#include "Container.h"

/// <summary>
/// Placeholder control for asset references to be serialized.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetAssetPlaceholder : public FudgetControl
{
    using Base = FudgetControl;
    DECLARE_SCRIPTING_TYPE(FudgetAssetPlaceholder);
public:
    void Serialize(SerializeStream& stream, const void* otherObj) override;
    void Deserialize(DeserializeStream& stream, ISerializeModifier* modifier) override;

private:
    Guid _sourceAsset;
    FudgetContainer _deserializationParent;
};
