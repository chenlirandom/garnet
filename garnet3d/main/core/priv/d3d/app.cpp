#include "pch.h"
#include "garnet/GNd3d.h"

//
//
// ----------------------------------------------------------------------------
int GN::d3d::App::run()
{
    GN_GUARD_ALWAYS;

    if( !init() ) return -1;

    while( !mD3D.windowClosed() )
    {
        update();
        render();
    }

    GN_UNGUARD_ALWAYS_NO_THROW;
    quit();
    return -1;
}

//
//
// ----------------------------------------------------------------------------
bool GN::d3d::App::init()
{
    GN_GUARD;

    //
    // �����Debug�汾��������CRT���ڴ�й©��⹦��
    //
#if GN_DEBUG
    int tmpDbgFlag;
    tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(tmpDbgFlag);
#endif

    if( !mD3D.init(mInitParams) ) return false;

    return appInit() && devCreate() && devRestore();

    GN_UNGUARD;
}

//
//
// ----------------------------------------------------------------------------
void GN::d3d::App::quit()
{
    GN_GUARD;

    devInvalidate();
    devDestroy();
    appQuit();

    GN_UNGUARD;
}
