# -*- coding: GB18030 -*-

import sys, os, os.path, re, fnmatch

# try import xenon extension for SCons
try:
	sys.path.append( "env/scons/xenon" )
	import xenon
	CONF_has_xenon_extension = True
except ImportError:
	CONF_has_xenon_extension = False
	print "no xenon extension for scons found."

# enviroment use by local functions
LOCAL_env = Environment( tools=[] )

################################################################################
#
# �������ѡ��
#
################################################################################

# ��ȡ��������
def UTIL_getenv( name, defval = '' ):
	if name in os.environ: return os.environ[name]
	else: return defval

# ��⵱ǰ��OS��CPU����
CONF_os = None
CONF_cpu = None
if 'win32' == LOCAL_env['PLATFORM']:
	CONF_os = 'mswin'
	if 'AMD64' == UTIL_getenv('PROCESSOR_ARCHITECTURE') or 'AMD64' == UTIL_getenv('PROCESSOR_ARCHITEW6432'):
		CONF_cpu = 'x64'
	else:
		CONF_cpu = 'x86'
elif 'cygwin' == LOCAL_env['PLATFORM']:
	CONF_os = 'cygwin'
	CONF_cpu = 'x86'
elif 'posix' == LOCAL_env['PLATFORM']:
	CONF_os = 'posix'
	CONF_cpu = 'x86'
else:
	print 'FATAL: Unknown platform:', LOCAL_env['PLATFORM']
	Exit(-1)

# ������õı������б�
class Compiler:
	def __init__( self, name, os, cpu ):
		self.name = name; # compiler name
		self.os = os; # target OS
		self.cpu = cpu; # target CPU

	def __repr__( self ) : return '%s-%s-%s'%(self.name,self.os,self.cpu)
	def __str__( self ) : return '%s-%s-%s'%(self.name,self.os,self.cpu)

CONF_allCompilers = []
if 'mswin' == CONF_os:
	CONF_allCompilers.append( Compiler('vc100','mswin','x86') )
	CONF_allCompilers.append( Compiler('vc90','mswin','x86') )
	CONF_allCompilers.append( Compiler('icl','mswin','x86') )
	CONF_allCompilers.append( Compiler('vc100','mswin','x64') )
	CONF_allCompilers.append( Compiler('vc90','mswin','x64') )
	CONF_allCompilers.append( Compiler('icl','mswin','x64') )
	CONF_allCompilers.append( Compiler('icl','mswin','ia64') )
	CONF_allCompilers.append( Compiler('mingw','mswin','x86') )
	if CONF_has_xenon_extension:
		if xenon.exists( LOCAL_env ):
			CONF_allCompilers.append( Compiler('xenon','xenon','ppc') )
elif 'cygwin' == CONF_os:
	CONF_allCompilers.append( Compiler('gcc','cygwin','x86') )
else:
	assert( 'posix' == CONF_os )
	CONF_allCompilers.append( Compiler('gcc','posix','x86') )

# Get a compiler for specific target OS and CPU type.
def CONF_getCompiler( os, cpu ):
	for c in CONF_allCompilers:
		if c.os == os and c.cpu == cpu : return c;
	return None;

# �������ģʽ
CONF_allVariants = 'debug profile retail'

# ����ȱʡ��������ѡ��
CONF_defaultCmdArgs = {
	'trace'     : UTIL_getenv('GN_BUILD_TRACE',  '0'),
	'variant'   : UTIL_getenv('GN_BUILD_VARIANT', 'debug'),
	'compiler'  : UTIL_getenv('GN_BUILD_COMPILER', CONF_getCompiler(CONF_os,CONF_cpu).name ),
	'os'        : UTIL_getenv('GN_BUILD_TARGET_OS', CONF_os ),
	'cpu'       : UTIL_getenv('GN_BUILD_TARGET_CPU', CONF_cpu ),
	'cg'        : int(UTIL_getenv('GN_BUILD_ENABLE_CG', 1)), # use Cg by default.
	'xedeploy'  : int(UTIL_getenv('GN_BUILD_XEDEPLOY', 1)), # copy to devkit, default is true.
	'locale'    : UTIL_getenv('GN_BUILD_LOCALE', 'CHS'),
	'sdkroot'   : UTIL_getenv('GN_BUILD_SDK_ROOT', '<svnroot>\\bin\\sdk') # root directory of SDK installation
	}

# �Ƿ��trace
CONF_trace = float( ARGUMENTS.get('trace', CONF_defaultCmdArgs['trace']) )

# �����������
CONF_variant = ARGUMENTS.get('variant', CONF_defaultCmdArgs['variant'] )

# �������������
CONF_compiler = Compiler( ARGUMENTS.get('compiler', CONF_defaultCmdArgs['compiler'] ),
						  ARGUMENTS.get('os', CONF_defaultCmdArgs['os'] ),
						  ARGUMENTS.get('cpu', CONF_defaultCmdArgs['cpu'] ) )

# �Ƿ�֧��Cg����.
CONF_enableCg  = float( ARGUMENTS.get( 'cg', CONF_defaultCmdArgs['cg'] ) )

# copy to devkit
CONF_xedeploy = float( ARGUMENTS.get( 'xedeploy', CONF_defaultCmdArgs['xedeploy'] ) )

# installation directory
CONF_sdkroot = ARGUMENTS.get( 'sdkroot', os.path.join( '#build.bin', 'sdk' ) )

################################################################################
#
# �ֲ��ڱ��ļ��Ĺ��ߺ���
#
################################################################################

# ���������Ϣ
def UTIL_trace( level, msg ):
	level = float(level)
	assert( 0 != level )
	if ( CONF_trace > 0 and level <= CONF_trace ) or ( CONF_trace < 0 and level == -CONF_trace ):
		print 'TRACE(%d) : %s'%(level,msg)

# �����ʾ��Ϣ
def UTIL_info( msg ): print 'INFO : %s'%msg

# ���������Ϣ
def UTIL_warn( msg ):
	#print '===================================================================='
	print 'WARNING : %s'%msg
	#print '===================================================================='

# ���������Ϣ
def UTIL_error( msg ):
	print '===================================================================='
	print 'ERROR : %s'%msg
	print '===================================================================='

# Always run static build.
def UTIL_staticBuild( v ): return True

# get sub directory of a specific compiler and build variant
def UTIL_bldsubdir( compiler, variant ):
	if not isinstance( compiler, Compiler ):
		assert( isinstance(compiler,int) )
		assert( isinstance(variant,int) )
		return ''
	else:
		return compiler.os + '.' + compiler.cpu + '.' + compiler.name + '.' + variant

