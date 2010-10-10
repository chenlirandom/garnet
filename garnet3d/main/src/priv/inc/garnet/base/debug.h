#ifndef __GN_BASE_DEBUG_H__
#define __GN_BASE_DEBUG_H__
// *****************************************************************************
/// \file
/// \brief   debug functions and macros
/// \author  chenlee (2005.4.17)
// *****************************************************************************

///
/// Assert failture
///
#define GN_ASSERT_FAILURE( desc )                                \
    {                                                            \
        static bool sIgnoreFromNowOn = false;                    \
        if( !sIgnoreFromNowOn && GN::assertFunc( desc, __FILE__, \
            __LINE__, &sIgnoreFromNowOn ) )                      \
        { ::GN::breakIntoDebugger(); }                                    \
    }

///
/// Perform runtime assert.
///
/// This macro will perform assertion in all builds, in case you want assert in
/// release build. Normally, you don't need this.
///
#define GN_DO_ASSERT( exp, desc ) if( !(exp) ) GN_ASSERT_FAILURE(desc) else void(0)


///
/// Assert macro with description
///
#if GN_ENABLE_ASSERT
#define GN_ASSERT_EX( exp, desc ) GN_DO_ASSERT( exp, desc )
#else
#define GN_ASSERT_EX( exp, desc ) void(0)
#endif

///
/// Verification macro with description
///
#if GN_ENABLE_ASSERT
#define GN_VERIFY_EX( exp, desc ) GN_DO_ASSERT( exp, desc )
#else
#define GN_VERIFY_EX( exp, desc ) (exp)
#endif

///
/// assert macro
///
#define GN_ASSERT( exp ) GN_ASSERT_EX( exp, #exp )

///
/// verify macro
///
#define GN_VERIFY( exp ) GN_VERIFY_EX( exp, #exp )

///
/// Meet unexpected value
///
#define GN_UNEXPECTED_EX( message ) GN_ASSERT_FAILURE( message )

///
/// Meet unexpected value
///
#define GN_UNEXPECTED() GN_UNEXPECTED_EX( "Unexpected value or behaviour" )

///
/// Assert for unimplemented functionality
///
#define GN_UNIMPL() GN_ASSERT_FAILURE( "Unimplemented!" )

///
/// Output a warning message for unimplemented functionality
///
#define GN_UNIMPL_WARNING() GN_DO_ONCE( GN_WARN(GN::getLogger("GN.base.todo"))( "TODO: function %s is not implmented yet.", GN_FUNCTION ) );

///
/// Output a todo message.
///
#define GN_TODO(msg) GN_DO_ONCE( GN_WARN(GN::getLogger("GN.base.todo"))( "TODO: %s", msg ) );

// *****************************************************************************
/// \name error check macros
// *****************************************************************************

//@{

#define GN_OGL_CHECK_DO_DESC( func, errDesc, something )                    \
    if( true ) {                                                            \
        func;                                                               \
        GLenum err = glGetError();                                          \
        if( GL_NO_ERROR != err )                                            \
        {                                                                   \
            static GN::Logger * sLogger = GN::getLogger("GN.gfx.OGLError"); \
            GN_ERROR(sLogger)( "%s%s!", errDesc,                            \
                (const char*)::gluErrorString(err) );                       \
            GN_UNEXPECTED();                                                \
            something                                                       \
        }                                                                   \
    } else void(0)
//
#define GN_OGL_CHECK_RV_DESC( func, errDesc, retVal ) GN_OGL_CHECK_DO_DESC( func, errDesc, return retVal; )
//
#define GN_OGL_CHECK_R_DESC( func, errDesc ) GN_OGL_CHECK_DO_DESC( func, errDesc, return; )
//
#if GN_ENABLE_DEBUG
#define GN_OGL_CHECK_DESC( func, errDesc ) GN_OGL_CHECK_DO_DESC( func, errDesc, )
#else
#define GN_OGL_CHECK_DESC( func, errDesc ) func
#endif
//
#define GN_OGL_CHECK_DO( X, S ) GN_OGL_CHECK_DO_DESC( X, "", S )
#define GN_OGL_CHECK_RV( X, V ) GN_OGL_CHECK_RV_DESC( X, "", V )
#define GN_OGL_CHECK_R( X )     GN_OGL_CHECK_R_DESC( X, "" )
#define GN_OGL_CHECK( X )       GN_OGL_CHECK_DESC( X, "" )

