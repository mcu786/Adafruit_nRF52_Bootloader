/**************************************************************************/
/*!
    @file     flash.c
    @author   hathach (tinyusb.org)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2018, Adafruit Industries (adafruit.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#include <string.h>
#include "flash.h"
#include "boards.h"

#include "nrf_sdm.h"

#define FLASH_PAGE_SIZE    4096

#define NO_CACHE 0xffffffff

static uint32_t _fl_addr = NO_CACHE;
static uint8_t _fl_buf[FLASH_PAGE_SIZE] __attribute__((aligned(4)));
static bool _first_flush = true;


void flash_flush(void)
{
  if ( _fl_addr == NO_CACHE ) return;

  if ( _first_flush )
  {
    _first_flush = false;

    // disable softdevice
    sd_softdevice_disable();

    led_blink_fast(true);
  }

  if ( memcmp(_fl_buf, (void *) _fl_addr, FLASH_PAGE_SIZE) != 0 )
  {
    nrf_nvmc_page_erase(_fl_addr);
    nrf_nvmc_write_words(_fl_addr, (uint32_t *) _fl_buf, FLASH_PAGE_SIZE / sizeof(uint32_t));
  }

  _fl_addr = NO_CACHE;
}

void flash_write (uint32_t dst, const void *src, int len)
{
  uint32_t newAddr = dst & ~(FLASH_PAGE_SIZE - 1);

  if ( newAddr != _fl_addr )
  {
    flash_flush();
    _fl_addr = newAddr;
    memcpy(_fl_buf, (void *) newAddr, FLASH_PAGE_SIZE);
  }
  memcpy(_fl_buf + (dst & (FLASH_PAGE_SIZE - 1)), src, len);
}