# get root directory of build directory
def UTIL_buildRoot() :
	return os.path.join( '#build.tmp' )

# get build directory of specific compiler and build variant
def UTIL_buildDir( compiler, variant ) :
	return os.path.join( UTIL_buildRoot(), UTIL_bldsubdir( compiler, variant ) )

#
# Create new build environment
#
def UTIL_newEnv( compiler, variant ):
	return UTIL_newEnvEx( compiler, variant, False )

#
# Create new build environment
#
def UTIL_newEnvEx( compiler, variant, batch ):

	if not isinstance( compiler, Compiler ):
		assert( isinstance(compiler,int) )
		assert( isinstance(variant,int) )
		return Environment( tools=[] )

	if 'xenon' == compiler.name:

		# construct empty environment for xenon platform
		env = Environment(
			tools          = [],
			MSVC_VERSION   = "8.0",
			MSVS8_PLATFORM = 'Xbox 360',
			ENV            = {
			                 	# Required by Xenon C++ compiler version 15.00.8153.
			                 	# Or else the compiler will report error D8037
								'SystemDrive' : UTIL_getenv('SystemDrive'),
								'SystemRoot'  : UTIL_getenv('SystemRoot'),
				}
			)

		xedk = UTIL_getenv('XEDK');

		inc = os.path.join( xedk, "include" ) + ";" + os.path.join( xedk, "include\\sys" );
		lib = os.path.join( xedk, "lib\\xbox" );

		# Import environment variables that are used by C/C++ compiler.
	 	env.PrependENVPath( 'PATH'     , UTIL_getenv('PATH') );
	 	env.PrependENVPath( 'INCLUDE'  , inc );
	 	env.PrependENVPath( 'LIB'      , lib );
	 	env['ENV']['LANG']     = UTIL_getenv('LANG');
	 	env['ENV']['LANGUAGE'] = UTIL_getenv('LANGUAGE');

		# attach xenon build tools to the environment
		xenon.generate( env )

	else:

		# construct build environment for other platforms

		if 'mswin' == CONF_os:
			tools = ['msvc','mslink','mslib','msvs','masm']
		else:
			tools = ['default']

		# determine msvs platform
		msvs_platform = 'x86'
		if 'icl' == compiler.name :
			tools += ['intelc']
			if 'x64' == compiler.cpu :
				icl_abi = 'em64t'
				msvs_platform = 'x64'
		elif 'x64' == compiler.cpu :
			msvs_platform = 'x64'
		elif 'mingw' == compiler.name :
			tools = ['mingw']

		# create new environment
		env = Environment(
			tools          = tools,
			MSVS8_PLATFORM = msvs_platform,
			MSVC_BATCH     = batch,
			)

		# Import environment variables that are used by C/C++ compiler.
	 	env.PrependENVPath( 'PATH'     , UTIL_getenv('PATH') );
	 	env.PrependENVPath( 'INCLUDE'  , UTIL_getenv('INCLUDE') );
	 	env.PrependENVPath( 'LIB'      , UTIL_getenv('LIB') );
	 	env['ENV']['LANG']     = UTIL_getenv('LANG');
	 	env['ENV']['LANGUAGE'] = UTIL_getenv('LANGUAGE');

	# cache implicit dependencies
	env.SetOption( 'implicit_cache', int(UTIL_getenv('SCONS_CACHE_IMPLICIT_DEPS', 1)) );

	# setup default decider
	env.Decider( 'MD5-timestamp' );

	# setup signature file
	env.SConsignFile( File( os.path.join( UTIL_buildRoot(), '.sconsign.dbm' ) ).path )

	# setup builder for gcc precompiled header
	if 'g++' == env['CXX']:

		import SCons.Defaults
		import SCons.Tool

		# attach gch builder
		bld = Builder(
			action = '$CXXCOM',
			suffix = '.h.gch',
			source_scanner = SCons.Tool.SourceFileScanner )
		env.Append( BUILDERS={'PCH':bld} )

		# Sets up the PCH dependencies for an object file
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

	# ȱʡ����ѡ��
	def generate_empty_options() : return { 'neutral':[], 'common':[],'debug':[],'profile':[],'retail':[] }
	cppdefines = generate_empty_options()
	cpppath    = generate_empty_options()
	libpath    = generate_empty_options()
	libs       = generate_empty_options()
	ccflags    = generate_empty_options()
	cxxflags   = generate_empty_options()
	linkflags  = generate_empty_options()

	cpppath['common'] = [
		'src/extern/inc',
		UTIL_buildDir( compiler, variant ) + "/src/priv/inc",
		'src/priv/inc']

	libpath['common'] = ['src/extern/lib/' + compiler.os + '.' + compiler.cpu]

	# ���Ʋ�ͬ����ģʽ�ı���ѡ��
	cppdefines['common']  += ['UNICODE', '_UNICODE']
	cppdefines['debug']   += ['GN_ENABLE_DEBUG=1', 'GN_ENABLE_PROFILING=1']
	cppdefines['profile'] += ['GN_ENABLE_DEBUG=0', 'GN_ENABLE_PROFILING=1', 'NDEBUG']
	cppdefines['retail']  += ['GN_ENABLE_DEBUG=0', 'GN_ENABLE_PROFILING=0', 'NDEBUG']

	# ���Ʋ�ͬƽ̨�ı���ѡ��
	if 'xenon' == compiler.os:
		ccflags['common']     += ['/QVMX128']
		cppdefines['profile'] += ['PROFILE']
		linkflags['profile']  += ['/NODEFAULTLIB:xapilib.lib']
		libs['common']        += Split('xboxkrnl xbdm dxerr9')
		libs['debug']         += Split('xapilibd d3d9d d3dx9d xgraphicsd xnetd xaudiod2 xactd3 vcompd libcmtd')
		libs['profile']       += Split('xapilibi d3d9i d3dx9  xgraphics  xnet  xaudio2  xacti3 vcomp  libcmt')
		libs['retail']        += Split('xapilib  d3d9  d3dx9  xgraphics  xnet  xaudio2  xact3  vcomp  libcmt')
	elif 'mswin' == compiler.os:
		libs['common']        += Split('kernel32 user32 gdi32 shlwapi advapi32 shell32')
	else:
		cpppath['common']     += Split('/usr/X11R6/include /usr/local/include')
		libpath['common']     += Split('/usr/X11R6/lib /usr/local/lib')

	# ���Ʋ�ͬ�������ı���ѡ��
	if 'cl' == env['CC']:
		#
		# To workaround an bug in scons 0.96.1, this variable has to be imported
		# or else, link.exe will report error: cannot open file 'TEMPFILE'
		#
		env['ENV']['USERPROFILE'] = os.environ['USERPROFILE']

		cppdefines['debug'] += ['_DEBUG']

		cxxflags['common']  += ['/EHa']

		ccflags['common']   += ['/W4', '/WX', '/Ot', '/Oi', '/Z7', '/Yd'] # favor speed, enable intrinsic functions.
		ccflags['debug']    += ['/MTd', '/GR', '/RTCscu']
		ccflags['profile']  += ['/MT', '/Ox']
		ccflags['retail']   += ['/MT', '/Ox', '/GL']

		linkflags['common']  += ['/MANIFEST', '/NODEFAULTLIB:libcp.lib', '/FIXED:NO'] # this is for vtune and magellan to do instrumentation
		linkflags['profile'] += ['/OPT:REF']
		linkflags['retail']  += ['/OPT:REF']

	elif 'icl' == env['CC']:
		ccflags['common']  += ['/W3','/WX','/Wcheck','/Qpchi-','/Zc:forScope','/Zi','/debug:full']
		ccflags['debug']   += ['/MTd','/GR','/Ge','/traceback']
		ccflags['profile'] += ['/O2','/MT']
		ccflags['retail']  += ['/O2','/MT']

		cxxflags['common'] += ['/EHs']

		cppdefines['debug'] += ['_DEBUG']

		linkflags['common']  += ['/FIXED:NO', '/DEBUGTYPE:CV,FIXUP'] # this is for vtune and magellan to do instrumentation
		linkflags['profile'] += ['/OPT:REF']
		linkflags['retail']  += ['/OPT:REF']

	elif 'gcc' == env['CC']:
		ccflags['common']  += ['-Wall','-Werror', '-finput-charset=GBK', '-fexec-charset=GBK']
		ccflags['debug']   += ['-g']
		ccflags['profile'] += ['-O3']
		ccflags['retail']  += ['-O3']
		if 'mingw' == compiler.name:
			cppdefines['common'] += ['WINVER=0x500']

	else:
		UTIL_error( 'unknown compiler: %s'%env['CC'] )
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
	apply_options( env, variant )
	apply_options( env, 'common' )

	# end of function default_env()
	return env

