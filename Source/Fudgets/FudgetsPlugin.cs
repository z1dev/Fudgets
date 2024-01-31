using System;
using FlaxEngine;

namespace Fudgets
{
    /// <summary>
    /// The sample game plugin.
    /// </summary>
    /// <seealso cref="FlaxEngine.GamePlugin" />
    public class FudgetsPlugin : GamePlugin
    {
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

            Debug.Log("Hello from plugin code!");

            FudgetThemes.Initialize();
        }

        /// <inheritdoc />
        public override void Deinitialize()
        {
            // Use it to cleanup data

            FudgetThemes.UninitializeManaged();
            FudgetThemes.Uninitialize();

            base.Deinitialize();
        }
    }
}
