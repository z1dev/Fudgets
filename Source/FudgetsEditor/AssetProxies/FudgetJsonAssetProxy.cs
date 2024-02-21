using System;
using FlaxEditor.Content.Thumbnails;
using FlaxEditor.Viewport.Previews;
using FlaxEditor.Windows;
using FlaxEditor.Windows.Assets;
using FlaxEngine;
using FlaxEngine.GUI;
using Fudgets;
using FudgetsEditor;
using Object = FlaxEngine.Object;

namespace FlaxEditor.Content
{
    /// <summary>
    /// Content proxy for <see cref="FudgetJsonAssetItem"/>.
    /// </summary>
    /// <seealso cref="FlaxEditor.Content.JsonAssetBaseProxy" />
    [ContentContextMenu("New/Fudget")]
    public sealed class FudgetJsonAssetProxy : JsonAssetBaseProxy
    {
        /// <summary>
        /// The fudget files extension.
        /// </summary>
        public static readonly string Extension = "json";

        /// <summary>
        /// The fudget asset data typename.
        /// </summary>
        public static readonly string AssetTypename = typeof(FudgetJsonAsset).FullName;

        /// <inheritdoc />
        public override string Name => "Fudget";

        /// <inheritdoc />
        public override string FileExtension => Extension;

        /// <inheritdoc />
        public override EditorWindow Open(Editor editor, ContentItem item)
        {
            FudgetsEditorWindow window = new FudgetsEditorWindow(editor, (AssetItem)item);
            return window;
        }

        /// <inheritdoc />
        public override bool IsProxyFor(ContentItem item)
        {
            return item is FudgetJsonAssetItem;
        }

        /// <inheritdoc />
        public override bool IsProxyFor<T>()
        {
            return typeof(T) == typeof(FudgetJsonAsset);
        }

        /// <inheritdoc />
        public override Color AccentColor => Color.FromRGB(0x7eef21);

        /// <inheritdoc />
        public override string TypeName => AssetTypename;

        /// <inheritdoc />
        public override AssetItem ConstructItem(string path, string typeName, ref Guid id)
        {
            return new FudgetJsonAssetItem(path, id);
        }

        /// <inheritdoc />
        public override bool CanCreate(ContentFolder targetLocation)
        {
            return targetLocation.CanHaveAssets;
        }

        /// <inheritdoc />
        public override void Create(string outputPath, object arg)
        {
            // Create an empty fudget asset.
            FudgetJsonAsset asset = FlaxEngine.Content.CreateVirtualAsset<FudgetJsonAsset>();
            asset.WidgetData = new FudgetAssetRoot();
            FudgetAssetRoot root = asset.WidgetData;
            root.SourceAsset = asset.ID;

            FudgetFilledBox box = new FudgetFilledBox();
            box.Color = Color.Blue;
            FudgetFilledBox box2 = new FudgetFilledBox();
            box2.Color = Color.Green;

            FudgetContainer containerTest = new FudgetContainer();
            FudgetFilledBox box3 = new FudgetFilledBox();
            box3.Color = Color.DarkRed;
            FudgetFilledBox box4 = new FudgetFilledBox();
            box4.Color = Color.Beige;

            //box.HintSize = new Float2(9999, 9999);
            //box2.HintSize = new Float2(9999, 9999);
            //box3.HintSize = new Float2(9999, 9999);
            //box4.HintSize = new Float2(9999, 9999);
            //containerTest.HintSize = new Float2(9999, 9999);

            containerTest.CreateLayout<FudgetListLayout>();
            containerTest.AddChild(box3);
            containerTest.AddChild(box4);

            box.Name = "Box";
            box2.Name = "Box 2";
            box3.Name = "Box 3";
            box4.Name = "Box 4";
            containerTest.Name = "Container.";

            root.CreateLayout<FudgetListLayout>();
            root.AddChild(box);
            root.AddChild(box2);
            root.AddChild(containerTest);

            asset.Save(outputPath);
            Object.Destroy(asset);
        }

        /*
        /// <inheritdoc />
        public override void OnThumbnailDrawPrepare(ThumbnailRequest request)
        {
            if (_preview == null)
            {
                _preview = new PrefabPreview(false);
                InitAssetPreview(_preview);
            }

            // TODO: disable streaming for asset during thumbnail rendering (and restore it after)
        }*/

        /// <inheritdoc />
        public override bool CanDrawThumbnail(ThumbnailRequest request)
        {
            /*if (!_preview.HasLoadedAssets)
                return false;

            // Check if asset is streamed enough
            var asset = (Prefab)request.Asset;
            return asset.IsLoaded;*/

            return true;
        }

        /*/// <inheritdoc />
        public override void OnThumbnailDrawBegin(ThumbnailRequest request, ContainerControl guiRoot, GPUContext context)
        {
            _preview.Prefab = (Prefab)request.Asset;
            _preview.Parent = guiRoot;
            _preview.Scale = Float2.One;
            _preview.ShowDefaultSceneActors = true;
            _preview.SyncBackbufferSize();

            // Special case for UI prefabs
            if (_preview.Instance is UIControl uiControl && uiControl.HasControl)
            {
                // Ensure to place UI in a proper way
                uiControl.Control.Location = Float2.Zero;
                uiControl.Control.Scale *= PreviewsCache.AssetIconSize / uiControl.Control.Size.MaxValue;
                uiControl.Control.AnchorPreset = AnchorPresets.TopLeft;
                uiControl.Control.AnchorPreset = AnchorPresets.MiddleCenter;

                // Tweak preview
                _preview.ShowDefaultSceneActors = false;
            }
            else
            {
                // Update some actors data (some actor types update bounds/data later but its required to be done before rendering)
                Prepare(_preview.Instance);

                // Auto fit actor to camera
                float targetSize = 30.0f;
                var bounds = _preview.Instance.EditorBoxChildren;
                var maxSize = Math.Max(0.001f, (float)bounds.Size.MaxValue);
                _preview.Instance.Scale = new Float3(targetSize / maxSize);
                _preview.Instance.Position = Vector3.Zero;
            }

            _preview.Task.OnDraw();
        }

        /// <inheritdoc />
        public override void OnThumbnailDrawEnd(ThumbnailRequest request, ContainerControl guiRoot)
        {
            _preview.RemoveChildren();
            _preview.Prefab = null;
            _preview.Parent = null;
        }

        /// <inheritdoc />
        public override void Dispose()
        {
            if (_preview != null)
            {
                _preview.Dispose();
                _preview = null;
            }

            base.Dispose();
        }*/
    }
}