#
# Check sysem configuration
#
def UTIL_checkConfig( conf, confDir, compiler, variant ):

	# Create config environment with "batch" mode disabled,
	# since batch mode triggers scons 1.2.0.d20091224 config
	# code bug.
	env = UTIL_newEnvEx( compiler, variant, None )

	# Do NOT treat warning as error
	ccflags = str(env.Dictionary('CCFLAGS'))
	ccflags = ccflags.replace( '/WX', '' )
	ccflags = ccflags.replace( '/W4', '/W3' )
	ccflags = ccflags.replace( '-Werror', '')
	env.Replace( CCFLAGS = Split( ccflags ) )
	c = env.Configure(
		conf_dir = confDir,
		log_file = os.path.join(confDir,'config.log') )

	# =================
	# �Ƿ�֧��Cg shader
	# =================
	if CONF_enableCg :
		conf['has_cg']       = c.CheckLibWithHeader( 'cg', ['Cg/cg.h'], 'C', 'cgCreateContext();' )
		conf['has_cg_ogl']   = c.CheckLibWithHeader( 'cgGL', ['Cg/cg.h','Cg/cgGL.h'], 'C', "cgGLRegisterStates(0);" )
		conf['has_cg_d3d9']  = c.CheckLibWithHeader( 'cgD3D9', ['Cg/cg.h','Cg/cgD3D9.h'], 'C', "cgD3D9SetDevice(0);" )
		conf['has_cg_d3d10'] = c.CheckLibWithHeader( 'cgD3D10', ['Cg/cg.h','Cg/cgD3D10.h'], 'C', "cgD3D10SetDevice(0,0);" )
		conf['has_cg_d3d11'] = c.CheckLibWithHeader( 'cgD3D11', ['Cg/cg.h','Cg/cgD3D11.h'], 'C', "cgD3D11SetDevice(0,0);" )
	else:
		conf['has_cg']       = 0
		conf['has_cg_ogl']   = 0
		conf['has_cg_d3d9']  = 0
		conf['has_cg_d3d10'] = 0
		conf['has_cg_d3d11'] = 0

	# ==============
	# �Ƿ�֧��OpenGL
	# ==============
	if c.CheckLibWithHeader( 'opengl32', ['GL/glew.h'], 'C', 'glVertex3f(0,0,0);' ) and \
		c.CheckLibWithHeader( 'glu32', ['GL/glew.h','GL/glu.h'], 'C', 'gluOrtho2D(0,0,0,0);' ) :
		conf['has_ogl'] = True
	elif c.CheckLibWithHeader( 'GL', ['GL/glew.h'], 'C', 'glVertex3f(0,0,0);' ) and \
		c.CheckLibWithHeader( 'GLU', ['GL/glew.h','GL/glu.h'], 'C', 'gluOrtho2D(0,0,0,0);' ) :
		conf['has_ogl'] = True
	else :
		conf['has_ogl'] = False

	# ============
	# �Ƿ�֧��XTL
	# ============
	conf['has_xtl'] = c.CheckCHeader( 'xtl.h' )

	# =================
	# Misc. D3D headers
	# =================
	conf['has_dxerr']   = c.CheckCXXHeader( ['windows.h','dxerr.h'] )
	conf['has_xnamath'] = c.CheckCXXHeader( ['windows.h','xnamath.h'] ) or c.CheckCXXHeader( ['xtl.h','xnamath.h'] )

	# ============
	# �Ƿ�֧��D3D9
	# ============
	conf['has_d3d9'] = conf['has_xtl'] or ( conf['has_dxerr'] and c.CheckCXXHeader( 'd3d9.h' ) and c.CheckCXXHeader( 'd3dx9.h' ) )

	# =============
	# �Ƿ�֧��D3D10
	# =============

	# Detect Windows Vista and Windows 7
	def isVistaOrWin7( env ):
		if 'win32' != env['PLATFORM']: return False
		stdout = os.popen( "ver" )
		str = stdout.read()
		stdout.close()
		import string
		vista = string.find( str, "6.0" ) >= 0
		win7  = string.find( str, "6.1" ) >= 0
		return  vista or win7

	conf['has_d3d10'] = False #conf['has_dxerr'] and conf['has_d3d9'] and c.CheckCXXHeader( 'd3d10.h' ) and c.CheckCXXHeader( 'd3dx10.h' ) and ( isVistaOrWin7(env) or not UTIL_staticBuild( variant ) )

	# =============
	# �Ƿ�֧��D3D11
	# =============
	conf['has_d3d11'] = conf['has_dxerr'] and conf['has_d3d9'] and c.CheckLibWithHeader( 'd3d11.lib', 'd3d11.h', 'C++' ) and isVistaOrWin7(env);

	# ===================
	# �Ƿ�֧��DirectInput
	# ===================
	conf['has_dinput'] = c.CheckCHeader( ['windows.h', 'dxerr.h', 'dinput.h'] )

	# ==============
	# �Ƿ�֧��XInput
	# ==============
	conf['has_xinput'] = c.CheckCHeader( ['windows.h', 'xinput.h'] ) or c.CheckCHeader( ['xtl.h', 'xinput.h'] )
	conf['has_xinput2'] = c.CheckCHeader( ['windows.h', 'xinput2.h'] ) or c.CheckCHeader( ['xtl.h', 'xinput2.h'] )

	# =========================
	# ����Ƿ����boost library
	# =========================
	conf['has_boost'] = c.CheckCXXHeader( 'boost/any.hpp' )

	# ==========
	# ��� cegui
	# ==========
	conf['has_cegui'] = c.CheckCXXHeader( 'CEGUI.h' )

	# ===========
	# ��� maxsdk
	# ===========
	conf['has_maxsdk'] = c.CheckCHeader( ['max.h','maxapi.h','maxversion.h'] )

	# =========
	# ��� GLUT
	# =========
	if c.CheckLibWithHeader( 'glut', 'GL/glut.h', 'C', 'glutInit(0,0);' ) :
	    conf['has_glut'] = True
	    conf['glut'] = 'glut'
	elif c.CheckLibWithHeader( 'glut32', 'GL/glut.h', 'C', 'glutInit(0,0);' ) :
	    conf['has_glut'] = True
	    conf['glut'] = 'glut32'
	else :
	    conf['has_glut'] = None

	# =====
	# ICONV
	# =====
	if c.CheckLibWithHeader( 'iconv', 'iconv.h', 'c', 'iconv_open(0,0);' ):
	    conf['has_iconv'] = True
	    conf['iconv_lib'] = 'iconv'
	elif c.CheckLibWithHeader( 'libiconv', 'iconv.h', 'c', 'iconv_open(0,0);' ):
	    conf['has_iconv'] = True
	    conf['iconv_lib'] = 'libiconv'
	elif c.CheckLibWithHeader( 'c', 'iconv.h', 'c', 'iconv_open(0,0);' ):
	    conf['has_iconv'] = True
	    conf['iconv_lib'] = 'c'
	else :
		conf['has_iconv'] = None
		conf['iconv_lib'] = None

	# ===
	# FBX
	# ===

	conf['has_fbx'] = False
	FBX_dir = [
		[ '',
		  '' ],
		# TODO: query install directory from Registry
		[ 'C:/Program Files/Autodesk/FBX/FbxSdk/2011.3.1/include/',
		  'C:/Program Files/Autodesk/FBX/FbxSdk/2011.3.1/lib/' ]
		]
	FBX_libs = [
		['fbxsdk_mt2010',       ['wininet']],
		['fbxsdk_mt2010d',      ['wininet']],
		['fbxsdk_mt2010_amd64', ['wininet']],
		['fbxsdk_mt2010_amd64d',['wininet']],
		['fbxsdk_gcc4',  []],
		['fbxsdk_gcc4d', []],
		]
	for d in FBX_dir:
		env.Append(
			CPPPATH = [d[0]],
			LIBPATH = [d[1]],
			)
		for l in FBX_libs:
			env.Append( LIBS = l[1] )
			if (not conf['has_fbx']) and c.CheckLibWithHeader( l[0], ['fbxsdk.h','fbxfilesdk/fbxfilesdk_nsuse.h'], 'C++', 'KFbxSdkManager::Create();' ):
				conf['has_fbx'] = True
				conf['fbx_inc_path']  = d[0]
				conf['fbx_lib_path']  = d[1]
				conf['fbx_libs'] = [l[0],'wininet']
				break;
			for ll in l[1]: env['LIBS'].remove( ll )

		env['CPPPATH'].remove( d[0] )
		env['LIBPATH'].remove( d[1] )

		if conf['has_fbx']: break;

	# =====================
	# Copy to devkit or not
	# =====================
	conf['xedeploy'] = CONF_xedeploy;

	# =============
	# finish config
	# =============
	c.Finish()

