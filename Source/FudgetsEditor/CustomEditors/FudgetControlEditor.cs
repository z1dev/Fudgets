using FlaxEditor;
using FlaxEditor.CustomEditors;
using FlaxEditor.CustomEditors.Editors;
using FlaxEditor.CustomEditors.Elements;
using FlaxEditor.GUI.ContextMenu;
using FlaxEditor.Scripting;
using FlaxEngine;
using Fudgets;

namespace FudgetsEditor.CustomEditors
{
    [CustomEditor(typeof(FudgetControl))]
    sealed class FudgetControlEditor : GenericEditor
    {
        private GroupElement _group;
        private LabelElement _label;
        private ButtonElement _button;

        /// <inheritdoc />
        public override void Initialize(LayoutElementsContainer layout)
        {
            base.Initialize(layout);

            // TODO: Support editing multiple layouts?
            // Note: this also handles if Values.Count = 0.
            if (Values.Count != 1)
                return;

            FudgetControl control = (FudgetControl)Values[0];
            if (control.Parent == null)
                return;

            FudgetContainer parent = control.Parent;
            FudgetLayout fudgetLayout = parent.Layout;

            FudgetLayoutSlot slot = fudgetLayout.GetSlotInternal(control.IndexInParent);

            // TODO: I might be doing this wrong. Investigate further.
            CustomValueContainer values = new CustomValueContainer(new ScriptType(typeof(FudgetLayoutSlot)), (instance, index) => 
            {
                FudgetControl control = instance as FudgetControl;
                if (control.Parent == null)
                    return null;

                FudgetContainer container = control.Parent;
                FudgetLayout fudgetLayout = container.Layout;

                return fudgetLayout.GetSlotInternal(control.IndexInParent);
            });
            values.Add(slot);

            GenericEditor editor = new GenericEditor();
            _group = layout.Group("Layout Properties", null);
            CustomEditor newEditor = _group.Object(values, editor);

            newEditor.Refresh();
            newEditor.RebuildLayout();
            RebuildLayout();

            if (control is not FudgetContainer container)
                return;

            _group = layout.Group("Layout", null);
            _label = _group.Label($"Current Layout: {FlaxEditor.Utilities.Utils.GetPropertyNameUI(new ScriptType(fudgetLayout.GetType()).Name)}");
            _button = _group.Button("Change Layout");
            _button.Button.Clicked += () =>
            {
                ContextMenu newMenu = new ContextMenu();

                ScriptType controlType = new ScriptType(typeof(FudgetLayout));
                var allTypes = Editor.Instance.CodeEditing.All.Get();
                foreach (var type in allTypes)
                {
                    if (controlType.IsAssignableFrom(type))
                    {
                        ContextMenuButton button = newMenu.AddButton(FlaxEditor.Utilities.Utils.GetPropertyNameUI(type.Name));
                        button.Tag = type;
                        button.ButtonClicked += (x) =>
                        {
                            ScriptType type = (ScriptType)x.Tag;
                            FudgetContainer control = Values[0] as FudgetContainer;
                            var old_layout = control.SetLayout(type.CreateInstance() as FudgetLayout);
                            if (old_layout != null)
                                FlaxEngine.Object.Destroy(old_layout);
                            _label.Label.Text = $"Current Layout: {FlaxEditor.Utilities.Utils.GetPropertyNameUI(type.Name)}";
                        };
                    }
                }

                Float2 cmLocation = _button.Control.Location;
                cmLocation.Y -= newMenu.Height;
                newMenu.Show(_group.Control, cmLocation);
            };
        }
    }
}