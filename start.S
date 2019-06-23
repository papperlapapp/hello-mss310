#include "mipsregs.h"

// make it accessible outside
.globl _start

// Tell binutils it's a function
.ent _start
.text

_start:
    // Set up a stack
    li sp, STARTADDRESS + 0x10000

    // And jump to C
    la t0, entrypoint
    jr t0
    nop

.end _start