#
# Copy to Xenon devkit
#
class UTIL_copy_to_devkit:

	def __init__( self, targetDir ):
		self.targetDir = targetDir

	def mkdir( self, path, env ):
		import popen2
		parent = os.path.dirname( path )
		if len(parent) > 0 :
			self.mkdir( parent, env )
		popen2.popen3( 'xbmkdir %s'%path )

	def __call__( self, target, source, env ):
		for x in target:
			self.mkdir( self.targetDir, env )
			# Note: xbcp.exe requires environment variable "SystemRoot".
			cmdline = 'set SystemRoot=%s&xbcp /Q /D /Y %s %s'%(UTIL_getenv("SystemRoot"),x.abspath,self.targetDir)
			env.Execute( cmdline )
			t = '%s\\%s'%(self.targetDir,os.path.basename(x.path))
		return 0

################################################################################
#
# ���� GarnetEnv class
#
################################################################################

class GarnetEnv :

	def __init__( self ) :
		self.compiler = None
		self.variant = None
		self.conf = {}

	# UTIL functions
	def buildDir( self, compiler, variant ) : return UTIL_buildDir( compiler, variant )
	def newEnv( self, compiler, variant ) : return UTIL_newEnv( compiler, variant )
	def newEnvEx( self, compiler, variant, batch ) : return UTIL_newEnvEx( compiler, variant, batch )
	def trace( self, level, msg ): UTIL_trace( level, msg )
	def info( self, msg ): UTIL_info( msg )
	def warn( self, msg ): UTIL_warn( msg )
	def error( self, msg ): UTIL_error( msg )
	def copy_to_devkit( self, targetDir ) : return UTIL_copy_to_devkit( targetDir )
	def getenv( self, name, defval=None ) : return UTIL_getenv( name, defval )

	# ���ɴ�target��base�����·��
	def relpath( self, target, base ):
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

	# ����ָ��Ŀ¼�µ��ļ�
	def glob( self, patterns, recursive = False ):
		def do_glob( pattern, dir, recursive ):
			files = []
			root = Dir(dir).srcnode().abspath;
			try:
				for file in os.listdir( root ):
					if os.path.isdir( os.path.join(root,file) ):
						if recursive and ( not '.svn' == file ) : # ignore subversion directory
							files = files + do_glob( pattern, os.path.join(dir,file), recursive )
					else:
						# Note: ignore precompiled header
						if not ('pch.cpp' == file or 'stdafx.cpp' == file):
							#print 'fnmatch(%s,%s) = %s'%(file,pattern,fnmatch.fnmatch(file, pattern))
							if fnmatch.fnmatch(file, pattern):
								files.append( os.path.join( dir, file ) )
			except WindowsError:
				pass
			return files
		#print 'glob %s'%patterns )
		files = []
		if not patterns is list: patterns = [patterns]
		for p in Flatten(patterns):
			if os.path.isdir( GetBuildPath(p) ):
				#print '    do_glob(*.*,%s)'%p
				files += do_glob( '*.*', p, recursive )
			else:
				(dir,pattern) = os.path.split(p);
				if '' == pattern: pattern = '*.*';
				if '' == dir: dir = '.';
				#print '    do_glob(%s,%s)'%(pattern,dir)
				files += do_glob( pattern, dir, recursive )
		return files

	# ���� source cluster
	def newSourceCluster( self, sources, pchHeader = None, pchSource = None ):
		s = SourceCluster()
		s.sources = [File(x) for x in Flatten(sources)]
		if pchHeader: s.pchHeader = pchHeader
		if pchSource: s.pchSource = File(pchSource)
		return s

	# ���� custom source cluster
	def newCustomSourceCluster( self, sources, action ):
		s = SourceCluster()
		s.sources = [File(x) for x in sources]
		s.action = action
		return s

	# ���� Target
	def newTarget( self, type, name, sources, dependencies = [], ignoreDefaultDependencies = False, pdb = None ):
		# create new target instance
		t = Target()
		if 'shlib' == type:
			if UTIL_staticBuild( self.variant ) : type = 'stlib'
			else : type = 'dylib'
		t.compiler = self.compiler
		t.variant = self.variant
		t.type = type
		t.path = Dir('.')
		for s in sources : t.addCluster( s )
		t.dependencies = dependencies
		t.ignoreDefaultDependencies = ignoreDefaultDependencies
		if pdb : t.pdb = File(pdb)
		else   : t.pdb = File("%s.pdb"%(name))
		ALL_targets[self.compiler][self.variant][name] = t # insert to global target list
		return t

	# ���� custom compiler and platform _DEPENDANT_ target
	def newCustomTarget( self, name, sources ):

		# create new target instance
		t = Target()
		t.compiler = self.compiler
		t.variant = self.variant
		t.type = 'custom'
		t.path = Dir('.')
		for s in sources : t.addCluster( s )
		ALL_targets[self.compiler][self.variant][name] = t # insert to global target list
		return t

	# ���� compiler _INDEPENDANT_ target.
	def newNeutralCustomTarget( self, name, sources ):

		# check for redundant target
		if name in ALL_targets[0][0] : return ALL_targets[0][0]

		# create new target instance
		t = Target()
		t.compiler = self.compiler
		t.variant = self.variant
		t.type = 'custom'
		t.path = Dir('.')
		for s in sources : t.addCluster( s )
		ALL_targets[0][0][name] = t # insert to global target list
		return t

