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

    /*
        /// <summary>
        /// Initializes a new instance of the <see cref="SpriteHandle"/> struct.
        /// </summary>
        FudgetSpriteHandle()
        {
            Index = -1;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="SpriteHandle"/> struct.
        /// </summary>
        /// <param name="atlas">The sprite atlas.</param>
        /// <param name="index">The sprite slot index.</param>
        FudgetSpriteHandle(const SpriteHandle &original)
            : Atlas(original.Atlas)
        {
            Index = original.Index;
        }

        SpriteHandle ToHandle() const
        {
            return SpriteHandle(Atlas, Index);
        }
    */
}


/// <summary>
/// 
/// </summary>
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
    public FudgetTextDrawSettings() : this(new Float2(0.0f), Color.White, TextAlignment.Near, TextAlignment.Center, new FudgetPadding(0.0f), -1)
    {
    }

    /// <summary>
    /// Initialize structure with every setting
    /// </summary>
    public FudgetTextDrawSettings(Float2 offset, Color color, TextAlignment horzAlign, TextAlignment vertAlign, FudgetPadding padding, int material_id)
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
    public FudgetTextDrawSettings(Color color) : this(new Float2(0.0f), color, TextAlignment.Near, TextAlignment.Center, new FudgetPadding(0.0f), -1)
    {
    }

    /// <summary>
    /// Initialize structure with draw offset
    /// </summary>
    public FudgetTextDrawSettings(Float2 offset) : this(offset, Color.White, TextAlignment.Near, TextAlignment.Center, new FudgetPadding(0.0f), -1)
    {
    }

    /// <summary>
    /// Initialize structure with padding
    /// </summary>
    public FudgetTextDrawSettings(FudgetPadding padding) : this(new Float2(0.0f), Color.White, TextAlignment.Near, TextAlignment.Center, padding, -1)
    {
    }

    /// <summary>
    /// Initialize structure with text alignment
    /// </summary>
    public FudgetTextDrawSettings(TextAlignment horzAlign, TextAlignment vertAlign) : this(new Float2(0.0f), Color.White, horzAlign, vertAlign, new FudgetPadding(0.0f), -1)
    {
    }
}

/// <summary>
/// 
/// </summary>
public partial struct FudgetDrawArea
{
    /// <summary>
    /// Initializes draw area to fill a rectangle with the specified color
    /// </summary>
    /// <param name="color">Color used for filling the rectangle</param>
    public FudgetDrawArea(Color color) //: AreaType(FudgetFillType::Color), Tint(color), TextureOffset(0.0f), TextureScale(1.0f), Texture(nullptr), SpriteHandle(), Borders(-1.0f)
    {
        AreaType = FudgetFillType.Color;
        Tint = color;
        TextureOffset = new Float2(0.0f);
        TextureScale = new Float2(1.0f);
        Texture = null;
        Borders = new FudgetPadding(-1.0f);
    }

    /// <summary>
    /// Initializes draw area to draw a border in a rectangle with the specified color and border width.
    /// /// </summary>
    /// <param name="borders">The width of borders on each side</param>
    /// <param name="color">Color of the border</param>
    /// <param name="border_type">Where to draw the border</param>
    public FudgetDrawArea(FudgetPadding borders, Color color, FudgetFrameType border_type) 
    {
        AreaType = FudgetFillType.Color |
            (border_type == FudgetFrameType.Inside ? FudgetFillType.BorderInside : border_type == FudgetFrameType.Outside ? FudgetFillType.BorderOutside :
            FudgetFillType.BorderCentered);

        Tint = color;
        TextureOffset = new Float2(0.0f);
        TextureScale = new Float2(1.0f);
        Texture = null;
        Borders = borders;
    }

    /// <summary>
    /// Initializes draw area to fill a rectangle with a texture
    /// </summary>
    /// <param name="texture">Texture to draw in a rectangle</param>
    /// <param name="stretch">Stretching the texture instead of tiling</param>
    /// <param name="point_tex">Draw texture with point rendering</param>
    /// <param name="tex_offset">Offset of the texture for tiled drawing</param>
    /// <param name="tex_scale">Scale of the texture for tiled drawing</param>
    /// <param name="tint">Color to multiply the texture with</param>
    public FudgetDrawArea(TextureBase texture, bool stretch, bool point_tex, Float2 tex_offset, Float2 tex_scale, Color tint)
    {
        AreaType = (stretch ? FudgetFillType.Stretch : FudgetFillType.None) | (point_tex? FudgetFillType.Point : FudgetFillType.None);
        Tint = tint;
        Texture = texture;
        TextureOffset = tex_offset;
        TextureScale = tex_scale;
        Borders = new FudgetPadding(-1.0f);
    }

    /// <summary>
    /// Initializes draw area to fill a border with a texture
    /// </summary>
    /// <param name="texture">Texture to fill inside the borders</param>
    /// <param name="border_widths">Width of the border</param>
    /// <param name="border_type">Where to draw the border</param>
    /// <param name="stretch">Stretching the texture instead of tiling</param>
    /// <param name="point_tex">Draw texture with point rendering</param>
    /// <param name="tex_offset">Offset of the texture when not drawing stretched</param>
    /// <param name="tex_scale">Scale of the texture for tiled drawing</param>
    /// <param name="tint">Color to multiply the texture with</param>
    public FudgetDrawArea(TextureBase texture, FudgetPadding border_widths, FudgetFrameType border_type, bool stretch, bool point_tex, Float2 tex_offset, Float2 tex_scale, Color tint)
    {
        AreaType = (stretch ? FudgetFillType.Stretch : FudgetFillType.None) | (point_tex ? FudgetFillType.Point : FudgetFillType.None) |
            (border_type == FudgetFrameType.Inside ? FudgetFillType.BorderInside : border_type == FudgetFrameType.Outside ? FudgetFillType.BorderOutside :
            FudgetFillType.BorderCentered);
        Tint = tint;
        TextureOffset = tex_offset;
        TextureScale = tex_scale;
        Texture = texture;
        Borders = border_widths;
    }

