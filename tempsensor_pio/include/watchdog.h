#ifndef TEMPSENS_WATCHDOG_HH
#define TEMPSENS_WATCHDOG_HH

class Watchdog {

public:
  void delayWithPatWatchdog(int ms);
  void initWDT();
  void resetWDT();
};

#endif // TEMPSENS_WATCHDOG_HH
