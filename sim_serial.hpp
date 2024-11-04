/******************************************************************************
 *  File Name:
 *    sim_serial.hpp
 *
 *  Description:
 *    Simulator specific interface to the serial driver
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef MBEDUTILS_SIM_SERIAL_HPP
#define MBEDUTILS_SIM_SERIAL_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstddef>
#include <string>

namespace mb::hw::serial::sim
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/
  /**
   * @brief Configures the simulator serial interface
   *
   * This constructs a new ZMQ socket and binds/connects it to the provided
   * endpoint. The endpoint should be a valid ZMQ endpoint string.
   *
   * @param channel   Which serial channel to configure
   * @param endpoint  The ZMQ endpoint to connect to
   * @param bind      True if the endpoint should be bound to, false to connect
   */
  void configure( const size_t channel, const std::string &endpoint, const bool bind = true );

}  // namespace mb::hw::serial::sim

#endif  /* !MBEDUTILS_SIM_SERIAL_HPP */
