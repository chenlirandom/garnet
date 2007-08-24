#include "pch.h"
#include "d3d9VtxDecl.h"

static GN::Logger * sLogger = GN::getLogger( "GN.gfx2.D3D9Kernel" );

//
//
// -----------------------------------------------------------------------------
static const GN::gfx::SurfaceView *
sFindViewByName( const GN::StrA & name, const GN::gfx::KernelPortBindingDesc & desc )
{
    using namespace GN;
    using namespace GN::gfx;

    std::map<StrA,SurfaceView>::const_iterator i = desc.views.find( name );

    if( desc.views.end() == i ) return 0;

    return &i->second;
}

// *****************************************************************************
// D3D9KernelPortBinding
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D9KernelPortBinding::D3D9KernelPortBinding( D3D9Kernel & e )
    : mKernel( e )
    , mVtxDecl( 0 )
    , mHasZBuf(0)
    , mHasIdxBuf(0)
{
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D9KernelPortBinding::~D3D9KernelPortBinding()
{
    safeRelease( mVtxDecl );
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D9KernelPortBinding::setup( const KernelPortBindingDesc & bindings )
{
    GN_GUARD;

    GN_ASSERT( 0 == mVtxDecl );
    GN_ASSERT( 0 == mHasZBuf );
    GN_ASSERT( 0 == mHasIdxBuf );

    BindItem b;

    StackArray<const SurfaceElementFormat *,MAX_SURFACE_ELEMENT_ATTRIBUTES> vtxfmt;

    size_t numPorts = mKernel.getRefl().ports.size();
    for( size_t i = 0; i < numPorts; ++i )
    {
        const D3D9KernelPort & port = mKernel.getPortT<D3D9KernelPort>( i );

        const D3D9KernelPortType & portType = port.getPortType();

        const SurfaceView * view = sFindViewByName( port.getRefl().name, bindings );

        if( 0 == view || 0 == view->surf )
        {
            if( D3D9_KERNEL_PORT_RENDER_TARGET == portType ||
                D3D9_KERNEL_PORT_DEPTH_BUFFER == portType ||
                D3D9_KERNEL_PORT_TEXTURE == portType )
            {
                b.port        = i;
                b.target.surf = 0;
                mBindItems.append( b );
            }
        }
        else
        {
            if( !port.compatible( view->surf ) )
            {
                return false;
            }

            b.port   = i;
            b.target = *view;

            if( view->surf )
            {
                switch( portType )
                {
                    case D3D9_KERNEL_PORT_DEPTH_BUFFER:
                        mHasZBuf = true;
                        break;

                    case D3D9_KERNEL_PORT_VTXBUF:
                        vtxfmt.append( &view->surf->getDesc().layout.format );
                        break;

                    case D3D9_KERNEL_PORT_IDXBUF :
                        mHasIdxBuf = true;
                        break;

                    default:
                        // do nothing
                        break;
                };

                mBindItems.append( b );
            }
            else
            {
                // target surface is NULL.

                if( D3D9_KERNEL_PORT_VTXBUF != portType &&
                    D3D9_KERNEL_PORT_IDXBUF != portType )
                {
                    mBindItems.append( b );
                }
            }
        }
    }

    // create vertex declaration
    if( !vtxfmt.empty() )
    {
        mVtxDecl = createD3D9VtxDecl( mKernel.d3d9gs().d3ddev(), vtxfmt.cptr(), vtxfmt.size() );
    }

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D9KernelPortBinding::apply() const
{
    // setup vertex decl
    if( mVtxDecl )
    {
        mKernel.d3d9gs().d3ddev()->SetVertexDeclaration( mVtxDecl );
    }

    // bind each port
    for( size_t i = 0; i < mBindItems.size(); ++i )
    {
        const BindItem & b = mBindItems[i];

        mKernel.getPortT<D3D9KernelPort>(b.port).bind( b.target );
    }
}

// *****************************************************************************
// D3D9Kernel
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D9Kernel::D3D9Kernel( const char * name, D3D9GraphicsSystem & gs )
    : BaseKernel( name )
    , mGraphicsSystem(gs)
    , mDefaultPortBinding(0)
{
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D9Kernel::~D3D9Kernel()
{
    delete mDefaultPortBinding;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::KernelPortBinding *
GN::gfx::D3D9Kernel::createPortBinding( const KernelPortBindingDesc & desc )
{
    GN_GUARD;

    AutoObjPtr<D3D9KernelPortBinding> b( new D3D9KernelPortBinding(*this) );

    if( !b || !b->setup( desc ) ) return 0;

    return b.detach();

    GN_UNGUARD;
}
