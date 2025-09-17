#include<lpc21xx.h>

typedef unsigned char u8;

#define   AA_BIT 2
#define   SI_BIT 3
#define  STO_BIT 4
#define  STA_BIT 5
#define I2EN_BIT 6
#define CCLK      60000000  //Hz
#define PCLK      CCLK/4    //Hz
#define I2C_SPEED 100000    //Hz
#define LOADVAL   ((PCLK/I2C_SPEED)/2)

# define LCD_P (0xFF << 10)	//p0.10 to p0.17 as data pins
# define RS_P 0x00000100	//p0.8 is enabled as Register Select 
# define E_P 0x00000200		//p0.9 is enabled as Enable

void delay_ms(unsigned int);
void init_i2c(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_restart(void);
void i2c_write(u8);
u8 i2c_read(void);
u8 i2c_nack(void);
void i2c_eeprom_write(u8,u8,u8);
u8   i2c_eeprom_read(u8,u8);

void LCD_INIT(void);
void LCD_COMMAND(unsigned int cmd);
void LCD_DATA(unsigned int d);
void delay_ms(unsigned int ms);

void LCD_INIT(void)
{
    IODIR0 |= LCD_P|RS_P|E_P;  //refer line no. : 15-17 in the above...
    LCD_COMMAND(0x01);
    LCD_COMMAND(0x02);
    LCD_COMMAND(0x0C);
    LCD_COMMAND(0x38);
}

void LCD_COMMAND(unsigned int cmd)
{
    IOCLR0 = LCD_P;
    IOSET0 = (cmd << 10);  //Since the LCD pins are connected from p0.10 - p0.17..., so need to shift the cmd to these pins
    IOCLR0 = RS_P;
    IOSET0 = E_P;
    delay_ms(5);
    IOCLR0 = E_P;
}

void LCD_DATA(unsigned int d)
{
    IOCLR0 = LCD_P;
    IOSET0 = (d << 10);  //Since the LCD pins are connected from p0.10 - p0.17..., so need to shift the cmd to these pins
    IOSET0 = RS_P;
    IOSET0 = E_P;
    delay_ms(5);
    IOCLR0 = E_P;
}

void init_i2c(void)
{
	 PINSEL0 = 0X00000050;		//p0.2&p0.3 is I2C pins(p0.2 - SCL & p0.3 - SDA)
	 I2SCLL=LOADVAL;
	 I2SCLH=LOADVAL;
	 I2CONSET=1<<I2EN_BIT; 
}

void i2c_start(void)
{
	I2CONSET=1<<STA_BIT;
	while(((I2CONSET>>SI_BIT)&1)==0);
	I2CONCLR=1<<STA_BIT;
}

void i2c_restart(void)
{
	I2CONSET=1<<STA_BIT;
	I2CONCLR=1<<SI_BIT;
	while(((I2CONSET>>SI_BIT)&1)==0);
	I2CONCLR=1<<STA_BIT;
}

void i2c_write(u8 dat)
{
	I2DAT=dat;
	I2CONCLR = 1<<SI_BIT;
	while(((I2CONSET>>SI_BIT)&1)==0);
}

void i2c_stop(void)
{
	I2CONSET=1<<STO_BIT;
	I2CONCLR = 1<<SI_BIT;
}									   
u8 i2c_nack(void)
{
	I2CONSET = 0x00; //Assert Not of Ack
  	I2CONCLR = 1<<SI_BIT;
	while(((I2CONSET>>SI_BIT)&1)==0);
	return I2DAT;
}

void i2c_eeprom_write(u8 slaveAddr,u8 wBuffAddr,u8 dat)
{
	i2c_start();	
 	i2c_write(slaveAddr<<1); //slaveAddr + w
	i2c_write(wBuffAddr);    //wBuffAddr
	i2c_write(dat);   	//data
	i2c_stop();
	delay_ms(10);
}			 

u8 i2c_eeprom_read(u8 slaveAddr,u8 rBuffAddr)
{
	u8 dat;
	i2c_start();	
  	i2c_write(slaveAddr<<1); //slaveAddr + w
	i2c_write(rBuffAddr);    //rBuffAddr
	i2c_restart();	
	i2c_write(slaveAddr<<1|1); //slaveAddr + r  
  	dat=i2c_nack();	
	i2c_stop();
	return dat;
}

void delay_ms(unsigned int ms)
{
	 T0PR=14999;
	 T0TCR=0X01;
	 while(T0TC<ms);
	 T0TCR=0X03;
	 T0TCR=0X00;
}

 /*
int main()
{
	u8 ch;
	init_i2c();
	LCD_INIT();
	LCD_COMMAND(0x01);
	i2c_eeprom_write(0X50,0X00,'A');	 //s1 should be written on eeprom
//	i2c_eeprom_read(0X50,0X00);
	ch=i2c_eeprom_read(0X50,0X00);	//the data whicvh is written on the eeprom has to be read
	LCD_DATA(ch);
}
   */
