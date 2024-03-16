using FlaxEngine;

namespace Fudgets;



public partial struct FudgetSpriteHandle
{

    /// <summary>
    /// Initializes a wrapper from a SpriteHandle to get around a possible engine bug
    /// </summary>
    /// <param name="original">The sprite handle to wrap</param>
    public FudgetSpriteHandle(in SpriteHandle original)
    {
        Atlas = original.Atlas;
        Index = original.Index;
    }

    /// <summary>
	/// Returns a sprite handle from this wrapper
    /// </summary>
    /// <returns></returns>
    public SpriteHandle ToHandle()
    {
        return new SpriteHandle(Atlas, Index);
    }
}


public partial struct FudgetFontSettings
{
    /// <summary>
    /// Initializes the font settings
    /// </summary>
    public FudgetFontSettings(int fontId, float size, bool bold, bool italics)
    {
        FontId = fontId;
        Size = size;
        Bold = bold;
        Italics = italics;
    }
}

public partial struct FudgetTextDrawSettings
{
    /// <summary>
    /// Initialize structure with white font and no other influencing setting
    /// </summary>
    public FudgetTextDrawSettings() : this(new Int2(0), Color.White, TextAlignment.Near, TextAlignment.Center, new FudgetPadding(0), -1)
    {
    }

    /// <summary>
    /// Initialize structure with every setting
    /// </summary>
    public FudgetTextDrawSettings(Int2 offset, Color color, TextAlignment horzAlign, TextAlignment vertAlign, FudgetPadding padding, int material_id)
    {
        Offset = offset;
        Color = color;
        HorizontalAlignment = horzAlign;
        VerticalAlignment = vertAlign;
        Padding = padding;
        MaterialId = material_id;
    }

    /// <summary>
    /// Initialize structure with text color
    /// </summary>
    public FudgetTextDrawSettings(Color color) : this(new Int2(0), color, TextAlignment.Near, TextAlignment.Center, new FudgetPadding(0), -1)
    {
    }

    /// <summary>
    /// Initialize structure with draw offset
    /// </summary>
    public FudgetTextDrawSettings(Int2 offset) : this(offset, Color.White, TextAlignment.Near, TextAlignment.Center, new FudgetPadding(0), -1)
    {
    }

    /// <summary>
    /// Initialize structure with padding
    /// </summary>
    public FudgetTextDrawSettings(FudgetPadding padding) : this(new Int2(0), Color.White, TextAlignment.Near, TextAlignment.Center, padding, -1)
    {
    }

    /// <summary>
    /// Initialize structure with text alignment
    /// </summary>
    public FudgetTextDrawSettings(TextAlignment horzAlign, TextAlignment vertAlign) : this(new Int2(0), Color.White, horzAlign, vertAlign, new FudgetPadding(0), -1)
    {
    }
}

public partial struct FudgetDrawArea
{

    /// <summary>
    /// Initializes the draw area to fill a rectangle with a texture
    /// </summary>
    /// <param name="texture">Texture to draw inside a rectangle</param>
    /// <param name="point_draw">Draw texture with point rendering</param>
    /// <param name="alignment">Placement and stretching options to draw the texture inside the rectangle</param>
    /// <param name="tint">Color to multiply the texture with</param>
    /// <param name="tex_scale">Scale of the texture when not drawing stretched</param>
    /// <param name="tex_offset">Offset of the texture when not drawing stretched</param>
    public FudgetDrawArea(TextureBase texture, bool point_draw, FudgetImageAlignment alignment, Color tint, Float2 tex_scale, Float2 tex_offset)
    {
        FillType = point_draw ? FudgetFillType.Point : FudgetFillType.Normal;
        ImageAlignment = alignment;
        Tint = tint;
        Texture = texture;
        Offset = tex_offset;
        Scale = tex_scale;
        Slices = new FudgetPadding(-1);
    }

