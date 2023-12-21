#ifndef DEBUG_H
#define DEBUG_H
void panic(char *filename, int line, const char *func, const char *condition);

#define PANIC(...) panic(__FILE__, __LINE__, __func__, __VA_ARGS__);

#ifndef KERNEL_DEBUG
#define ASSERT(CONDITION) ((void)0)
#else
#define ASSERT(CONDITION)  \
    if (CONDITION)         \
    {                      \
    }                      \
    else                   \
    {                      \
        PANIC(#CONDITION); \
    }
#endif // !

#endif // !DEBUG_H