//- ============================================================================
//
//- = CONTEXT
//-   Tango Generic Client for Matlab
//
//- = FILENAME
//-   TangoBinding.cpp
//
//- = AUTHOR
//-   Nicolas Leclercq - SOLEIL
//
//- ============================================================================

#ifndef _TIMER_H_
#define _TIMER_H_

// ============================================================================
// TIME MACROS
// ============================================================================
#include <ctime>
#include <limits>
#if defined (WIN32)
# include <sys/timeb.h>
# include <time.h>
#else
# include <sys/time.h>
#endif

#if defined (WIN32)
  namespace std 
  { 
    using ::clock_t; 
    using ::clock; 
  }
#endif

#if ! defined (WIN32) 
 
// ============================================================================
// class Timer
// ============================================================================
class Timer
{
public:
	//- instanciate then resets the Timer
  Timer () 
  { 
    this->restart();
  } 
  
  //- resets the Timer
  inline void restart() 
  {
    ::gettimeofday(&_start_time, NULL); 
  }

  //- returns elapsed time in seconds
  inline double elapsed_sec ()             
  { 
    struct timeval now;
    ::gettimeofday(&now, NULL);
    return (now.tv_sec - _start_time.tv_sec) + 1e-6 * (now.tv_usec - _start_time.tv_usec);
  }

  //- returns elapsed time in milliseconds
  inline double elapsed_msec ()             
  { 
    struct timeval now;
    ::gettimeofday(&now, NULL);
    return 1e3 * (now.tv_sec - _start_time.tv_sec) + 1e-3 * (now.tv_usec - _start_time.tv_usec);
  }

  //- returns elapsed time in microseconds
  inline double elapsed_usec ()              
  { 
    struct timeval now;
    ::gettimeofday(&now, NULL);
    return 1e6 * (now.tv_sec - _start_time.tv_sec) + (now.tv_usec - _start_time.tv_usec);
  }

private:
  struct timeval _start_time;
};

#else // ! WIN32

// ============================================================================
// class: Timer
// ============================================================================
class Timer 
{
public:
  //- instanciates/resets the Timer
  Timer () 
  {
    _start.QuadPart = 0;
    _stop.QuadPart = 0;
    ::QueryPerformanceFrequency(&_frequency);
    ::QueryPerformanceCounter(&_start);
  }
  
  //- resets the Timer
  inline void restart() 
  {
    ::QueryPerformanceCounter(&_start) ;
  }
  
  //- returns elapsed time in seconds
  inline double elapsed_sec ()             
  { 
    ::QueryPerformanceCounter(&_stop);
    LARGE_INTEGER dt;
    dt.QuadPart = _stop.QuadPart - _start.QuadPart;
    return li_to_secs(dt);
  }

  //- returns elapsed time in milliseconds
  inline double elapsed_msec ()           
  { 
    return  1.E3 * elapsed_sec (); 
  }

  //- returns elapsed time in microseconds
  inline double elapsed_usec ()            
  { 
    return  1.E6 * elapsed_sec (); 
  }
  
private:
  LARGE_INTEGER _start;
  LARGE_INTEGER _stop;
  LARGE_INTEGER _frequency;
  inline double li_to_secs ( LARGE_INTEGER & li) {
    return (double)li.QuadPart / (double)_frequency.QuadPart;
  }
};

#endif // ! WIN32

// ============================================================================
// class: Timeout
// ============================================================================
class Timeout 
{
public:
  //- Timeout value type
  typedef double TimeoutValue;
  
  //- Timeout units
  typedef enum  
  {
    TMO_UNIT_SEC,  //- second
    TMO_UNIT_MSEC, //- millisecond
    TMO_UNIT_USEC, //- microsecond
  } TimeoutUnit;
  
  //- instanciates a Timeout (default dtor)
  Timeout () 
  	: _unit(TMO_UNIT_MSEC), _tmo(0), _enabled(false)
  {
		//- noop 
  }
  
  //- instanciates a Timeout
  Timeout (TimeoutValue tmo_in_unit, TimeoutUnit unit = TMO_UNIT_MSEC, bool enabled = false) 
  	: _unit(unit), _tmo(tmo_in_unit), _enabled(enabled)
  {
		//- noop 
  }
  
  //- restarts/(re)enables the Timeout 
  //- in the <disabled> state, Timeout::expired will always return false
  inline void restart () 
  {
  	enable();
  }
  
  //- enables the Timeout 
  //- in the <disabled> state, Timeout::expired will always return false
  inline void enable (bool restart_timer = true) 
  {
  	if (restart_timer) _t.restart();
    _enabled = true;
  }
  
  //- disables the Timeout 
  //- in the <disabled> state, Timeout::expired will always return false
  inline void disable () 
  {
    _enabled = false;
  }
    
  //- enabled?
  inline bool enabled () const
  {
    return _enabled;
  }
  
  //- Timeout expired?
  inline bool expired ()
  {
    //- a disabled Timeout never expire
    if (! _enabled)
    	return false;
    //- however, an enabled Timeout might expire :-)
    double dt = 0.;
    switch (_unit) 
    {
      case TMO_UNIT_SEC:
        dt = _t.elapsed_sec();
        break;
      case TMO_UNIT_MSEC:
        dt = _t.elapsed_msec();
        break;
      case TMO_UNIT_USEC:
        dt = _t.elapsed_usec();
        break;
      default:
        break;
    }
    return dt >= _tmo;
  }
  
  //- time to expiration in Timeout unit?
  //- a negative value means: expired ... tmo-unit ago 
  //- a positive value means: will expired in ... tmo-unit
  //- for a disabled Timeout, returns the infinity value 
  inline TimeoutValue time_to_expiration ()
  {
    //- undefined if disabled...
    if (! _enabled)
      return std::numeric_limits<TimeoutValue>::infinity();
    double dt = 0.;
    switch (_unit) 
    {
      case TMO_UNIT_SEC:
        dt = _t.elapsed_sec();
        break;
      case TMO_UNIT_MSEC:
        dt = _t.elapsed_msec();
        break;
      case TMO_UNIT_USEC:
        dt = _t.elapsed_usec();
        break;
      default:
        break;
    }
    return _tmo - dt;
  }
  
  //- changes the Timeout value
  inline void set_value (TimeoutValue tmo) 
  {
    _tmo = tmo;
  }
  
  //- returns the Timeout value 
  inline TimeoutValue get_value () const
  {
    return _tmo;
  }
  
  //- changes the Timeout unit
  inline void set_unit (TimeoutUnit unit) 
  {
    _unit = unit;
  }
  
  //- returns the Timeout unit 
  inline TimeoutUnit get_unit () const
  {
    return _unit;
  }
  
private:
  Timer _t;
	TimeoutUnit _unit;
  double _tmo;
  bool _enabled;
};

#endif
