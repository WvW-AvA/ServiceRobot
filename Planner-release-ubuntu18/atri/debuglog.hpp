#ifndef __DEBUGLOG_H__
#define __DEBUGLOG_H__

//#define __BACKTRACE__
//#define __DEBUG__

#include <iostream>
#ifdef __BACKTRACE__
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <cxxabi.h>
#endif

#define RESET "\033[0m"
#define BLACK "\033[30m"     /* Black */
#define RED "\033[31m"       /* Red */
#define GREEN "\033[32m"     /* Green */
#define YELLOW "\033[33m"    /* Yellow */
#define BLUE "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"   /* Magenta */
#define CYAN_BLUE "\033[36m" /* Cyan_Blue */

static void backtrace()
{
#ifdef __DEBUG__
#ifdef __BACKTRACE__
    printf("\033[31mBackTrace:\033[0m\n");
    unw_cursor_t cursor;
    unw_context_t context;

    // Initialize cursor to current frame for local unwinding.
    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    // Unwind frames one by one, going up the frame stack.
    while (unw_step(&cursor) > 0)
    {
        unw_word_t offset, pc;
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        if (pc == 0)
        {
            break;
        }
        printf("0x%lx:", pc);

        char sym[256];
        if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0)
        {
            char *nameptr = sym;
            int status;
            char *demangled = abi::__cxa_demangle(sym, nullptr, nullptr, &status);
            if (status == 0)
            {
                nameptr = demangled;
            }
            std::printf("(\033[31m%s\033[0m +0x%lx)\n", nameptr, offset);
            std::free(demangled);
        }
        else
        {
            printf(" -- error: unable to obtain symbol name for this frame\n");
        }
    }
#endif
#endif
}

#ifdef __DEBUG__
#define LOG(fmt, ...) printf("[LOG]:" fmt "\n", ##__VA_ARGS__);
#define LOG_ERROR(fmt, ...) printf("\033[31m[ERROR]:%s:%d " fmt "\033[0m\n", __FILE__, __LINE__, ##__VA_ARGS__);
#else
#define LOG_ERROR(fmt, ...) printf("\033[31m[ERROR]:%s:%d " fmt "\033[0m\n", __FILE__, __LINE__, ##__VA_ARGS__);
#define LOG(fmt, ...)
#endif

#endif