# Create garnet build environment (singleton)
GN = GarnetEnv()

################################################################################
#
# ���� build target ��
#
################################################################################

class CompileFlags:

	def __init__(self):
		self.cpppath = []
		self.cppdefines = []
		self.ccflags = []
		self.cxxflags = []

class LinkFlags:

	def __init__(self):
		self.libpath = []
		self.linkflags = []

class SourceCluster:

	def __init__(self):
		self.sources = []
		self.pchHeader = None
		self.pchSource = None
		self.extraCompileFlags = CompileFlags()
		self.removedCompileFlags = CompileFlags()
		self.externalDependencies = []
		self.action = None # custom action object, only used for 'custom' type target.
						   # Should accept a enviroment and a list of sources as parameter,
						   # and return list of targets.

	def addCompileFlags( self,
						 CPPPATH = None,
						 CPPDEFINES = None,
						 CCFLAGS = None,
						 CXXFLAGS = None ):
		if CPPPATH :
			for x in CPPPATH:
				p1 = Dir(x).path
				p2 = Dir(x).srcnode().path
				self.extraCompileFlags.cpppath.append( p1 )
				if p2 != p1: self.extraCompileFlags.cpppath.append( p2 )
		if CPPDEFINES : self.extraCompileFlags.cppdefines += CPPDEFINES
		if CCFLAGS : self.extraCompileFlags.ccflags += CCFLAGS
		if CXXFLAGS : self.extraCompileFlags.cxxflags += CXXFLAGS

	def removeCompileFlags( self,
							CPPPATH = None,
							CPPDEFINES = None,
							CCFLAGS = None,
							CXXFLAGS = None ):
		if CPPPATH : self.removedCompileFlags.cpppath += CPPPATH
		if CPPDEFINES : self.removedCompileFlags.cppdefines += CPPDEFINES
		if CCFLAGS : self.removedCompileFlags.ccflags += CCFLAGS
		if CXXFLAGS : self.removedCompileFlags.cxxflags += CXXFLAGS

class Target:

	def __init__(self):
		self.compiler = None
		self.variant = None
		self.type = None # could be 'stlib, dylib, shlib, prog, custom'
		self.path = None
		self.targets = []
		self.sources = [] # list of source clusters
		self.dependencies = [] # list of denpendencies. item in this list must be valid target name.
		self.externalDependencies = [] # list of external dependencies. Could be any name.
		self.ignoreDefaultDependencies = False
		self.pdb = None
		self.extraLinkFlags = LinkFlags()
		self.removedLinkFlags = LinkFlags()

	def addCluster( self, c ):
		self.sources.append( c )
		self.externalDependencies += c.externalDependencies

	def addLinkFlags( self, LIBPATH = None, LINKFLAGS = None ):
		if LIBPATH : self.extraLinkFlags.libpath += LIBPATH
		if LINKFLAGS : self.extraLinkFlags.linkflags += LINKFLAGS

	def removeLinkFlags( self, LIBPATH = None, LINKFLAGS = None ):
		if LIBPATH : self.removedLinkFlags.libpath += LIBPATH
		if LINKFLAGS : self.removedLinkFlags.linkflags += LINKFLAGS

################################################################################
#
# Collect configurations
#
################################################################################

