#include "Fudget.h"
//#include "RootControl.h"
#include "Container.h"

#include "Engine/Level/Scene/Scene.h"
#include "Engine/Profiler/Profiler.h"
#include "Engine/Render2D/Render2D.h"
#include "Engine/Graphics/GPUContext.h"
#include "Engine/Graphics/RenderTask.h"
#include "Engine/Graphics/RenderBuffers.h"
#include "Engine/Core/Log.h"
#include "Engine/Profiler/ProfilerGPU.h"
#include "Engine/Profiler/ProfilerCPU.h"
#include "Engine/Engine/Time.h"


//Renderer::FudgetRenderer(const SpawnParams &params) : PostProcessEffect(params)
//{
//}

bool FudgetRenderer::CanRender() const
{
    // This is a const in the base class, but C# files can somehow declare it non-const.
    // We can cheat with const_cast<> which will either blow up or it will just not work.

    //// Sync with canvas options
    //Location = Canvas->GetRenderLocation();
    //Order = Canvas->GetOrder();

    FudgetRenderer *obj = const_cast<FudgetRenderer*>(this);
    obj->Location = Canvas->GetRenderLocation();
    //obj->Order = Canvas->GetOrder();

    return PostProcessEffect::CanRender();
}

void FudgetRenderer::Render(GPUContext* context, API_PARAM(Ref) RenderContext& renderContext, GPUTexture* input, GPUTexture* output)
{
    if (!Canvas->IsVisible(renderContext.View.RenderLayersMask))
        return;
    auto bounds = Canvas->GetBounds();
    bounds.Transformation.Translation -= renderContext.View.Origin;
    if (renderContext.View.Frustum.Contains(bounds.GetBoundingBox()) == ContainmentType::Disjoint)
        return;

    int profilerEvent = ProfilerGPU::BeginEvent(TEXT("UI Canvas"));

    // Calculate rendering matrix (world*view*projection)
    Matrix worldMatrix;
    Canvas->GetWorldMatrix(renderContext.View.Origin, worldMatrix);

    Matrix viewMatrix;
    Matrix::Multiply(worldMatrix, renderContext.View.View, viewMatrix);
    Matrix viewProjectionMatrix;
    Matrix::Multiply(viewMatrix, renderContext.View.Projection, viewProjectionMatrix);

    // Pick a depth buffer
    GPUTexture* depthBuffer = Canvas->GetIgnoreDepth() ? nullptr : renderContext.Buffers->DepthBuffer;

    // Render GUI in 3D
    auto features = Render2D::Features;

    if (Canvas->GetRenderMode() == FudgetRenderMode::WorldSpace || Canvas->GetRenderMode() == FudgetRenderMode::WorldSpaceFaceCamera)
        Render2D::Features = (Render2D::RenderingFeatures)((int)Render2D::Features & ~(int)Render2D::RenderingFeatures::VertexSnapping);

    //Render2D::CallDrawing(Canvas->GUI, context, input, depthBuffer, viewProjectionMatrix);
    // Replacement of the CallDrawing call which is missing from C++, but only takes up a few lines:

    if (context != nullptr && input != nullptr && Canvas->GetGUI() != nullptr)
    {
        Render2D::Begin(context, input);
        // TODO: check if we can get around the try/catch, since it's bad for peformance.
        try
        {
            Canvas->GetGUI()->Draw();
        }
        catch (...)
        {
            LOG(Error, "Drawing of the UI failed with an exception.");
        }
        Render2D::End();
    }

    Render2D::Features = features;

    ProfilerGPU::EndEvent(profilerEvent);
}

bool FudgetRenderer2D::CanRender() const
{
    // This is a const in the base class, but C# files can somehow declare it non-const.
    // We can cheat with const_cast<> which will either blow up or it will just not work.

    //// Sync with canvas options
    //Location = Canvas->GetRenderLocation();
    //Order = Canvas->GetOrder();

    FudgetRenderer2D *obj = const_cast<FudgetRenderer2D*>(this);
    obj->Location = Canvas->GetRenderLocation();
    //obj->Order = Canvas->GetOrder();

    return PostProcessEffect::CanRender();
}

