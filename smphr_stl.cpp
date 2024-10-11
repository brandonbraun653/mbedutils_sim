/******************************************************************************
 *  File Name:
 *    smphr_stl.cpp
 *
 *  Description:
 *    Implementation of the semaphore interface using the C++ standard library
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/

#include <chrono>
#include <cstddef>
#include <mbedutils/interfaces/smphr_intf.hpp>
#include <memory>
#include <semaphore>
#include <thread>
#include <unordered_map>

namespace mb::osal
{
  struct SemaphoreWrapper
  {
    std::counting_semaphore<> smphr;
    SemaphoreWrapper( size_t maxCount, size_t initialCount ) : smphr( initialCount )
    {
    }
  };

  // Maps to keep track of semaphores
  std::unordered_map<mb_smphr_t, std::unique_ptr<SemaphoreWrapper>> semaphore_map;

  void initSmphrDriver()
  {
    // No specific initialization needed for STL semaphores
  }

  bool createSmphr( mb_smphr_t &s, const size_t maxCount, const size_t initialCount )
  {
    auto new_smphr     = std::make_unique<SemaphoreWrapper>( maxCount, initialCount );
    s                  = new_smphr.get();
    semaphore_map[ s ] = std::move( new_smphr );
    return true;
  }

  void destroySmphr( mb_smphr_t &s )
  {
    semaphore_map.erase( s );
    s = nullptr;
  }

  bool allocateSemaphore( mb_smphr_t &s, const size_t maxCount, const size_t initialCount )
  {
    return createSmphr( s, maxCount, initialCount );
  }

  void deallocateSemaphore( mb_smphr_t &s )
  {
    destroySmphr( s );
  }

  size_t getSmphrAvailable( mb_smphr_t &s )
  {
    // Not directly supported by std::counting_semaphore, so we simulate it
    // by trying to acquire and then release the semaphore
    size_t count = 0;
    while( static_cast<SemaphoreWrapper *>( s )->smphr.try_acquire() )
    {
      ++count;
    }
    for( size_t i = 0; i < count; ++i )
    {
      static_cast<SemaphoreWrapper *>( s )->smphr.release();
    }
    return count;
  }

  void releaseSmphr( mb_smphr_t &s )
  {
    static_cast<SemaphoreWrapper *>( s )->smphr.release();
  }

  void releaseSmphrFromISR( mb_smphr_t &s )
  {
    releaseSmphr( s );
  }

  void acquireSmphr( mb_smphr_t &s )
  {
    static_cast<SemaphoreWrapper *>( s )->smphr.acquire();
  }

  bool tryAcquireSmphr( mb_smphr_t &s )
  {
    return static_cast<SemaphoreWrapper *>( s )->smphr.try_acquire();
  }

  bool tryAcquireSmphr( mb_smphr_t &s, const size_t timeout )
  {
    auto start = std::chrono::steady_clock::now();
    while( std::chrono::steady_clock::now() - start < std::chrono::milliseconds( timeout ) )
    {
      if( static_cast<SemaphoreWrapper *>( s )->smphr.try_acquire() )
      {
        return true;
      }
      std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
    }
    return false;
  }
}    // namespace mb::osal
