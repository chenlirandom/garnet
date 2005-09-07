# -*- coding: utf-8 -*-

Import( 'GN_conf GN_targets GN_alias' )

import os, os.path, re, fnmatch

################################################################################
#
# 定义全局变量
#
################################################################################

# 定义编译类型
GN_conf['static'] = ( 'stdbg' == GN_conf['variant'] or 'strel' == GN_conf['variant'] )
GN_conf['debug'] = ('debug' == GN_conf['variant'] or 'stdbg' == GN_conf['variant'])

################################################################################
#
# 定义局部变量
#
################################################################################

root_dir = Dir( os.path.join('tmp', 'scons' ) ).abspath
variant_dir = os.path.join( root_dir, GN_conf['platform'], GN_conf['compiler'], GN_conf['variant'] )
conf_dir = os.path.join( variant_dir, 'conf' )
sig_file = os.path.join( root_dir, GN_conf['platform'], GN_conf['compiler'], '.sconsign.dbm' )
cache_dir = os.path.join( root_dir, 'cache' )

# 创建必要的目录
if not os.path.exists( root_dir ) : os.makedirs( root_dir )
if not os.path.exists( variant_dir ) : os.makedirs( variant_dir )
if not os.path.exists( conf_dir ) : os.makedirs( conf_dir )
if not os.path.exists( cache_dir ) : os.makedirs( cache_dir )

################################################################################
#
# 定义全局函数
#
################################################################################

# 输出提示信息
def GN_info( msg ):
    print '===================================================================='
    print 'INFO : %s'%msg
    print '===================================================================='

# 输出警告信息
def GN_warn( msg ):
    print '===================================================================='
    print 'WARNING : %s'%msg
    print '===================================================================='

# 输出错误信息
def GN_error( msg ):
    print '===================================================================='
    print 'ERROR : %s'%msg
    print '===================================================================='

# 生成从target到base的相对路径
def GN_relpath(target, base):
    """
    Return a relative path to the target from either the current dir or an optional base dir.
    Base can be a directory specified either as absolute or relative to current dir.
    """

    base_list = (os.path.abspath(base)).split(os.sep)
    target_list = (os.path.abspath(target)).split(os.sep)

    # On the windows platform the target may be on a completely different drive from the base.
    if os.name in ['nt','dos','os2'] and base_list[0] <> target_list[0]:
        raise OSError, 'Target is on a different drive to base. Target: '+target_list[0].upper()+', base: '+base_list[0].upper()

    # Starting from the filepath root, work out how much of the filepath is
    # shared by base and target.
    for i in range(min(len(base_list), len(target_list))):
        if base_list[i] <> target_list[i]: break
    else:
        # If we broke out of the loop, i is pointing to the first differing path elements.
        # If we didn't break out of the loop, i is pointing to identical path elements.
        # Increment i so that in all cases it points to the first differing path elements.
        i+=1

    rel_list = [os.pardir] * (len(base_list)-i) + target_list[i:]
    return os.path.join(*rel_list)

# 查找指定目录下的文件
def GN_glob( patterns, dirs = ['.'],
             recursive = False ):
    patterns = Flatten( [patterns] )
    dirs = Flatten( [dirs] )
    files = []
    for dir in dirs:
        root = Dir(dir).srcnode().abspath;
        try:
            for file in os.listdir( root ):
                if os.path.isdir( os.path.join(root,file) ):
                    if recursive and ( not '.svn' == file ) : # ignore subversion directory
                        files = files + GN_glob( patterns, os.path.join(dir,file), recursive )
                else:
                    # Note: ignore precompiled header
                    if not ('pch.cpp' == file or 'stdafx.cpp' == file):
                        for pattern in patterns:
                            if fnmatch.fnmatch(file, pattern):
                                files.append( os.path.join( dir, file ) )
        except WindowsError:
            pass
    return files

# 编译器是否会生成manifest文件
def GN_has_manifest(env): return 'vs8' == GN_conf['compiler'] and not GN_conf['static']

# setup environment for producing PCH and PDB
def GN_setup_PCH_PDB( env, pchstop, pchcpp, pdb ):
    if pdb:
        env['PDB'] = pdb
    if 'PCH' in env['BUILDERS'] and pchcpp:
        env['PCH'] = env.PCH(pchcpp)[0]
        env['PCHSTOP'] = pchstop

