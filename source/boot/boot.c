__asm__(".code16gcc");

#include "boot.h"

void boot_entry(void)
{
    ((void (*)(void))LOADER_ENTRY)();
}
