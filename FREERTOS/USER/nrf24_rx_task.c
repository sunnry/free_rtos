#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "misc.h"
#include "nrf24.h"
#include "nrf24_rx_task.h"
#include "protocol.h"


void NRF24RX_Task(void * pvParameters){
	
		static const uint8_t nRF24_ADDR[] = { 'M', 'A', 'S' };
		uint8_t nRF24_payload[PAYLOAD_LENGTH];
		uint8_t payload_length;
		//nRF24_RXResult pipe;
	
    nRF24_SetRFChannel(40); // Set RF channel
    nRF24_SetDataRate(nRF24_DR_250kbps); // Set data rate
    nRF24_SetCRCScheme(nRF24_CRC_2byte);
    nRF24_SetAddrWidth(3);

    // Configure RX PIPE
    nRF24_SetAddr(nRF24_PIPE1, nRF24_ADDR); // program address for pipe
    nRF24_SetRXPipe(nRF24_PIPE1, nRF24_AA_ON, 10); // Auto-ACK: enabled, payload length: 10 bytes

    // Set TX power for Auto-ACK (maximum, to ensure that transmitter will hear ACK reply)
    nRF24_SetTXPower(nRF24_TXPWR_0dBm);

    nRF24_SetOperationalMode(nRF24_MODE_RX); // Set operational mode (PRX == receiver)
    
    nRF24_ClearIRQFlags(); // Clear any pending IRQ flags
		
    nRF24_SetPowerMode(nRF24_PWR_UP); // Wake the transceiver

    // Put the transceiver to the RX mode
    nRF24_CE_H();
		
    // The main loop
    for (;;) {
			int i = 0;
			for(i=0;i<PAYLOAD_LENGTH;i++){
				nRF24_payload[i] = 0;
			}
    	//
    	// Constantly poll the status of the RX FIFO and get a payload if FIFO is not empty
    	//
    	// This is far from best solution, but it's ok for testing purposes
    	// More smart way is to use the IRQ pin :)
    	//
    	if (nRF24_GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY) {
    		// Get a payload from the transceiver
    		nRF24_RXResult pipe = nRF24_ReadPayload(nRF24_payload, &payload_length);

				nRF24_ClearIRQFlags(); // Clear all pending IRQ flags

			// Print a payload contents to UART
			//UART_SendStr("RCV PIPE#");
			//UART_SendInt(pipe);
			//UART_SendStr(" PAYLOAD:>");
			//UART_SendBufHex((char *)nRF24_payload, payload_length);
			//UART_SendStr("<\r\n");
				if(nRF24_payload[0] == 0x5A){
					if(nRF24_payload[1] == 0x01){
						GPIOA->ODR |= GPIO_Pin_8;
					}
					else{
						GPIOA->ODR &= 0xfeff;
					}
				}
    	}
			
			vTaskDelay(3 / portTICK_PERIOD_MS);  //delay 3ms
    }		
	
}