def COLLECT_getCompilers( candidate ):
	result = []
	for c in CONF_allCompilers:
		if ( 'all' == candidate.name or c.name == candidate.name ) and \
		   ( 'all' == candidate.os or c.os == candidate.os ) and \
		   ( 'all' == candidate.cpu or c.cpu == candidate.cpu ) : result.append( c )
	if 0 == len(result): print 'ERROR : invalid compiler : %s'%candidate
	return result

COLLECT_compilers = []
COLLECT_variants = []
if 'all' in COMMAND_LINE_TARGETS:
	COLLECT_compilers = CONF_allCompilers
	COLLECT_variants = Split( CONF_allVariants )
else:
	COLLECT_compilers = COLLECT_getCompilers( CONF_compiler )
	if 'all' == CONF_variant:
		COLLECT_variants = Split( CONF_allVariants )
	else:
		COLLECT_variants = Split( CONF_variant )

ALL_conf = []
ALL_targets = { 0 : { 0 : {} } }
for c in COLLECT_compilers:

	ALL_targets[c] = {}

	for v in COLLECT_variants:

		if not v in CONF_allVariants:
			print "ERROR: Ignore invalid variant '%s'"%v
			continue

		conf = {}
		UTIL_checkConfig( conf, os.path.join(UTIL_buildDir(c,v),'config'), c, v )

		ALL_conf.append( [c,v,conf] )

		ALL_targets[c][v] = {}

################################################################################
#
# Call SConscript to generate target instances
#
################################################################################

for c in ALL_conf:
	GN.compiler = c[0]
	GN.variant = c[1]
	GN.conf = c[2]
	SConscript( 'SConscript', exports=['GN'], variant_dir=UTIL_buildDir( GN.compiler, GN.variant ), duplicate=1 )

################################################################################
#
# buill targets
#
################################################################################

BUILD_compiler = None
BUILD_variant = None
BUILD_env = None
BUILD_bldDir = None
BUILD_libDir = None
BUILD_binDir = None

#define all targets

TARGET_stlibs = [
	'GNextern',
	'GNbase',
	'GNgfxUtil',
	'GNgpuCommon',
	'GNengine',
	'GNutil',
	]

TARGET_shlibs = [
    'GNcore',
    'GNgpuOGL',
	'GNgpuXenon',
#	'GNgpuD3D10',
	'GNgpuD3D11',
    ]

TARGET_tests = [
	'GNtestAssimp',
    'GNtestCegui',
    'GNtestD3D10',
    'GNtestEngine',
    'GNtestFbx',
    'GNtestFt2',
    'GNtestGpu',
    'GNtestInput',
    'GNtestPcre',
    'GNtestRenderToTexture',
    'GNtestGpuResDB',
    'GNtestSIMD',
    'GNtestSprite',
    'GNut',
    'GNtestXenonNegativeZRange',
    'GNtestXenonStackTexture',
    'GNtestXenonVertexEndian',
    'GNtestXml',
    ]

TARGET_samples = [
    'GNsampleBezier',
    'GNsampleDolphin',
    'GNsampleRenderToCube',
    'GNsampleRipple',
    'GNsampleShadowMap',
    'GNsampleThickLine',
    ]

TARGET_tools = [
    'GNtoolD3D10DumpViewer',
    'GNtoolD3D9DumpViewer',
    'GNtoolD3D9Wrapper',
    'GNtoolGPUBenchmark',
    'GNtoolMeshConverter',
    'GNtoolMeshViewer',
    'GNtoolOGLInfo',
    ]

TARGET_misc = [
	'GNmedia',
	]

ALL_aliases = {} # empty map

#
# define new build alias
#
def ALIAS_add_default( alias, targets ):
	Alias( alias, targets )
	Default( alias )
	ALL_aliases[alias] = 1

#
# define non-default build alias
#
def ALIAS_add_non_default( alias, targets ):
	Alias( alias, targets )
	ALL_aliases[alias] = 0

#
# Get libarary suffix. Currently, none.
#
def BUILD_getSuffix(): return ""

#
# Create new compile environment for C/C++ compile
#
def BUILD_newCompileEnv( cluster ):
	env = BUILD_env.Clone()

	a = cluster.extraCompileFlags
	env.Append(
		CPPPATH = a.cpppath,
		CPPDEFINES = a.cppdefines,
		CCFLAGS = a.ccflags,
		CXXFLAGS = a.cxxflags )

	def doRemove( env, name, flags ):
		if flags:
			for x in flags:
				if x in env[name]:
					env[name].remove(x)

	a = cluster.removedCompileFlags
	doRemove( env, 'CPPPATH', a.cpppath )
	doRemove( env, 'CPPDEFINES', a.cppdefines )
	doRemove( env, 'CCFLAGS', a.ccflags )
	doRemove( env, 'CXXFLAGS', a.cxxflags )

	return env

#
# Create new link environment
#
def BUILD_newLinkEnv( target ):
	env = BUILD_env.Clone()

	def extname( path ):
		p,e = os.path.splitext( str(path) )
		return e

	if target.pdb:
		p,e = os.path.splitext( target.pdb.abspath )
		env['PDB'] = p+BUILD_getSuffix()+e

	if 'gcc' == env['CC']:
		env.Prepend( LIBPATH = [BUILD_binDir,BUILD_libDir] )
	else:
		env.Prepend( LIBPATH = [BUILD_libDir] )

	a = target.extraLinkFlags
	env.Append(
		LIBPATH = a.libpath,
		LINKFLAGS = a.linkflags )

	def doRemove( env, name, flags ):
		if flags:
			for x in flags:
				if x in env[name]:
					env[name].remove(x)

	a = target.removedLinkFlags
	doRemove( env, 'LIBPATH', a.libpath )
	doRemove( env, 'LINKFLAGS', a.linkflags )

	# success
	return env

#
# setup environment for producing PCH, return PCH object file
#
def BUILD_setupPCH( env, pchHeader, pchSource ):
	if 'PCH' in env['BUILDERS'] and pchSource:
		pch = env.PCH(pchSource)
		env['PCH'] = pch[0]
		env['PCHSTOP'] = pchHeader
		if 'g++' == env['CXX']:
			# need to add ".gch" into include path, to make sure
                        # it is referenced before the original header.
			env.Prepend( CPPPATH=[os.path.dirname(pch[0].path)] )
		if( len(pch) > 1 ):
			return [pch[1]]
		else:
			return []
	else:
		return []

#
# build static objects
#
def BUILD_staticObjs( cluster ):
	env = BUILD_newCompileEnv( cluster )
	pchObj = BUILD_setupPCH( env, cluster.pchHeader, cluster.pchSource )
	return Flatten( [env.Object(x) for x in cluster.sources] + pchObj )

