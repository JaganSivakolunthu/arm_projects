#include<lpc21xx.h>
#include"i2c_lcd.h"  //the i2c_lcd.c should be converted into header.h

#define led1 (1<<0)	//p0.0 is connected to the LED(active-low in ***** Board)
#define led2 (1<<5)	//p0.5 is connected to the LED(active-low in ***** Board)
#define ir1_pin (1<<6)	//p0.6 is connected to the IR_1 op pin
#define ir2_pin (1<<7)	//p0.7 is connected to the IR_2 op pin

void LCD_STRING(const char *ptr)
{
    while(*ptr)
    {
        LCD_DATA(*ptr);
        ptr++;
    }
}

int main()
{
	u8 s1=0;		 // if '0' is used, in lcd_data, don't add +48
							// if 0 is used, in lcd_data, add +48
	init_i2c();
	IODIR0 |= led1|led2;	  // always see the IODIR to check the output of the LED 
	LCD_INIT();
	LCD_COMMAND(0x01);
	LCD_COMMAND(0x80);
	LCD_STRING(" Automatic room ");
	LCD_COMMAND(0xc0);
	LCD_STRING("counter - ce9j1");
	delay_ms(1000);
	s1 = i2c_eeprom_read(0X50,0X00);	//the data which is written on the eeprom has to be read
	while(1)
	{
		if((IOPIN0 & ir1_pin)==0)
			{
				if(s1<10)	s1++;
				while(0);	//dummy statement
				if((s1>0)&&(s1<5))	
					{
						IOCLR0 = led1;
						//LCD_DATA((IOPIN0 >> 15) & 1 + 48); // Check LED1 pin status
						IOSET0 = led2;
					}
				else if((s1>=5)&&(s1<10))	IOCLR0 = led1|led2;
				else if(s1>=10){
						LCD_COMMAND(0x01);
						LCD_STRING("Maxlimit reached");
					}
				else if(s1==0){
						LCD_COMMAND(0x01);
						LCD_STRING("Room is Empty ");
						IOSET0 = led1|led2;
					}
				while(0);	//dummy statement
				if((s1)&&(s1<10)){
						LCD_COMMAND(0x01);
						LCD_STRING("Room Counter:");
						LCD_DATA(s1 + 48);
					}
				while((IOPIN0 & ir1_pin) == 0);  // Wait until sensor is released
    			delay_ms(200); 
				i2c_eeprom_write(0X50,0X00,s1);	//will write the data on the EEPROM
			}
		while(0);	//dummy statement
		if((IOPIN0 & ir2_pin)==0)
			{
				if(s1>0)	s1--;
				LCD_COMMAND(0x01);
				if((s1>0)&&(s1<5))	
					{
						IOCLR0 = led1;
						IOSET0 = led2;
					}
				else if((s1>5)&&(s1<10))	IOCLR0 = led1|led2;
			/*	else if(s1==10){
						LCD_STRING("Maxlimit reached");
					}	   */
				else if(s1==0)
					{
						LCD_COMMAND(0x01);
						LCD_STRING("Room is Empty ");
						IOSET0 = led1|led2;
					}
				while(0);	//dummy statement
				if(s1){
						LCD_STRING("Room Counter:");
						LCD_DATA(s1 + 48);
					}
				while((IOPIN0 & ir2_pin) == 0);  // Wait until sensor is released
    			delay_ms(200); 
				i2c_eeprom_write(0X50,0X00,s1);	//will write the data on the EEPROM
			}
	}
}
