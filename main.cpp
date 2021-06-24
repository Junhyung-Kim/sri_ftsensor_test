#include <iostream>
#include <sys/types.h>
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>

int main()
{
	//Serial Setting
	int fd, fd1, a;
	char serialport[]= "/dev/ttyUSB0";
	fd = open(serialport, O_RDONLY | O_NOCTTY );

	if(fd<0)
	{
		printf("serial port is error");
	}
	else
	{
		fcntl(fd, F_SETFL, 0);
		printf("port is open.\n");
	}

	struct termios port_settings;      
	cfsetispeed(&port_settings, B230400);    
	cfsetospeed(&port_settings, B230400);
	port_settings.c_cflag |= CS8;
	port_settings.c_cflag &= ~PARODD;	
	port_settings.c_cflag &= ~CSTOPB;

	int c =	tcsetattr(fd, TCSANOW, &port_settings); 
	if(c<0)
	{
		printf("serial port setting is error");

		return 0;
	}
	else
	{
		printf("serial port OK.\n");
	}

	uint8_t buf;
	memset (&buf, '\0', sizeof buf);
	
	uint32_t ft_buf[6];
	uint8_t rx_packet[300];
	uint16_t rx_length = 0;
	uint16_t wait_length = 34;
	float ft_data[6];

	while(true)
	{
		rx_length += read(fd,&buf,sizeof buf);

		if(rx_length < 100)
		{
			rx_packet[rx_length - 1] = buf;
		}		

		if(rx_length >= wait_length)
		{
		   uint16_t idx = 0;
			
		   //find packet header
		   for(idx = 0; idx < (rx_length - 2) ;idx++)
		   {
			if(rx_packet[idx] == 0xAA && rx_packet[idx+1] == 0x55)
			{
				rx_length = 0;
				break;
			}
		   }


		   //force data
		   ft_buf[0] = rx_packet[idx+9];
		   ft_buf[1] = rx_packet[idx+13];	
		   ft_buf[2] = rx_packet[idx+17];

		   //moment data
		   ft_buf[3] = rx_packet[idx+21];
		   ft_buf[4] = rx_packet[idx+25];	
		   ft_buf[5] = rx_packet[idx+29];

		   for(int i = 0; i < 6; i++)
  		   {
			for(int j = 1; j < 4; j++)
			{
				ft_buf[i] = (ft_buf[i] << 8) | rx_packet[idx+9+4*i-j];
			}

    	                memcpy(&f[i], &ft_buf[i], sizeof(ft_buf[i]));
		   }
		}
	}

 	return 0;
}


