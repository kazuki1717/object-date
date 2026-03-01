#pragma once

#ifndef _DF_EXCEPTION_T_
#define _DF_EXCEPTION_T_

#include <exception>

#include <stdarg.h>
#include <stdio.h>



class df_exception_t : public std::exception {
    char message[512];

public:
    df_exception_t(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(message, 512, fmt, args);
        va_end(args);
    }

    const char* what() const noexcept override {
        return message;
    }

    void print() const {
        fprintf(stderr, "df_exception: %s\n", message);
    }
};



#define df_debug(fmt, ...) \
    fprintf(stderr, "debug %s-%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);



#endif // _DF_EXCEPTION_T_