# 编译 static object
def GN_build_static_object( env, source=[], pchstop=0, pchcpp=0, pdb=0 ):
    env = env.Copy()
    GN_setup_PCH_PDB( env, pchstop, pchcpp, pdb )
    return env.Object(source)

# 编译 static object list
def GN_build_static_objects( env, sources=[], pchstop=0, pchcpp=0, pdb=0 ):
    env = env.Copy()
    env.Append( CPPDEFINES=['_GN_LIB'] )
    GN_setup_PCH_PDB( env, pchstop, pchcpp, pdb )
    return [env.Object(x) for x in sources]

# 编译 shared object
def GN_build_shared_object( env, source=[], pchstop=0, pchcpp=0, pdb=0 ):
    env = env.Copy()
    GN_setup_PCH_PDB( env, pchstop, pchcpp, pdb )
    if GN_conf['static']:
        return env.Object(source)
    else:
        return env.SharedObject(source)

# 编译 shared object list
def GN_build_shared_objects( env, sources=[], pchstop=0, pchcpp=0, pdb=0 ):
    env = env.Copy()
    GN_setup_PCH_PDB( env, pchstop, pchcpp, pdb )
    if GN_conf['static']:
        return [env.Object(x) for x in sources]
    else:
        return [env.SharedObject(x) for x in sources]

# 编译 static library
def GN_build_static_library( env, target, sources=[],
                             pchstop=0, pchcpp=0,
                             pdb=0,
                             addToTargetList=1 ):
    env = env.Copy()
    env.Append( CPPDEFINES=['_GN_LIB'] )
    if not pdb and target: pdb = target + '.pdb'
    GN_setup_PCH_PDB( env, pchstop, pchcpp, pdb )
    result = env.Library(target,sources)
    if addToTargetList: GN_targets[target] = result
    return result

# 编译 shared library
def GN_build_shared_library( env, target, sources=[],
                             pchstop=0, pchcpp=0,
                             pdb=0,
                             libs=[],
                             addToTargetList=1 ):
    if GN_conf['static']:
        return GN_build_static_library( env, target, sources, pchstop, pchcpp, pdb, addToTargetList )
    else:

        # Add libraries to link list
        def add_libs( env, libs ):
            libs.reverse()
            for lib in libs:
                dir = os.path.dirname(GN_targets[lib][0].abspath)
                if 'GNcoreLib' == lib:
                    env.Prepend( LIBPATH = [dir], LIBS = ['GNcore'] )
                else:
                    env.Prepend( LIBPATH = [dir], LIBS = [lib] )

        env = env.Copy()
        if not pdb and target: pdb = target + '.pdb'
        GN_setup_PCH_PDB( env, pchstop, pchcpp, pdb )
        if 0 == len(libs):
            if 'GNcore' == target: libs = Split('GNbase GNextern')
            else: libs = Split('GNcoreLib GNbase GNextern')
        add_libs( env, libs )
        result = env.SharedLibrary( target, sources )

        # handle manifest file
        if GN_has_manifest(env):
            manifest = File( os.path.join( os.path.dirname(result[0].abspath), '%s.dll.manifest'%target ) )
            env.SideEffect( manifest, result )
            result += [manifest]

        if addToTargetList: GN_targets[target] = result
        return result

# 编译可执行文件
def GN_build_program( env, target, sources=[],
                      pchstop=0, pchcpp=0,
                      pdb=0,
                      libs=[],
                      addToTargetList=1 ):
    env = env.Copy()
    if not pdb and target: pdb = target + '.pdb'
    GN_setup_PCH_PDB( env, pchstop, pchcpp, pdb )
    if 0 == len(libs): libs = Split('GNcoreLib GNbase GNextern')
    result = env.Program( target, sources + [ '#' + GN_targets[x][0].path for x in libs ] )

    # handle manifest file
    if GN_has_manifest(env):
        manifest = File( os.path.join( os.path.dirname(result[0].abspath), '%s.exe.manifest'%target ) )
        env.SideEffect( manifest, result )
        result += [manifest]

    if( addToTargetList ): GN_targets[target] = result
    return result

# register custom builder to scons.
from SCons.Script.SConscript import SConsEnvironment
SConsEnvironment.GN_has_manifest = GN_has_manifest
SConsEnvironment.GN_build_static_object = GN_build_static_object
SConsEnvironment.GN_build_static_objects = GN_build_static_objects
SConsEnvironment.GN_build_shared_object = GN_build_shared_object
SConsEnvironment.GN_build_shared_objects = GN_build_shared_objects
SConsEnvironment.GN_build_static_library = GN_build_static_library
SConsEnvironment.GN_build_shared_library = GN_build_shared_library
SConsEnvironment.GN_build_program = GN_build_program

