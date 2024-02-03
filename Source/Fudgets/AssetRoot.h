#pragma once

#include "Container.h"

/// <summary>
/// Root container representing the whole area where UI controls can appear. For example the screen.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetAssetRoot : public FudgetContainer
{
    using Base = FudgetContainer;
    DECLARE_SCRIPTING_TYPE(FudgetAssetRoot);
public:
    /// <summary>
    /// The GUID of the asset this root object originated from.
    /// </summary>
    API_FIELD() Guid SourceAsset;

    /// <summary>
    /// Placeholder summary so the code gen doesn't complain.
    /// </summary>
    API_FUNCTION() String SerializationTester();

    void Serialize(SerializeStream& stream, const void* otherObj) override;
    void Deserialize(DeserializeStream& stream, ISerializeModifier* modifier) override;

private:
    bool _isRoot;
};
