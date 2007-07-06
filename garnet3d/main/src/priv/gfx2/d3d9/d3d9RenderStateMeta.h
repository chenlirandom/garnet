GN_D3D9_RENDER_STATE( D3DRS_ZENABLE                    , TRUE )
GN_D3D9_RENDER_STATE( D3DRS_FILLMODE                   , D3DFILL_SOLID )
GN_D3D9_RENDER_STATE( D3DRS_SHADEMODE                  , D3DSHADE_GOURAUD )
GN_D3D9_RENDER_STATE( D3DRS_ZWRITEENABLE               , TRUE )
GN_D3D9_RENDER_STATE( D3DRS_ALPHATESTENABLE            , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_LASTPIXEL                  , TRUE )
GN_D3D9_RENDER_STATE( D3DRS_SRCBLEND                   , D3DBLEND_ONE )
GN_D3D9_RENDER_STATE( D3DRS_DESTBLEND                  , D3DBLEND_ZERO )
GN_D3D9_RENDER_STATE( D3DRS_CULLMODE                   , D3DCULL_CCW )
GN_D3D9_RENDER_STATE( D3DRS_ZFUNC                      , D3DCMP_LESSEQUAL )
GN_D3D9_RENDER_STATE( D3DRS_ALPHAREF                   , 0 )
GN_D3D9_RENDER_STATE( D3DRS_ALPHAFUNC                  , D3DCMP_ALWAYS )
GN_D3D9_RENDER_STATE( D3DRS_DITHERENABLE               , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_ALPHABLENDENABLE           , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_FOGENABLE                  , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_SPECULARENABLE             , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_FOGCOLOR                   , 0 )
GN_D3D9_RENDER_STATE( D3DRS_FOGTABLEMODE               , D3DFOG_NONE )
GN_D3D9_RENDER_STATE( D3DRS_FOGSTART                   , 0 )
GN_D3D9_RENDER_STATE( D3DRS_FOGEND                     , 0x3F800000 ) // 1.0f
GN_D3D9_RENDER_STATE( D3DRS_FOGDENSITY                 , 0x3F800000 ) // 1.0f
GN_D3D9_RENDER_STATE( D3DRS_RANGEFOGENABLE             , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_STENCILENABLE              , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_STENCILFAIL                , D3DSTENCILOP_KEEP )
GN_D3D9_RENDER_STATE( D3DRS_STENCILZFAIL               , D3DSTENCILOP_KEEP )
GN_D3D9_RENDER_STATE( D3DRS_STENCILPASS                , D3DSTENCILOP_KEEP )
GN_D3D9_RENDER_STATE( D3DRS_STENCILFUNC                , D3DCMP_ALWAYS )
GN_D3D9_RENDER_STATE( D3DRS_STENCILREF                 , 0 )
GN_D3D9_RENDER_STATE( D3DRS_STENCILMASK                , 0xFFFFFFFF )
GN_D3D9_RENDER_STATE( D3DRS_STENCILWRITEMASK           , 0xFFFFFFFF )
GN_D3D9_RENDER_STATE( D3DRS_TEXTUREFACTOR              , 0xFFFFFFFF )
GN_D3D9_RENDER_STATE( D3DRS_WRAP0                      , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP1                      , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP2                      , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP3                      , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP4                      , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP5                      , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP6                      , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP7                      , 0 )
GN_D3D9_RENDER_STATE( D3DRS_CLIPPING                   , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_LIGHTING                   , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_AMBIENT                    , 0 )
GN_D3D9_RENDER_STATE( D3DRS_FOGVERTEXMODE              , D3DFOG_NONE )
GN_D3D9_RENDER_STATE( D3DRS_COLORVERTEX                , TRUE )
GN_D3D9_RENDER_STATE( D3DRS_LOCALVIEWER                , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_NORMALIZENORMALS           , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_DIFFUSEMATERIALSOURCE      , D3DMCS_COLOR1 )
GN_D3D9_RENDER_STATE( D3DRS_SPECULARMATERIALSOURCE     , D3DMCS_COLOR2 )
GN_D3D9_RENDER_STATE( D3DRS_AMBIENTMATERIALSOURCE      , D3DMCS_MATERIAL )
GN_D3D9_RENDER_STATE( D3DRS_EMISSIVEMATERIALSOURCE     , D3DMCS_MATERIAL )
GN_D3D9_RENDER_STATE( D3DRS_VERTEXBLEND                , D3DVBF_DISABLE )
GN_D3D9_RENDER_STATE( D3DRS_CLIPPLANEENABLE            , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_POINTSIZE                  , 0 )
GN_D3D9_RENDER_STATE( D3DRS_POINTSIZE_MIN              , 0 )
GN_D3D9_RENDER_STATE( D3DRS_POINTSPRITEENABLE          , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_POINTSCALEENABLE           , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_POINTSCALE_A               , 0x3F800000 ) // 1.0F
GN_D3D9_RENDER_STATE( D3DRS_POINTSCALE_B               , 0 )
GN_D3D9_RENDER_STATE( D3DRS_POINTSCALE_C               , 0 )
GN_D3D9_RENDER_STATE( D3DRS_MULTISAMPLEANTIALIAS       , TRUE )
GN_D3D9_RENDER_STATE( D3DRS_MULTISAMPLEMASK            , 0xFFFFFFFF )
GN_D3D9_RENDER_STATE( D3DRS_PATCHEDGESTYLE             , D3DPATCHEDGE_DISCRETE )
GN_D3D9_RENDER_STATE( D3DRS_DEBUGMONITORTOKEN          , D3DDMT_ENABLE )
GN_D3D9_RENDER_STATE( D3DRS_POINTSIZE_MAX              , 0x42800000 ) // 64.0f
GN_D3D9_RENDER_STATE( D3DRS_INDEXEDVERTEXBLENDENABLE   , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_COLORWRITEENABLE           , 0xF )
GN_D3D9_RENDER_STATE( D3DRS_TWEENFACTOR                , 0 )
GN_D3D9_RENDER_STATE( D3DRS_BLENDOP                    , D3DBLENDOP_ADD )
GN_D3D9_RENDER_STATE( D3DRS_POSITIONDEGREE             , D3DDEGREE_CUBIC )
GN_D3D9_RENDER_STATE( D3DRS_NORMALDEGREE               , D3DDEGREE_LINEAR )
GN_D3D9_RENDER_STATE( D3DRS_SCISSORTESTENABLE          , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_SLOPESCALEDEPTHBIAS        , 0 )
GN_D3D9_RENDER_STATE( D3DRS_ANTIALIASEDLINEENABLE      , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_MINTESSELLATIONLEVEL       , 0x3F800000 ) // 1.0f
GN_D3D9_RENDER_STATE( D3DRS_MAXTESSELLATIONLEVEL       , 0x3F800000 ) // 1.0f
GN_D3D9_RENDER_STATE( D3DRS_ADAPTIVETESS_X             , 0 )
GN_D3D9_RENDER_STATE( D3DRS_ADAPTIVETESS_Y             , 0 )
GN_D3D9_RENDER_STATE( D3DRS_ADAPTIVETESS_Z             , 0 )
GN_D3D9_RENDER_STATE( D3DRS_ADAPTIVETESS_W             , 0 )
GN_D3D9_RENDER_STATE( D3DRS_ENABLEADAPTIVETESSELLATION , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_TWOSIDEDSTENCILMODE        , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_CCW_STENCILFAIL            , D3DSTENCILOP_KEEP )
GN_D3D9_RENDER_STATE( D3DRS_CCW_STENCILZFAIL           , D3DSTENCILOP_KEEP )
GN_D3D9_RENDER_STATE( D3DRS_CCW_STENCILPASS            , D3DSTENCILOP_KEEP )
GN_D3D9_RENDER_STATE( D3DRS_CCW_STENCILFUNC            , D3DCMP_ALWAYS )
GN_D3D9_RENDER_STATE( D3DRS_COLORWRITEENABLE1          , 0xF )
GN_D3D9_RENDER_STATE( D3DRS_COLORWRITEENABLE2          , 0xF )
GN_D3D9_RENDER_STATE( D3DRS_COLORWRITEENABLE3          , 0xF )
GN_D3D9_RENDER_STATE( D3DRS_BLENDFACTOR                , 0xFFFFFFFF )
GN_D3D9_RENDER_STATE( D3DRS_SRGBWRITEENABLE            , 0 )
GN_D3D9_RENDER_STATE( D3DRS_DEPTHBIAS                  , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP8                      , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP9                      , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP10                     , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP11                     , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP12                     , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP13                     , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP14                     , 0 )
GN_D3D9_RENDER_STATE( D3DRS_WRAP15                     , 0 )
GN_D3D9_RENDER_STATE( D3DRS_SEPARATEALPHABLENDENABLE   , FALSE )
GN_D3D9_RENDER_STATE( D3DRS_SRCBLENDALPHA              , D3DBLEND_ONE )
GN_D3D9_RENDER_STATE( D3DRS_DESTBLENDALPHA             , D3DBLEND_ZERO )
GN_D3D9_RENDER_STATE( D3DRS_BLENDOPALPHA               , D3DBLENDOP_ADD )