################################################################################
#
# 定义局部函数
#
################################################################################

# ================
# 创建缺省编译环境
# ================
def default_env( options = None ):

    # create environment instance
    tools = ['default']
    msvs_version = '7.1'
    if 'icl' == GN_conf['compiler'] :
        tools += ['intelc']
    elif 'vs8' == GN_conf['compiler'] :
        msvs_version = '8.0'
    env = Environment(
        tools = tools,
        MSVS_VERSION = msvs_version,
        options = options,
        )

    # setup builder for gcc precompiled header
    if 'g++' == env['CXX']:

        # attach gch builder
        bld = Builder(
            action = '$CXXCOM',
            suffix = '.h.gch' )
        env.Append( BUILDERS={'PCH':bld} )

        # Sets up the PCH dependencies for an object file
        import SCons.Defaults
        def pch_emitter( target, source, env, parent_emitter ):
            parent_emitter( target, source, env )
            if env.has_key('PCH') and env['PCH']:
                env.Depends(target, env['PCH'])
            return (target, source)
        def static_pch_emitter(target,source,env):
            return pch_emitter(target,source,env,SCons.Defaults.StaticObjectEmitter)
        def shared_pch_emitter(target,source,env):
            return pch_emitter(target,source,env,SCons.Defaults.SharedObjectEmitter)
        for suffix in Split('.c .C .cc .cxx .cpp .c++'):
            env['BUILDERS']['StaticObject'].add_emitter( suffix, static_pch_emitter );
            env['BUILDERS']['SharedObject'].add_emitter( suffix, shared_pch_emitter );

    # 定义sconsign文件
    env.SConsignFile( sig_file )

    # 缺省编译选项
    def generate_empty_options() : return { 'common':[],'debug':[],'release':[],'stdbg':[],'strel':[] }
    cppdefines = generate_empty_options()
    cpppath    = generate_empty_options()
    libpath    = generate_empty_options()
    libs       = generate_empty_options()
    ccflags    = generate_empty_options()
    cxxflags   = generate_empty_options()
    linkflags  = generate_empty_options()

    # define profile tag
    if float(GN_conf['enable_profile']):
        cppdefines['common'] = ['GN_ENABLE_PROFILE=1']

    cpppath['common']  = ['#core/pub/sdk/inc/common']
    cpppath['debug']   = ['#core/pub/sdk/inc/debug']
    cpppath['release'] = ['#core/pub/sdk/inc/release']
    cpppath['stdbg']   = ['#core/pub/sdk/inc/stdbg']
    cpppath['strel']   = ['#core/pub/sdk/inc/strel']

    libpath['common']  = ['#core/pub/sdk/lib/%s/common'%env['PLATFORM']]
    libpath['debug']   = ['#core/pub/sdk/lib/%s/debug'%env['PLATFORM']]
    libpath['release'] = ['#core/pub/sdk/lib/%s/release'%env['PLATFORM']]
    libpath['stdbg']   = ['#core/pub/sdk/lib/%s/stdbg'%env['PLATFORM']]
    libpath['strel']   = ['#core/pub/sdk/lib/%s/strel'%env['PLATFORM']]

    # 定制不同平台的编译选项
    if 'win32' == env['PLATFORM']:
        libs['common'] += Split('kernel32 user32 gdi32 shlwapi')
    else:
        cpppath['common'] += Split('/usr/X11R6/include /usr/local/include')
        libpath['common'] += Split('/usr/X11R6/lib /usr/local/lib')
        libs['common']    += Split('X11 glut GL GLU')

    # 定制不同编译模式的编译选项
    cppdefines['debug']   += ['GN_DEBUG=1']
    cppdefines['release'] += ['NDEBUG']
    cppdefines['stdbg']   += ['GN_STATIC=1', 'GN_DEBUG=1']
    cppdefines['strel']   += ['GN_STATIC=1', 'NDEBUG']

    # 定制不同编译器的编译选项
    if 'cl' == env['CC']:
        #
        # To workaround an bug in scons 0.96.1, this variable has to be imported
        # on AMD64 platform, to make msvs link.exe work.
        #
        env['ENV']['USERPROFILE'] = os.environ['USERPROFILE']

        cppdefines['debug']   += Split('_DEBUG')
        cppdefines['stdbg']   += Split('_DEBUG')

        if float(env['MSVS_VERSION']) >= 8.0:
            # Note: disable CRT deprecate warnings by now.
            ccflags['common']   += Split('/W4 /WX /D_CRT_SECURE_NO_DEPRECATE /D_CRT_NONSTDC_NO_DEPRECATE')
            cxxflags['common']  += ['/EHa']
            linkflags['common'] += Split( '/NODEFAULTLIB:libcp.lib' )
        else:
            ccflags['common']   += Split('/W4 /WX')
            cxxflags['common']  += ['/EHs']

        ccflags['debug']   += Split('/MDd /GR')
        ccflags['release'] += Split('/O2 /MD')
        ccflags['stdbg']   += Split('/MTd /GR')
        ccflags['strel']   += Split('/O2 /MT')

        # this is for vtune and magellan to do instrumentation
        linkflags['common']  += ['/FIXED:NO', '/DEBUGTYPE:CV,FIXUP']

        linkflags['release'] += ['/OPT:REF']
        linkflags['strel']   += ['/OPT:REF']

    elif 'icl' == env['CC']:
        ccflags['common']  += Split('/W4 /WX /Qpchi- /Zc:forScope')
        ccflags['debug']   += Split('/MDd /GR /Ge /traceback')
        ccflags['release'] += Split('/O2 /MD')
        ccflags['stdbg']   += Split('/MTd /GR /Ge /traceback')
        ccflags['strel']   += Split('/O2 /MT')

        cxxflags['common'] += ['/EHs']

        linkflags['common'] += ['/FORCE']

        cppdefines['debug']   += Split('_DEBUG')
        cppdefines['stdbg']   += Split('_DEBUG')

        # this is for vtune to do instrumentation
        linkflags['release'] = Split('/FIXED:NO /OPT:REF')
        linkflags['strel']   = Split('/FIXED:NO /OPT:REF')

    elif 'gcc' == env['CC']:
        ccflags['common']  += Split('-Wall -Werror')
        ccflags['debug']   += ['-g']
        ccflags['release'] += ['-O3']
        ccflags['stdbg']   += ['-g']
        ccflags['strel']   += ['-O3']

    else:
        print 'unknown compiler: ' + env['CC']
        Exit(-1)

    def apply_options( env, variant ):
        env.Append(
            CPPDEFINES = cppdefines[variant],
            CPPPATH = cpppath[variant],
            LIBPATH = libpath[variant],
            CCFLAGS = ccflags[variant],
            CXXFLAGS = cxxflags[variant],
            LINKFLAGS = linkflags[variant] );
        env.Prepend(
            LIBS    = libs[variant] ); # for gcc link

    # apply compile options based on current build
    apply_options( env, GN_conf['variant'] )
    apply_options( env, 'common' )

    # end of function default_env()
    return env

