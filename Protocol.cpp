#include "Protocol.h"


/////////////////////////////////////////////////////
unsigned short HMI_protocol::crc16(byte *buffer, unsigned char buffer_length)
{
    unsigned char crc_hi = 0xFF;    // high CRC byte initialized 
    unsigned char crc_lo = 0xFF;    // low CRC byte initialized 
    unsigned char i;                // will index into CRC lookup

    // pass through message buffer 
    while (buffer_length--) {
        i = crc_hi ^ *buffer++;     // calculate the CRC 
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}
/////////////////////////////////////////////////////
unsigned char HMI_protocol::checksum_data (byte *uart_read, unsigned char len_read)
{
  unsigned short crc;
  int i_p = 0;

  st_uart.adress   = uart_read[0];
  st_uart.function = uart_read[1];
  st_uart.tranid   = uart_read[2];
  st_uart.len      = uart_read[3]*256 + uart_read[4];
  st_uart.crc      = uart_read[(st_uart.len+5)]*256 + uart_read[(st_uart.len+6)];
    
  if(st_uart.adress != ADRESS)        return ERR;  //Adress of button & printer
  if((st_uart.len+7) > len_read)      return ERR_FRAME;
  if((st_uart.tranid) == tran_id)  return LOOP_TRANID;


  crc = crc16(uart_read, st_uart.len + 5);
  //st_uart.crc = crc16(uart_read, st_uart.len + 5); //test
  if(crc == st_uart.crc) {
      if(st_uart.len > 0) {
          for(i_p=0; i_p<st_uart.len; i_p++){ 
            st_uart.uart_data[i_p] = uart_read[i_p+5];
          }
      }
      tran_id = st_uart.tranid;
      return PAS;
  }
  else {
      return ERR;
  }
}
