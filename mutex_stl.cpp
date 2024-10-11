/******************************************************************************
 *  File Name:
 *    mutex_stl.cpp
 *
 *  Description:
 *    Implementation of the mutex interface using the C++ standard library
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <chrono>
#include <mbedutils/interfaces/mutex_intf.hpp>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace mb::osal
{
  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/
  static std::mutex s_mtx_map_lock;
  static std::mutex s_rmtx_map_lock;

  static std::vector<std::mutex *>           s_mtx_vector;
  static std::vector<std::recursive_mutex *> s_rmtx_vector;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initMutexDriver()
  {
    s_mtx_vector.clear();
    s_rmtx_vector.clear();
  }

  bool createMutex( mb_mutex_t &mutex )
  {
    std::lock_guard<std::mutex> lock( s_mtx_map_lock );

    s_mtx_vector.push_back( new std::mutex() );
    mutex = s_mtx_vector.back();

    return true;
  }

  void destroyMutex( mb_mutex_t &mutex )
  {
    std::lock_guard<std::mutex> lock( s_mtx_map_lock );

    auto it = std::find( s_mtx_vector.begin(), s_mtx_vector.end(), mutex );
    if( it != s_mtx_vector.end() )
    {
      delete *it;
      s_mtx_vector.erase( it );
      mutex = nullptr;
    }
  }

  bool allocateMutex( mb_mutex_t &mutex )
  {
    return createMutex( mutex );
  }

  void deallocateMutex( mb_mutex_t &mutex )
  {
    destroyMutex( mutex );
  }

  void lockMutex( mb_mutex_t mutex )
  {
    static_cast<std::mutex *>( mutex )->lock();
  }

  bool tryLockMutex( mb_mutex_t mutex )
  {
    return static_cast<std::mutex *>( mutex )->try_lock();
  }

  bool tryLockMutex( mb_mutex_t mutex, const size_t timeout )
  {
    // Not directly supported by std::mutex, so we simulate it
    auto start = std::chrono::steady_clock::now();
    while( std::chrono::steady_clock::now() - start < std::chrono::milliseconds( timeout ) )
    {
      if( static_cast<std::mutex *>( mutex )->try_lock() )
      {
        return true;
      }
      std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
    }
    return false;
  }

  void unlockMutex( mb_mutex_t mutex )
  {
    static_cast<std::mutex *>( mutex )->unlock();
  }

  bool createRecursiveMutex( mb_recursive_mutex_t &mutex )
  {
    std::lock_guard<std::mutex> lock( s_rmtx_map_lock );

    s_rmtx_vector.push_back( new std::recursive_mutex() );
    mutex = s_rmtx_vector.back();

    return true;
  }

  void destroyRecursiveMutex( mb_recursive_mutex_t &mutex )
  {
    std::lock_guard<std::mutex> lock( s_rmtx_map_lock );

    auto it = std::find( s_rmtx_vector.begin(), s_rmtx_vector.end(), mutex );
    if( it != s_rmtx_vector.end() )
    {
      delete *it;
      s_rmtx_vector.erase( it );
      mutex = nullptr;
    }
  }

  bool allocateRecursiveMutex( mb_recursive_mutex_t &mutex )
  {
    return createRecursiveMutex( mutex );
  }

  void deallocateRecursiveMutex( mb_recursive_mutex_t &mutex )
  {
    destroyRecursiveMutex( mutex );
  }

  void lockRecursiveMutex( mb_recursive_mutex_t mutex )
  {
    static_cast<std::recursive_mutex *>( mutex )->lock();
  }

  bool tryLockRecursiveMutex( mb_recursive_mutex_t mutex )
  {
    return static_cast<std::recursive_mutex *>( mutex )->try_lock();
  }

  bool tryLockRecursiveMutex( mb_recursive_mutex_t mutex, const size_t timeout )
  {
    // Not directly supported by std::recursive_mutex, so we simulate it
    auto start = std::chrono::steady_clock::now();
    while( std::chrono::steady_clock::now() - start < std::chrono::milliseconds( timeout ) )
    {
      if( static_cast<std::recursive_mutex *>( mutex )->try_lock() )
      {
        return true;
      }
      std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
    }
    return false;
  }

  void unlockRecursiveMutex( mb_recursive_mutex_t mutex )
  {
    static_cast<std::recursive_mutex *>( mutex )->unlock();
  }
}    // namespace mb::osal
