#include "pch.h"
#include "oglResource.h"
#include "oglRenderer.h"

GN::gfx::OGLResource::OGLResource( OGLRenderer & r )
    : mRenderer(r)
{
    mRenderer.insertResource( this );
}

GN::gfx::OGLResource::~OGLResource()
{
    mRenderer.removeResource( this );
}
