#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "basetype.h"
#include "utl_vt.h"
#include "usart.h"
#include "delay.h"

const static UI8_T  print_initscreen[][80]=
{
  "\n\r******************************************************************",
  "\n\r*                                                                *",
  "\n\r*                       Lori's Card                              *",
  "\n\r*                                                                *",
  "\n\r*              Copyright 2019-2020.All Rights Reserved           *",
  "\n\r*                                                                *",
  "\n\r******************************************************************",
};


void UTL_VT_InitScreen(void)
{

    UTL_VT_PutString(print_initscreen[0]);
    UTL_VT_PutString(print_initscreen[1]);
    UTL_VT_PutString(print_initscreen[2]);
    UTL_VT_PutString(print_initscreen[3]);
    UTL_VT_PutString(print_initscreen[4]);
    UTL_VT_PutString(print_initscreen[5]);
    UTL_VT_PutString(print_initscreen[6]);
    return;
}

/*******************************************************************************
 * FUNCTION NAME:
 *      None
 * DESCRIPTION:
 *      None
 * INPUT:
 *      None
 * OUTPUT:
 *      None
 * RETURN:
 *      None
 * NOTES:
 *      None
 * HISTORY:
 *      Ver1.00     2007.02.14      Panda Xiong         Create
*******************************************************************************/
I8_T UTL_VT_ReadKey(void)
{
    static I8_T  lastkeys[2] = {-1, -1};
    I8_T    key, key1;
    UI16_T  i,   comb_key_timeout;

    comb_key_timeout = 100;

    for (i=0; i<2; i++)
    {
        if (lastkeys[i] != -1)
        {
            key = lastkeys[i];
            lastkeys[i] = -1;
            return key;
        }
    }

    while ((key = UTL_VT_GetByte()) == -1)
    {}
    if (key != KEY_ESC)
    {
        return key;
    }
    
    /* wait '[' or 'O' */
    for (i=0; i<comb_key_timeout; i++)
    {
		msleep(10);
		key = UTL_VT_GetByte();
        if (key != -1)
            break;
      
    }

    if (key == -1)
        return KEY_ESC;
   
    switch (key)
    {
        case '[':
        case 'O':
            key1 = key;
            break;
        default:
            lastkeys[0] = key;
            return KEY_ESC;
    }

    /* wait 'A/B/C/D' or 'P/Q/R/S' */
    for (i=0; i<comb_key_timeout; i++)
    {
		msleep(10); //必须增加此延时，否则无法收到组合键
        key = UTL_VT_GetByte();
        if (key != -1)
            break;
    }

    if (key == -1)
        return KEY_ESC;
 
    switch (key1)
    {
        case '[':
            switch (key)
            {
                case 'A':
                    key = (I8_T)KEY_UP;
                    break;
                case 'B':
                    key = (I8_T)KEY_DOWN;
                    break;
                case 'C':
                    key = (I8_T)KEY_RIGHT;
                    break;
                case 'D':
                    key = (I8_T)KEY_LEFT;
                    break;
                case 'H':
		    key = (I8_T)KEY_HOME;
		    break;
		case 'K':
		    key = (I8_T)KEY_END;
		    break;
	        case '@':
		    key = (I8_T)KEY_INSERT;
		    break;            
                default:
                    lastkeys[0] = '[';
                    lastkeys[1] = key;
                    key = KEY_ESC;
                    break;
            }
            break;

        case 'O':
            switch (key)
            {
                case 'P':
                    key = (I8_T)KEY_F1;
                    break;
                case 'Q':
                    key = (I8_T)KEY_F2;
                    break;
                case 'R':
                    key = (I8_T)KEY_F3;
                    break;
                case 'S':
                    key = (I8_T)KEY_F4;
                    break;
                default:
                    lastkeys[0] = 'O';
                    lastkeys[1] = key;
                    key = KEY_ESC;
                    break;
            }
            break;

        default:
            break;
    }

    return key;
}




