/* Copyright (C) 2015 Goswin von Brederlow <goswin-v-b@web.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* Information about the different Raspberry Pi models
 */

#include <stddef.h>

#include "arch_info.h"
#include "kprintf.h"

enum Model model;
const char *model_name;
uint32_t mem_total;
uint32_t initrd_start;
uint32_t initrd_size;
uint32_t peripheral_base;
uint32_t led_act_pin;
uint32_t led_pwr_pin;

// find token in string and return it or return NULL
const char *find(const char *str, const char *token) {
    while(*str) {
        const char *p = str;
        const char *q = token;
        // as long as str matches token
        while(*p && *p == *q) {
            // keep comparing
            ++p;
            ++q;
        }
        if (*q == 0) return str; // found token
        // token not found, try again
        ++str;
    }

    // end of string, nothing found
    return NULL;
}

enum Tag {
    NONE = 0x00000000,
    CORE = 0x54410001,
    MEM = 0x54410002,
    INITRD2 = 0x54420005,
    CMDLINE = 0x54410009,
};

typedef struct Mem {
    uint32_t size;
    uint32_t start;
} Mem;

typedef struct Initrd2 {
    uint32_t start;
    uint32_t size;
} Initrd2;

typedef struct Cmdline {
    char line[1];
} Cmdline;

struct Atag {
    uint32_t tag_size;
    uint32_t tag;
    union {
        Mem mem;
        Initrd2 initrd2;
        Cmdline cmdline;
    };
};

const Atag * next(const Atag *atag) {
    if (atag->tag == NONE) {
	return NULL;
    }
    return (const Atag *)(((uint32_t *)atag) + atag->tag_size);
}

void arch_info_init(const Atag *atag) {
    const char *cmdline = "";
    while (atag) {
        switch (atag->tag) {
        case MEM: {
            // assumes atag->mem.start == 0
            mem_total = atag->mem.size;
            break;
        }
        case INITRD2: {
            initrd_start = atag->initrd2.start;
            initrd_size = atag->initrd2.size;
            break;
        }
        case CMDLINE: {
            cmdline = atag->cmdline.line;
            if (find(cmdline, "bcm2708.disk_led_gpio=47")) {
                // Raspberry PI b+
                model = RASPBERRY_PI_B_PLUS;
                model_name = "Raspberry Pi b+";
                peripheral_base = 0x20000000;
                led_act_pin = 47;
		led_pwr_pin = 35;
            } else if (find(cmdline, "bcm2709.disk_led_gpio=47")) {
                // Raspberry PI 2
                model = RASPBERRY_PI_2;
                model_name = "Raspberry Pi 2";
                peripheral_base = 0x3F000000;
                led_act_pin = 47;
		led_pwr_pin = 35;
            } else {
                // Raspberry PI
                model = RASPBERRY_PI;
                model_name = "Raspberry Pi";
                peripheral_base = 0x20000000;
                led_act_pin = 16;
		led_pwr_pin = NO_LED;
            }
            break;
        }
        default: {
            // should only be the CORE tag
        }
        }
        atag = next(atag);
    }
    kprintf("\nDetected '%s'\n", model_name);
    kprintf("Memory      : %#8.8lx\n", mem_total);
    kprintf("Initrd start: %#8.8lx\n", initrd_start);
    kprintf("Initrd size : %#8.8lx\n", initrd_size);
    kprintf("Commandline : '%s'\n", cmdline);
}
