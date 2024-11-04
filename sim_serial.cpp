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
#include <iostream>
#include <mbedutils/assert.hpp>
#include <mbedutils/interfaces/serial_intf.hpp>
#include <mutex>
#include <unordered_map>
#include "sim_io_pipe.hpp"

namespace mb::hw::serial::sim
{
  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  struct SerialChannel
  {
    std::unique_ptr<std::recursive_timed_mutex>     lock;
    std::unique_ptr<mb::hw::sim::BidirectionalPipe> pipe;
    mb::hw::serial::intf::RXCompleteCallback        rx_callback;
    mb::hw::serial::intf::TXCompleteCallback        tx_callback;
  };

  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  static std::recursive_mutex                                       s_channel_impl_mtx;
  static std::unordered_map<size_t, std::unique_ptr<SerialChannel>> s_channel_impl;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void configure( const size_t channel, const std::string &endpoint, const bool bind = true )
  {
    /*-------------------------------------------------------------------------
    Ensure the channel is not already configured
    -------------------------------------------------------------------------*/
    if( s_channel_impl.find( channel ) != s_channel_impl.end() )
    {
      throw std::runtime_error( "Channel already configured" );
    }

    /*-------------------------------------------------------------------------
    Create the new pipe
    -------------------------------------------------------------------------*/
    std::lock_guard lock( s_channel_impl_mtx );
    auto            new_channel = std::make_unique<SerialChannel>( SerialChannel{
        std::make_unique<std::recursive_timed_mutex>(), std::make_unique<mb::hw::sim::BidirectionalPipe>( endpoint, bind ) } );

    /*-------------------------------------------------------------------------
    Start the pipe
    -------------------------------------------------------------------------*/
    s_channel_impl[ channel ] = std::move( new_channel );
    mbed_assert( s_channel_impl[ channel ]->pipe->start() );
  }
}    // namespace mb::hw::serial::sim

namespace mb::hw::serial::intf
{
  using namespace mb::hw::serial::sim;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  bool lock( const size_t channel, const size_t timeout )
  {
    std::lock_guard lock( s_channel_impl_mtx );
    if( s_channel_impl.find( channel ) == s_channel_impl.end() )
    {
      return false;
    }

    return s_channel_impl[ channel ]->lock->try_lock_for( std::chrono::milliseconds( timeout ) );
  }


  void unlock( const size_t channel )
  {
    std::lock_guard lock( s_channel_impl_mtx );
    if( s_channel_impl.find( channel ) == s_channel_impl.end() )
    {
      return;
    }

    s_channel_impl[ channel ]->lock->unlock();
  }


  void flush( const size_t channel )
  {
    // Nothing to do here
  }


  void disable_interrupts( const size_t channel )
  {
    // Nothing to do here
  }


  void enable_interrupts( const size_t channel )
  {
    // Nothing to do here
  }


  int write_async( const size_t channel, const void *data, const size_t length )
  {
    /*-------------------------------------------------------------------------
    Ensure the input channel is valid
    -------------------------------------------------------------------------*/
    std::lock_guard lock( s_channel_impl_mtx );
    if( s_channel_impl.find( channel ) == s_channel_impl.end() )
    {
      return -1;
    }

    /*-------------------------------------------------------------------------
    Write the data to the pipe
    -------------------------------------------------------------------------*/
    s_channel_impl[ channel ]->pipe->write( std::vector<uint8_t>( ( const uint8_t * )data, ( const uint8_t * )data + length ) );

    /*-------------------------------------------------------------------------
    Invoke the user callback if it exists
    -------------------------------------------------------------------------*/
    if( s_channel_impl[ channel ]->tx_callback )
    {
      s_channel_impl[ channel ]->tx_callback( channel, length );
    }

    return length;
  }


  void on_tx_complete( const size_t channel, mb::hw::serial::intf::TXCompleteCallback callback )
  {
    std::lock_guard lock( s_channel_impl_mtx );
    if( s_channel_impl.find( channel ) == s_channel_impl.end() )
    {
      return;
    }

    s_channel_impl[ channel ]->tx_callback = callback;
  }


  void write_abort( const size_t channel )
  {
    // Nothing to do here
  }


  int read_async( const size_t channel, void *data, const size_t length, const size_t timeout )
  {
    /*-------------------------------------------------------------------------
    Ensure the input channel is valid
    -------------------------------------------------------------------------*/
    std::lock_guard lock( s_channel_impl_mtx );
    if( s_channel_impl.find( channel ) == s_channel_impl.end() )
    {
      return -1;
    }

    /*-------------------------------------------------------------------------
    Read the data from the pipe as a lambda callback
    -------------------------------------------------------------------------*/
    auto user_rx_callback = s_channel_impl[ channel ]->rx_callback;

    s_channel_impl[ channel ]->pipe->setReceiveCallback(
        [ channel, data, length, user_rx_callback ]( const std::vector<uint8_t> &data_in ) {
          if( data_in.size() > length )
          {
            std::cerr << "Read data length " << data_in.size() << " too large for buffer of size " << length << std::endl;
            return;
          }

          std::copy( data_in.begin(), data_in.end(), ( uint8_t * )data );

          if( user_rx_callback )
          {
            user_rx_callback( channel, data_in.size() );
          }
        } );

    return length;
  }


  void on_rx_complete( const size_t channel, mb::hw::serial::intf::RXCompleteCallback callback )
  {
    std::lock_guard lock( s_channel_impl_mtx );
    if( s_channel_impl.find( channel ) == s_channel_impl.end() )
    {
      return;
    }

    s_channel_impl[ channel ]->rx_callback = callback;
  }


  void read_abort( const size_t channel )
  {
    // Nothing to do here
  }
}    // namespace mb::hw::serial::intf
