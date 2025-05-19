#include "marker.h"

MarkerThreadInfo::MarkerThreadInfo(int threadId)
    : id(threadId),
      canContinue(true),
      shouldTerminate(false),
      waitingForSignal(false) {}