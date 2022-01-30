/*
 * UART_Driver.c
 *
 * Created: 1/24/2022 12:19:11 AM
 * Author : Khaled Ali
 */ 

#include "../ECUAL/LED/LED.h"
#include "../MCAL/UART/UART.h"
#include "../MCAL/DIO/DIO.h"
LED_t led_err = {PORTB, PIN4};
int main(void)
{
	LED_u8Init(&led_err);
	DIO_u8SetPinDirection(PORTD, PIN0, INPUT_PIN_NO_PULLUP);
	DIO_u8SetPinDirection(PORTD, PIN1, OUTPUT_PIN);
	UART_u8Init(9600);
	uint8_t data;
	uint8_t status;
    while (1) 
    {
		status = UART_u8ReceiveData(&data);
		if(status == OK){
			UART_u8SendData(data);
		}
		else{
			LED_u8ON(&led_err);
		}
    }
}

