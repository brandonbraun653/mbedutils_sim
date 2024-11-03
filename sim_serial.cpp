/******************************************************************************
 *  File Name:
 *    sim_serial.cpp
 *
 *  Description:
 *    Mbedutils serial interface for the simulator
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/serial_intf.hpp>

namespace mb::hw::serial::intf
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  bool lock( const size_t channel, const size_t timeout )
  {
    return true;
  }


  void unlock( const size_t channel )
  {
  }


  void flush( const size_t channel )
  {
  }


  void disable_interrupts( const size_t channel )
  {
  }


  void enable_interrupts( const size_t channel )
  {
  }


  int write_async( const size_t channel, const void *data, const size_t length )
  {
    return length;
  }


  void on_tx_complete( const size_t channel, mb::hw::serial::intf::TXCompleteCallback callback )
  {
  }


  void write_abort( const size_t channel )
  {
  }


  int read_async( const size_t channel, void *data, const size_t length, const size_t timeout )
  {
    return length;
  }


  void on_rx_complete( const size_t channel, mb::hw::serial::intf::RXCompleteCallback callback )
  {
  }


  void read_abort( const size_t channel )
  {
  }
}    // namespace mb::hw::serial::intf