    /// <summary>
	/// Initializes the draw area to fill a rectangle with a texture using 9-slicing.
	/// </summary>
	/// <param name="texture">Texture to draw inside a rectangle</param>
	/// <param name="slices_9p">The division size on each side for 9-slicing</param>
	/// <param name="point_draw">Draw texture with point rendering</param>
	/// <param name="alignment">Placement and stretching options to draw the texture inside the rectangle</param>
	/// <param name="tex_scale">Scale of the texture when not drawing stretched</param>
	/// <param name="tex_offset">Offset of the texture when not drawing stretched</param>
	/// <param name="tint">Color to multiply the texture with</param>
	public FudgetDrawArea(TextureBase texture, FudgetPadding slices_9p, bool point_draw, FudgetImageAlignment alignment, Color tint, Float2 tex_scale, Float2 tex_offset)
    {
        FillType = (FudgetFillType.Sliced | (point_draw ? FudgetFillType.Point : FudgetFillType.Normal));
        ImageAlignment = alignment;
        Tint = tint;
        Texture = texture;
        Offset = tex_offset;
        Scale = tex_scale;
        Slices = slices_9p;
    }

    /// <summary>
    /// Initializes the draw area to fill a rectangle with a texture
    /// </summary>
    /// <param name="sprite_handle">Sprite to draw inside a rectangle</param>
    /// <param name="point_draw">Draw texture with point rendering</param>
    /// <param name="alignment">Placement and stretching options to draw the texture inside the rectangle</param>
    /// <param name="tex_scale">Scale of the texture when not drawing stretched</param>
    /// <param name="tex_offset">Offset of the texture when not drawing stretched</param>
    /// <param name="tint">Color to multiply the texture with</param>
    public FudgetDrawArea(SpriteHandle sprite_handle, bool point_draw, FudgetImageAlignment alignment, Color tint, Float2 tex_scale, Float2 tex_offset)
    {
        FillType = point_draw ? FudgetFillType.Point : FudgetFillType.Normal;
        ImageAlignment = alignment;
        Tint = tint;
        Texture = null;
        Offset = tex_offset;
        Scale = tex_scale;
        SpriteHandle = new FudgetSpriteHandle(sprite_handle);
        Slices = new FudgetPadding(-1);
    }

    /// <summary>
    /// Initializes the draw area to fill a rectangle with a texture using 9-slicing.
    /// </summary>
    /// <param name="sprite_handle">Sprite to draw inside a rectangle</param>
	/// <param name="slices_9p">The division size on each side for 9-slicing</param>
    /// <param name="point_draw">Draw texture with point rendering</param>
    /// <param name="alignment">Placement and stretching options to draw the texture inside the rectangle</param>
    /// <param name="tex_scale">Scale of the texture when not drawing stretched</param>
    /// <param name="tex_offset">Offset of the texture when not drawing stretched</param>
    /// <param name="tint">Color to multiply the texture with</param>
    public FudgetDrawArea(SpriteHandle sprite_handle, FudgetPadding slices_9p, bool point_draw, FudgetImageAlignment alignment, Color tint, Float2 tex_scale, Float2 tex_offset)
    {
        FillType = FudgetFillType.Sliced | (point_draw ? FudgetFillType.Point : FudgetFillType.Normal);
        ImageAlignment = alignment;
        Tint = tint;
        Texture = null;
        Offset = tex_offset;
        Scale = tex_scale;
        SpriteHandle = new FudgetSpriteHandle(sprite_handle);
        Slices = slices_9p;
    }
}

public partial struct FudgetDrawBorder
{
    /// <summary>
    /// Initializes the border draw object to draw a border with a single color and specified border edge sizes.
    /// /// </summary>
    /// <param name="color">Color of the border</param>
    /// <param name="borders">The width of border edges on each side</param>
    /// <param name="placement">Where to draw the border on a rectangle's edges</param>
    public FudgetDrawBorder(Color color, FudgetBorder borders, FudgetBorderPlacement placement) 
    {
        BorderType = FudgetBorderType.Color;
        BorderPlacement = placement;
        Tint = color;
        Offset = new Float2(0.0f);
        Scale = new Float2(1.0f);
        Texture = null;
        Borders = borders;
    }

