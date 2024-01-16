using Flax.Build;

public class FudgetsEditorTarget : GameProjectEditorTarget
{
    /// <inheritdoc />
    public override void Init()
    {
        base.Init();

        // Reference the modules for editor
        Modules.Add("Fudgets");
        Modules.Add("FudgetsEditor");
    }
}
