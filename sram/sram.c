#include "sram.h"
#include "stdint.h"
//#include <user_board.h>
#include "asf.h"
#include "cdc.h"
#include <string.h>

long int count= 0;
uint16_t address = 0; //so address can go to 65535 if necessary
uint16_t new_temp;
uint16_t for_read;
uint16_t byte_count = 0;
struct spi_device spi_device_conf = 
{
          .id = SRAM_CS_pin
};



void sram_init(void)
{
	
	ioport_set_pin_dir(SRAM_CS_pin, IOPORT_DIR_OUTPUT);  //CS
	ioport_set_pin_dir(SRAM_MOSI_pin, IOPORT_DIR_OUTPUT); //SI
	ioport_set_pin_dir(SRAM_MISO_pin, IOPORT_DIR_INPUT); //SO
	ioport_set_pin_dir(SRAM_SCK_pin, IOPORT_DIR_OUTPUT);//SCK
	ioport_set_pin_dir(SRAM_HOLD_pin, IOPORT_DIR_OUTPUT);
	
        ioport_set_pin_level(SRAM_CS_pin, IOPORT_INIT_HIGH); 
	ioport_set_pin_level(SRAM_HOLD_pin,IOPORT_INIT_HIGH);
}


void spi_init_module(void)
{
      
     

      spi_master_init(&SPIC);
      spi_master_setup_device(&SPIC, &spi_device_conf, SPI_MODE_0, 1000000, 0);
      spi_enable(&SPIC);
}


void write_mode(uint8_t mode)
{
	uint8_t instruction_w =  1;/*WRSR instruction to allow setting of 
                                    operation mode for read and write
                                    operations*/
	uint8_t mode_w;

       if (mode== 1) //Sequential Mode
         {
            mode_w = 64;
         }
       else if(mode==2)//Page Mode
         {
            mode_w =128;
         }
       else //Byte Mode
         {
            mode_w = 0;
         }
      

	spi_select_device(&SPIC, &spi_device_conf);/*WRSR Sequence Begin*/
	                                          /*Per page 12 of datasheet*/
                                                  /*bringing CS Low since using 
                                                  select_device function brings
                                                  device low and you must bring 
                                                  device low prior to write 
                                                  attempt*/
	
	spi_write_packet(&SPIC, &instruction_w, 1);/*Sends WRSR instruction*/
	
	spi_write_packet(&SPIC, &mode_w, 1); /*Sends command to put SRAM in 
					      chosen write 
                                              mode.Tells it to send 1 byte as  
                                              last argument and also & so that 
                                              it to address of value in status*/

	spi_deselect_device(&SPIC, &spi_device_conf);/*End WRSR sequence*/
						     /*Bring CS High to end 
                                                      writing*/
                                                     /*Ends per page 12 of 
                                                       datasheet*/
	

}

void write_data(uint8_t data[])
{

       
	uint16_t write_num_bytes = strlen((const char*)data)+1;//casting array to
                                                           //const char and adding 
                                                           //one so that null 
                                                           //char included
	for_read = write_num_bytes;
	byte_count = for_read + byte_count;
	//cdc_write_hex(write_num_bytes);
	//cdc_newline();

	if (count >= 1)
        {	
           address = new_temp; //address gets set equal to next free memory byte
        }
	
	cdc_write_hex(address);
	cdc_newline();

	
	uint8_t writecommand = 2;
        uint8_t LSB = (uint8_t) address;//stores LSB of address in variable
	uint8_t MSB = (uint8_t) (address>>8);//stores MSB of address in variable
	uint8_t address_var[2]; //array of 2 8 bit values to be used to send 16 
                                //bits to chip for addressing read/write 
                                //purposes 

	address_var[0] = MSB;
        address_var[1] = LSB;
	
	spi_select_device(&SPIC, &spi_device_conf);/*Begin Write 
                                                    Sequence*/
                                                  /*bringing CS Low since using 
                                                  select_device function brings
                                                  device low and you must bring 
                                                  device low prior to      
                                                  write       
                                                  attempt*/
	
	spi_write_packet(&SPIC, &writecommand, 1);//Sends command to write 
	
	spi_write_packet(&SPIC, address_var, 2);/*Sends the 2 byte address to 
	                                         SRAM for writing*/
        
 

	spi_write_packet(&SPIC, data, write_num_bytes);//writes data to SRAM. 
                                                     //Each char 
                                                    //is a byte
	
		

	spi_deselect_device(&SPIC, &spi_device_conf);/*End Write Sequence*/
                                                     /*Bring CS High to end 
                                                     writing*/
	
	count++;
        new_temp= address + write_num_bytes-1; /*sets it so new_temp holds value of 
                                             address*/
	/*cdc_write_hex(new_temp);
	cdc_newline();*/
	


	

	
	

}



