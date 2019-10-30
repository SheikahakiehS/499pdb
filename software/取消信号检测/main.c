#include <reg51.h>
#include <stdio.h>
#include "reg51.h"
#include "intrins.h"

#define H (1)
#define L (0)

sbit TVP_PDN = P1^6;
sbit TVP_RESETB = P1^5;
sbit FLCOS_nRST = P1^2;
sbit FLCOS_SEN = P1^1;
sbit FLCOS_SCLK = P1^0;
sbit FLCOS_SDAT = P3^7;
sbit TVP_SDA = P3^3;
sbit TVP_SCL = P3^4;
sbit BL_U = P3^1;
sbit BL_D = P3^0;

#define SDA TVP_SDA
#define SCL TVP_SCL


void delay_ms(unsigned int times);
void delay_10us(unsigned int times);
unsigned char iic_write_check(unsigned char addr, unsigned char reg, unsigned char val);
unsigned char iic_read_byte(unsigned char addr, unsigned char reg);
void iic_write_byte(unsigned char addr, unsigned char reg, unsigned char val);


void init_sys(void);
void spi_send_byte(unsigned char addr, unsigned char val);
void flcos_send(void);
unsigned char buf;


void main(void)
{
	unsigned char val;
	init_sys();
	for(;;)
	{
	
			delay_ms(10);
			flcos_send();
		
		delay_ms(10);
		if(BL_U == L)
		{
			val = iic_read_byte(0xB8, 0x09);
			if((int)val + 3 > 255)
			{
				val = 255;
			}
			else
			{
				val += 3;
			}
			iic_write_byte(0xB8, 0x09, val);	
		}
		else
		{
			if(BL_D == L)
			{
				val = iic_read_byte(0xB8, 0x09);
				if((int)val - 3 < 0)
				{
					val = 0;
				}
				else
				{
					val -= 3;
				}
				iic_write_byte(0xB8, 0x09, val);		
			}
		}
		delay_ms(10);	
	}

}

void init_sys(void)
{

	TVP_SDA = H;
	TVP_SCL = H;
	FLCOS_SDAT = H;
	FLCOS_SCLK = H;
	FLCOS_SEN = H;
	FLCOS_nRST = H;
	TVP_RESETB = H;
	TVP_PDN = H;
	BL_U = H;
	BL_D = H;
	delay_ms(10);
	TVP_RESETB = L;
	delay_ms(10);
	TVP_RESETB = H;
	delay_ms(100);

	iic_write_check(0xB8, 0x03, 0xFD);
  iic_write_check(0xB8, 0x04, 0x00);
	iic_write_check(0xB8, 0x0D, 0x40);
	iic_write_check(0xB8, 0x12, 0x04);
	iic_write_check(0xB8, 0x16, 0x68);
	iic_write_check(0xB8, 0x09, 0xB1);
	//while(!iic_write_check(0xB8, 0x09, 0xB1));

	FLCOS_nRST = L;
	delay_ms(10);
	FLCOS_nRST = H;
	delay_ms(10);

}

void spi_send_byte(unsigned char addr, unsigned char val)
{
	int index;
	FLCOS_SEN = L;
	delay_10us(2);

	for(index = 7; index>=0; index--)
	{
		if((addr>>index)&0x1 == 1)
		{
			FLCOS_SDAT = H;
		}
		else
		{
			FLCOS_SDAT = L;
		}
		delay_10us(1);
		FLCOS_SCLK = H;
		delay_10us(1);
		FLCOS_SCLK = L;
		delay_10us(1);
	}

	delay_10us(2);

	for(index = 7; index>=0; index--)
	{
		if((val>>index)&0x1 == 1)
		{
			FLCOS_SDAT = H;
		}
		else
		{
			FLCOS_SDAT = L;
		}
		delay_10us(1);
		FLCOS_SCLK = H;
		delay_10us(1);
		FLCOS_SCLK = L;
		delay_10us(1);
	}

	delay_10us(2);
   	FLCOS_SEN = H;
	delay_10us(2);
}

void flcos_send(void)
{	
	spi_send_byte(0x01, 0x35);
	spi_send_byte(0x04, 0x08);
	spi_send_byte(0x05, 0x00);
	spi_send_byte(0x07, 0x08);
	spi_send_byte(0x08, 0xEB);
	spi_send_byte(0x09, 0x10);
	spi_send_byte(0x0A, 0x16);
	spi_send_byte(0x0B, 0x05);
	spi_send_byte(0x0C, 0x16);
	spi_send_byte(0x06, 0x48); //如果水平镜像，则改成0x68
}


void delay_ms(unsigned int times)
{
	unsigned int count;
 	while(times--)
	{
		count = 0x106;
		while(count--);
	}
}


void delay_10us(unsigned int times)
{
 	while(times--);
}



void iic_start(void)
{
	SDA = H;
	SCL = H;
	delay_10us(1);

	SDA = L;
  delay_10us(1);
	SCL = L;
	delay_10us(1);
}

