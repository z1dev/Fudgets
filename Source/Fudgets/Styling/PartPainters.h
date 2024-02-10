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
    /// <param name="theme"></param>
    API_FUNCTION() virtual void Initialize(FudgetTheme *theme);

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
    API_PROPERTY() FORCE_INLINE FudgetStyle* GetStyle();
protected:
    /// <summary>
    /// Override in derived classes to get the token of the style used for the painter.
    /// </summary>
    API_FUNCTION() virtual FudgetToken GetStyleToken() const { return FudgetToken::Invalid; }
private:
    FudgetTheme *_theme;
    FudgetStyle *_style;
};

/// <summary>
/// Painter for controls that have a background and a border around a field. For text editors
/// list boxes, drop down menus.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetControlFieldPainter : public FudgetPartPainter
{
    using Base = FudgetPartPainter;
    DECLARE_SCRIPTING_TYPE(FudgetControlFieldPainter);
public:
    /// <summary>
    /// Creates the style for drawing, filling the resources to use that will be looked up in the current theme.
    /// </summary>
    API_FUNCTION() static void CreateStyle();

    void Initialize(FudgetTheme *theme) override;
    void Draw(FudgetControl *control) override;

    API_FIELD(ReadOnly) static FudgetToken SelfToken;

    API_FIELD(ReadOnly) static FudgetToken FieldBackgroundToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledFieldBackgroundToken;
    API_FIELD(ReadOnly) static FudgetToken FieldPaddingToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledFieldPaddingToken;

    API_FIELD(ReadOnly) static FudgetToken BorderDrawToken;
    API_FIELD(ReadOnly) static FudgetToken FocusedBorderDrawToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledBorderDrawToken;

    API_FIELD(ReadOnly) static FudgetToken BorderPaddingToken;
    API_FIELD(ReadOnly) static FudgetToken FocusedBorderPaddingToken;
    API_FIELD(ReadOnly) static FudgetToken DisabledBorderPaddingToken;

    API_FIELD(ReadOnly) static FudgetToken ContentPaddingToken;

    API_PROPERTY() FORCE_INLINE FudgetPadding GetContentPadding() const { return _inner_padding; }
protected:
    FudgetToken GetStyleToken() const override { return SelfToken; }
private:
    FudgetDrawArea _field_bg;
    FudgetDrawArea _disabled_field_bg;
    FudgetPadding _field_padding;
    FudgetPadding _disabled_field_padding;

    FudgetDrawArea _border_area;
    FudgetDrawArea _focused_border_area;
    FudgetDrawArea _disabled_border_area;
    FudgetPadding _border_padding;
    FudgetPadding _focused_border_padding;
    FudgetPadding _disabled_border_padding;

    FudgetPadding _inner_padding;
};