    /// <summary>
    /// Initializes the border draw object to draw a border with a texture
    /// </summary>
    /// <param name="texture">Texture to fill inside the borders</param>
    /// <param name="borders">The width of border edges on each side</param>
    /// <param name="placement">Where to draw the border on a rectangle's edges</param>
    /// <param name="stretch">Stretching the texture instead of tiling</param>
    /// <param name="point_draw">Draw texture with point rendering</param>
    /// <param name="tint">Color to multiply the texture with</param>
    /// <param name="tex_offset">Offset of the texture when not drawing stretched</param>
    /// <param name="tex_scale">Scale of the texture when not drawing stretched</param>
    public FudgetDrawBorder(TextureBase texture, FudgetBorder borders, FudgetBorderPlacement placement, bool stretch, bool point_draw, Color tint, Float2 tex_offset, Float2 tex_scale)
    {
        BorderType = FudgetBorderType.Texture | (stretch ? FudgetBorderType.Stretch : FudgetBorderType.None) | (point_draw ? FudgetBorderType.Point : FudgetBorderType.None);
		BorderPlacement = placement;
        Tint = tint; 
        Offset = tex_offset;
        Scale = tex_scale; 
        Texture = texture;
        Borders = borders;
    }


    /// <summary>
    /// Initializes the border draw object to draw a border with a sprite
    /// </summary>
    /// <param name="sprite_handle">Sprite to fill inside the borders</param>
    /// <param name="borders">The width of border edges on each side</param>
    /// <param name="placement">Where to draw the border on a rectangle's edges</param>
    /// <param name="stretch">Stretching the sprite instead of tiling</param>
    /// <param name="point_draw">Draw sprite with point rendering</param>
    /// <param name="tint">Color to multiply the sprite with</param>
    /// <param name="tex_offset">Offset of the sprite when not drawing stretched</param>
    /// <param name="tex_scale">Scale of the sprite when not drawing stretched</param>
    public FudgetDrawBorder(SpriteHandle sprite_handle, FudgetBorder borders, FudgetBorderPlacement placement, bool stretch, bool point_draw, Color tint, Float2 tex_offset, Float2 tex_scale)
    {
        BorderType = FudgetBorderType.Texture | (stretch ? FudgetBorderType.Stretch : FudgetBorderType.None) | (point_draw ? FudgetBorderType.Point : FudgetBorderType.None);
		BorderPlacement = placement;
        Tint = tint;
        Offset = tex_offset;
        Scale = tex_scale;
        Texture = null;
        SpriteHandle = new FudgetSpriteHandle(sprite_handle);
        Borders = borders;
    }
}

public partial struct FudgetDrawColors
{
    /// <summary>
    /// Returns the index of a state within the colors list that matches the passed state flags. The index of the first
    /// state that fully matches is returned. The 0 state in the colors list matches any combination of state flags.
    /// </summary>
    /// <param name="states">State flags to match</param>
    /// <returns>Index to the first matching state in the colors list or -1 on failure</returns>
    public int FindMatchingState(ulong states)
    {
        for (int ix = 0, siz = _states.Length; ix < siz; ++ix)
        {
            ulong state = _states[ix];
            if ((states & state) == state)
                return ix;
        }
        return -1;
    }

    /// <summary>
    /// Finds a color for the state matching the passed state flags and returns it if there was a match. Otherwise returns white. The first
    /// state that fully matches is used. The 0 state in the colors list matches any combination of state flags.
    /// Use FindMatchingState instead to check the success of the state matching.
    /// </summary>
    /// <param name="states">State flags to match</param>
    /// <returns>Color matching the states flags or a white color if no match was found.</returns>
    public Color FindMatchingColor(ulong states)
	{

        int ix = FindMatchingState(states);
		if (ix < 0)
			return Color.White;
		return _colors[ix];
	}

}