    /// <summary>
    /// Initializes draw area to fill a rectangle with a texture using 9-slicing.
    /// </summary>
    /// <param name="texture">Texture to draw inside the border</param>
    /// <param name="borders_9p">Fixed sized not stretched borders around the texture</param>
    /// <param name="point_tex">Draw texture with point rendering</param>
    /// <param name="tex_offset">Offset of the texture when not drawing stretched</param>
    /// <param name="tex_scale">Scale of the texture for tiled drawing</param>
    /// <param name="tint">Color to multiply the texture with</param>
    public FudgetDrawArea(TextureBase texture, FudgetPadding borders_9p, bool point_tex, Float2 tex_offset, Float2 tex_scale, Color tint)
    {
        AreaType = FudgetFillType.Sliced | (point_tex ? FudgetFillType.Point : FudgetFillType.None);
        Tint = tint;
        TextureOffset = tex_offset;
        TextureScale = tex_scale;
        Texture = texture;
        Borders = borders_9p;
    }

    /// <summary>
    /// Initializes draw area to fill a rectangle with a sprite
    /// </summary>
    /// <param name="sprite_handle">Sprite handle for sprite to draw in a rectangle</param>
    /// <param name="stretch">Stretching the sprite instead of tiling</param>
    /// <param name="point_sprite">Draw sprite with point rendering</param>
    /// <param name="tex_offset">Offset of the sprite when not drawing stretched</param>
    /// <param name="tex_scale">Scale of the texture for tiled drawing</param>
    /// <param name="tint">Color to multiply the sprite with</param>
    public FudgetDrawArea(SpriteHandle sprite_handle, bool stretch, bool point_sprite, Float2 tex_offset, Float2 tex_scale, Color tint)
    {
        AreaType = (stretch ? FudgetFillType.Stretch : FudgetFillType.None) | (point_sprite ? FudgetFillType.Point : FudgetFillType.None);

        Tint = tint;
        TextureOffset = tex_offset;
        TextureScale = tex_scale;
        Texture = null;
        SpriteHandle = new FudgetSpriteHandle(sprite_handle);
        Borders = new FudgetPadding(-1.0f);
    }

    /// <summary>
    /// Initializes draw area to fill a border with a sprite
    /// </summary>
    /// <param name="sprite_handle">Sprite handle for sprite to fill inside the borders</param>
    /// <param name="border_widths">Width of the border</param>
    /// <param name="border_type">Where to draw the border</param>
    /// <param name="stretch">Stretching the sprite instead of tiling</param>
    /// <param name="point_sprite">Draw sprite with point rendering</param>
    /// <param name="tex_offset">Offset of the sprite when not drawing stretched</param>
    /// <param name="tex_scale">Scale of the texture for tiled drawing</param>
    /// <param name="tint">Color to multiply the sprite with</param>
    FudgetDrawArea(SpriteHandle sprite_handle, FudgetPadding border_widths, FudgetFrameType border_type, bool stretch, bool point_sprite, Float2 tex_offset, Float2 tex_scale, Color tint)
    {
        AreaType = (stretch ? FudgetFillType.Stretch : FudgetFillType.None) | (point_sprite ? FudgetFillType.Point : FudgetFillType.None) |
            (border_type == FudgetFrameType.Inside ? FudgetFillType.BorderInside : border_type == FudgetFrameType.Outside ? FudgetFillType.BorderOutside :
            FudgetFillType.BorderCentered);

        Tint = tint;
        TextureOffset = tex_offset;
        TextureScale = tex_scale;
        Texture = null;
        SpriteHandle = new FudgetSpriteHandle(sprite_handle);
        Borders = border_widths;
    }

    /// <summary>
    /// Initializes draw area to fill a rectangle with a sprite using 9-slicing.
    /// </summary>
    /// <param name="sprite_handle">Sprite to draw inside the border</param>
    /// <param name="borders_9p">Fixed sized not stretched borders around the sprite</param>
    /// <param name="point_sprite">Draw sprite with point rendering</param>
    /// <param name="tex_offset">Offset of the sprite when not drawing stretched</param>
    /// <param name="tex_scale">Scale of the texture for tiled drawing</param>
    /// <param name="tint">Color to multiply the sprite with</param>
    public FudgetDrawArea(SpriteHandle sprite_handle, FudgetPadding borders_9p, bool point_sprite, Float2 tex_offset, Float2 tex_scale, Color tint)
    {
        AreaType = FudgetFillType.Sliced | (point_sprite ? FudgetFillType.Point : FudgetFillType.None);

        Tint = tint;
        TextureOffset = tex_offset;
        TextureScale = tex_scale;
        Texture = null;
        SpriteHandle = new FudgetSpriteHandle(sprite_handle);
        Borders = borders_9p;
    }
}
