#include "pch.h"

//
//
// -----------------------------------------------------------------------------
void GN::calcBoundingSphere( Spheref & result, const Vector3f * positions, size_t count, size_t strideInBytes )
{
    GN_GUARD;

    Boxf box;

    calcBoundingBox( box, positions, count, strideInBytes );

    result.center = box.center();
    result.radius = Vector3f::sDistance( result.center, box.pos() );

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::calcBoundingBox( Boxf & result, const Vector3f * positions, size_t count, size_t strideInBytes )
{
    GN_GUARD;

    if( 0 == positions )
    {
        GN_ERROR( "Null position array." );
        return;
    }
    if( 0 == count )
    {
        GN_ERROR( "There's no way to calculate bounding box for 0 vertices." );
        return;
    }

    Vector3f vMin( positions[0] );
    Vector3f vMax( positions[0] );

    const uint8_t * ptr = (const uint8_t *)positions;

    for( size_t i = 1; i < count; ++i, ptr += strideInBytes )
    {
        const Vector3f & v = ((const Vector3f*)ptr)[0];

        vMin.x = min( vMin.x, v.x );
        vMin.y = min( vMin.y, v.y );
        vMin.z = min( vMin.z, v.z );

        vMax.x = max( vMax.x, v.x );
        vMax.y = max( vMax.y, v.y );
        vMax.z = max( vMax.z, v.z );
    }

    result.pos() = vMin;
    result.size() = vMax - vMin;

    GN_UNGUARD;
}
