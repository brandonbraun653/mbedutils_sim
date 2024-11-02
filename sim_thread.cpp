/******************************************************************************
 *  File Name:
 *    thread_intf.cpp
 *
 *  Description:
 *    Threading interface implementation using C++ STL primitives.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mbedutils/drivers/threading/thread.hpp>
#include <mbedutils/interfaces/util_intf.hpp>
#include <mbedutils/threading.hpp>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <unordered_map>


namespace mb::thread
{
  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  struct TaskData
  {
    TaskHandle                   handle;
    std::unique_ptr<std::thread> thread;
    mb::thread::Task::Config     cfg;
    bool                         kill_request;
    bool                         start_request;
  };

  using TaskMap = std::unordered_map<TaskId, std::shared_ptr<TaskData>>;

  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  static std::condition_variable s_task_created_cv;
  static std::mutex              s_module_mutex;
  static TaskMap                 s_task_internal_map;
  static size_t                  s_module_ready = ~DRIVER_INITIALIZED_KEY;

  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Looks up a task in the internal map based on the handle.
   *
   * @param handle Handle generated from the create_task() function
   * @return std::unordered_map<TaskId, TaskData>::iterator
   */
  static TaskMap::iterator find_task( const TaskHandle handle )
  {
    for( auto it = s_task_internal_map.begin(); it != s_task_internal_map.end(); ++it )
    {
      if( it->second->handle == handle )
      {
        return it;
      }
    }

    return s_task_internal_map.end();
  }

  /*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/

  Task::Task() noexcept : mId( TASK_ID_INVALID ), mName( "" ), mHandle( 0 ), pImpl( nullptr )
  {
    mName.clear();
  }


  Task::~Task()
  {
  }


  Task::Task( Task &&other ) noexcept
  {
    this->mId     = other.mId;
    this->mName   = other.mName;
    this->mHandle = other.mHandle;
    this->pImpl   = other.pImpl;
  }


  Task &Task::operator=( Task &&other ) noexcept
  {
    this->mId     = other.mId;
    this->mName   = other.mName;
    this->mHandle = other.mHandle;
    this->pImpl   = other.pImpl;

    return *this;
  }


  void Task::start()
  {
    std::lock_guard<std::mutex> lock( s_module_mutex );

    auto task_iter = find_task( mHandle );
    if( task_iter == s_task_internal_map.end() )
    {
      throw std::runtime_error( "Task not found in map" );
    }

    pImpl = reinterpret_cast<void *>( task_iter->second.get() );
    task_iter->second->start_request = true;
  }


  void Task::kill()
  {
    auto tsk_data = reinterpret_cast<TaskData *>( pImpl );
    if( tsk_data )
    {
      tsk_data->kill_request = true;
    }
  }


  bool Task::killPending()
  {
    auto tsk_data = reinterpret_cast<TaskData *>( pImpl );
    if( tsk_data )
    {
      return tsk_data->kill_request;
    }

    return false;
  }

  void Task::join()
  {
    auto tsk_data = reinterpret_cast<TaskData *>( pImpl );
    if( tsk_data && tsk_data->thread->joinable() )
    {
      tsk_data->thread->join();
    }
  }


  bool Task::joinable()
  {
    auto tsk_data = reinterpret_cast<TaskData *>( pImpl );
    if( tsk_data )
    {
      return tsk_data->thread->joinable();
    }

    // By default returning true, I'm assuming that a null implementation
    // means the thread is already gone and join() can be called safely.
    return true;
  }


  TaskId Task::id() const
  {
    return mId;
  }


  TaskName Task::name() const
  {
    return mName;
  }


  TaskHandle Task::implementation() const
  {
    return mHandle;
  }

  namespace this_thread
  {
    TaskName get_name()
    {
      auto id = std::this_thread::get_id();

      std::lock_guard<std::mutex> lock( s_module_mutex );
      for( auto &task : s_task_internal_map )
      {
        if( task.second->thread->get_id() == id )
        {
          return task.second->cfg.name;
        }
      }

      return "";
    }


    void sleep_for( const size_t timeout )
    {
      std::this_thread::sleep_for( std::chrono::milliseconds( timeout ) );
    }


    void sleep_until( const size_t wakeup )
    {
      auto wakeup_time = std::chrono::system_clock::time_point(std::chrono::milliseconds(wakeup));
      std::this_thread::sleep_until(wakeup_time);
    }


    void yield()
    {
      std::this_thread::yield();
    }


    TaskId id()
    {
      auto id = std::this_thread::get_id();

      std::lock_guard<std::mutex> lock( s_module_mutex );
      for( auto &task : s_task_internal_map )
      {
        if( task.second->thread->get_id() == id )
        {
          return task.first;
        }
      }

      return TASK_ID_INVALID;
    }
  }    // namespace this_thread
}    // namespace mb::thread


namespace mb::thread::intf
{
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
      std::unique_lock<std::mutex> lock( s_module_mutex );
      while( s_task_internal_map.find( id ) == s_task_internal_map.end() )
      {
        s_task_created_cv.wait( lock );
      }
    }

    /*-------------------------------------------------------------------------
    Wait for the signal to start. This should be coming from the Task::start()
    method.
    -------------------------------------------------------------------------*/
    auto task_iter = s_task_internal_map.find( id );
    if( task_iter == s_task_internal_map.end() )
    {
      // We've broken an assumption with the STL map timing. This should never happen.
      throw std::runtime_error( "Task not found in map" );
    }

    auto &task_data = s_task_internal_map[ id ];
    while( !task_data->start_request )
    {
      std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
    }

    /*-------------------------------------------------------------------------
    Execute the user task, then terminate
    -------------------------------------------------------------------------*/
    task_data->cfg.func( task_data->cfg.user_data );
  }

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void driver_setup()
  {
    if( s_module_ready == DRIVER_INITIALIZED_KEY )
    {
      return;
    }

    s_module_ready = DRIVER_INITIALIZED_KEY;
    s_task_internal_map.clear();
  }


  void driver_teardown()
  {
    if( s_module_ready != DRIVER_INITIALIZED_KEY )
    {
      return;
    }

    /*-------------------------------------------------------------------------
    Destroy all tasks
    -------------------------------------------------------------------------*/
    for( auto &task : s_task_internal_map )
    {
      if( task.second->thread->joinable() )
      {
        task.second->thread->join();
      }
    }

    s_task_internal_map.clear();
    s_module_ready = ~DRIVER_INITIALIZED_KEY;
  }


  mb::thread::TaskHandle create_task( mb::thread::Task::Config &cfg )
  {
    std::lock_guard<std::mutex> lock( s_module_mutex );

    /*-------------------------------------------------------------------------
    Ensure the task ID is unique
    -------------------------------------------------------------------------*/
    if( s_task_internal_map.find( cfg.id ) != s_task_internal_map.end() )
    {
      return -1;
    }

    /*-------------------------------------------------------------------------
    Construct the task and wait for it to be ready.
    -------------------------------------------------------------------------*/
    s_task_internal_map[ cfg.id ] =
        std::make_shared<TaskData>( -1, std::make_unique<std::thread>( task_func, cfg.id ), cfg, false, false );

    do
    {
      std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
    } while( s_task_internal_map[ cfg.id ]->thread->get_id() == std::thread::id() );

    s_task_internal_map[ cfg.id ]->handle = s_task_internal_map[ cfg.id ]->thread->native_handle();

    /*-------------------------------------------------------------------------
    Notify the task that we've injected its configuration into the map.
    -------------------------------------------------------------------------*/
    s_task_created_cv.notify_all();
    std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );

    return s_task_internal_map[ cfg.id ]->handle;
  }

  void destroy_task( mb::thread::TaskHandle task )
  {
    std::lock_guard<std::mutex> lock( s_module_mutex );

    auto iter = find_task( task );
    if( iter != s_task_internal_map.end() )
    {
      iter->second->kill_request = true;

      if( iter->second->thread->joinable() )
      {
        iter->second->thread->join();
      }

      s_task_internal_map.erase( iter );
    }
  }

  void set_affinity( mb::thread::TaskHandle task, size_t coreId )
  {
    // Setting thread affinity is platform-specific and not directly supported by the C++ STL.
  }

  void start_scheduler()
  {
    std::lock_guard<std::mutex> lock( s_module_mutex );
    for( auto &task : s_task_internal_map )
    {
      task.second->start_request = true;
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
