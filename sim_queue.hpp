/******************************************************************************
 *  File Name:
 *    sim_queue.hpp
 *
 *  Description:
 *    Various queue implementations for the simulator
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef MBEDUTILS_SIM_QUEUE_HPP
#define MBEDUTILS_SIM_QUEUE_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <queue>
#include <mutex>
#include <condition_variable>

namespace mb::hw::sim
{
  /*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/

  /**
   * @brief  Simple thread safe queue implementation
   *
   * @tparam T   Type of data to store in the queue
   */
  template<typename T>
  class ThreadSafeQueue
  {
  public:
    void push( T item )
    {
      std::lock_guard<std::mutex> lock( mutex_ );
      queue_.push( std::move( item ) );
      cv_.notify_one();
    }

    bool pop( T &item, std::chrono::milliseconds timeout = std::chrono::milliseconds( 100 ) )
    {
      std::unique_lock<std::mutex> lock( mutex_ );
      if( !cv_.wait_for( lock, timeout, [ this ] { return !queue_.empty(); } ) )
      {
        return false;
      }
      item = std::move( queue_.front() );
      queue_.pop();
      return true;
    }

  private:
    std::queue<T>           queue_;
    mutable std::mutex      mutex_;
    std::condition_variable cv_;
  };
}    // namespace mb::hw::sim

#endif /* !MBEDUTILS_SIM_QUEUE_HPP */