# ============
# 读取配置信息
# ============
def read_config( conf, conf_file ):
    try:
        fp = file( conf_file, 'rt' )
        lines = fp.readlines()
        parser = re.compile( '^\s*(\w*)\s*=\s*(.*)$' )
        for x in lines:
            m = parser.match(x)
            if not None == m:
                if 'False' == m.group(2) or '0' == m.group(2):
                    conf[m.group(1)] = False
                elif 'True' == m.group(2) or '1' == m.group(2):
                    conf[m.group(1)] = True
                else:
                    conf[m.group(1)] = m.group(2)
        return True;
    except:
        return False;

# ============
# 存储配置信息
# ============
def write_config( conf, conf_file ):
    try:
        fp = file( conf_file, 'wt' )
        items = conf.items();
        items.sort()
        for x in items:
            fp.write( '%s = %s\n'%(x[0],x[1]) )
        fp.close()
    except:
        GN_error( 'fail to generate config file!' )
        Exit(-1);

# ============
# 检查系统配置
# ============
def check_config( conf, conf_dir ):
    env = default_env()
    # Do NOT treat warning as error
    ccflags = str(env.Dictionary('CCFLAGS'))
    ccflags = ccflags.replace( '/WX', '' )
    ccflags = ccflags.replace( '-Werror', '')
    env.Replace( CCFLAGS = Split( ccflags ) )
    c = env.Configure(
        conf_dir = conf_dir,
        log_file = os.path.join(conf_dir,'config.log'),
        );

    # =================
    # 是否支持Cg shader
    # =================
    conf['has_cg'] = c.CheckCHeader('cg/cg.h')

    # ===========
    # 是否支持D3D
    # ===========
    conf['has_d3d'] = c.CheckCXXHeader( 'd3d9.h' ) and c.CheckCXXHeader( 'd3dx9.h' )

    # =========================
    # 检查是否存在boost library
    # =========================
    conf['has_boost'] = c.CheckCXXHeader( 'boost/any.hpp' )

    # =================
    # 是否支持freetype2
    # =================
    has_ft2 = 0
    ft2_inc_path = ''
    ft2_libs = ''
    if 'win32' == env['PLATFORM']:
        if c.CheckCHeader( Split('ft2build.h freetype/freetype.h') ):
            if 'debug' == GN_conf['variant']:
                if c.CheckLib('ft2MDd','FT_Init_FreeType', autoadd=0): ft2_libs = 'ft2MDd'
            elif 'release' == GN_conf['variant']:
                if c.CheckLib('ft2MD','FT_Init_FreeType', autoadd=0): ft2_libs = 'ft2MD'
            elif 'stdbg' == GN_conf['variant']:
                if c.CheckLib('ft2MTd','FT_Init_FreeType', autoadd=0): ft2_libs = 'ft2MTd'
            elif 'strel' == GN_conf['variant']:
                if c.CheckLib('ft2MT','FT_Init_FreeType', autoadd=0): ft2_libs = 'ft2MT'
            if ft2_libs: has_ft2 = True
    else:
        # config freetype is easy for linux-like system
        e = env.Copy( CPPPATH = [], LIBS = [] )
        e.ParseConfig( 'freetype-config --cflags' )
        e.ParseConfig( 'freetype-config --libs' )
        def list2str( l ):
            s = ''
            for x in l : s += '%s '%str(x)
            return s
        ft2_inc_path = list2str( e['CPPPATH'] )
        ft2_libs = list2str( e['LIBS'] )
        has_ft2 = ( None == os.popen( 'freetype-config --cflags' ).close() )
    conf['has_ft2'] = has_ft2
    conf['ft2_inc_path'] = ft2_inc_path
    conf['ft2_libs'] = ft2_libs

    # =====================
    # 检查 RenderMonkey SDK
    # =====================
    """has_rendermonkey = os.environ.has_key('RENDER_MONKEY_ROOT_PATH')
    if has_rendermonkey:
        rendermonkey_root_path = os.environ['RENDER_MONKEY_ROOT_PATH']
        GN_info( 'Using RenderMonkey installation at "%s".'%rendermonkey_root_path )
    else:
        rendermonkey_root_path = ''
        GN_warn( 'Please specify RenderMonkey location using environment RENDERMONKEY_ROOT_PATH.' )
    conf['has_rendermonkey'] = has_rendermonkey
    conf['rendermonkey_root_path'] = rendermonkey_root_path"""

    # =============
    # finish config
    # =============
    c.Finish();

