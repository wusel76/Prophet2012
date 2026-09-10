#ifdef _CONSOLE
   #ifdef _MAC_OS
      #include <CoreServices/CoreServices.h>
      #include <mach/mach.h>
      #include <mach/mach_time.h>
      #include <sys/time.h>
      #include <unistd.h>
   #else
      #include <windows.h>
   #endif
#else
   #include <wx/wx.h>
#endif

#include "stopwatch.h"


StopWatch::StopWatch()
{
#ifdef _CONSOLE
   Start();
#endif
}

StopWatch::~StopWatch()
{
}

void StopWatch::Start(void)
{
#ifdef _CONSOLE
   m_reftime = GetTime();
#else
   m_stopwatch.Start();
#endif
}

void StopWatch::Sleep_ms(long ms)
{
#ifdef _CONSOLE
#ifdef _MAC_OS
   usleep(ms * 1000);
#else
   Sleep(ms);
#endif
#else
   wxThread::Sleep(ms);
#endif
}


#ifdef _CONSOLE
long long StopWatch::GetTime(void) 
{
 #ifdef _MAC_OS
   timeval time;
   gettimeofday(&time, NULL);
   long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
   return millis;
 #else
    static LARGE_INTEGER s_frequency;
    static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
    if (s_use_qpc) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / s_frequency.QuadPart;
    } else {
        return GetTickCount();
    }
 #endif
}
#endif

long StopWatch::Time(void)
{
#ifdef _CONSOLE
   long long t = GetTime();
   return (long)(t - m_reftime);
#else
   return m_stopwatch.Time();
#endif
}