#
# build shared objects
#
def BUILD_sharedObjs( cluster ):
	env = BUILD_newCompileEnv( cluster )
	pchObj = BUILD_setupPCH( env, cluster.pchHeader, cluster.pchSource )
	return Flatten( [env.SharedObject(x) for x in cluster.sources] + pchObj )

#
# build static library
#
def BUILD_staticLib( name, target ):
	objs = []
	for s in target.sources: objs += BUILD_staticObjs( s )
	env = BUILD_newLinkEnv( target )
	libName = '%s%s%s%s'%( env['LIBPREFIX'], name, BUILD_getSuffix(), env['LIBSUFFIX'] )
	target.targets = env.Library( os.path.join(BUILD_libDir,libName), objs )
	ALIAS_add_default( name, target.targets )

#
# handle dependencies
#
def BUILD_toList( x ):
	if x : return Flatten(x)
	else : return []

def BUILD_addLib( env, name, lib, addSuffix ):
	if lib in env['LIBS']:
		return # ignore redundant libraries
	if addSuffix:
		env.Prepend( LIBS = [lib+BUILD_getSuffix()] )
	else:
		env.Prepend( LIBS = [lib] )
	GN.trace( 2, 'Add depends of %s : %s'%(name,lib) )

#
# add external dependencies
#
def BUILD_addExternalDependencies( env, name, deps ):
	for x in deps:
		BUILD_addLib( env, name, x, False )

#
# add dependencies to other garnet components
#
def BUILD_addDependencies( env, name, deps ):
	targets = ALL_targets[BUILD_compiler][BUILD_variant]
	for x in deps:
		if x in targets:
			BUILD_addExternalDependencies( env, name, BUILD_toList(targets[x].externalDependencies) )
			BUILD_addDependencies( env, name, BUILD_toList(targets[x].dependencies) )
			if 'GNcore' == x or 'stlib' == targets[x].type : # here we ignore dynamic libraries other then GNcore.
				BUILD_addLib( env, name, x, True )
		else:
			GN.warn( "Ingore non-exist dependency for target %s: %s"%(name,x) )

#
# handle compiler generated manifest file.
#
def BUILD_handleManifest( env, target ):
	if ( not UTIL_staticBuild( BUILD_variant ) ) and ( 'cl' == env['CC'] or 'icl' == env['CC'] ) :
		manifest = File( '%s.manifest'%target[0] )
		env.SideEffect( manifest, target )
		target += [manifest]

#
# build dynamic linked library
#
def BUILD_dynamicLib( name, target ):
	objs = []
	for s in target.sources: objs += BUILD_sharedObjs( s )

	env = BUILD_newLinkEnv( target )

	stdlibs = []
	if not target.ignoreDefaultDependencies:
		stdlibs += TARGET_stlibs
		# dynamic libraries other than GNcore depend on GNcore.
		if 'GNcore' != name : stdlibs += ['GNcore']

	BUILD_addExternalDependencies( env, name, BUILD_toList(target.externalDependencies) )
	BUILD_addDependencies( env, name, BUILD_toList(target.dependencies) + stdlibs )
	GN.trace( 2, "Depends of %s : %s"%(name,env['LIBS']) )

	# Duplicate all libarieres twice, to walk around GCC link order limitation
	if( 'gcc' == env['CC'] ): env['LIBS'] += env['LIBS']

	# get final share lib prefix
	#
	# Note: scons 1.2.0, on linux system, has a bug that env.SharedLibrary() returns
	# a object with invalid name, if library name is prefixed with variable (starts with '$')
	prefix = env['SHLIBPREFIX']
	while( len(prefix) > 1 and '$' == prefix[0] ):
		prefix = env[prefix[1:]]

	libName = '%s%s%s%s'%(prefix,name,BUILD_getSuffix(),env['SHLIBSUFFIX'])
	lib = env.SharedLibrary( os.path.join(str(target.path),libName), objs )
	BUILD_handleManifest( env, lib )

	def extname( path ):
		p,e = os.path.splitext( str(path) )
		return e

	target.targets = []
	for x in lib:
		e = extname( x )
		if '.lib' == e or '.exp' == e  or '.a' == e :
			d = BUILD_libDir
		else:
			d = BUILD_binDir
		target.targets.append( env.Install( d, x )[0] )

	ALIAS_add_default( name, target.targets )

#
# build executable
#
def BUILD_program( name, target ):
	objs = []
	for s in target.sources: objs += BUILD_staticObjs( s )

	env = BUILD_newLinkEnv( target )

    # setup standard library list
	stdlibs = []
	if not target.ignoreDefaultDependencies:
		stdlibs += TARGET_stlibs + TARGET_shlibs;

	BUILD_addExternalDependencies( env, name, BUILD_toList(target.externalDependencies) )
	BUILD_addDependencies( env, name, stdlibs + BUILD_toList(target.dependencies) )
	GN.trace( 2, "Depends of %s : %s"%(name,env['LIBS']) )

	# Duplicate all libarieres twice, to walk around GCC link order limitation
	if( 'gcc' == env['CC'] ): env['LIBS'] += env['LIBS']

	exeName = '%s%s%s%s'%(env['PROGPREFIX'],name,BUILD_getSuffix(),env['PROGSUFFIX'])
	prog = env.Program( os.path.join(str(target.path),exeName), objs )
	BUILD_handleManifest( env, prog )

	target.targets = env.Install( BUILD_binDir, prog )

	if 'xenon' == BUILD_compiler.name and CONF_xedeploy:
		env.AddPostAction( target.targets[0], UTIL_copy_to_devkit('xe:\\garnet3d\\%s'%BUILD_variant) )

	ALIAS_add_default( name, target.targets )

#
# build custom target
#
def BUILD_custom( name, target ):
	assert( 'custom' == target.type )
	env = BUILD_env.Clone()
	target.targets = []
	for s in target.sources:
		target.targets += s.action( env, s.sources, BUILD_bldDir )
	ALIAS_add_default( name, target.targets )

