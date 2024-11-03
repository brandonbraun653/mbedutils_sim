/******************************************************************************
 *  File Name:
 *    sim_spi.cpp
 *
 *  Description:
 *    Mbedutils SPI interface for the simulator
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/spi_intf.hpp>

namespace mb::hw::spi::intf
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void driver_setup()
  {
  }


  void driver_teardown()
  {
  }


  void init( const mb::hw::spi::SpiConfig &config )
  {
  }


  void deinit( const mb::hw::spi::Port_t port )
  {
  }


  int write( const mb::hw::spi::Port_t port, const void *data, const size_t length )
  {
    return length;
  }


  int read( const mb::hw::spi::Port_t port, void *data, const size_t length )
  {
    return length;
  }


  int transfer( const mb::hw::spi::Port_t port, const void *tx, void *rx, const size_t length )
  {
    return length;
  }


  void lock( const mb::hw::spi::Port_t port )
  {
  }


  void unlock( const mb::hw::spi::Port_t port )
  {
  }

}    // namespace mb::hw::spi::intf
