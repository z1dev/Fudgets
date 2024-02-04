using System;
using FlaxEngine;

namespace Fudgets
{
    internal partial class FudgetInitializer
    {
        public void Initialize()
        {
            SetGamePluginInitialized(true);
        }
        public void Uninitialize()
        {
            SetGamePluginInitialized(false);
        }
    }

    /// <summary>
    /// The sample game plugin.
    /// </summary>
    /// <seealso cref="FlaxEngine.GamePlugin" />
    public class FudgetsPlugin : GamePlugin
    {
        private FudgetInitializer fudgetInitializer = null;

        /// <inheritdoc />
        public FudgetsPlugin()
        {
            _description = new PluginDescription
            {
                Name = "Fudgets",
                Category = "Other",
                Author = "",
                AuthorUrl = null,
                HomepageUrl = null,
                RepositoryUrl = "https://github.com/FlaxEngine/Fudgets",
                Description = "This is an example plugin project.",
                Version = new Version(0, 0, 1),
                IsAlpha = false,
                IsBeta = false,
            };
        }

        /// <inheritdoc />
        public override void Initialize()
        {
            base.Initialize();

            fudgetInitializer = new FudgetInitializer();
            fudgetInitializer.Initialize();

            FudgetThemes.Initialize(true);
            FudgetThemes.InitializeManaged(true);
        }

        /// <inheritdoc />
        public override void Deinitialize()
        {
            // Use it to cleanup data

            FudgetThemes.RuntimeUse = true;
            FudgetThemes.UninitializeManaged(true);
            FudgetThemes.Uninitialize(true);

            fudgetInitializer.Uninitialize();
            fudgetInitializer = null;

            base.Deinitialize();
        }
    }
}
