/******************************************************************************
 *  File Name:
 *    sim_system.cpp
 *
 *  Description:
 *    Simulator implementation of the system interface
 *
 *  2025 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/system_intf.hpp>
#include <mbedutils/logging.hpp>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>

namespace mb::system::intf
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void warm_reset()
  {
    /*-------------------------------------------------------------------------
    Get path to our own executable
    -------------------------------------------------------------------------*/
    char    exe_path[ PATH_MAX ];
    ssize_t len = readlink( "/proc/self/exe", exe_path, sizeof( exe_path ) - 1 );
    if( len == -1 )
    {
      LOG_ERROR( "Failed to get executable path" );
      exit( EXIT_FAILURE );
    }

    exe_path[ len ] = '\0';

    /*-------------------------------------------------------------------------
    Create minimal argument array
    -------------------------------------------------------------------------*/
    char *new_argv[] = { exe_path, NULL };

    /*-------------------------------------------------------------------------
    Replace current process
    -------------------------------------------------------------------------*/
    execvp( exe_path, new_argv );
    LOG_ERROR( "Failed to restart: %s", strerror( errno ) );
    exit( EXIT_FAILURE );
  }
}    // namespace mb::system::intf
