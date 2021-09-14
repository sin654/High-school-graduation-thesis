/*
 * m16_lcd_usart.c
 *
 * Created: 12.03.2016 17:59:50
 * Author : honza
 */ 
#define F_CPU 16000000UL	//FREKVENCE extern�ho oscil�toru (16 Mhz)
#include <avr/io.h>			//knihovna obsahuj�c� nastaven� pro AVR mikrokontrol�ry
#include <util/delay.h>		//knihovna k pr�ci s prodlevou    _delay_ms()   
#include <stdio.h>			//knihovna obsahuj�c� z�kladn� operace
#include <avr/interrupt.h>	//knihovna obsahuj�c� p�eru�en�
#include <string.h>			//knihovna pro pr�ci s �et�zci
#include "lcd.h"			//knihovna k ovl�d�n� LCD displeje

//deklarace funkc�
unsigned char uart_getc(void);
void uart_putc(unsigned char data );

void C1(void);
void D1(void);
void E1(void);
void F1(void);
void G1(void);
void A1(void);
void H1(void);
void C2(void);

//prom�nn� na pr�ci s USART daty
volatile unsigned char receivedByte;
volatile unsigned char newByte;


//definice prom�nn�ch pro hodiny
volatile unsigned char sec = 48;
volatile unsigned char des_sec = 48;

volatile unsigned char min = 57;
volatile unsigned char des_min = 53;

volatile unsigned char hod = 51;
volatile unsigned char des_hod = 50;

int cas_x = 0;
int cas_y = 0;

//prom�nn� Klav�r
volatile int ton = 10;

//prom�nn� pro zm�nu interpretace vstupn�ho byte
volatile int druh_info = 0;	//speci�ln� znak pro zm�nu interpretov�n� p��choz�ch byt�

volatile int time_set_len = 0;	//d�lka �et�zce pro zm�nu nastaven� "HODIN"

//prom�nn� pro b��c� text
volatile int text_x = 0;	
int text_y = 1;


volatile int pul_sec = 0;	//prom�nn� pro prodlou�en� doby skoku "b��c�ho textu"

char pole_change[20];		//pole na ukl�d�n� znak�





ISR(USART_RXC_vect)		//P�ERU�EN�: P�IJMUT� ZNAKU
{
	receivedByte = uart_getc();
	
	switch (receivedByte)
	{
		case 60:	//ascii "<" (zm�na hodin)
			druh_info = 100;
			time_set_len = 7;
			break;
		
		case 123:	//ascii "{" (zah�jen� zm�ny b��c�ho textu)
			druh_info = 99;
			time_set_len = -1;
			break;
			
		case 62:	//ascii ">" (maz�n� textu zapsan�ho na prvn�m ��dku)
			for (int i = 0; i < 20; i++)
			{
				lcd_gotoxy(i, 0);
				lcd_putc(32);
			}
			lcd_gotoxy(0,0);
			druh_info = 98;
			break;
			
		case 0:
			druh_info = 97;		//pro toho, kdo do tohoto k�du nahledne, tak druh_info jsou pouze random hodnoty k rozli�en� ur�en� p�ijat�ho bitu :-)
			break;
			
		case 1:
			druh_info = 96;
			break;
		
		case 2:
			druh_info = 95;
			break;
		
		case 3:
			druh_info = 94;
			break;
		
		case 4:
			druh_info = 93;
			break;
			
		case 5:
			druh_info = 92;
			break;
			
		case 6:
			druh_info = 91;
			break;	
			
		case 7:
			druh_info = 90;
			break;
	}
	
	if (druh_info == 100)
	{
		
		switch (time_set_len)
		{
			case 7:
				time_set_len--;
				break;
			
			case 6:
				des_hod = receivedByte;
				time_set_len--;
				break;
			
			case 5:
				hod = receivedByte;
				time_set_len--;
				break;
			
			case 4:
				des_min = receivedByte;
				time_set_len--;
				break;
			
			case 3:
				min =receivedByte;
				time_set_len--;
				break;
			
			case 2:
				des_sec = receivedByte;
				time_set_len--;
				break;
			
			case 1:
				sec = receivedByte;
				time_set_len--;
				druh_info = 0;
				break;
			
		}
			
	}
	
	else if (druh_info == 99)
	{
		if (time_set_len == -1)
		{
			time_set_len++;
		}
		else
		{
			if (receivedByte != 125)
			{
				pole_change[time_set_len] = receivedByte;
				time_set_len++;
			}
			else
			{
				pole_change[time_set_len] = '\0';
				druh_info = 0;
				text_x = 0;
			}
		}
	}
	else if (druh_info == 98)
	{
		druh_info = 0;
		cas_x = 0;
	}
	else if (druh_info == 97)
		ton = 0;	//C1
	else if (druh_info == 96)
		ton = 1;	//D1
	else if (druh_info == 95)
		ton = 2;	//E1
	else if (druh_info == 94)
		ton = 3;	//F1
	else if (druh_info == 93)
		ton = 4;	//G1
	else if (druh_info == 92)
		ton = 5;	//A1
	else if (druh_info == 91)
		ton = 6;	//H1
	else if (druh_info == 90)
		ton = 7;	//C2
		
	else
	{
		newByte = 1;
	}
		
}

