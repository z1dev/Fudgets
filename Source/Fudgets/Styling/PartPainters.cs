using FlaxEngine.Utilities;


namespace Fudgets
{
    public partial struct FudgetPainterStateHelper
    {
        /// <summary>
        /// Initializes the helper with no state set
        /// </summary>
        public FudgetPainterStateHelper()
        {
            State = FudgetFramedFieldState.Normal;
        }

        /// <summary>
        /// Adds or removes a state flag
        /// </summary>
        /// <param name="value">Flag to change</param>
        /// <param name="set">Whether to add the flag</param>
        public void SetState(FudgetFramedFieldState value, bool set = true)
        {
            if (set)
                this.State |= value;
            else
                this.State &= ~value;
        }

        /// <summary>
        /// Checks if a state flag is set in State
        /// </summary>
        /// <param name="value">The flag to check</param>
        /// <returns>Whether the flag is found in State</returns>
        public readonly bool HasState(FudgetFramedFieldState value) { return (State & value) == value; }

        /// <summary>
        /// State doesn't have the Disabled flag
        /// </summary>
        public readonly bool Enabled { get => !HasState(FudgetFramedFieldState.Disabled); }
        /// <summary>
        /// State has the Hovered flag
        /// </summary>
        public readonly bool Hovered { get => HasState(FudgetFramedFieldState.Hovered); }
        /// <summary>
        /// State has the Pressed flag
        /// </summary>
        public readonly bool Pressed { get => HasState(FudgetFramedFieldState.Pressed); }
        /// <summary>
        /// State has the Down flag
        /// </summary>
        public readonly bool Down { get => HasState(FudgetFramedFieldState.Down); }
        /// <summary>
        /// State has the Focused flag
        /// </summary>
        public readonly bool Focused { get => HasState(FudgetFramedFieldState.Focused); }

    }
}
