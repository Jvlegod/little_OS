#include "klib.h"
#include "kmath.h"
#include "task/task.h"
extern proc_manager_t proc_manager;

/**
 * @brief 以byte为单位复制size个大小的bytes的数据到dest
 *
 * @param dest
 * @param data
 * @param size
 */
void kmemset(void *dest, uint8_t data, int size)
{
    if (size < 0)
    {
        /*此处可以打印错误信息*/
        return;
    }

    uint8_t *d = (uint8_t *)dest;
    while (size--)
    {
        *d++ = data;
    }
}

/**
 * @brief 以byte为单位复制size个大小的byte到dest
 *
 * @param dest
 * @param src
 * @param size
 */
void kmemcpy(void *dest, void *src, int size)
{
    if (size < 0)
    {
        /*此处可以打印错误信息*/
        return;
    }

    uint8_t *d = (uint8_t *)dest;
    uint8_t *s = (uint8_t *)src;

    while (size--)
    {
        *d++ = *s++;
    }
}

/**
 * @brief 打印格式化字符串
 *
 * @param fmt
 * @param ...
 */
void klog_printf(const char *fmt, ...)
{
    char buf[256];
    va_list args_list;

    kmemset(buf, 0, sizeof(buf));

    va_start(args_list, fmt);
    klog_vsprintf(buf, fmt, args_list);
    va_end(args_list);

    char *p = buf;
    while (*p != 0)
    {
        write_serial(*p++);
    }
    /* 打印回车和换行 */
    // write_serial_end();
}

/**
 * @brief 格式化字符串输出到buf
 *
 * @param buf
 * @param fmt
 * @param args
 */
void klog_vsprintf(char *buf, const char *fmt, va_list args)
{
    /* 状态机的状态 */
    enum
    {
        NORMAL,
        FORMAT
    } state = NORMAL;

    char *ch = buf;
    while (*fmt != 0)
    {
        switch (state)
        {
        case NORMAL:
            if (*fmt == '%')
            {
                state = FORMAT;
                fmt++;
                break;
            }

            if (*fmt == '\n')
            {
                *ch++ = '\n';
                *ch++ = '\r';
                fmt++;
            }
            else
            {
                *ch++ = *fmt++;
            }
            break;
        case FORMAT:
            /* 错误类型 */
            if (*fmt == '%')
            {
                *ch++ = '%';
            }
            /* int类型 */
            else if (*fmt == 'd')
            {
                int num = va_arg(args, int);
                int len = kintlen(num);
                char p[len + 1];
                char *ptr = p;
                kitoa_s(p, num, len);
                while (*ptr != 0)
                {
                    *ch++ = *ptr++;
                }
            }
            /* char类型 */
            else if (*fmt == 'c')
            {
                *ch++ = va_arg(args, int);
            }
            /* str类型 */
            else if (*fmt == 's')
            {
                char *cbuf = va_arg(args, char *);
                while (*cbuf != 0)
                {
                    *ch++ = *cbuf++;
                }
            }
            else if (*fmt == 'x')
            {
                int num = va_arg(args, int);
                char cbuf[50];
                char *ptr = cbuf;
                kmemset(cbuf, 0, sizeof(char) * 50);
                kdecimalToHexadecimal(cbuf, num);

                *ch++ = '0';
                *ch++ = 'x';

                while (*ptr != 0)
                {
                    *ch++ = *ptr++;
                }
            }
            fmt++;

            state = NORMAL;
            break;
        }
    }
}

/**
 * @brief 将整数转换为字符串
 *
 * @param value
 */
char *kitoa_s(char *buf, int value, int size)
{
    int i = 0;
    char *p = buf;
    kmemset(buf, 0, sizeof(buf));
    int num = value;
    while (num)
    {
        *p++ = '0' + (num % 10);
        num /= 10;
        i++;
    }
    buf[i] = 0;
    kreverse_charstr(buf);
}

/**
 * @brief 倒置buf
 *
 * @param buf
 * @return char*
 */
void kreverse_intstr(int *buf)
{
    int l = 0, r = karr_strlen(buf) - 1;
    while (l < r)
    {
        kswap(&buf[l], &buf[r], __INT_TYPE);
        l++;
        r--;
    }
}

/**
 * @brief 倒置buf
 *
 * @param buf
 * @return char*
 */
void kreverse_charstr(char *buf)
{
    int l = 0, r = kstrlen(buf) - 1;
    while (l < r)
    {
        kswap(&buf[l], &buf[r], __CHAR_TYPE);
        l++;
        r--;
    }
}

/**
 * @brief 解析出字符串的大小
 *
 * @param buf
 * @return int
 */
int kstrlen(char const *buf)
{
    int cnt = 0;
    while (buf[cnt] != '\0')
    {
        cnt++;
    }
    return cnt;
}

/**
 * @brief 计算int数组的长度
 *
 * @param buf
 * @return int
 */
int karr_strlen(int *buf)
{
    int cnt = 0;
    while (buf[cnt] != '\0')
    {
        cnt++;
    }
    return cnt;
}

/**
 * @brief 计算int类型的长度
 *
 * @return int
 */
int kintlen(int n)
{
    int i = 0;
    if (n == 0)
    {
        return 1;
    }

    while (n)
    {
        n /= 10;
        i++;
    }
    return i;
}

/**
 * @brief 交换两个地址的数值
 *
 * @param a
 * @param b
 * @param type
 */
void kswap(void *a, void *b, DateType type)
{
    switch (type)
    {
    case __INT_TYPE:
    {
        int *p = (int *)a;
        int *s = (int *)b;

        int temp = *p;
        *p = *s;
        *s = *p;
    }
    break;
    case __UINT8_TYPE:
    {
        uint8_t *p = (uint8_t *)a;
        uint8_t *s = (uint8_t *)b;

        uint8_t temp = *p;
        *p = *s;
        *s = *p;
    }
    break;
    case __UINT16_TYPE:
    {
        uint16_t *p = (uint16_t *)a;
        uint16_t *s = (uint16_t *)b;

        uint16_t temp = *p;
        *p = *s;
        *s = *p;
    }
    break;
    case __UINT32_TYPE:
    {
        uint32_t *p = (uint32_t *)a;
        uint32_t *s = (uint32_t *)b;

        uint32_t temp = *p;
        *p = *s;
        *s = *p;
    }
    break;
    case __FLOAT_TYPE:
    {
        float *p = (float *)a;
        float *s = (float *)b;

        float temp = *p;
        *p = *s;
        *s = *p;
    }
    break;
    case __DOUBLE_TYPE:
    {
        double *p = (double *)a;
        double *s = (double *)b;

        double temp = *p;
        *p = *s;
        *s = *p;
    }
    break;
    case __CHAR_TYPE:
    {
        char *p = (char *)a;
        char *s = (char *)b;

        char temp = *p;
        *p = *s;
        *s = temp;
    }
    break;
    default:
        klog_printf("kswap erro: TYPE cannot found\n");
        break;
    }
}

/**
 * @brief 字符串复制操作
 *
 * @param dest
 * @param src
 */
void kstrcpy(char *dest, char const *src)
{
    int len = kstrlen(src);

    for (int i = 0; i < len; i++)
    {
        dest[i] = src[i];
    }
}