#ifndef  __PROTOCOL_H
#define	 __PROTOCOL_H

#include "FreeRTOS.h"

#define PAYLOAD_LENGTH	6

struct ControlFrameCtx{
	uint8_t		FrontLight;
	uint8_t		TailLight;
	uint8_t		LeftThruttle;
	uint8_t		RightThruttle;
};

#endif