#
# build all targets
#
for compiler, variants in ALL_targets.iteritems() :
	BUILD_compiler = compiler
	for variant, targets in variants.iteritems():

		BUILD_env = UTIL_newEnv( compiler, variant )

		# Determine Graphics defines and libraries.
		gfxlibs = []
		dbg = ( variant == "debug" )
		if GN.conf['has_d3d9'] :
			if isinstance( compiler, Compiler ) and compiler.os == "mswin":
				gfxlibs += ['d3d9.lib', 'd3dx9d.lib'] if dbg else ['d3d9.lib', 'd3dx9.lib']
		if GN.conf['has_d3d10'] or GN.conf['has_d3d11']:
			gfxlibs += ['dxgi.lib', 'dxguid.lib', 'dxerr.lib']
		if GN.conf['has_d3d10']:
			gfxlibs += ['d3d10.lib', 'd3dx10d.lib'] if dbg else ['d3d10.lib', 'd3dx10.lib']
		if GN.conf['has_d3d11']:
			gfxlibs += ['d3d11.lib', 'd3dx11d.lib'] if dbg else ['d3d11.lib', 'd3dx11.lib']
		BUILD_env.Prepend( LIBS = gfxlibs )

		BUILD_variant = variant
		BUILD_bldDir = UTIL_buildDir( compiler, variant )
		BUILD_libDir = os.path.join( BUILD_bldDir, 'lib' )
		BUILD_binDir = os.path.join( BUILD_bldDir, 'bin' )

		################################################################################
		#
		# Build all targets
		#
		################################################################################

		for name, x in targets.iteritems():
			if 'stlib' == x.type :
				BUILD_staticLib( name, x )
			elif 'dylib' == x.type :
				BUILD_dynamicLib( name, x )
			elif 'prog' == x.type :
				BUILD_program( name, x )
			elif 'custom' == x.type :
				BUILD_custom( name, x )
			else: GN.error( 'Unknown target type for target %s: %s'%(name,x.type) )
			GN.trace( 1, "%s : compiler(%s), variant(%s), type(%s), path(%s), targets(%s)"%(
				name, compiler, variant, x.type, x.path, [str(t) for t in x.targets] ) )

		################################################################################
		#
		# Setup additional dependencies:
		#
		################################################################################

		def getTargets( n ):
			if n in targets : return targets[n].targets
			else : return []

		progs = TARGET_tests + TARGET_samples + TARGET_tools

		# Everything should depend on the external binary target
		for n in ( TARGET_stlibs + TARGET_shlibs + progs ):
			for t in getTargets(n):
				Depends( t, 'GNexternBin' )

		# Make binaries depend on their by-products, such as manifest and PDB, to make sure
		# those files are copied to binary directory, before execution of the binaries.
		for n in ( TARGET_shlibs + progs ):
			t = getTargets(n)
			for x in t[1:] :
				Depends( t[0], x )

		# Make executables depend on shared libraries and media files.
		for pn in progs:
			for pt in getTargets(pn):
				Depends( pt, 'GNmedia' )
				for sn in TARGET_shlibs:
					for st in getTargets(sn):
						Depends( pt, st )

		################################################################################
		#
		# SDK INSTALLATION
		#
		################################################################################

		#define installation alias
		ALIAS_add_default( "install", CONF_sdkroot )

		def installTargets( dstroot, files ):
			dstdir = os.path.join( CONF_sdkroot, dstroot + "." + UTIL_bldsubdir( compiler, variant ) )
			for f in files:
				Install( dstdir, getTargets(f) )

		installTargets( 'bin',   TARGET_shlibs + TARGET_tools + TARGET_samples + ['GNexternBin'] )
		installTargets( 'lib',   TARGET_stlibs + TARGET_shlibs )
		installTargets( 'doc',   ['GNdoc'] )
		Install( os.path.join( CONF_sdkroot, 'inc/garnet/base' ),   getTargets('GNinc') )

		# install media files
		mediaroot = os.path.join( UTIL_buildDir( compiler, variant ), 'media' )
		for src in getTargets('GNmedia'):
			relpath = GN.relpath( src, mediaroot )
			dst     = os.path.join( CONF_sdkroot, "media", relpath )
			InstallAs( dst, src )

################################################################################
#
# Install SDK headers and media files
#
################################################################################

def installHeaders( dstroot, srcroot ):
	if compiler and variant:
		headers = GN.glob( srcroot, True )
		for src in headers:
			relpath = GN.relpath( src, srcroot )
			dst     = os.path.join( CONF_sdkroot, dstroot, relpath )
			InstallAs( dst, src )

installHeaders( 'inc/garnet', '#src/priv/inc/garnet' )
installHeaders( 'inc', '#src/extern/inc' )

################################################################################
#
# ���� help screen
#
################################################################################

def HELP_generateAliasList():
	s = ''
	for a in sorted(ALL_aliases.items()):
		if a[1]: d = '(default)'
		else   : d = '(non defualt)'
 		s += '\n    ' + a[0] + ('%' + str(24-len(a[0])) +'s')%' ' + d
	return s

HELP_opts = Variables()
HELP_opts.Add(
	'trace',
	'Set trace level (GN_BUILD_TRACE).\n'
	'           Specify level  N to enable levels [1..N],\n'
	'           Specify level -N to enable level N only.'
	,
	CONF_defaultCmdArgs['trace'] )
HELP_opts.Add(
	'compiler',
	'Specify compiler. Could be : one of (%s) or "all". (GN_BUILD_COMPILER)'%'...',
	CONF_defaultCmdArgs['compiler'] )
HELP_opts.Add(
	'os',
	'Specify build target OS. Could be : one of (%s) or "all". (GN_BUILD_TARGET_OS)'%'...',
	CONF_defaultCmdArgs['os'] )
HELP_opts.Add(
	'cpu',
	'Specify build target CPU type. Could be : one of (%s) or "all". (GN_BUILD_TARGET_CPU)'%'...',
	CONF_defaultCmdArgs['cpu'] )
HELP_opts.Add(
	'variant',
	'Specify variant. Could be %s or "all". (GN_BUILD_VARIANT)'%CONF_allVariants,
	CONF_defaultCmdArgs['variant'] )
HELP_opts.Add(
	'cg',
	'Support Cg language or not. This flag has no effect, if Cg library is not found. (GN_BUILD_ENABLE_CG)',
	CONF_defaultCmdArgs['cg'] )
HELP_opts.Add(
	'xedeploy',
	'Copy to Xenon devkit. Only effective when building Xenon binaries. (GN_BUILD_XEDEPLOY)',
	CONF_defaultCmdArgs['xedeploy'] )
HELP_opts.Add(
	'sdkroot',
	'Specify SDK installation directory. (GN_BUILD_SDK_ROOT)',
	CONF_defaultCmdArgs['sdkroot'] )

HELP_text = """
Usage:
	scons [options] [target1 target2 ...]

Options:%s

Targets:%s
""" % (
	HELP_opts.GenerateHelpText( Environment( variables = HELP_opts, tools=[] ) ),
	HELP_generateAliasList()
	)
Help( HELP_text )
