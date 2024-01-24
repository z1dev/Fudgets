using System;
using FlaxEngine;
using Fudgets;

namespace FlaxEditor.Content
{
    /// <summary>
    /// Content item that contains <see cref="FudgetJsonAsset"/> data.
    /// </summary>
    /// <seealso cref="FlaxEditor.Content.JsonAssetItem" />
    public sealed class FudgetJsonAssetItem : JsonAssetItem
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="FudgetJsonAssetItem"/> class.
        /// </summary>
        /// <param name="path">The asset path.</param>
        /// <param name="id">The asset identifier.</param>
        public FudgetJsonAssetItem(string path, Guid id)
        : base(path, id, FudgetJsonAssetProxy.AssetTypename)
        {
        }

        /// <inheritdoc />
        public override bool OnEditorDrag(object context)
        {
            return true;
        }

        /// <inheritdoc />
        public override Actor OnEditorDrop(object context)
        {
            return null;
            //return PrefabManager.SpawnPrefab(FlaxEngine.Content.LoadAsync<Prefab>(ID), null);
        }

        /// <inheritdoc />
        public override ContentItemType ItemType => ContentItemType.Asset;

        /// <inheritdoc />
        public override ContentItemSearchFilter SearchFilter => ContentItemSearchFilter.Other;

        /// <inheritdoc />
        public override SpriteHandle DefaultThumbnail => Editor.Instance.Icons.BuildSettings128;

        /// <inheritdoc />
        public override bool IsOfType(Type type)
        {
            return type.IsAssignableFrom(typeof(FudgetJsonAsset));
        }
    }
}