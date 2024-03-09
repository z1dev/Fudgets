using Fudgets;
using FlaxEditor.GUI.Tree;
using FlaxEditor.SceneGraph;
using System.Collections.Generic;
using FlaxEngine.GUI;
using System;
using FlaxEngine;

namespace FudgetsEditor;

public partial class FudgetsEditorWindow
{
    private List<FudgetControl> GetAllControls(FudgetContainer container)
    {
        List<FudgetControl> controls = new List<FudgetControl>();
        for (int i = 0; i < container.ChildCount; i++)
        {
            FudgetControl control = container.ChildAt(i);
            controls.Add(control);

            if (control is FudgetContainer con)
            {
                controls.AddRange(GetAllControls(con));
            }
        }

        return controls;
    }

    /// <summary>
    /// Gets the tree node for the target control.
    /// </summary>
    /// <param name="control">The control to search for.</param>
    /// <returns>The node in the tree, or null if nothing is found.</returns>
    public FudgetControlNode FindNodeByControl(FudgetControl control)
    {
        List<FudgetControlNode> subset = RecurseAndGetSubset(_tree, (node) =>
        {
            if (node.Tag == (object)control)
            {
                return true;
            }

            return false;
        });

        return subset.Count >= 1 ? subset[0] : null;
    }

    /// <summary>
    /// Additively selects the tree node of a specified control.
    /// </summary>
    /// <param name="control">The control to select.</param>
    public void SelectNodeByControl(FudgetControl control)
    {
        FudgetControlNode node = FindNodeByControl(control);
        if (node == null)
            return;

        List<TreeNode> nodes = new List<TreeNode>();
        nodes.AddRange(_tree.Selection);
        nodes.Add(node);

        _tree.SetSelectionSilent(nodes);
    }

    private List<FudgetControlNode> RecurseAndGetSubset(ContainerControl current, Func<FudgetControlNode, bool> validator)
    {
        List<FudgetControlNode> subset = new List<FudgetControlNode>();
        foreach (Control child in current.Children)
        {
            if (child is not FudgetControlNode node)
            {
                continue;
            }

            if (validator(node))
            {
                subset.Add(node);
            }

            subset.AddRange(RecurseAndGetSubset(node, validator));
        }

        return subset;
    }

    private class FudgetRenderImage : Image
    {
        public FudgetsEditorWindow EditingWindow => (FudgetsEditorWindow)Tag;

        private bool _dragging = false;

        public override void OnMouseMove(Float2 location)
        {
            base.OnMouseMove(location);
            if (_dragging)
            {
                List<FudgetControl> controls = GetControlsAtLocation(location);
                Debug.Log($"Dragging at coordinates: {ConvertFromControlLocation(location)}");
                Debug.Log("Found Controls:");
                foreach (FudgetControl control in controls)
                {
                    Debug.Log($"  - '{control.Name}' ({control.Width}x{control.Height})");
                }
            }
        }

        public override bool OnMouseDown(Float2 location, MouseButton button)
        {
            if (base.OnMouseDown(location, button))
                return true;

            _dragging = true;
            return true;
        }

        public override bool OnMouseUp(Float2 location, MouseButton button)
        {
            if (base.OnMouseUp(location, button))
                return true;

            _dragging = false;
            return true;
        }

        public override void OnMouseLeave()
        {
            base.OnMouseLeave();
            _dragging = false;
        }

        private Float2 ConvertFromControlLocation(Float2 location)
        {
            float xRatio = EditingWindow._resolution.X / Width;
            float yRatio = EditingWindow._resolution.Y / Height;

            return new Float2(location.X * xRatio, location.Y * yRatio);
        }

        private List<FudgetControl> GetControlsAtLocation(Float2 location)
        {
            location = ConvertFromControlLocation(location);
            List<FudgetControl> controls = EditingWindow.GetAllControls(EditingWindow.RootObject);
            List<FudgetControl> intersectingControls = new List<FudgetControl>();

            foreach (FudgetControl control in controls)
            {
                Float2 localLocation = control.GlobalToLocal(location);
                //localLocation += control.Position;
                localLocation = new Float2(localLocation.X + control.Position.X, localLocation.Y + control.Position.Y);
                if (control.BoundsInParent.Contains(localLocation))
                {
                    intersectingControls.Add(control);
                }
            }

            return intersectingControls;
        }
    }
}