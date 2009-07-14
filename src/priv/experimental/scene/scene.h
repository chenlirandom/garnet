// Experiment with a world with infinit detail (like earth in google earth)
#include "GNgfx.h"

using namespace GN::gfx;

namespace GN
{
    struct Resource : NoCopy
    {
        virtual bool load( const StrA & filename ) = 0;
        virtual void unload() = 0;
        virtual bool dump( const StrA & filename ) = 0;
    };

    class GraphicsResourceManager
    {
    };

    struct SpatialInfo
    {
        Entity * parentEntity;
    };

    struct VisualInfo
    {
    };

    struct Entity : public NoCopy
    {
        virtual SpatialInfo & getSpatialInfo() = 0;
        virtual VisualInfo  & getVisualInfo() = 0;
    };

    struct CameraInfo
    {
    };

    struct World : public NoCopy
    {
        // resource managers
        ResourceManagerTempl<Entity*>  entities;

        // render to world (draw to screen, play sound and etc.)
        virtual void render( const CameraInfo & ) = 0;
    };
}
