#include <18F4550.h>
#fuses HS,NOPROTECT,NOWDT,NOMCLR,NOPBADEN
#use delay(clock=20000000)
#use i2c(master, sda= PIN_B0, scl= PIN_B1)//verificar sempre no hardware
#define RTC_SCLK PIN_B1
#define RTC_IO   PIN_B0
#define RTC_RST  PIN_A4
#include  <ds1302.c>

void init_ext_eeprom() {
   output_float(PIN_B1);
   output_float(PIN_B0);
}

int1 ext_eeprom_ready() {
   int1 ack;
   i2c_start();            // If the write command is acknowledged,
   ack = i2c_write(0xa0);  // then the device is ready.
   i2c_stop();
   return !ack;
}

void write_ext_eeprom(unsigned int16 address, BYTE data) {
   while(!ext_eeprom_ready());
   i2c_start();
   i2c_write((0xa0|(BYTE)(address>>7))&0xfe);
   i2c_write(address);
   i2c_write(data);
   i2c_stop();
}


BYTE read_ext_eeprom(unsigned int16 address) {
   BYTE data;

   while(!ext_eeprom_ready());
   i2c_start();
   i2c_write((0xa0|(BYTE)(address>>7))&0xfe);
   i2c_write(address);
   i2c_start();
   i2c_write((0xa0|(BYTE)(address>>7))|1);
   data=i2c_read(0);
   i2c_stop();
   return(data);
}

void main(void)
{	
	
	int hora,minuto,segundo = 0;
	int aminuto = 0;
	int i,k = 0;
	int memoria[4];
	unsigned int valor;
	float tensao;
	float temperatura;
	int itemp;
	
	//rtc
	rtc_init();
	rtc_set_datetime(10,11,15,3,10,28);
	
	//adc
	setup_adc( ADC_CLOCK_INTERNAL ); 
	setup_adc_ports( AN0 );
	set_adc_channel(0);

	init_ext_eeprom();

	while(true)
	{
		valor = read_adc();
		tensao = valor*0.0196;
		temperatura = tensao *100;
		itemp = (int)temperatura;
		
		rtc_get_time(hora,minuto,segundo);
		
		if ( minuto != aminuto)
		{
			memoria[0]=	hora;
			memoria[1]=	minuto;
			memoria[2]= segundo;
			memoria[3]= itemp;
			
			aminuto = minuto;

			for(i=0;i<=3;i++)
			{
			write_ext_eeprom(k,memoria[i]);
			k++;
			}
			output_high(PIN_D1);
			
		}
		
		
		
		delay_ms(1000);
		output_low(PIN_D1);
			

	}	
	

}
