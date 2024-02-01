
namespace Fudgets;

public partial struct FudgetFontSettings
{
    /// <summary>
    /// Initializes the font settings
    /// </summary>
    public FudgetFontSettings(FudgetToken fontToken, float size, bool bold = false, bool italics = false)
    {
        FontToken = fontToken;
        Size = size;
        Bold = bold;
        Italics = italics;
    }
}