ISR(TIMER0_OVF_vect)	//P�ERU�EN�: B̎�C� TEXT
{
	if (pul_sec == 25)	// v�t�� znamen� del�� prodlevu mezi skoky textu (25 = +-0.5s)
	{
		
		//vykreslov�n� b��c�ho textu
		
		
		lcd_gotoxy(text_x -1, text_y);		//maz�n� znaku p�ed nov�m �et�zcem
		lcd_putc(32);
		
		for (int i = (text_x + strlen(pole_change));i < 20;i++)		//maz�n� 2. ��dku za �et�zcem
		{
			lcd_gotoxy(i, text_y);
			lcd_putc(32);
		}
		
		lcd_gotoxy(text_x, text_y);
		lcd_puts(pole_change);
		int pole_len = strlen(pole_change);
		
		if (text_x == (20 - pole_len))	//d�lka �et�zce z bodu text_x p�esahuje d�lku 2. ��dku >> vynuluje text_x
		{
			text_x = -1;
		}
		text_x++;
		pul_sec = 0;
	}
	pul_sec++;
}

ISR(TIMER1_OVF_vect)	//P�ERU�EN�: HODINY
{
	TCNT1 = 3036;
	
	//vykreslov�n� hodin
	lcd_gotoxy(10,3);
	lcd_putc(des_hod);
	
	lcd_gotoxy(11,3);
	lcd_putc(hod);
	
	lcd_gotoxy(12,3);
	lcd_puts(":");
	
	lcd_gotoxy(13,3);
	lcd_putc(des_min);
	
	lcd_gotoxy(14,3);
	lcd_putc(min);
	
	lcd_gotoxy(15,3);
	lcd_puts(":");
	
	lcd_gotoxy(16,3);
	lcd_putc(des_sec);
	
	lcd_gotoxy(17,3);
	lcd_putc(sec);
	
	
	
	if (sec == 57)
	{
		if (des_sec == 53)
		{
			if (min == 57)
			{
				if (des_min == 53)
				{
					if (hod == 57 || hod == 51)
					{
						if (hod == 51 && des_hod == 50)
						{
							des_hod = 48;
							hod = 47;
						}
						else if (hod == 57)
						{
							hod = 47;
							des_hod++;
						}

					}
					des_min = 47;
					hod++;
				}
				min = 47;
				des_min++;
			}
			des_sec = 47;
			min++;
		}
		sec = 47;
		des_sec++;
	}
	sec++;	//p�i�ten� 1 k sekund�m
	
}