void FudgetRenderer2D::Render(GPUContext* context, API_PARAM(Ref) RenderContext& renderContext, GPUTexture* input, GPUTexture* output)
{
    if (!Canvas->IsVisible(renderContext.View.RenderLayersMask))
        return;
    auto bounds = Canvas->GetBounds();
    bounds.Transformation.Translation -= renderContext.View.Origin;
    if (renderContext.View.Frustum.Contains(bounds.GetBoundingBox()) == ContainmentType::Disjoint)
        return;

    int profilerEvent = ProfilerGPU::BeginEvent(TEXT("UI Canvas 2D"));

    if (context != nullptr && input != nullptr && Canvas->GetGUI() != nullptr)
    {
        Render2D::Begin(context, input);
        try
        {
            //if (!size_set)
            //{
                //size_set = true;
                const Viewport &v = Render2D::GetViewport();
                Canvas->_guiRoot->SetSize(v.Size);
            //}

            Canvas->GetGUI()->Draw();
        }
        catch (...)
        {
            LOG(Error, "Drawing of the 2D UI failed with an exception.");
        }
        Render2D::End();
    }

    //// Calculate rendering matrix (world*view*projection)
    //Matrix worldMatrix;
    //Canvas->GetWorldMatrix(renderContext.View.Origin, worldMatrix);

    //Matrix viewMatrix;
    //Matrix::Multiply(worldMatrix, renderContext.View.View, viewMatrix);
    //Matrix viewProjectionMatrix;
    //Matrix::Multiply(viewMatrix, renderContext.View.Projection, viewProjectionMatrix);

    //// Pick a depth buffer
    //GPUTexture* depthBuffer = Canvas->GetIgnoreDepth() ? nullptr : renderContext.Buffers->DepthBuffer;

    //// Render GUI in 3D
    //auto features = Render2D::Features;

    //if (Canvas->GetRenderMode() == FudgetRenderMode::WorldSpace || Canvas->GetRenderMode() == FudgetRenderMode::WorldSpaceFaceCamera)
    //    Render2D::Features = (Render2D::RenderingFeatures)((int)Render2D::Features & ~(int)Render2D::RenderingFeatures::VertexSnapping);

    ////Render2D::CallDrawing(Canvas->GUI, context, input, depthBuffer, viewProjectionMatrix);
    //// Replacement of the CallDrawing call which is missing from C++, but only takes up a few lines:

    //if (context != nullptr && input != nullptr && Canvas->GetGUI() != nullptr)
    //{
    //    Render2D::Begin(context, input);
    //    // TODO: check if we can get around the try/catch, since it's bad for peformance.
    //    try
    //    {
    //        Canvas->GetGUI()->Draw();
    //    }
    //    catch (...)
    //    {
    //        LOG(Error, "Drawing of the UI failed with an exception.");
    //    }
    //    Render2D::End();
    //}

    //Render2D::Features = features;

    ProfilerGPU::EndEvent(profilerEvent);
}



Fudget::Fudget(const SpawnParams& params) : Actor(params), _guiRoot(New<FudgetContainer>(/*this*/))
{
    //_guiRoot->SetIsLayoutLocked(false);
    NavigateUp = New<FudgetInputEvent>(TEXT("NavigateUp"));
    NavigateDown = New<FudgetInputEvent>(TEXT("NavigateDown"));
    NavigateLeft = New<FudgetInputEvent>(TEXT("NavigateLeft"));
    NavigateRight = New<FudgetInputEvent>(TEXT("NavigateRight"));
    NavigateSubmit = New<FudgetInputEvent>(TEXT("NavigateSubmit"));
}

Fudget::~Fudget()
{
    Delete(NavigateUp);
    Delete(NavigateDown);
    Delete(NavigateLeft);
    Delete(NavigateRight);
    Delete(NavigateSubmit);

    if (_isRegisteredForTick)
    {
        // TODO: Figure out how to register to Scripting Events.

        _isRegisteredForTick = false;
        //Scripting::Update -= OnUpdate;

        // C# and C++ ticking at different places. This should work (until proven wrong)
        auto scene = GetScene();
        if (scene != nullptr)
            scene->Ticking.Update.RemoveTick(this);
    }

    Delete(_guiRoot);
    // Set to null or 2d ui drawing blows up.
    _guiRoot = nullptr;
}

