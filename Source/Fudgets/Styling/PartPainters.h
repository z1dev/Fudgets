#pragma once

#include "Engine/Scripting/ScriptingObject.h"

#include "../MarginStructs.h"
#include "StyleStructs.h"

class FudgetTheme;
class FudgetStyle;
class FudgetControl;

/// <summary>
/// Base class for objects that paint the standard controls' parts.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetPartPainter : public ScriptingObject
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE(FudgetPartPainter);
public:
    ~FudgetPartPainter();

    /// <summary>
    /// Initializes the painter, caching the resources it will draw with
    /// </summary>
    /// <param name="theme">Theme used for drawing</param>
    /// <param name="style">Style providing the resources for the drawing</param>
    API_FUNCTION() virtual void Initialize(FudgetTheme *theme, FudgetStyle *style);

    /// <summary>
    /// Draws the standard control part based on the control's current state.
    /// </summary>
    /// <param name="control">Control to paint</param>
    API_FUNCTION() virtual void Draw(FudgetControl *control) {}

    /// <summary>
    /// Updates the drawing state of the painter. Called by controls in OnUpdate if they registered to get updates
    /// </summary>
    /// <param name="delta_time">Time passed since last update</param>
    API_FUNCTION() virtual void Update(float delta_time) {}

    /// <summary>
    /// The theme used for drawing 
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetTheme* GetTheme() const { return _theme; }

    /// <summary>
    /// The style used for drawing. In derived classes requres GetStyleToken to return a valid style token
    /// </summary>
    API_PROPERTY() FORCE_INLINE FudgetStyle* GetStyle() const { return _style != nullptr ? _style : GetDefaultStyle(); }
protected:
    /// <summary>
    /// Tries to find a style that can be used to draw with this painter when a valid style wasn't provided on initialization
    /// </summary>
    /// <returns>A style appropriate for the painter</returns>
    API_PROPERTY() FudgetStyle* GetDefaultStyle() const;
private:
    FudgetTheme *_theme;
    FudgetStyle *_style;
};

/// <summary>
/// Painter for controls that have a background and a frame around a field. For text editors
/// list boxes, drop down menus.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetFramedFieldPainter : public FudgetPartPainter
{
    using Base = FudgetPartPainter;
    DECLARE_SCRIPTING_TYPE(FudgetFramedFieldPainter);
public:
    /// <summary>
    /// Creates the default style for drawing, filling the resources to use that will be looked up in the current theme.
    /// </summary>
    API_FUNCTION() static void CreateStyle();

    /// <inheritdoc />
    void Initialize(FudgetTheme *theme, FudgetStyle *style) override;
    void Draw(FudgetControl *control) override;

    API_FIELD(ReadOnly) static FudgetToken SelfToken;

    API_FIELD(ReadOnly) static FudgetToken FieldBackgroundToken;
    API_FIELD(ReadOnly) static FudgetToken FocusedFieldBackgroundToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledFieldBackgroundToken;
    API_FIELD(ReadOnly) static FudgetToken FieldPaddingToken;
    API_FIELD(ReadOnly) static FudgetToken FocusedFieldPaddingToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledFieldPaddingToken;

    API_FIELD(ReadOnly) static FudgetToken FrameDrawToken;
    API_FIELD(ReadOnly) static FudgetToken FocusedFrameDrawToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledFrameDrawToken;
    API_FIELD(ReadOnly) static FudgetToken FramePaddingToken;
    API_FIELD(ReadOnly) static FudgetToken FocusedFramePaddingToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledFramePaddingToken;

    API_FIELD(ReadOnly) static FudgetToken ContentPaddingToken;

    API_PROPERTY() FORCE_INLINE FudgetPadding GetContentPadding() const { return _inner_padding; }
private:
    FudgetDrawArea _field_bg;
    FudgetDrawArea _focused_field_bg;
    FudgetDrawArea _disabled_field_bg;
    FudgetPadding _field_padding;
    FudgetPadding _focused_field_padding;
    FudgetPadding _disabled_field_padding;

    FudgetDrawArea _Frame_area;
    FudgetDrawArea _focused_Frame_area;
    FudgetDrawArea _disabled_Frame_area;
    FudgetPadding _Frame_padding;
    FudgetPadding _focused_Frame_padding;
    FudgetPadding _disabled_Frame_padding;

    FudgetPadding _inner_padding;
};

