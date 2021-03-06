#ifndef		__PRE_DEFINES_H
#define		__PRE_DEFINES_H

/* when you debug NRF24 tx ability, you can open PURE_TX_DEBUG to close TX auto-ACK and resender ability
 * when open TRANSMIT_MODE it will open TX Mode, when you mark TRANSMIT_MODE it will open RX Mode 
 */

//#define		PURE_TX_DEBUG		1   //use debug mode to tx data or not, this is only used when debug tx, when
																//you debug handshake between TX and RX, make sure to mark this MACRO, because
																//if you open this, it will disable auto-ACK and resender and make handshake failed

#define		TRANSMIT_MODE			1   //if TRANSMIT_MODE defined as 1 = tx mode, if not defined rx mode

#endif