void Fudget::SetRenderMode(FudgetRenderMode value)
{
    if (_renderMode != value)
    {
        auto previous = _renderMode;

        _renderMode = value;

        Setup();

        // Reset size
        if (previous == FudgetRenderMode::ScreenSpace || (_renderMode == FudgetRenderMode::WorldSpace || _renderMode == FudgetRenderMode::WorldSpaceFaceCamera))
            SetSize(Float2(500, 500));
    }
}

//void Fudget::SetOrder(int value)
//{
//    if (_order != value)
//    {
//        _order = value;
//        UIXRootControl::GetCanvasRoot()->SortCanvases();
//    }
//}

Float2 Fudget::GetSize() const
{
    return _guiRoot->GetSize();
}

void Fudget::SetSize(Float2 value)
{
    if (_renderMode == FudgetRenderMode::WorldSpace || _renderMode == FudgetRenderMode::WorldSpaceFaceCamera || _isLoading)
    {
        _guiRoot->SetSize(value);
    }
}

OrientedBoundingBox Fudget::GetBounds() const
{
    OrientedBoundingBox bounds = OrientedBoundingBox();
    bounds.Extents = Float3(_guiRoot->GetSize() * 0.5f, 0.1e-7);

    Matrix world;
    GetWorldMatrix(world);

    Matrix offset;
    Matrix::Translation((float)bounds.Extents.X, (float)bounds.Extents.Y, 0, offset);

    Matrix boxWorld;
    Matrix::Multiply(offset, world, boxWorld);
    boxWorld.Decompose(bounds.Transformation);
    return bounds;
}

