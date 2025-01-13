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
#include "sim_queue.hpp"
#include "zmq.hpp"
#include <chrono>
#include <mbedutils/logging.hpp>
#include <iostream>

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
      /*-----------------------------------------------------------------------
      Bind or connect the socket based on the configuration
      -----------------------------------------------------------------------*/
      if( should_bind_ )
      {
        socket_.bind( endpoint_ );
      }
      else
      {
        socket_.connect( endpoint_ );
      }

      /*-----------------------------------------------------------------------
      Configure the socket for better performance
      -----------------------------------------------------------------------*/
      int linger = 0;    // Do not wait for unsent messages to be sent when closing
      socket_.set( zmq::sockopt::linger, linger );

      int sndhwm = 250;    // Set high water mark for outbound messages
      socket_.set( zmq::sockopt::sndhwm, sndhwm );

      int rcvhwm = 250;    // Set high water mark for inbound messages
      socket_.set( zmq::sockopt::rcvhwm, rcvhwm );

      /*-----------------------------------------------------------------------
      Clear out any old messages that may be lingering
      -----------------------------------------------------------------------*/
      zmq::message_t message;
      while( socket_.recv( message, zmq::recv_flags::dontwait ) )
      {
        // Discard the message
      }

      std::vector<uint8_t> data;
      while( send_queue_.pop( data ) )
      {
        // Discard the message
      }

      /*-----------------------------------------------------------------------
      Start the send/receive threads
      -----------------------------------------------------------------------*/
      running_        = true;
      receive_thread_ = std::thread( &BidirectionalPipe::receiveLoop, this );
      send_thread_    = std::thread( &BidirectionalPipe::sendLoop, this );
      return true;
    }
    catch( const zmq::error_t &e )
    {
      std::cerr << endpoint_ << ": Failed to start ZMQ: " << e.what() << std::endl;
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
        zmq::poll( items, 1, std::chrono::milliseconds( 10 ) );

        if( items[ 0 ].revents & ZMQ_POLLIN )
        {
          zmq::message_t message;
          if( socket_.recv( message, zmq::recv_flags::dontwait ) )
          {
            if( receive_callback_ )
            {
              // std::cout << endpoint_ << ": RX " << message.size() << " bytes" << std::endl;
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
          std::cerr << endpoint_ << ": Receive error: " << e.what() << std::endl;
        }
      }
    }
  }


  void BidirectionalPipe::sendLoop()
  {
    while( running_ )
    {
      std::vector<uint8_t> data;
      if( send_queue_.pop( data, std::chrono::milliseconds( 1 ) ) )
      {
        try
        {
          socket_.send( zmq::buffer( data ), zmq::send_flags::dontwait );
          // std::cout << endpoint_ << ": TX " << data.size() << " bytes" << std::endl;
        }
        catch( const zmq::error_t &e )
        {
          std::cerr << endpoint_ << ": Send error: " << e.what() << std::endl;
        }
      }
    }
  }

}    // namespace mb::hw::sim
