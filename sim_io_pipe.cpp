/******************************************************************************
 *  File Name:
 *    sim_io_pipe.cpp
 *
 *  Description:
 *    Generic IO pipe interface for the simulator to move data between devices
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "sim_io_pipe.hpp"
#include "mbedutils/drivers/logging/logging_macros.hpp"
#include "sim_queue.hpp"
#include <iostream>
#include <mbedutils/logging.hpp>

namespace mb::hw::sim
{
  /*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/

  BidirectionalPipe::BidirectionalPipe( const std::string &endpoint, bool bind ) :
      endpoint_( endpoint ), should_bind_( bind ), context_( 1 ), socket_( context_, zmq::socket_type::pair )
  {
  }


  BidirectionalPipe::~BidirectionalPipe()
  {
    stop();
  }


  bool BidirectionalPipe::start()
  {
    try
    {
      if( should_bind_ )
      {
        socket_.bind( endpoint_ );
      }
      else
      {
        socket_.connect( endpoint_ );
      }

      running_        = true;
      receive_thread_ = std::thread( &BidirectionalPipe::receiveLoop, this );
      send_thread_    = std::thread( &BidirectionalPipe::sendLoop, this );

      std::cout << "ZMQ pipe started: " << endpoint_.c_str() << std::endl;

      return true;
    }
    catch( const zmq::error_t &e )
    {
      std::cerr << "Failed to start ZMQ: " << e.what() << std::endl;
      return false;
    }
  }


  void BidirectionalPipe::stop()
  {
    running_ = false;

    if( receive_thread_.joinable() )
    {
      receive_thread_.join();
    }

    if( send_thread_.joinable() )
    {
      send_thread_.join();
    }

    socket_.close();
    context_.close();
  }


  void BidirectionalPipe::write( const std::vector<uint8_t> &data )
  {
    std::cout << "Writing data to pipe: " << data.size() << " bytes" << std::endl;
    send_queue_.push( data );
  }


  void BidirectionalPipe::setReceiveCallback( ReceiveCallback callback )
  {
    receive_callback_ = std::move( callback );
  }


  void BidirectionalPipe::receiveLoop()
  {
    while( running_ )
    {
      try
      {
        zmq::pollitem_t items[] = { { socket_, 0, ZMQ_POLLIN, 0 } };

        // Poll with timeout to allow checking running_ flag
        zmq::poll( items, 1, std::chrono::milliseconds( 100 ) );

        if( items[ 0 ].revents & ZMQ_POLLIN )
        {
          zmq::message_t message;
          if( socket_.recv( message ) )
          {
            std::cout << "Received data from pipe: " << message.size() << " bytes" << std::endl;
            if( receive_callback_ )
            {
              std::vector<uint8_t> data( static_cast<uint8_t *>( message.data() ),
                                         static_cast<uint8_t *>( message.data() ) + message.size() );
              receive_callback_( data );
            }
          }
        }
      }
      catch( const zmq::error_t &e )
      {
        if( running_ )
        {
          std::cerr << "Receive error: " << e.what() << std::endl;
        }
      }
    }
  }


  void BidirectionalPipe::sendLoop()
  {
    while( running_ )
    {
      std::vector<uint8_t> data;
      if( send_queue_.pop( data ) )
      {
        try
        {
          socket_.send( zmq::buffer( data ) );
        }
        catch( const zmq::error_t &e )
        {
          std::cerr << "Send error: " << e.what() << std::endl;
        }
      }
    }
  }

}    // namespace mb::hw::sim
