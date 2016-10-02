#ifndef __GN_GFX2_GFX2_H
#define __GN_GFX2_GFX2_H
// *****************************************************************************
/// \file
/// \brief   Graphics System Version 2
/// \author  chenlee(2013.03.23)
// *****************************************************************************

#include "GNbase.h"

namespace GN
{
///
/// namespace for GFX module
///
namespace gfx2
{

class GpuBlob : public RefCounter
{
};

/// this could be a texture or a buffer.
class GpuResource : public RefCounter
{
    /// immediate update, no dealy, no async. do hazard tracking yourself.
    virtual void * getDirectPointer(UINT subResourceId, UINT * pRowPitch, UINT * pSlicePitch) = 0;
};

class GpuResourceTable : public RefCounter
{
};

class GpuProgram : public RefCounter
{
};

class GpuPipeline : public RefCounter
{
};

class GpuRenderState : public RefCounter
{
};

struct GpuDrawParameters
{
};

class GpuCommandList : public RefCounter
{
public:
    virtual void   Update(GpuResource * resource, UINT subResourceId, ...) = 0;
    virtual void   Copy(GpuResource * source, UINT sourceSubResId, GpuResource * target, UINT targetSubResId, ...) = 0;
    virtual void   Bind(GpuResourceTable * resources) = 0;
    virtual void   Draw(GpuPipeline * pipeline, const GpuDrawParameters * dp) = 0;
    virtual uint64 Mark() = 0;
    virtual void   Wait(uint64 fence) = 0; // insert a wait-for-fence into command list
};

class Gpu : public RefCounter
{
public:
    virtual AutoRef<GpuCommandList> CreateCommandList(...) = 0;
    virtual AutoRef<GpuResource>    CreateResource(...) = 0;
    virtual AutoRef<GpuRenderState> CreateRenderState(...) = 0;
    virtual AutoRef<GpuPipeline>    CreatePipeline(GpuProgram * program, GpuRenderState * state) = 0;
    virtual AutoRef<GpuProgram>     CreateProgram(const void * compiledBlob, size_t sizeInBytes) = 0;
    virtual AutoRef<GpuBlob>        CompileProgram(...) = 0;
    virtual void                    Run(GpuCommandList *) = 0;
    virtual int                     Present(...) = 0; // returns a fence that marks completion of the present.
    virtual void                    Wait(int fence) = 0; // block caller thread until the fence completes.
};

AutoRef<Gpu> createGpu(...);

} // end of namespace gfx2
} // end of namespace GN

// *****************************************************************************
//                         END of GNgfx2.h
// *****************************************************************************
#endif
