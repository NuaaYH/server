#include "timermg.h"
#include "make_unique.h"

std::unique_ptr<TimerManager> timer_=make_unique<TimerManager>();