int main(void)			//HLAVN� PROGRAM
{
	DDRC |=0b00000011;
	PORTC |=0b00000001;
	
	DDRD |=0b00010000;
	
	//p��prava LCD displeje
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	lcd_gotoxy(0,2);
	lcd_puts(pole_change);
	
	
	//definice USART
	UBRRL = 0b01100111;	
	UBRRH = 0b00000000;
	
	UCSRB = 0b10011000;
	
	UCSRC = 0b10000110;
	
	//definice ��ta�e
	TCCR1B = 0b00000100;	//16bit ��ta�: norm�ln� re�im, deli�ka frekvence 256
	TCCR0 = 0b00000101;		//8bit ��ta�: norm�ln� re�im, d�li�ka frekvence 1024
	TIMSK = 0b00000101;		//povolen� p�eru�en� na 16bit a 8bit ��ta�i
	
	
	sei();	//glob�ln� povolen� p�eru�en�
	
	newByte = 0;	//vynulov�n� p�ijet� znaku
	
	while (1)
	{
		
		if (newByte == 1)	//obdr�en� znaku k zaps�n� na 1. ��dek displeje
		{
			newByte = 0;
			lcd_gotoxy(cas_x,cas_y);
			lcd_putc(receivedByte);
			if (cas_x<19)
			{
				cas_x++;
			}
			else if (cas_x == 19)
			{
				cas_x = 0;
			}
			
			
		}
		
		switch (ton)
		{
			case 0:
				C1();
				ton = 10;
				uart_putc(0);
				break;
			
			case 1:
				D1();
				ton = 10;
				uart_putc(1);
				break;
			
			case 2:
				E1();
				ton = 10;
				uart_putc(2);
				break;
			
			case 3:
				F1();
				ton = 10;
				uart_putc(3);
				break;
			
			case 4:
				G1();
				ton = 10;
				uart_putc(4);
				break;
			
			case 5:
				A1();
				ton = 10;
				uart_putc(5);
				break;
			
			case 6:
				H1();
				ton = 10;
				uart_putc(6);
				break;
			
			case 7:
				C2();
				ton = 10;
				uart_putc(7);
				break;
		}
	}
	
	
	
	
	return 0;
}



unsigned char uart_getc(void)	//P�IJMUT� ZNAKU
{
	// �ek�n� na p�ijet� znaku
	while (!(UCSRA & (1<<RXC)));
	return UDR;
}	 

void uart_putc(unsigned char data)	//ODESL�N� ZNAKU
{
	// �ek�n� na vypr�zdn�n� registru UDR
	while (!( UCSRA & (1<<UDRE)));
	UDR = data;
}

//FUNKCE T�NY

void C1(void)
{
	for (int i = 0;i < 52;i++)
	{
		PORTC |= 0b00000010;
		_delay_us(1908);
		PORTC &= 0b11111101;
		_delay_us(1908);
		
	}
}

void D1(void)
{
	for (int i = 0;i < 59;i++)
	{
		PORTC |= 0b00000010;
		_delay_us(1700);
		PORTC &= 0b11111101;
		_delay_us(1700);
		
	}
}

void E1(void)
{
	for (int i = 0;i < 66;i++)
	{
		PORTC |= 0b00000010;
		_delay_us(1515);
		PORTC &= 0b11111101;
		_delay_us(1515);
		
	}
}

void F1(void)
{
	for (int i = 0;i < 70;i++)
	{
		PORTC |= 0b00000010;
		_delay_us(1432);
		PORTC &= 0b11111101;
		_delay_us(1432);
		
	}
}

void G1(void)
{
	for (int i = 0;i < 78;i++)
	{
		PORTC |= 0b00000010;
		_delay_us(1275);
		PORTC &= 0b11111101;
		_delay_us(1275);
		
	}
}

void A1(void)
{
	for (int i = 0;i < 88;i++)
	{
		PORTC |= 0b00000010;
		_delay_us(1136);
		PORTC &= 0b11111101;
		_delay_us(1136);
		
	}
}

void H1(void)
{
	for (int i = 0;i < 99;i++)
	{
		PORTC |= 0b00000010;
		_delay_us(1012);
		PORTC &= 0b11111101;
		_delay_us(1012);
		
	}
}

void C2(void)
{
	for (int i = 0;i < 105;i++)
	{
		PORTC |= 0b00000010;
		_delay_us(956);
		PORTC &= 0b11111101;
		_delay_us(956);
		
	}
}	  