################################################################################
#
# 配置编译环境
#
################################################################################


conf_file = os.path.join( conf_dir, 'build.conf' )
conf  = {}
if not read_config( conf, conf_file ) or GN_conf['genconf']:
    check_config( conf, conf_dir )
    write_config( conf, conf_file )
GN_conf.update(conf)

################################################################################
#
# 初始化编译环境
#
################################################################################

env = default_env()

env.Prepend(
    CPPPATH = Split('%(root)s/extern/inc %(root)s/priv/inc'%{'root':variant_dir})
    )

if float(GN_conf['enable_cache']): env.CacheDir( cache_dir )

################################################################################
#
# export全局变量
#
################################################################################

GN_func = {
    'info'                  : GN_info,
    'warn'                  : GN_warn,
    'error'                 : GN_error,
    'glob'                  : GN_glob,
    'relpath'               : GN_relpath,
    #'setup_PCH_PDB'         : GN_setup_PCH_PDB,
    }

env.Export(
    'env',
    'GN_conf',
    'GN_alias',
    'GN_targets',
    'GN_func',
    )

################################################################################
#
# build sub-directories
#
################################################################################

SConscript( dirs = [variant_dir], src_dir='core', build_dir=variant_dir, duplicate=0 )
SConscript( dirs = ['bin'] )
if 'cl' == env['CC']: SConscript( dirs = ['msvc'], src_dir='msvc', build_dir=os.path.join(variant_dir,'msvc') )
