#ifndef __GN_TEST_TESTAPP_H__
#define __GN_TEST_TESTAPP_H__
// *****************************************************************************
/// \file
/// \brief   Common header for test applications
/// \author  chen@@CHENLI-HOMEPC (2009.4.17)
// *****************************************************************************

#include "garnet/GNgfx.h"

class CommandLineArguments
{
public:

    // *************************************************************************
    // public types
    // *************************************************************************

    //@{

    /// indicate command line parsing result
    enum Status
    {
        /// Command line parsing succeed. Continue application execution.
        CONTINUE_EXECUTION,

        /// User is asking for command line help.
        SHOW_HELP,

        /// Common line parsing fail.
        INVALID_COMMAND_LINE,
    };
    //@}

    // *************************************************************************
    // public data
    // *************************************************************************

    //@{

    const char             * applicationName;
    GN::gfx::GpuOptions rendererOptions;
    bool                     useMultiThreadGpu;
    Status                   status;

    // command line arguments that are not recoganized by the parser
    size_t                   extraArgc;
    const char     * const * extraArgv;

    GN::Logger             * logger;

    //@}

    // *************************************************************************
    // public method
    // *************************************************************************

    /// ctor
#if GN_XENON
    // Xenon platform does not have command line arguments
    CommandLineArguments( int, const char *[] )
        : applicationName( "xenonapp" )
        , useMultiThreadGpu( true )
        , status( CONTINUE_EXECUTION )
        , extraArgc(0)
        , extraArgv(NULL)
        , logger(GN::getLogger( "GN.test.CommandLineArguments" ))
    {
    }
#else
    CommandLineArguments( int argc, const char * argv[] )
        : applicationName( argv[0] )
        , useMultiThreadGpu( true )
        , status( INVALID_COMMAND_LINE )
        , extraArgc(0)
        , extraArgv(NULL)
        , logger(GN::getLogger( "GN.test.CommandLineArguments" ))
    {
        status = parseCommandLine( argc, argv );

        if( CONTINUE_EXECUTION == status )
        {
            extraArgc = mExtraArgs.size();
            extraArgv = mExtraArgs.cptr();
        }
    }
#endif

    /// show command line options
    void showStandardCommandLineOptions() const
    {
        GN_INFO(logger)(
            "Standard command line options:\n"
            "\n"
            "   -fs   [on|off]          Full screen rendering. Default is off.\n"
            "\n"
            "   -h\n"
            "   -?                      Show help.\n"
            "\n"
            "   -mt   [on|off]          Use multithread renderer. Default is on.\n"
            "\n"
            "   -rapi [auto|ogl|d3d10]  Choose rendering API. Default is AUTO.\n"
            );
    }

    /// show default help screen, assuming there's no application specific arguments
    void showDefaultHelp() const
    {
        using namespace GN;

        StrA executableName = fs::baseName( applicationName ) + fs::extName( applicationName );

        GN_INFO(logger)( "Usage: %s [options]\n", executableName.cptr() );
        showStandardCommandLineOptions();
    }

    // *************************************************************************
    // private data
    // *************************************************************************

private:

    GN::DynaArray<const char*> mExtraArgs;

    // *************************************************************************
    // private methods
    // *************************************************************************

private:

    /// parse bool argument
    bool parseBool( bool & result, const char * option, const char * value )
    {
        using namespace GN;

        if( 0 == strCmpI( "on", value ) ||
            0 == strCmpI( "yes", value ) ||
            0 == strCmpI( "true", value ) ||
            0 == strCmpI( "1", value ) )
        {
            result = true;
            return true;
        }
        else if(
            0 == strCmpI( "off", value ) ||
            0 == strCmpI( "no", value ) ||
            0 == strCmpI( "false", value ) ||
            0 == strCmpI( "0", value ) )
        {
            result = false;
            return true;
        }
        else
        {
            GN_ERROR(logger)(
                "Invalid boolean argument value (%s) for option %s",
                value, option );
            return false;
        }
    }

    bool parseGpuAPI( GN::gfx::GpuAPI & result, const char * value )
    {
        using namespace GN;
        using namespace GN::gfx;

        if( 0 == strCmpI( "auto", value ) )
        {
            result = GpuAPI::AUTO;
        }
        else if( 0 == strCmpI( "ogl", value ) )
        {
            result = GpuAPI::OGL;
        }
        else if( 0 == strCmpI( "d3d10", value ) )
        {
            result = GpuAPI::D3D10;
        }
        else if( 0 == strCmpI( "d3d11", value ) )
        {
            result = GpuAPI::D3D11;
        }
        else
        {
            GN_ERROR(logger)( "invalid renderer API: %s", value );
            return false;
        }

        return true;
    }

    const char * getOptionValue( int argc, const char * argv[], int & i )
    {
        if( i+1 == argc || '-' == *argv[i+1] )
        {
            GN_ERROR(logger)( "Argument value of option %s is missing.", argv[i] );
            return NULL;
        }

        return argv[++i];
    }

    Status parseCommandLine( int argc, const char * argv[] )
    {
        using namespace GN;

        for( int i = 1; i < argc; ++i )
        {
            const char * a = argv[i];

            if( 0 == strCmpI( "-h", a ) ||
                0 == strCmpI( "/h", a ) ||
                0 == strCmpI( "-?", a ) ||
                0 == strCmpI( "/?", a ) ||
                0 == strCmpI( "--help", a ) )
            {
                return SHOW_HELP;
            }
            else if( '-' == *a
                #if GN_MSWIN
                || '/' == *a
                #endif
                )
            {
                // this is a command line option name

                if( 0 == strCmpI( "fs", a+1 ) )
                {
                    const char * value = getOptionValue( argc, argv, i );
                    if( NULL == value ) return INVALID_COMMAND_LINE;

                    if( !parseBool( rendererOptions.fullscreen, a, value ) )
                        return INVALID_COMMAND_LINE;
                }
                else if( 0 == strCmpI( "mt", a+1 ) )
                {
                    const char * value = getOptionValue( argc, argv, i );
                    if( NULL == value ) return INVALID_COMMAND_LINE;

                    if( !parseBool( useMultiThreadGpu, a, value ) )
                        return INVALID_COMMAND_LINE;
                }
                else if( 0 == strCmpI( "rapi", a+1 ) )
                {
                    const char * value = getOptionValue( argc, argv, i );
                    if( NULL == value ) return INVALID_COMMAND_LINE;

                    if( !parseGpuAPI( rendererOptions.api, value ) )
                        return INVALID_COMMAND_LINE;
                }
                else
                {
                    // this is an extra option
                    mExtraArgs.append( a );
                }
            }
            else
            {
                // this is an extra argument
                mExtraArgs.append( a );
            }
        }

        // done
        return CONTINUE_EXECUTION;
    }
};

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_TEST_TESTAPP_H__
