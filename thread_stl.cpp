/******************************************************************************
 *  File Name:
 *    thread_intf.cpp
 *
 *  Description:
 *    Threading interface implementation using C++ STL primitives.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#include "mbedutils/drivers/threading/thread.hpp"
#include <cstddef>
#include <condition_variable>
#include <mbedutils/threading.hpp>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <unordered_map>


namespace mb::thread
{
  /*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/

  Task::Task() noexcept : taskId( TASK_ID_INVALID ), taskName( "" ), taskImpl( nullptr ), pimpl( nullptr )
  {
  }


  Task::~Task()
  {

  }


  Task &Task::operator=( Task &&other ) noexcept
  {
    this->taskId   = other.taskId;
    this->taskImpl = other.taskImpl;
    this->pimpl    = other.pimpl;

    return *this;
  }


  void Task::start()
  {
    // check if the task is already started



    pimpl = new STLConfig();
    // Send an event to the task to start it.
  }


  void Task::kill()
  {
    // Send an event to the task to kill it.
  }


  void Task::join()
  {
    // Wait for the task to end (intf?)

    // Lock global mutex, then release the task control block.
  }


  bool Task::joinable()
  {
    return false;
  }


  TaskId Task::id() const
  {
    return taskId;
  }


  TaskName Task::name() const
  {
    return taskName;
  }


  TaskHandle Task::implementation() const
  {
    return taskImpl;
  }
}    // namespace mb::thread


namespace mb::thread::intf
{
  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  struct TaskData
  {
    std::thread              thread;
    std::condition_variable  cv;
    std::mutex               mutex;
    mb::thread::Task::Config cfg;
  };

  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/
  static std::unordered_map<TaskHandle, TaskId> root_map;
  static std::unordered_map<TaskId, TaskData> task_map;
  static std::mutex                                           task_map_mutex;
  static std::condition_variable                              cv;

  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Surrogate function to execute the user task.
   *
   * This allows us to mimic most RTOS behavior by having the task wait until
   * it is signaled to start.
   *
   * @param id Which task to execute
   */
  static void task_func( const mb::thread::TaskId id )
  {
    /*-------------------------------------------------------------------------
    Wait until this particular task configuration has made it into the map.
    -------------------------------------------------------------------------*/
    {
      std::unique_lock<std::mutex> lock( task_map_mutex );
      while( task_map.find( id ) == task_map.end() )
      {
        cv.wait( lock );
      }
    }

    /*-------------------------------------------------------------------------
    Wait for the signal to start
    -------------------------------------------------------------------------*/
    auto &task_data = task_map[ id ];

    std::unique_lock<std::mutex> lock2( task_data.mutex );
    task_data.cv.wait( lock2 );

    /*-------------------------------------------------------------------------
    Execute the task function
    -------------------------------------------------------------------------*/
    task_data.cfg.func( task_data.cfg.user_data );

  }

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void driver_setup()
  {
  }


  void driver_teardown()
  {
  }


  mb::thread::TaskHandle create_task( const mb::thread::Task::Config &cfg )
  {
    std::lock_guard<std::mutex> lock( task_map_mutex );

    auto task_func = [ cfg ]() {
      if( cfg.func )
      {
        cfg.func( cfg.user_data );
      }
    };

    std::thread
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
    std::lock_guard<std::mutex> lock( task_map_mutex );

    /*-------------------------------------------------------------------------
    Notify all tasks to start. This is assuming that the tasks are waiting on
    their respective condition variable.
    -------------------------------------------------------------------------*/
    for( auto &task : task_map )
    {
      std::unique_lock<std::mutex> lock( task.second.mutex );
      task.second.cv.notify_one();
    }
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
