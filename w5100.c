/*
 *  w5100.c      library of target-independent AVR support routines
 *               for the Wiznet W5100 Ethernet interface device
 *
 *  This file is derived from the excellent work found here:
 *  www.ermicro.com/blog/?p=1773
 *  by RWB.  I am leaving the header from the original file intact below,
 *  but you need to remember the rest of the source here is fairly
 *  heavily modified.  Go to the above site for the original.
 */

/*****************************************************************************
//  File Name    : wiznetping.c
//  Version      : 1.0
//  Description  : Wiznet W5100
//  Author       : RWB
//  Target       : AVRJazz Mega168 Board
//  Compiler     : AVR-GCC 4.3.2; avr-libc 1.6.6 (WinAVR 20090313)
//  IDE          : Atmel AVR Studio 4.17
//  Programmer   : AVRJazz Mega168 STK500 v2.0 Bootloader
//               : AVR Visual Studio 4.17, STK500 programmer
//  Last Updated : 01 July 2010
*****************************************************************************/


/*
 *  The following code turns the above wiznetping.c source code into a
 *  generic library of W5100 support routines that are target-independent.
 *  That is, you build this library for a generic AVR ATmega device, then
 *  write your application to use the W51_xxx routines below for accessing
 *  the W5100 chip.  Because these routines are target-independent, you
 *  never have to rebuild them just because you are moving your code from,
 *  say, a 'mega128 to an 'xmega128a1 device.
 *
 *  For this to work properly, your application must provide three target-
 *  specific functions and must register the addresses of those functions
 *  with the W5100 library at run-time.  These functions are:
 *
 *  select              target-specific function for enabling the W5100 chip
 *  xchg                target-specific function for exchanging a byte with the W5100 chip
 *  deselect    target-specific function for disabling the W5100 chip
 *  reset               target-specific function for hardware reset of the W5100 chip
 *
 *  Your application registers these three functions with the W5100 library
 *  by invoking the W51_register() function.  Your application must make this
 *  call one time and must make this call before calling any other W5100
 *  functions.
 */


#include <util/delay.h>
#include "w5100.h"



#ifndef  FALSE
#define  FALSE          0
#define  TRUE           !FALSE
#endif


/*
 *  Define the function pointers used to access the SPI port assigned to the
 *  W5100 device.  These pointers will be filled in at run-time when the host
 *  calls W51_register().
 */
static  void                                    (*select)(void) = (void *)0;
static  unsigned char                   (*xchg)(unsigned char  val) = (void *)0;
static  void                                    (*deselect)(void) = (void *)0;
static  void                                    (*reset)(void) = (void *)0;


static  unsigned char                   inited = FALSE;




void  W51_register(W5100_CALLBACKS  *pcallbacks)
{
        select = pcallbacks->_select;
        xchg = pcallbacks->_xchg;
        deselect = pcallbacks->_deselect;
        reset = pcallbacks->_reset;
        inited = FALSE;
        if ((select) && (xchg) && (deselect))  inited = TRUE;   // these functions must be valid
}




void  W51_write(unsigned int  addr, unsigned char  data)
{
        if (!inited)  return;                                           // not set up, ignore request

        select();                                                                       // enable the W5100 chip
        xchg(W5100_WRITE_OPCODE);                                       // need to write a byte
        xchg((addr & 0xff00) >> 8);                             // send MSB of addr
        xchg(addr & 0xff);                                                      // send LSB
        xchg(data);                                                                     // send the data
        deselect();                                                                     // done with the chip
}


unsigned char  W51_read(unsigned int  addr)
{
        unsigned char                           val;

        if (!inited)  return  0;                                        // not set up, ignore request

        select();                                                                       // enable the W5100 chip
        xchg(W5100_READ_OPCODE);                                        // need to read a byte
        xchg((addr & 0xff00) >> 8);                             // send MSB of addr
        xchg(addr & 0xff);                                                      // send LSB
        val = xchg(0x00);                                                       // need to send a dummy char to get response
        deselect();                                                                     // done with the chip
        return  val;                                                            // tell her what she's won
}



void  W51_init(void)
{
        if (reset)  reset();                                            // if host provided a reset function, use it
        else            W51_write(W5100_MR, W5100_MR_SOFTRST);          // otherwise, force the w5100 to soft-reset
        _delay_ms(1);
}



unsigned char  W51_config(W5100_CFG  *pcfg)
{
        if (pcfg == 0)  return  W5100_FAIL;

        W51_write(W5100_GAR + 0, pcfg->gtw_addr[0]);    // set up the gateway address
        W51_write(W5100_GAR + 1, pcfg->gtw_addr[1]);
        W51_write(W5100_GAR + 2, pcfg->gtw_addr[2]);
        W51_write(W5100_GAR + 3, pcfg->gtw_addr[3]);
        _delay_ms(1);

        W51_write(W5100_SHAR + 0, pcfg->mac_addr[0]);   // set up the MAC address
        W51_write(W5100_SHAR + 1, pcfg->mac_addr[1]);
        W51_write(W5100_SHAR + 2, pcfg->mac_addr[2]);
        W51_write(W5100_SHAR + 3, pcfg->mac_addr[3]);
        W51_write(W5100_SHAR + 4, pcfg->mac_addr[4]);
        W51_write(W5100_SHAR + 5, pcfg->mac_addr[5]);
        _delay_ms(1);

        W51_write(W5100_SUBR + 0, pcfg->sub_mask[0]);   // set up the subnet mask
        W51_write(W5100_SUBR + 1, pcfg->sub_mask[1]);
        W51_write(W5100_SUBR + 2, pcfg->sub_mask[2]);
        W51_write(W5100_SUBR + 3, pcfg->sub_mask[3]);
        _delay_ms(1);

        W51_write(W5100_SIPR + 0, pcfg->ip_addr[0]);    // set up the source IP address
        W51_write(W5100_SIPR + 1, pcfg->ip_addr[1]);
        W51_write(W5100_SIPR + 2, pcfg->ip_addr[2]);
        W51_write(W5100_SIPR + 3, pcfg->ip_addr[3]);
        _delay_ms(1);

        W51_write(W5100_RMSR, 0x55);                                    // use default buffer sizes (2K bytes RX and TX for each socket
        W51_write(W5100_TMSR, 0x55);

        return  W5100_OK;                                                               // everything worked, show success
}
