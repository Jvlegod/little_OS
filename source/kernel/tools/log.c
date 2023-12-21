#include "log.h"

/**
 * @brief 初始化串口
 *
 */
void log_init()
{
    outb(PORT_COM1 + 1, 0x00); // Disable all interrupts
    outb(PORT_COM1 + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(PORT_COM1 + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT_COM1 + 1, 0x00); //                  (hi byte)
    outb(PORT_COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(PORT_COM1 + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT_COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR set

    // // Check if serial is faulty (i.e: not same byte as sent)
    // if (inb(PORT_COM1 + 0) != 0xAE)
    // {
    //     return;
    // }

    // // If serial is not faulty set it in normal operation mode
    // // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    // outb(PORT_COM1 + 4, 0x0F);
    return;
}

/**
 * @brief 判断是否为空
 *
 * @return int
 */
bool is_transmit_empty()
{
    return inb(PORT_COM1 + 5) & 0x20;
}

/**
 * @brief 实现写串口的操作
 *
 * @param a
 */
void write_serial(char a)
{
    while (is_transmit_empty() == false)
        ;

    outb(PORT_COM1, a);
}

void write_serial_end()
{
    outb(PORT_COM1, '\r');
    outb(PORT_COM1, '\n');
}