#ifndef __STOPWATCHH__
#define __STOPWATCHH__


class StopWatch
{
public:
   StopWatch();
   ~StopWatch();

   void Start(void);
   long Time(void);

   static void Sleep_ms(long ms);

private:
#ifdef _CONSOLE
   long long m_reftime;
   long long GetTime(void);
#else
   wxStopWatch m_stopwatch;
#endif
};


#endif
