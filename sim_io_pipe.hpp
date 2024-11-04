/******************************************************************************
 *  File Name:
 *    sim_io_pipe.hpp
 *
 *  Description:
 *    Generic IO pipe interface for the simulator to move data between devices
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef MBEDUTILS_SIM_IO_PIPE_HPP
#define MBEDUTILS_SIM_IO_PIPE_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <zmq.hpp>
#include <vector>
#include <string>
#include <thread>
#include <functional>
#include "sim_queue.hpp"

namespace mb::hw::sim
{
  /*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/
  class BidirectionalPipe
  {
  public:
    using ReceiveCallback = std::function<void( const std::vector<uint8_t> & )>;

    BidirectionalPipe( const std::string &endpoint, bool bind );
    ~BidirectionalPipe();

    bool start();
    void stop();
    void write( const std::vector<uint8_t> &data );
    void setReceiveCallback( ReceiveCallback callback );

  private:
    void receiveLoop();
    void sendLoop();

    std::string                           endpoint_;
    bool                                  should_bind_;
    zmq::context_t                        context_;
    zmq::socket_t                         socket_;
    std::atomic<bool>                     running_{ false };
    std::thread                           receive_thread_;
    std::thread                           send_thread_;
    ThreadSafeQueue<std::vector<uint8_t>> send_queue_;
    ReceiveCallback                       receive_callback_;
  };
}    // namespace mb::hw::sim

#endif /* !MBEDUTILS_SIM_IO_PIPE_HPP */