void read_data(uint16_t read_num_bytes,uint8_t user_array[])
{
	
	if(read_num_bytes>byte_count) //used in case user specifies more bytes to 
                                      //read than were previously written
	{
	   read_num_bytes = byte_count;
           byte_count = 0;
        }

	address = 0;
	uint8_t readcommand = 3;
	uint8_t LSB = (uint8_t) address;//stores LSB of address in variable
	uint8_t MSB = (uint8_t) (address>>8);//stores MSB of address in variable
	uint8_t address_var[2]; //array of 2 8 bit values to be used to send 16 
                                //bits to chip for addressing read/write 
                                //purposes
        address_var[0] = MSB;
        address_var[1] = LSB;
	
	spi_select_device(&SPIC, &spi_device_conf);/*Begin Read 
                                                   Sequence*/
	                                         

	spi_write_packet(&SPIC, &readcommand, 1);//Sends command to read 

	spi_write_packet(&SPIC, address_var, 2);/*Sends the 2 byte address to  
                                                 SRAM for reading*/
			
        spi_read_packet(&SPIC, user_array, read_num_bytes);//write read data to 
                                                      //allocated variable

        spi_deselect_device(&SPIC, &spi_device_conf);/*End Read 
                                                      Sequence*/ 
                                                     /*Bring CS High to end 
                                                       writing*/
 	user_array[read_num_bytes] = '\0';//so that removed null is put back into 
                                          //array avoiding erroneous output
	
	//Erasing previously written data and starting address back at beginning
	address =0;
	uint8_t writecommand = 2;
	uint8_t clear[2] = " ";
        uint8_t LSB_c = (uint8_t) address;//stores LSB of address in variable
	uint8_t MSB_c = (uint8_t) (address>>8);//stores MSB of address in variable
	uint8_t address_var_c[2]; //array of 2 8 bit values to be used to send 16 
                                //bits to chip for addressing read/write 
                                //purposes 

	address_var_c[0] = MSB_c;
        address_var_c[1] = LSB_c;
	
	spi_select_device(&SPIC, &spi_device_conf);/*Begin Write 
                                                    Sequence*/
                                                  /*bringing CS Low since using 
                                                  select_device function brings
                                                  device low and you must bring 
                                                  device low prior to      
                                                  write       
                                                  attempt*/
	
	spi_write_packet(&SPIC, &writecommand, 1);//Sends command to write 
	
	spi_write_packet(&SPIC, address_var_c, 2);/*Sends the 2 byte address to 
	                                         SRAM for writing*/
        
 

	spi_write_packet(&SPIC, clear, for_read);//writes data to SRAM. 
                                                     //Each char 
                                                    //is a byte
	
		

	spi_deselect_device(&SPIC, &spi_device_conf);/*End Write Sequence*/
                                                     /*Bring CS High to end 
                                                     writing*/
	
	address =0; //going back to beginning
	count = 0;//resetting count so that write function starts at begining
	new_temp = 0;//resetting new_temp so that write function starts at begining
	
      
}






