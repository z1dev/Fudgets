using Flax.Build;

public class FudgetsTarget : GameProjectTarget
{
    /// <inheritdoc />
    public override void Init()
    {
        base.Init();

        // Reference the modules for game
        Modules.Add("Fudgets");
    }
}
