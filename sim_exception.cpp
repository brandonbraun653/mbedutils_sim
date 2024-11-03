/******************************************************************************
 *  File Name:
 *    sim_exception.cpp
 *
 *  Description:
 *    Mbedutils exception handling interface for the simulator
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/exception_intf.hpp>

namespace mb::hw::exception::intf
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void driver_setup()
  {
    // Nothing to do here for the simulator
  }

}  // namespace mb::hw::exception::intf