void iic_stop(void)
{
	SDA = L;
	SCL = H;
	delay_10us(1);
	SDA = H;
	delay_10us(1);
}

unsigned char iic_read_byte(unsigned char addr, unsigned char reg)
{
	int i;
	unsigned char ret;

	addr >>= 1;
	addr <<= 1;
	addr |= 0;

	iic_start();

	for(i = 7; i >= 0; i--)
	{
		if((addr>>i)&0x01 == 1)
		{
			SDA = H;
		}
		else
		{
			SDA = L;
		}
		delay_10us(1);
		SCL = L;
		delay_10us(1);
		SCL = H;
		delay_10us(1);
		SCL = L;
		SDA = H;
		delay_10us(1);
	}
	
	while(SDA);
	delay_10us(1);
	SCL = L;
	delay_10us(1);
	SCL = H;
	delay_10us(1);
	SCL = L;
	delay_10us(1);

	for(i = 7; i >= 0; i--)
	{
		if((reg>>i)&0x01 == 1)
		{
			SDA = H;
		}
		else
		{
			SDA = L;
		}
		delay_10us(1);
		SCL = L;
		delay_10us(1);
		SCL = H;
		delay_10us(1);
		SCL = L;
		SDA = H;
		delay_10us(1);
	}
	
	while(SDA);
	delay_10us(1);
	SCL = L;
	delay_10us(1);
	SCL = H;
	delay_10us(1);
	SCL = L;
	delay_10us(1);

	//-----------------------------
	addr >>= 1;
	addr <<= 1;
	addr |= 1;

	iic_start();

	for(i = 7; i >= 0; i--)
	{
		if((addr>>i)&0x01 == 1)
		{
			SDA = H;
		}
		else
		{
			SDA = L;
		}
		delay_10us(1);
		SCL = L;
		delay_10us(1);
		SCL = H;
		delay_10us(1);
		SCL = L;
		SDA = H;
		delay_10us(1);
	}
	
	while(SDA);
	delay_10us(1);
	SCL = L;
	delay_10us(1);
	SCL = H;
	delay_10us(1);
	SCL = L;
	delay_10us(100);
 	
	ret = 0;
	for(i = 7; i >= 0; i--)
	{
		delay_10us(1);
		SCL = L;
		delay_10us(1);
		SCL = H;
		delay_10us(1);
		ret <<= 1;
		if(SDA == H)
		{
			ret |= 1;
		}		
	}

	delay_10us(1);
	SCL = L;
	delay_10us(1);
	SCL = H;
	delay_10us(1);
	SCL = L;
	delay_10us(1);
	
	iic_stop();	
	
	return ret;		
}

void iic_write_byte(unsigned char addr, unsigned char reg, unsigned char val)
{
	int i;

	addr >>= 1;
	addr <<= 1;
	addr |= 0;

	iic_start();

	for(i = 7; i >= 0; i--)
	{
		if((addr>>i)&0x01 == 1)
		{
			SDA = H;
		}
		else
		{
			SDA = L;
		}
		delay_10us(1);
		SCL = L;
		delay_10us(1);
		SCL = H;
		delay_10us(1);
		SCL = L;
		SDA = H;
		delay_10us(1);
	}
	
	while(SDA);
	delay_10us(1);
	SCL = L;
	delay_10us(1);
	SCL = H;
	delay_10us(1);
	SCL = L;
	delay_10us(1);

	for(i = 7; i >= 0; i--)
	{
		if((reg>>i)&0x01 == 1)
		{
			SDA = H;
		}
		else
		{
			SDA = L;
		}
		delay_10us(1);
		SCL = L;
		delay_10us(1);
		SCL = H;
		delay_10us(1);
		SCL = L;
		SDA = H;
		delay_10us(1);
	}
	
	while(SDA);
	delay_10us(1);
	SCL = L;
	delay_10us(1);
	SCL = H;
	delay_10us(1);
	SCL = L;
	delay_10us(1);

	for(i = 7; i >= 0; i--)
	{
		if((val>>i)&0x01 == 1)
		{
			SDA = H;
		}
		else
		{
			SDA = L;
		}
		delay_10us(1);
		SCL = L;
		delay_10us(1);
		SCL = H;
		delay_10us(1);
		SCL = L;
		SDA = H;
		delay_10us(1);
	}
	
	while(SDA);
	delay_10us(1);
	SCL = L;
	delay_10us(1);
	SCL = H;
	delay_10us(1);
	SCL = L;
	delay_10us(1);
	
	iic_stop();		
}

unsigned char iic_write_check(unsigned char addr, unsigned char reg, unsigned char val)
{
	iic_write_byte(addr, reg, val);
	delay_ms(20);
	if(iic_read_byte(addr, reg) != val)
	{

		return 0;
	}
	else
	{

	}
	return 1;
}