///
/// check return value of Windows function (general version)
///
#define GN_MSW_CHECK_DO( func, something )                                   \
    if( true ) {                                                             \
        intptr_t rr = (intptr_t)(func);                                        \
        if( 0 == rr )                                                        \
        {                                                                    \
            static GN::Logger * sLogger = GN::getLogger("GN.base.MSWError"); \
            GN_ERROR(sLogger)( ::GN::getWin32LastErrorInfo() );              \
            something                                                        \
        }                                                                    \
    } else void(0)

///
/// check return value of Windows function
///
#if GN_ENABLE_DEBUG
#define GN_MSW_CHECK( func ) GN_MSW_CHECK_DO( func, void(0); )
#else
#define GN_MSW_CHECK( func ) func
#endif

///
/// check return value of Windows function, return if failed
///
#define GN_MSW_CHECK_RETURN_VOID( func ) GN_MSW_CHECK_DO( func, return; )

///
/// check return value of Windows function, return if failed
///
#define GN_MSW_CHECK_RETURN( func, rval ) GN_MSW_CHECK_DO( func, return rval; )

///
/// DX9 error check routine
///
#ifdef D3DCOMPILE_USEVOIDS
#define GN_DX_CHECK_DO( func, something ) func
#else
#define GN_DX_CHECK_DO( func, something )                                   \
    if( true ) {                                                            \
        HRESULT hr = func;                                                  \
        if( FAILED(hr) )                                                    \
        {                                                                   \
            static GN::Logger * sLogger = GN::getLogger("GN.gfx.DXError");  \
            GN_ERROR(sLogger)( GN::getDXErrorInfo(hr) );                    \
            something                                                       \
        }                                                                   \
    } else void(0)
#endif // D3DCOMPILE_USEVOIDS

///
/// DX error check routine
///
#if GN_ENABLE_DEBUG
#define GN_DX_CHECK( func )         GN_DX_CHECK_DO( func, )
#else
#define GN_DX_CHECK( func )         func
#endif

///
/// DX error check routine
///
#define GN_DX_CHECK_RETURN_VOID( func )  GN_DX_CHECK_DO( func, return; )

///
/// DX error check routine
///
#define GN_DX_CHECK_RETURN( func, rval ) GN_DX_CHECK_DO( func, return rval; )

///
/// check return value of XLib function (general version)
///
#define GN_X_CHECK_DO( func, something )                    \
    if( true ) {                                            \
        Status rr = (func);                                 \
        if( 0 == rr )                                       \
        {                                                   \
            static GN::Logger * sLogger = GN::getLogger("GN.gfx.XLibError"); \
            GN_ERROR(sLogger)( "XLib function %s failed: return(0x%X).", #func, rr );  \
            something                                       \
        }                                                   \
    } else void(0)

///
/// check return value of XLib function
///
#if GN_ENABLE_DEBUG
#define GN_X_CHECK( func ) GN_X_CHECK_DO( func, void(0); )
#else
#define GN_X_CHECK( func ) func
#endif

///
/// check return value of XLib function, return if failed
///
#define GN_X_CHECK_RETURN_VOID( func ) GN_X_CHECK_DO( func, return; )

///
/// check return value of XLib function, return if failed
///
#define GN_X_CHECK_RETURN( func, rval ) GN_X_CHECK_DO( func, return rval; )

//@}

namespace GN
{
    // ************************************************************************
    /// \name                       Debug functions
    // ************************************************************************

    //@{

    enum RuntimeAssertBehavior
    {
        RAB_ASK_USER,       ///< Ask user how to respond assert failure. This is default behavior
        RAB_BREAK_ALWAYS,   ///< Always break into debugger.
        RAB_LOG_ONLY,       ///< Ignore assert failure, output log message only.
        RAB_SILENCE,        ///< Silence ignore assert failure. No break, No message.
    };

    ///
    /// Change runtime assert behavior. Default is RAB_ASK_USER.
    ///
    /// \return
    ///     Return old behavior.
    ///
    RuntimeAssertBehavior setRuntimeAssertBehavior( RuntimeAssertBehavior );

    ///
    /// break into debugger ( ASCII version )
    ///
    bool
    assertFunc(
        const char * msg,
        const char * file,
        int          line,
        bool *       ignore ) throw();

	///
	/// Debug break function
	///
	void breakIntoDebugger();

#if GN_MSWIN || GN_XENON

    ///
    /// get OS error info (Windows specific)
    ///
    const char * getWin32ErrorInfo( uint32 win32ErrorCode ) throw();

    ///
    /// get OS error info (Windows specific)
    ///
    const char * getWin32LastErrorInfo() throw();

#endif

    ///
    /// get DX error string
    ///
    const char * getDXErrorInfo( sint32 hr ) throw();

    ///
    /// convert errno value to string
    ///
    const char * errno2str( int );

    //@}
}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_DEBUG_H__
