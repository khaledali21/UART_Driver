/*
 * UART.c
 *
 * Created: 1/24/2022 12:24:54 AM
 *  Author: Khaled Ali
 */ 

#include "UART.h"

static void (*receivce_CallBack)(uint8_t);

void UART_u8Init(uint32_t BaudRate){
	uint16_t UBRR_Value = ((UART_F_CPU)/(BaudRate*16UL)-1);
	UCSRC_REG =	(1 << URSEL_BIT) | (1 << UCSZ1_BIT) | (1 << UCSZ0_BIT);
	UBRRH_REG = (uint8_t) (UBRR_Value >> 8);
	UBRRL_REG = (uint8_t) UBRR_Value;
	SET_BIT(UCSRB_REG, TXEN_BIT);
	SET_BIT(UCSRB_REG, RXEN_BIT);
}
void UART_u8SendData(uint8_t data){
	
	while(GET_BIT(UCSRA_REG, UDRE_BIT) != 1);
	UDR_REG = data;	
	while(GET_BIT(UCSRA_REG, TXC_BIT) != 1);
}
UART_ERR_STATE UART_u8ReceiveData(uint8_t* data){
	UART_ERR_STATE u8Status = UART_OK;
	if(data == NULL){
		u8Status = UART_ERR;
	}
	else{
		while(GET_BIT(UCSRA_REG, RXC_BIT) != 1);
		if(GET_BIT(UCSRA_REG, FE_BIT) == 1){
			u8Status = UART_FRAME_ERR;
		}
		else if(GET_BIT(UCSRA_REG, DOR_BIT) == 1){
			u8Status = UART_DATA_OVERRUN;
		}
		else{
			*data = UDR_REG;
		}
	}	
	return u8Status;
}

UART_ERR_STATE UART_u8SendString(uint8_t* str){
	UART_ERR_STATE u8Status = UART_OK;
	if(str == NULL){
		u8Status = UART_ERR;
	}
	else{
		uint8_t* itr = str;
		while(*itr != '\0'){
			UART_u8SendData(*itr);
			itr++;
		}	
	}
	return u8Status;
}

UART_ERR_STATE UART_u8ReceiveString(uint8_t* str, uint8_t size){
	UART_ERR_STATE u8Status = UART_OK;
	if(str == NULL){
		u8Status = UART_ERR;
	}
	else{
		for(uint8_t itr = 0; itr < size && u8Status == UART_OK; ++itr){
			u8Status = UART_u8ReceiveData(&str[itr]);
		}
	}
	return u8Status;
}

UART_ERR_STATE UART_u8EnableReceiveInterrupt(void (*fncallBack)(uint8_t)){
	UART_ERR_STATE u8Status = UART_OK;
	if(fncallBack == NULL){
		u8Status = UART_ERR;
	}
	else{
		receivce_CallBack = fncallBack;
		SET_BIT(UCSRB_REG, RXCIE_BIT);
		SET_BIT(SREG_REG, I_BIT);
	}
	return u8Status;
}

void UART_u8DisableReceiveInterrupt(void){
	receivce_CallBack = NULL;
	CLR_BIT(UCSRB_REG, RXCIE_BIT);
}


void __vector_13(void){
	if(receivce_CallBack != NULL){
		receivce_CallBack(UDR_REG);
	}
}