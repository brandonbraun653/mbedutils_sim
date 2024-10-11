/******************************************************************************
 *  File Name:
 *    thread_intf.cpp
 *
 *  Description:
 *    Threading interface implementation using C++ STL primitives.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#include <cstddef>
#include <condition_variable>
#include <mbedutils/thread.hpp>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <vector>

namespace mb::thread::intf
{
  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/
  static std::unordered_map<mb::thread::TaskHandle, std::thread> task_map;
  static std::mutex                                              task_map_mutex;
  static std::condition_variable                                 cv;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    // Initialization logic if needed
  }

  mb::thread::TaskHandle create_task( const mb::thread::TaskConfig &cfg )
  {
    std::lock_guard<std::mutex> lock( task_map_mutex );

    auto task_func = [ cfg ]() {
      if( cfg.func )
      {
        cfg.func( cfg.user_data );
      }
    };

    std::thread            new_thread( task_func );
    mb::thread::TaskHandle handle = reinterpret_cast<mb::thread::TaskHandle>( new_thread.native_handle() );
    task_map[ handle ]            = std::move( new_thread );

    return handle;
  }

  void destroy_task( mb::thread::TaskHandle task )
  {
    std::lock_guard<std::mutex> lock( task_map_mutex );

    auto it = task_map.find( task );
    if( it != task_map.end() )
    {
      if( it->second.joinable() )
      {
        it->second.join();
      }
      task_map.erase( it );
    }
  }

  void set_affinity( mb::thread::TaskHandle task, size_t coreId )
  {
    // Setting thread affinity is platform-specific and not directly supported by the C++ STL.
    // This function can be implemented using platform-specific APIs if needed.
  }

  void start_scheduler()
  {
    // The C++ STL does not have a scheduler like RTOS. Threads start running as soon as they are created.
    // This function can be left empty or used to start any additional management tasks.
  }

  __attribute__( ( weak ) ) void on_stack_overflow()
  {
    throw std::runtime_error( "Stack overflow detected" );
  }

  __attribute__( ( weak ) ) void on_malloc_failed()
  {
    throw std::runtime_error( "Memory allocation failed" );
  }

  __attribute__( ( weak ) ) void on_idle()
  {
    // Default implementation does nothing
  }

  __attribute__( ( weak ) ) void on_tick()
  {
    // Default implementation does nothing
  }

}    // namespace mb::thread::intf