void UTL_VT_ClearRight(void)
{
    UI8_T  escapeStr[4];
    UI8_T *tempPtr;

   

    tempPtr = escapeStr;
    *tempPtr++ = KEY_ESC;
    *tempPtr++ = KEY_BRACKET;
    *tempPtr++ = 'K';
    *tempPtr   = KEY_NULL;

    UTL_VT_PutString(escapeStr);
}




void UTL_VT_MoveLeft( const UI8_T columns)
{
    UI8_T  escapeStr[6];
    UI8_T *tempPtr;

    if (columns == 0)
    {
        return;
    }

    tempPtr = escapeStr;
    *tempPtr++ = KEY_ESC;
    *tempPtr++ = KEY_BRACKET;
    *tempPtr++ = 0x30+(columns/10);
    *tempPtr++ = 0x30+(columns%10);
    *tempPtr++ = 'D';
    *tempPtr   = KEY_NULL;

    UTL_VT_PutString(escapeStr);
}




void UTL_VT_MoveRight( const UI8_T columns)
{
    UI8_T  escapeStr[5];
    UI8_T *tempPtr;

    if (columns == 0)
    {
        return;
    }

    tempPtr = escapeStr;
    *tempPtr++ = KEY_ESC;
    *tempPtr++ = KEY_BRACKET;
    *tempPtr++ = 0x30+columns;
    *tempPtr++ = 'C';
    *tempPtr   = KEY_NULL;

    UTL_VT_PutString(escapeStr);
}





void UTL_VT_PutAscii(const UI8_T *p_str, UI8_T size)
{
    if (POINTER_CHECK(p_str))
    {
        return;
    }

    while (size-- != 0)
    {
        UTL_VT_PutByte(*p_str++);
    }
}

unsigned char crc8(unsigned char *buf)
{
unsigned char i,j,crc; 

    crc=0;
    for(j=1;j<7;j++)
    {
	crc=crc^(*buf);
	for(i = 8; i > 0; i--)
        {
            if(crc & 0x80)
	        crc = (crc<< 1)^0x31;
	    else
                crc = crc<< 1;
        }
        buf++;
    }
    return crc;
}

const char CRC8Table[]={
  0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
  157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
  35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
  190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
  70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
  219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
  101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
  248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
  140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
  17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
  175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
  50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
  202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
  87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
  233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
  116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};
unsigned char CRC8_Table(unsigned char *p, char counter)
{
    unsigned char crc8 = 0;

    for( ; counter > 0; counter--){
        crc8 = CRC8Table[crc8^*p];
        p++;
    }
   
      
    return(crc8);

}


/*************************************************************
    描    述：  CRC16校验查表子,采用CRC16-CCITT x16+x12+x5+1
**************************************************************/
const static  unsigned int CRC16Table[256]={               /* CRC16 余式表 */
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};



/*************************************************************
    描    述：  CRC16校验查表子程序,速度快，占用空间大
    入口参数：  指向数组指针，校验字节个数
    出口参数：  16位CRC校验码
**************************************************************/
unsigned int CRC16_Table(unsigned char *pcrc, int count)
{
    unsigned int  crc16 = 0;
    unsigned char crcregister;

    for( ; count > 0; count--){
        crcregister = (unsigned int)crc16 >> 8;
        crc16 <<= 8;
        crc16 ^= CRC16Table[crcregister ^ *pcrc];
        pcrc++;
    }
    return(crc16);
}


//#define CON_putchar(data,len) UARTSend(UART_INDX_1,(UI8_T *)(data),(len))
void CON_putchar(const char *data,UI16_T len)
{
	if(len)
		while(len--)
	    {
			UartSendByte(UART_INDX_3,*data++);
		}
}

void print_buf(unsigned char * pBuf,  int lLen)
{
   unsigned long i;
   if(lLen<=0)
   	return;
   for(i=0;i<lLen;i++)
   {
   	  if(i%16==0)
	  {
		  printf("\r\n%d: ",i);
	  }
	  printf("%2x",pBuf[i]);
	  printf(" ");
	 
	  	
   }
   printf("\r\n");
}




