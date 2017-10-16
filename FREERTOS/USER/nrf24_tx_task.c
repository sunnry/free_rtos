#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "misc.h"
#include "led.h"
#include "nrf24.h"
#include "nrf24_tx_task.h"


void NRF24TX_Task(void * pvParameters){
		
  /*configure nrf24 transmit parameters*/	
	static nRF24_TXResult tx_res;
	static const uint8_t nRF24_ADDR[] = {'E','S','B'};
	uint8_t payload_length = 10;
	uint8_t nrf24_payload[10] = {'i',' ','l','o','v','e',' ','s','t','m'};
	
	nRF24_SetRFChannel(40);  
	nRF24_SetDataRate(nRF24_DR_250kbps);
	nRF24_SetCRCScheme(nRF24_CRC_2byte);
	nRF24_SetAddrWidth(3);
		
	//configure TX PIPE
	nRF24_SetAddr(nRF24_PIPETX, nRF24_ADDR); // program TX address
	nRF24_SetAddr(nRF24_PIPE0, nRF24_ADDR);  // program address for pipe#0, must be same as TX (for Auto-ACK)
		
	nRF24_SetTXPower(nRF24_TXPWR_0dBm); // Set TX power (maximum)
		
	nRF24_SetAutoRetr(nRF24_ARD_2500us, 10); // Configure auto retransmit: 10 retransmissions with pause of 2500s in between
		
	nRF24_EnableAA(nRF24_PIPE0); // Enable Auto-ACK for pipe#0 (for ACK packets)
	
	#ifdef PURE_TX_DEBUG 
		nRF24_SetAutoRetr(nRF24_ARD_2500us, 0);
		nRF24_DisableAA(nRF24_PIPE0);
	#endif
		
	nRF24_SetOperationalMode(nRF24_MODE_TX); // Set operational mode (PTX == transmitter)
		
	nRF24_ClearIRQFlags(); // Clear any pending IRQ flags
		
	nRF24_SetPowerMode(nRF24_PWR_UP); // Wake the transceiver

	
	for(;;){
		
		tx_res = nRF24_TransmitPacket(nrf24_payload, payload_length);
		
		switch (tx_res) {
			case nRF24_TX_SUCCESS:
				//UART_SendStr("OK");
				break;
			case nRF24_TX_TIMEOUT:
				//UART_SendStr("TIMEOUT");
				break;
			case nRF24_TX_MAXRT:
				//UART_SendStr("MAX RETRANSMIT");
				break;
			default:
				//UART_SendStr("ERROR");
				break;
		}
    	//UART_SendStr("\r\n");
		vTaskDelay(5 / portTICK_PERIOD_MS);  //delay 5ms
	}
	
}



// Function to transmit data packet
// input:
//   pBuf - pointer to the buffer with data to transmit
//   length - length of the data buffer in bytes
// return: one of nRF24_TX_xx values

nRF24_TXResult nRF24_TransmitPacket(uint8_t *pBuf, uint8_t length) {
	volatile uint32_t wait = nRF24_WAIT_TIMEOUT;
	uint8_t status;

	// Deassert the CE pin (in case if it still high)
	nRF24_CE_L();

	// Transfer a data from the specified buffer to the TX FIFO
	nRF24_WritePayload(pBuf, length);

	// Start a transmission by asserting CE pin (must be held at least 10us)
	nRF24_CE_H();

	// Poll the transceiver status register until one of the following flags will be set:
	//   TX_DS  - means the packet has been transmitted
	//   MAX_RT - means the maximum number of TX retransmits happened
	// note: this solution is far from perfect, better to use IRQ instead of polling the status
	do {
		status = nRF24_GetStatus();
		if (status & (nRF24_FLAG_TX_DS | nRF24_FLAG_MAX_RT)) {
			break;
		}
	} while (wait--);

	// Deassert the CE pin (Standby-II --> Standby-I)
	nRF24_CE_L();

	if (!wait) {
		// Timeout
		return nRF24_TX_TIMEOUT;
	}

	// Check the flags in STATUS register
	//UART_SendStr("[");
	//UART_SendHex8(status);
	//UART_SendStr("] ");

	// Clear pending IRQ flags
    nRF24_ClearIRQFlags();

	if (status & nRF24_FLAG_MAX_RT) {
		// Auto retransmit counter exceeds the programmed maximum limit (FIFO is not removed)
		return nRF24_TX_MAXRT;
	}

	if (status & nRF24_FLAG_TX_DS) {
		// Successful transmission
		return nRF24_TX_SUCCESS;
	}

	// Some banana happens, a payload remains in the TX FIFO, flush it
	nRF24_FlushTX();

	return nRF24_TX_ERROR;
}

