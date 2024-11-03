/******************************************************************************
 *  File Name:
 *    sim_gpio.cpp
 *
 *  Description:
 *    Mbedutils gpio interface for the simulator
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/gpio_intf.hpp>

namespace mb::hw::gpio::intf
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


  bool init( const mb::hw::gpio::PinConfig &config )
  {
  }


  void write( const mb::hw::gpio::Port_t port, const mb::hw::gpio::Pin_t pin, const mb::hw::gpio::State_t state )
  {
  }


  void toggle( const mb::hw::gpio::Port_t port, const mb::hw::gpio::Pin_t pin )
  {
  }


  mb::hw::gpio::State_t read( const mb::hw::gpio::Port_t port, const mb::hw::gpio::Pin_t pin )
  {
    return State_t::STATE_LOW;
  }


  void setAlternate( const mb::hw::gpio::Port_t port, const mb::hw::gpio::Pin_t pin, const mb::hw::gpio::Alternate_t alternate )
  {
  }


  void setPull( const mb::hw::gpio::Port_t port, const mb::hw::gpio::Pin_t pin, const mb::hw::gpio::Pull_t pull )
  {
  }


  void setDrive( const mb::hw::gpio::Port_t port, const mb::hw::gpio::Pin_t pin, const mb::hw::gpio::Drive_t drive )
  {
  }


  void setSpeed( const mb::hw::gpio::Port_t port, const mb::hw::gpio::Pin_t pin, const mb::hw::gpio::Speed_t speed )
  {
  }


  void setMode( const mb::hw::gpio::Port_t port, const mb::hw::gpio::Pin_t pin, const mb::hw::gpio::Mode_t mode )
  {
  }


  void attachInterrupt( const mb::hw::gpio::Port_t port, const mb::hw::gpio::Pin_t pin, const mb::hw::gpio::Trigger_t trigger,
                        const mb::hw::gpio::Callback_t &callback )
  {
  }


  void detachInterrupt( const mb::hw::gpio::Port_t port, const mb::hw::gpio::Pin_t pin )
  {
  }


  uint32_t getInterruptLine( const mb::hw::gpio::Port_t port, const mb::hw::gpio::Pin_t pin )
  {
    return 0;
  }

}    // namespace mb::hw::gpio::intf
