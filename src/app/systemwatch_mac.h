#ifndef SYSTEMWATCH_MAC_H
#define SYSTEMWATCH_MAC_H

#include "systemwatch.h"

class MacSystemWatch : public SystemWatch
{
public:
	MacSystemWatch();

	// These shouldn't be called from outside, but i can't hide them atm.
	void emitSleep() { emit sleep(); }
	void emitIdleSleep() { emit idleSleep(); }
	void emitWakeup() { emit wakeup(); }
};

#endif
