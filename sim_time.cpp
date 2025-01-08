/******************************************************************************
 *  File Name:
 *    sim_time.cpp
 *
 *  Description:
 *    STL implementation of the time interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/time_intf.hpp>
#include <chrono>
#include <thread>

namespace mb::time
{
  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  static inline int64_t absolute_system_time_ms()
  {
    return std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
  }

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  int64_t millis()
  {
    static int64_t start_time = absolute_system_time_ms();

    return absolute_system_time_ms() - start_time;
  }


  int64_t micros()
  {
    return millis() * 1000;
  }


  void delayMilliseconds( const size_t val )
  {
    std::this_thread::sleep_for( std::chrono::milliseconds( val ) );
  }


  void delayMicroseconds( const size_t val )
  {
    std::this_thread::sleep_for( std::chrono::microseconds( val ) );
  }

}  // namespace mb::time
