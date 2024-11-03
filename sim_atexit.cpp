/******************************************************************************
 *  File Name:
 *    sim_atexit.cpp
 *
 *  Description:
 *    Mbedutils atexit interface for the simulator
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/drivers/system/atexit.hpp>

namespace mb::system::atexit
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
  }


  bool registerCallback( mb::system::atexit::Callback &callback, uint32_t priority )
  {
    return true;
  }


  bool unregisterCallback( mb::system::atexit::Callback &callback )
  {
    return true;
  }


  void exit()
  {
  }

}    // namespace mb::system::atexit
