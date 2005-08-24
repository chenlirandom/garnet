#include "pch.h"
#include "garnet/GnD3DApp.h"

bool init()
{
    gTexMgr.getResource( "haha.bmp" );
    return true;
}

void quit()
{
}

void render()
{
    gD3D.present();
}

int main()
{
    //
    // �����Debug�汾��������CRT���ڴ�й©��⹦��
    //
#if GN_DEBUG
    int tmpDbgFlag;
    tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(tmpDbgFlag);
#endif

    GN::d3dapp::D3DInitParams d3dip;
    GN::d3dapp::D3D d3d;
    if( !d3d.init(d3dip) ) return -1;

    if( !init() ) { quit(); return -1; }

    // run
    while( !d3d.windowClosed() )
    {
        render();
    }

    quit();

    return 0;
}
