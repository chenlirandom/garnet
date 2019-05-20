#include "pch.h"
#include <garnet/GNrt.h>
#include <garnet/GNutil.h>
#include <garnet/gfx/fatModel.h>
#include <cli/CLI.hpp>
#include <chrono>

using namespace GN;
using namespace GN::gfx;
using namespace GN::rt;

struct SceneDesc
{
    struct Material
    {
        float albedo[3] = {.0f, .0f, .0f};
        float roughness = .0f;
        float emmisive[3] = {.0f, .0f, .0f};
        float refindex = .0f;
    };

    std::vector<FatModel> models;
    std::vector<Material> materials = {
        {{1.0, 1.0, 1.0}}, // white
        {{1.0, 0.0, 0.0}}, // red
        {{0.0, 1.0, 0.0}}, // green
        {{0.0, 0.0, 1.0}}, // blue
        {{1.0, 1.0, 0.0}}, // yellow
    };

    enum BuiltInMaterial
    {
        LAMBERT_WHITE,
        LAMBERT_RED,
        LAMBERT_GREEN,
        LAMBERT_BLUE,
        LAMBERT_YELLOW,
    };

    SceneDesc()
    {
        materials.push_back({{1., 1., 1.}});
        materials.push_back({{1., 0., 0.}});
    }

    void Load(const std::string &)
    {
        LoadSimpleCornellBox();
    }

    void LoadSimpleCornellBox()
    {
       
    }

    /*FatModel CreateCornellBox(float dimension)
    {
        float l = -dimension / 2.0f; // left
        float r = +dimension / 2.0f; // right
        float t = +dimension / 2.0f; // top
        float b = -dimension / 2.0f; // bottom
        float f = +dimension / 2.0f; // front
        float k = -dimension / 2.0f; // back
        Eigen::Vector3f v[] = {
            { l, b, f },
            { r, b, f },
            { r, t, f },
            { l, t, f },
            { l, b, k },
            { r, b, k },
            { r, t, k },
            { l, t, k },
        };
        FatModel m;
        AddRectFace(m, v, 5, 4, 7, 6, { 0, 0, 1 }, LAMBERT_WHITE); // back
        AddRectFace(m, v, 3, 2, 6, 7, { 0,-1, 0 }, LAMBERT_WHITE); // top
        AddRectFace(m, v, 4, 5, 1, 0, { 0, 1, 0 }, LAMBERT_WHITE); // bottom
        AddRectFace(m, v, 4, 0, 3, 7, { 1, 0, 0 }, LAMBERT_RED);   // left
        AddRectFace(m, v, 1, 5, 6, 2, {-1, 0, 0 }, LAMBERT_GREEN); // right
        return std::move(m);
    }

    static void AddRectFace(FatModel & m, const Eigen::Vector3f * v, int a, int b, int c, int d, const Eigen::Vector3f & normal, int material)
    {
        // if (m.meshes.empty()) {
        //     m.meshes.resize(1);
        //     //m.meshes[0].vertices.resize();
        // }
        // auto & mesh = m.meshes[0];
        // mesh.vertices.
        // m.meshes.resize
        // buffer.push_back({ {{v[a], 0.0, normal, 0.0},{v[b], 1.0, normal, 0.0},{v[c], 1.0, normal, 1.0}}, material });
        // buffer.push_back({ {{v[a], 0.0, normal, 0.0},{v[c], 1.0, normal, 1.0},{v[d], 0.0, normal, 1.0}}, material });
    };*/
};

struct Scene
{
    // currently, rect light only
    struct Light
    {
        Vec4 position;
        Vec4 edges[2];
    };

    template<typename T>
    struct TypedSurface
    {
        AutoRef<GN::gfx::Gpu2::Surface> g;
        std::vector<T>                  c;

        T & operator[](size_t i) { return c[i]; }
        const T & operator[](size_t i) const { return c[i]; }
    };

    TypedSurface<Vec4>  positions; // w channel is primitive ID
    TypedSurface<Vec4>  normals;   // w channel is material ID
    TypedSurface<AABB>  bvh;
    TypedSurface<Light> lights;

    ~Scene() { Cleanup(); }

    void Cleanup()
    {

    }

    void Load(const std::string &)
    {

    }

    struct DrawFrameParameters
    {
        Matrix44f proj;
        Matrix44f view;
        float     frameDuration;
    };

    void DrawFrame(const DrawFrameParameters &)
    {

    }
};

struct OrbitCamera
{
    GN::util::ArcBall arcball;
    Matrix44f proj;
    Matrix44f view;

public:

    OrbitCamera()
    {
        // TODO: hook with gInput signals,
    }

    void reset(const AABB & bb)
    {
        // setup default view and proj matrices.
        float radius = bb.GetDiagonalDistance();
        view.lookAtRh(GN::Vector3f(0,0,radius), GN::Vector3f(0,0,0), GN::Vector3f(0,1,0));
        proj.perspectiveD3D(GN_PI/4.0f, 16.0f/9.0f, radius / 100.0f, radius * 2.0f);
    }
};


int main( int argc, const char * argv[] )
{
    std::string filename;

    CLI::App app;
    app.add_option("filename", filename, "scene file name")->check(CLI::ExistingFile);
	CLI11_PARSE(app, argc, argv);

    // Create new window
    AutoObjPtr<GN::win::Window> win(GN::win::createWindow({"GNrt", 1280, 720}));

    // initalize input manager
    GN::input::initializeInputSystem();

    // create Gpu2
    auto gpu = GN::gfx::Gpu2::createGpu2({win});

    // initialie the scene
    Scene s;
    s.Load({filename});

    // main camera
    OrbitCamera camera;
    camera.reset(s.bvh[0]);

    // start the main loop
    float frameDuration = 1.0f / 30.0f;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    while(win->runUntilNoNewEvents()) {

        gInput.processInputEvents();

        s.DrawFrame({
            camera.view,
            camera.proj,
        });

        gpu->present({});

        // calculate frame time
        auto now = std::chrono::high_resolution_clock::now();
        frameDuration = (float)std::chrono::duration_cast<std::chrono::microseconds>(now - lastFrameTime).count() / 1e6f;
        lastFrameTime = now;
    }
}
