#include "pch.h"
#include <cuda_runtime.h>
#include <optix.h>
#include <optix_function_table_definition.h>
#include <optix_stubs.h>

using namespace GN;

static auto sLogger = GN::getLogger("GN.test.optix");

#define CUDA_CHECK(x, failed) do { cudaError_t result__ = (x); if (CUDA_SUCCESS != result__) { GN_ERROR(sLogger)("CUDA function failed: (%s) %s", cudaGetErrorName(result__), cudaGetErrorString(result__)); failed; } } while (0)
#define CUDA_RETURN_FALSE_ON_FAIL(x) CUDA_CHECK(x, return false)

#define OPTIX_CHECK(x, failed) do { OptixResult result__ = (x); if (OPTIX_SUCCESS != result__) { GN_ERROR(sLogger)("OptiX function failed: (%s) %s", optixGetErrorName(result__), optixGetErrorString(result__)); failed; } } while (0)
#define OPTIX_RETURN_ON_FAIL(x) OPTIX_CHECK(x, return)
#define OPTIX_RETURN_FALSE_ON_FAIL(x) OPTIX_CHECK(x, return false)

class OptixSample : public GN::util::SampleApp {

    OptixDeviceContext _context = 0;

    static void contextLogCallback( unsigned int level, const char* tag, const char* message, void* /*cbdata */)
    {
        std::cerr << "[OPTIX][" << std::setw( 2 ) << level << "][" << std::setw( 12 ) << tag << "]: "
        << message << "\n";
    }

public:

    OptixSample() {}

    bool onPreInit( InitParam & )
    {
        return true;
    }

    bool onInit()
    {
        // auto & g = *engine::getGpu();
        // uint32 width = g.getDispDesc().width;
        // uint32 height = g.getDispDesc().height;

        // Initialize CUDA
        CUDA_RETURN_FALSE_ON_FAIL(cudaFree(0));

        // initialize optix
        OPTIX_RETURN_FALSE_ON_FAIL(optixInit());
        OptixDeviceContextOptions options = {};
        options.logCallbackFunction       = &contextLogCallback;
        options.logCallbackLevel          = 4;
        OPTIX_RETURN_FALSE_ON_FAIL(optixDeviceContextCreate(0, &options, &_context));

        // done
        return true;
    }

    void onQuit() {
        if (_context) {
            optixDeviceContextDestroy(_context);
            _context = 0;
        }
    }

    void onKeyPress(input::KeyEvent key) {
        SampleApp::onKeyPress(key);
    }

    void onUpdate() {
    }

    void onRender() {
        auto g = engine::getGpu();
        g->clearScreen();
    }
};

int main(int argc, char * argv[]) {
    OptixSample app;
    return app.run( argc, argv );
}