void Fudget::GetWorldMatrix(Vector3 viewOrigin, API_PARAM(Out) Matrix& world) const
{
    auto transform = Transform();
    Float3 translation = transform.Translation - viewOrigin;
    // TODO: Remove inline out declarations, and replace them with locals. Uncomment the USE_EDITOR if case.

#if USE_EDITOR
        // Override projection for editor preview
    if (_editorTask)
    {
        if (_renderMode == FudgetRenderMode::WorldSpace)
        {
            Matrix::Transformation(transform.Scale, transform.Orientation, translation, world);
        }
        else if (_renderMode == FudgetRenderMode::WorldSpaceFaceCamera)
        {
            auto view = _editorTask->View;
            Matrix m1;
            Matrix::Translation(_guiRoot->GetWidth() * -0.5f, _guiRoot->GetHeight() * -0.5f, 0, m1);
            Matrix m2;
            Matrix::Scaling(transform.Scale, m2);
            Matrix m3;
            Matrix::Multiply(m1, m2, m3);
            Quaternion quat = Quaternion::Euler(180, 180, 0);
            Matrix::RotationQuaternion(quat, m2);
            Matrix::Multiply(m3, m2, m1);
            Matrix::Transformation(Float3::One, Quaternion::FromDirection(-view.Direction), translation, m2);
            Matrix::Multiply(m1, m2, world);
        }
        else if (_renderMode == FudgetRenderMode::CameraSpace)
        {
            auto view = _editorTask->View;
            auto frustum = view.Frustum;
            if (!frustum.IsOrthographic())
                _guiRoot->SetSize(Float2(frustum.GetWidthAtDepth(GetDistance()), frustum.GetHeightAtDepth(GetDistance())));
            else
                _guiRoot->SetSize(_editorTask->GetViewport().Size);
            Matrix::Translation(_guiRoot->GetWidth() / -2.0f, _guiRoot->GetHeight() / -2.0f, 0, world);
            Matrix tmp1;
            Matrix tmp2;
            Matrix::RotationYawPitchRoll(PI, PI, 0, tmp2);
            Matrix::Multiply(world, tmp2, tmp1);
            Float3 viewPos = view.Position - viewOrigin;
            auto viewRot = view.Direction != Float3::Up ? Quaternion::LookRotation(view.Direction, Float3::Up) : Quaternion::LookRotation(view.Direction, Float3::Right);
            // TODO: check if this is valid. Original auto viewUp = Float3::Up * viewRot.
            auto viewUp = viewRot * Float3::Up;
            auto viewForward = view.Direction;
            auto pos = view.Position + view.Direction * GetDistance();
            Matrix::Billboard(pos, viewPos, viewUp, viewForward, tmp2);
            Matrix::Multiply(tmp1, tmp2, world);
            return;
        }
        else
        {
            world = Matrix::Identity;
        }
        return;
    }
#endif

    // Use default camera is not specified
    auto camera = GetRenderCamera() == nullptr ? Camera::GetMainCamera() : GetRenderCamera();

    if (_renderMode == FudgetRenderMode::WorldSpace || (_renderMode == FudgetRenderMode::WorldSpaceFaceCamera && !camera))
    {
        // In 3D world
        Matrix::Transformation(transform.Scale, transform.Orientation, translation, world);
    }
    else if (_renderMode == FudgetRenderMode::WorldSpaceFaceCamera)
    {
        // In 3D world face camera
        Matrix m1;
        Matrix::Translation(_guiRoot->GetWidth() * -0.5f, _guiRoot->GetHeight() * -0.5f, 0, m1);
        Matrix m2;
        Matrix::Scaling(transform.Scale, m2);
        Matrix m3;
        Matrix::Multiply(m1, m2, m3);
        Quaternion quat = Quaternion::Euler(180, 180, 0);
        Matrix::RotationQuaternion(quat, m2);
        Matrix::Multiply(m3, m2, m1);
        Matrix::Transformation(Vector3::One, Quaternion::FromDirection(-camera->GetDirection()), translation, m2);
        Matrix::Multiply(m1, m2, world);
    }
    else if (_renderMode == FudgetRenderMode::CameraSpace && camera)
    {
        Matrix tmp1, tmp2;

        // Adjust GUI size to the viewport size at the given distance form the camera
        auto viewport = camera->GetViewport();
        if (camera->GetUsePerspective())
        {
            Matrix tmp3;
            camera->GetMatrices(tmp1, tmp3, viewport);
            Matrix::Multiply(tmp1, tmp3, tmp2);
            auto frustum = BoundingFrustum(tmp2);
            _guiRoot->SetSize(Float2(frustum.GetWidthAtDepth(GetDistance()), frustum.GetHeightAtDepth(GetDistance())));
        }
        else
        {
            _guiRoot->SetSize(viewport.Size * camera->GetOrthographicScale());
        }

        // Center viewport (and flip)
        Matrix::Translation(_guiRoot->GetWidth() / -2.0f, _guiRoot->GetHeight() / -2.0f, 0, world);
        Matrix::RotationYawPitchRoll(PI, PI, 0, tmp2);
        Matrix::Multiply(world, tmp2, tmp1);

        // In front of the camera
        Float3 viewPos = camera->GetPosition() - viewOrigin;
        auto viewRot = camera->GetOrientation();
        auto viewUp = viewRot * Float3::Up;
        auto viewForward = viewRot * Float3::Forward;
        auto pos = viewPos + viewForward * GetDistance();
        Matrix::Billboard(pos, viewPos, viewUp, viewForward, tmp2);

        Matrix::Multiply(tmp1, tmp2, world);
    }
    else
    {
        // Direct projection
        world = Matrix::Identity;
        
        //auto viewport = camera->GetViewport();
        //_guiRoot->SetSize(Float2(500.0, 500.0));
    }
}

void Fudget::OnEnable()
{
    Enable();
    Actor::OnEnable();
}

void Fudget::OnDisable()
{
    Actor::OnDisable();
    Disable();
}

void Fudget::OnBeginPlay()
{
    PostDeserialize();

    Actor::OnBeginPlay();
}

void Fudget::OnEndPlay()
{
    Actor::OnEndPlay();
}

void Fudget::OnParentChanged()
{
    Actor::OnParentChanged();

    ParentChanged();
}

void Fudget::OnTransformChanged()
{
    Actor::OnTransformChanged();

    _box = BoundingBox(_transform.Translation);
    _sphere = BoundingSphere(_transform.Translation, 0.0f);
}


Delegate<Float2&, Ray&> Fudget::CalculateRay;

void Fudget::DefaultCalculateRay(Float2 location, API_PARAM(Out) Ray& ray)
{
    auto camera = Camera::GetMainCamera();
    if (camera)
    {
        ray = camera->ConvertMouseToRay(location * Platform::GetDpiScale());
    }
    else
    {
        ray = Ray(Vector3::Zero, Vector3::Forward);
    }
}

void Fudget::Setup()
{
    if (_isLoading)
        return;

    switch (_renderMode)
    {
        case FudgetRenderMode::ScreenSpace:
        {
            //// Fill the screen area
            //if (_guiRoot != nullptr)
            //{
            //    _guiRoot->SetAnchorPreset(UIXAnchorPresets::StretchAll);
            //    _guiRoot->SetOffsets(UIXMargin(0.0f));
            //}
            if (_renderer)
            {
#if USE_EDITOR
                if (_editorTask != nullptr)
                    _editorTask->RemoveCustomPostFx(_renderer);
#endif
                SceneRenderTask::RemoveGlobalCustomPostFx(_renderer);
                _renderer->Canvas = nullptr;

                Delete(_renderer);

                _renderer = nullptr;
            }
            if (_renderer2d == nullptr)
            {
                _renderer2d = New<FudgetRenderer2D>();
                _renderer2d->Canvas = this;
                if (IsActiveInHierarchy() && GetScene() != nullptr)
                {
#if USE_EDITOR
                    if (_editorTask != nullptr)
                    {
                        _editorTask->AddCustomPostFx(_renderer2d);
                        break;
                    }
#endif
                    SceneRenderTask::AddGlobalCustomPostFx(_renderer2d);
                }
#if USE_EDITOR
                else if (_editorTask != nullptr && IsActiveInHierarchy())
                {
                    _editorTask->AddCustomPostFx(_renderer2d);
                }
#endif
            }

//#if USE_EDITOR
//            if (_guiRoot != nullptr && _editorRoot != nullptr && IsActiveInHierarchy())
//            {
//                _guiRoot->SetParent(_editorRoot);
//                _guiRoot->SetIndexInParent(0);
//            }
//#endif
            if (_isRegisteredForTick)
            {
                _isRegisteredForTick = false;

                //Scripting.Update -= OnUpdate;
                GetScene()->Ticking.Update.RemoveTick(this);
            }
            break;
        }
        case FudgetRenderMode::CameraSpace:
        case FudgetRenderMode::WorldSpace:
        case FudgetRenderMode::WorldSpaceFaceCamera:
        {
            //// Render canvas manually
            //_guiRoot->SetAnchorPreset(UIXAnchorPresets::TopLeft);
//#if USE_EDITOR
//            if (_editorRoot != nullptr && _guiRoot != nullptr)
//                _guiRoot->SetParent(nullptr);
//#endif

            if (_renderer2d)
            {
#if USE_EDITOR
                if (_editorTask != nullptr)
                    _editorTask->RemoveCustomPostFx(_renderer2d);
#endif
                SceneRenderTask::RemoveGlobalCustomPostFx(_renderer2d);
                _renderer->Canvas = nullptr;

                Delete(_renderer2d);

                _renderer2d = nullptr;
            }
            if (_renderer == nullptr)
            {
                _renderer = New<FudgetRenderer>();
                _renderer->Canvas = this;
                if (IsActiveInHierarchy() && GetScene() != nullptr)
                {
#if USE_EDITOR

                    if (_editorTask != nullptr)
                    {
                        _editorTask->AddCustomPostFx(_renderer);
                        break;
                    }
#endif
                    SceneRenderTask::AddGlobalCustomPostFx(_renderer);
                }
#if USE_EDITOR
                else if (_editorTask != nullptr && IsActiveInHierarchy())
                {
                    _editorTask->AddCustomPostFx(_renderer);
                }
#endif
            }

            if (!_isRegisteredForTick)
            {
                _isRegisteredForTick = true;

                GetScene()->Ticking.Update.AddTick<Fudget, &Fudget::OnUpdate>(this);
                //Scripting.Update += OnUpdate;
            }
            break;
        }
    }
}

void Fudget::OnUpdate()
{
    if (this && IsActiveInHierarchy() && _renderMode != FudgetRenderMode::ScreenSpace)
    {
        try
        {
            ProfilerCPU::BeginEvent(*GetName());
            _guiRoot->Update(Time::GetUnscaledDeltaTime());
        }
        catch (...)
        {
            ProfilerCPU::EndEvent();
            LOG(Error, "Exception thrown during gui root update.");
            return;
        }

        ProfilerCPU::EndEvent();
    }
}


void Fudget::ParentChanged()
{
//#if USE_EDITOR
////#if 0
//    if (_renderMode == FudgetRenderMode::ScreenSpace && _editorRoot != nullptr && _guiRoot != nullptr)
//    {
//        _guiRoot->SetParent(IsActiveInHierarchy() ? _editorRoot : nullptr);
//        _guiRoot->SetIndexInParent(0);
//    }
//#endif
}

void Fudget::Enable()
{

//    if (_guiRoot != nullptr)
//    {
//#if USE_EDITOR
//        //#if 0
//        if (_editorRoot != nullptr)
//        {
//            _guiRoot->SetParent(_editorRoot);
//            _guiRoot->SetIndexInParent(0);
//        }
//        else
//        {
//            _guiRoot->SetParent(UIXRootControl::GetCanvasRoot());
//        }
//#else
//        _guiRoot->SetParent(UIXRootControl::GetCanvasRoot());
//#endif
//    }

    if (_renderer)
    {
#if USE_EDITOR
        if (_editorTask != nullptr)
        {
            _editorTask->AddCustomPostFx(_renderer);
            return;
        }
#endif
        SceneRenderTask::AddGlobalCustomPostFx(_renderer);
    }

    if (_renderer2d)
    {
#if USE_EDITOR
        if (_editorTask != nullptr)
        {
            _editorTask->AddCustomPostFx(_renderer2d);
            return;
        }
#endif
        SceneRenderTask::AddGlobalCustomPostFx(_renderer2d);
    }
}

void Fudget::Disable()
{
    //if (_guiRoot != nullptr)
    //    _guiRoot->SetParent(nullptr);

    if (_renderer)
    {
        SceneRenderTask::RemoveGlobalCustomPostFx(_renderer);
    }
    if (_renderer2d)
    {
        SceneRenderTask::RemoveGlobalCustomPostFx(_renderer2d);
    }
}

void Fudget::EndPlay()
{
    if (_isRegisteredForTick)
    {
        _isRegisteredForTick = false;
        //Scripting.Update -= OnUpdate;
        GetScene()->Ticking.Update.RemoveTick(this);
    }

    if (_renderer)
    {
        SceneRenderTask::RemoveGlobalCustomPostFx(_renderer);
        _renderer->Canvas = nullptr;

        Delete(_renderer);
        _renderer = nullptr;
    }

    if (_renderer2d)
    {
        SceneRenderTask::RemoveGlobalCustomPostFx(_renderer2d);
        _renderer2d->Canvas = nullptr;

        Delete(_renderer2d);
        _renderer2d = nullptr;
    }

    Actor::EndPlay();
}

void Fudget::OnDeleteObject()
{
    Actor::OnDeleteObject();
}

void Fudget::PostDeserialize()
{
    Setup();
}

bool Fudget::IsVisible() const
{
    LayersMask lm = MainRenderTask::Instance != nullptr ? MainRenderTask::Instance->View.RenderLayersMask : LayersMask();
    return IsVisible(lm);
}

bool Fudget::IsVisible(LayersMask layersMask) const
{
#if USE_EDITOR
//#if 0
    if (_editorTask != nullptr || _editorRoot != nullptr)
        return true;
#endif
    return layersMask.HasLayer(GetLayer());
}

#if USE_EDITOR
void Fudget::EditorOverride(SceneRenderTask *task, FudgetContainer *root)
{
    if (_editorTask == task && _editorRoot == root)
        return;
    if (_editorTask != nullptr && _renderer != nullptr)
        _editorTask->RemoveCustomPostFx(_renderer);
    //if (_editorRoot != nullptr && _guiRoot != nullptr)
    //    _guiRoot->SetParent(nullptr);

    _editorTask = task;
    _editorRoot = root;
    Setup();

    //if (_renderMode == FudgetRenderMode::ScreenSpace && _editorRoot != nullptr && _guiRoot != nullptr && IsActiveInHierarchy())
    //{
    //    _guiRoot->SetParent(_editorRoot);
    //    _guiRoot->SetIndexInParent(0);
    //}
}

void Fudget::OnActiveInTreeChanged()
{
    //if (_renderMode == FudgetRenderMode::ScreenSpace && _editorRoot != nullptr && _guiRoot != nullptr)
    //{
    //    _guiRoot->SetParent(IsActiveInHierarchy() ? _editorRoot : nullptr);
    //    _guiRoot->SetIndexInParent(0);
    //}
}
#endif


