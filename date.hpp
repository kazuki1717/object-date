#pragma once

#ifndef DF_DATE_VERSION
#define DF_DATE_VERSION "c++ 1.4.0 2026-03-02"


#include "exception.hpp"

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>

#include <string>
#include <ostream>



// == constants ==

char DF_DATE_FORMATTING_BUFFER[256]{};

constexpr time_t DF_MINUTE = 60;
constexpr time_t DF_HOUR = 60 * DF_MINUTE;
constexpr time_t DF_DAY = 24 * DF_HOUR;


// == exceptions ==

class df_failed_parse_date_exception_t : public df_exception_t {
public:
    df_failed_parse_date_exception_t(const char* date_str, const char* fmt) : df_exception_t("failed to parse date since '%s' there are no meet anyone specifier in '%s'", date_str, fmt) {}
};



// == pre-processer ==

#ifdef _MSC_VER

#define DF_ENV_IS_MSVC(code) code
#define DF_ENV_NOT_MSVC(code)

#else

#define DF_ENV_IS_MSVC(code)
#define DF_ENV_NOT_MSVC(code) code

#endif


#ifdef _MSC_VER
// just reduce warning count, not the safe version since there are no sscanf_s() needed but suit for another compiler
#define df_sscanf(s, fmt, ...) sscanf_s(s, fmt, __VA_ARGS__)
#else
#define df_sscanf(s, fmt, ...) sscanf(s, fmt, __VA_ARGS__)
#endif






int df_strncasecmp(const char* s1, const char* s2, int n) {
    int c1, c2;
    const char* end = s1 + n;
    while (s1 < end) {
        c1 = tolower(*s1); c2 = tolower(*s2);

        if (c1 != c2) {
            return c1 - c2;
        }
        s1++; s2++;
    }
    return 0;
}

char* df_strncpy_s(char* dest, size_t dest_limit, const char* src, size_t src_length) {
    DF_ENV_IS_MSVC(
        strncpy_s(dest, dest_limit, src, src_length);
        return dest + src_length;
    )
    DF_ENV_NOT_MSVC(
        strncpy(dest, src, src_length);
        return dest + src_length;
    )
}







// == classes ==

class df_interval_t {
public:
    int years = 0, months = 0, days = 0;
    int hours = 0, minutes = 0, seconds = 0;

    short on_wday = -1;
    short on_mday = -1;
    short on_yday = -1;



    df_interval_t(const char* fmt) {
        const char* p = fmt;
        int value = 0;
        int c = *p;

        days = 0;

        while ((c = *p)) {
            // == skip spaces ==
            while (c != '\0' && isspace(c)) {
                c = *(++p);
            }

            if (c == '\0') {
                return;
            }

            // == parse value and unit ==
            if (isdigit(c)) {
                df_sscanf(p, "%d", &value);

                while ((c = *p++) && isdigit(c));
                continue;
            }

            if (!isalpha(c)) {
                c = *(p++);
                continue;
            }

            if (df_strncasecmp(p, "year", 4) == 0) {
                years = value;
                p += 4;
                continue;
            }
            if (df_strncasecmp(p, "month", 5) == 0) {
                months = value;
                p += 5;
                continue;
            }
            if (df_strncasecmp(p, "week", 4) == 0) {
                days += value * 7;
                p += 4;
                continue;
            }
            if (df_strncasecmp(p, "day", 3) == 0) {
                days += value;
                p += 3;
                continue;
            }
            if (df_strncasecmp(p, "hour", 4) == 0) {
                hours += value;
                p += 4;
                continue;
            }
            if (df_strncasecmp(p, "min", 3) == 0) {
                minutes += value;
                p += 3;
                goto label_fix_lessing;
            }
            if (df_strncasecmp(p, "sec", 3) == 0) {
                seconds = value;
                p += 3;
                continue;
            }

            // failed parsing
            p++;
            continue;
        label_fix_lessing:
            while ((c = *p++) != 0 && !isspace(c)) {
                p++;
            }
        }
    }



    // sum(years, months, ..., seconds), if total < 0, return -1. total == 0, return 0. total > 0, return 1
    int get_direction() const {
        int total = years + months + days + hours + minutes + seconds;

        return total < 0 ? -1
            : total == 0 ? 0
            : 1;
    }

    bool is_constant() const {
        return !(years || months);
    }

    time_t calculate_constant() const {
        return ((((years * 365 + months) * 30 + days) * 24 + hours) * 60 + minutes) * 60 + seconds;
    }


    // == formatting ==
    
    const char* to_cstr(char* buffer = DF_DATE_FORMATTING_BUFFER) const {
        char* p = buffer;

        p = df_strncpy_s(p, 256, "df_interval_t(", 14);
        if (years != 0) p += snprintf(p, 256, "%d year ", years);
        if (months != 0) p += snprintf(p, 256, "%d month ", months);
        if (days != 0) p += snprintf(p, 256, "%d day ", days);
        if (hours != 0) p += snprintf(p, 256, "%d hour ", hours);
        if (minutes != 0) p += snprintf(p, 256, "%d min ", minutes);
        if (seconds != 0) p += snprintf(p, 256, "%d sec ", seconds);
        
        if (*(p-1) == ' ') p--;
        *(p++) = ')';
        *(p++) = '\0';
        return buffer;
    }

    std::string to_string() const {
        return to_cstr();
    }


    // == std::cout ==

    friend std::ostream& operator<<(std::ostream& stream, const df_interval_t interval) {
        return stream << interval.to_cstr();
    }
};






class df_date_t {
public:
    // == core constants ==

    constexpr static char DEFAULT_FORMAT[] = "%Y-%m-%d %H:%M:%S";

    constexpr static int MONTH_TO_YDAY[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 366};
    constexpr static char MONTHS_NAMES[12][12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    constexpr static char WEEKDAYS_NAMES[7][10] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};


    // == core methods ==

    static time_t mktime(const struct tm* tm) noexcept {
        time_t seconds = tm->tm_sec + tm->tm_min * DF_MINUTE + tm->tm_hour * DF_HOUR;

        // convert yday / month to second
        int year = tm->tm_year;
        int yday = tm->tm_yday;
        int month = tm->tm_mon;
        int mday = tm->tm_mday;

        if (yday != 0) {
            seconds += (yday - (yday > 0)) * DF_DAY;
        }
        else {
            // adject abnormal month

            if (month < 0 || month > 12) {
                year += month / 12;
                month %= 12;

                if (month < 0) {
                    year -= 1;
                    month = 12 + month;
                }
            }

            // convert month -> yday -> seconds

            yday = MONTH_TO_YDAY[month] + mday - 1;

            if (year%4 == 0 && month > 2) {
                yday += 1;
            }

            seconds += yday * DF_DAY;
        }

        // convert year to seconds
        
        int leap_days = year / 4 + (year%4 == 0 ? 0 : 1);
        seconds += (year * 365 + leap_days) * DF_DAY;

        return seconds;
    }

    static struct tm* gmtime(struct tm* tm, const time_t* t) noexcept {
        DF_ENV_IS_MSVC(
            time_t less = *t;

            // == week day ==

            tm->tm_wday = (less / DF_DAY) % 7;
            if (tm->tm_wday < 0) {
                tm->tm_wday = 7 + tm->tm_wday;
            }
            tm->tm_wday += 1;

            // == year ==
            
            int leap_days = (less + 1401 * DF_DAY) / (1461 * DF_DAY);
            less -= leap_days * DF_DAY;

            tm->tm_year = less / (365 * DF_DAY);
            less -= tm->tm_year * 365 * DF_DAY;

            // == month and day ==

            tm->tm_yday = less / DF_DAY + 1;
            less %= DF_DAY;

            // find month and mday
            for (int i = 1; i < 13; i++) {
                if (tm->tm_yday <= MONTH_TO_YDAY[i]) {
                    tm->tm_mon = i - 1;
                    tm->tm_mday = tm->tm_yday - MONTH_TO_YDAY[tm->tm_mon];
                    break;
                }
            }

            // == time ==

            tm->tm_hour = less / DF_HOUR;
            less %= DF_HOUR;

            tm->tm_min = less / DF_MINUTE;
            tm->tm_sec = less - tm->tm_min * DF_MINUTE;

            return tm;
        )
        DF_ENV_NOT_MSVC(
            return gmtime(t);
        )
    }

    static size_t strftime(char* buf, const char* fmt, const struct tm* tm) noexcept {
        char* out = buf;
        const char* in = fmt;
        int c;

        while ((c = (in++)[0])) {
            if (c != '%') {
                (out++)[0] = c;
                continue;
            }

            c = (in++)[0];
            switch (c) {
                // == date parsing ==
                case '%':
                    (out++)[0] = c;
                    continue;
                case 'Y':
                    out += snprintf(out, 128, "%04d", tm->tm_year + 1900);
                    continue;
                case 'y':
                    out += snprintf(out, 128, "%02d", tm->tm_year % 100);
                    continue;
                case 'm':
                    out += snprintf(out, 128, "%02d", tm->tm_mon + 1);
                    continue;
                case 'B':
                    out = df_strncpy_s(out, 128, MONTHS_NAMES[tm->tm_mon], strlen(MONTHS_NAMES[tm->tm_mon]));
                    continue;
                case 'b':
                    out = df_strncpy_s(out, 128, MONTHS_NAMES[tm->tm_mon], 3);
                    continue;
                case 'd':
                    out += snprintf(out, 128, "%02d", tm->tm_mday);
                    continue;
                case 'e':
                    out += snprintf(out, 128, "%02d", tm->tm_mday + 1);
                    continue;
                case 'j':
                    out += snprintf(out, 128, "%02d", tm->tm_yday);
                    continue;
                case 'A':
                    out = df_strncpy_s(out, 128, WEEKDAYS_NAMES[tm->tm_wday - 1], strlen(WEEKDAYS_NAMES[tm->tm_wday]));
                    continue;
                case 'a':
                    out = df_strncpy_s(out, 128, WEEKDAYS_NAMES[tm->tm_wday - 1], 3);
                    continue;
                case 'u':
                    out += snprintf(out, 128, "%01d", tm->tm_wday);
                    continue;
                case 'w':
                    out += snprintf(out, 128, "%01d", 7 - tm->tm_wday);
                    continue;
                
                // == time ==
                case 'H':
                    out += snprintf(out, 128, "%02d", tm->tm_hour);
                    continue;
                case 'l':
                    out += snprintf(out, 128, "%02d", tm->tm_hour % 13);
                    continue;
                case 'p':
                    out = df_strncpy_s(out, 128, tm->tm_hour < 13 ? "AM" : "PM", 2);
                    continue;
                case 'M':
                    out += snprintf(out, 128, "%02d", tm->tm_min);
                    continue;
                case 'S':
                    out += snprintf(out, 128, "%02d", tm->tm_sec);
                    continue;
                case 'c':
                    out += strftime(out, "%a %b %d %H:%M:%S %Y", tm);
                    continue;
                case 'x':
                    out += strftime(out, "%d/%m/%y", tm);
                    continue;
                case 'X':
                    out += strftime(out, "%H:%M:%S", tm);
                    continue;
                default:
                    (out++)[0] = '%';
                    (out++)[0] = c;
                    continue;
            }
        }

        out[0] = 0;
        return (size_t)(out - buf);
    }





    static size_t parse_month(const char* strmonth, size_t n, int* month) noexcept {
        for (int i = 0; i < 12; i++) {
            if (df_strncasecmp(strmonth, MONTHS_NAMES[i], n) == 0) {
                *month = i;
                return strlen(MONTHS_NAMES[i]);
            }
        }
        return 0;
    }

    // parse weekday
    static size_t parse_weekday(const char* strweek, size_t n, int* week) noexcept {
        for (int i = 0; i < 7; i++) {
            if (df_strncasecmp(strweek, WEEKDAYS_NAMES[i], n) == 0) {
                *week = i + 1;
                return strlen(WEEKDAYS_NAMES[i]);
            }
        }
        return 0;
    }

    // self strptime for parsing time in win32
    // ! be causeful since this method will not complete all values
    static int strptime(const char* date_str, const char* fmt, struct tm* tm) noexcept {
        const char* fmt_start = fmt;
        int symbol, c;
        char buffer[10];
        int temp;

        // == reset tm values ==
        tm->tm_mon = 0;
        tm->tm_mday = 1;

        tm->tm_hour = 0;

        // == start parse ==

        while ((symbol = *fmt++) != 0) {
            // == filter ==
            // symbol is normal char: clear anything not suit to fmt
            if (symbol != '%') {
        filter:
                while ((c = *(date_str++)) && c != symbol);

                if (c == 0) {
                    break;
                }
                continue;
            }

            // == parse ==
            // if have known size, move date_str pointer and continue
            // if unknown size, continue to move by filter (!no savity)
            symbol = *(fmt++);

            switch (symbol) {
                // == date parsing ==
                case '%':
                    goto filter;
                case 'Y':
                    if (df_sscanf(date_str, "%04d", &tm->tm_year) == 0) {
                        return -1;
                    }

                    tm->tm_year -= 1900;
                    temp = 4;
                    goto label_pass;
                case 'y':
                    df_sscanf(date_str, "%02d", &tm->tm_year);
                    tm->tm_year += 100; // 2000 - 1900
                    temp = 2;
                    goto label_pass;
                case 'm':
                    df_sscanf(date_str, "%02d", &tm->tm_mon);
                    tm->tm_mon -= 1;
                    temp = 2;
                    goto label_pass;
                case 'B':
                    date_str += parse_month(date_str, 12, &tm->tm_mon);
                    continue;
                case 'b':
                    parse_month(date_str, 3, &tm->tm_mon);
                    temp = 3;
                    goto label_pass;
                case 'd':
                    df_sscanf(date_str, "%02d", &tm->tm_mday);
                    temp = 2;
                    goto label_pass;
                case 'e':
                    df_strncpy_s(buffer, sizeof(buffer), date_str, 2);
                    df_sscanf(buffer, "%d", &tm->tm_mday);
                    temp = 2;
                    goto label_pass;
                case 'j':
                    df_sscanf(date_str, "%03d", &tm->tm_yday);
                    temp = 3;
                    goto label_pass;
                case 'A':
                    date_str += parse_weekday(date_str, 12, &tm->tm_wday);
                    continue;
                case 'a':
                    parse_weekday(date_str, 3, &tm->tm_wday);
                    temp = 3;
                    goto label_pass;
                case 'u':
                    df_sscanf(date_str, "%01d", &tm->tm_wday);
                    temp = 1;
                    goto label_pass;
                case 'w':
                    df_sscanf(date_str, "%01d", &tm->tm_wday);
                    tm->tm_wday = 7 - tm->tm_wday;
                    temp = 1;
                    goto label_pass;
                
                // == time parsing ==
                case 'H':
                    if (df_sscanf(date_str, "%02d", &tm->tm_hour) == 0) {
                        return -1;
                    }
                    temp = 2;
                    goto label_pass;
                case 'l':
                    df_sscanf(date_str, "%02d", &temp);
                    tm->tm_hour = (tm->tm_hour + temp) % 24;
                    temp = 2;
                    goto label_pass;
                case 'p':
                    if (df_strncasecmp(date_str, "PM", 2) == 0) {
                        tm->tm_hour = (tm->tm_hour + 12) % 24;
                        date_str += 2;
                    }
                    continue;
                case 'M':
                    df_sscanf(date_str, "%02d", &tm->tm_min);
                    temp = 2;
                    goto label_pass;
                case 'S':
                    df_sscanf(date_str, "%02d", &tm->tm_sec);
                    temp = 2;
                    goto label_pass;
                case 'f':
                    continue;   // micro seconds (unusable)
                case 'c':
                    date_str += strptime(date_str, "%a %b %d %H:%M:%S %Y", tm);
                    continue;
                case 'x':
                    date_str += strptime(date_str, "%d/%m/%y", tm);
                    continue;
                case 'X':
                    date_str += strptime(date_str, "%H:%M:%S", tm);
                    continue;
                    
            }

            continue;
        label_pass:
            do {
            if ((c = *(date_str++)) == 0) {
                return 0;
            }
            if (!isdigit(c)) {
                date_str--;
                break;
            }
            }
            while (--temp > 0);
        }

        return 0;
    }



    inline static int get_time_zone() {
        time_t zero = 0;
        struct tm* tm;
        DF_ENV_IS_MSVC(
            struct tm _tm;
            localtime_s(&_tm, &zero);
            tm = &_tm;
        )
        DF_ENV_NOT_MSVC(
            tm = localtime(&zero);
        )
        return tm->tm_hour;
    }






private:
    time_t t;

    static time_t _parse(const char* date_str, const char* fmt = DEFAULT_FORMAT) {
        struct tm tm{};
        if (strptime(date_str, fmt, &tm) != 0) {
            throw df_failed_parse_date_exception_t(date_str, fmt);
        }
        return mktime(&tm);
    }


public:
    constexpr df_date_t() : t(0LL) {}

    constexpr df_date_t(time_t t) : t(t DF_ENV_IS_MSVC(+ 2209075200LL)) {}


    constexpr df_date_t(const char* date_str, const char* fmt = DEFAULT_FORMAT) : t(_parse(date_str, fmt)) {}

    constexpr df_date_t(const std::string& date_str, const char* fmt = DEFAULT_FORMAT) : t(_parse(date_str.c_str(), fmt)) {}

    constexpr df_date_t(const df_date_t& other) noexcept : t(other.t) {}



    // == incresce/reduce interval ==

    df_date_t& operator+=(time_t interval) {
        t += interval;
        return *this;
    }

    df_date_t operator+(time_t interval) const {
        return df_date_t(*this) += interval;
    }



    df_date_t& operator-=(time_t interval) {
        t -= interval;
        return *this;
    }

    df_date_t operator-(time_t interval) const {
        return df_date_t(*this) -= interval;
    }



    df_date_t& operator+=(const df_interval_t& interval) {
        struct tm _tm;
        struct tm* tm = gmtime(&_tm, &t);

        struct tm dest{};
        dest.tm_year = tm->tm_year + interval.years;
        dest.tm_mon = tm->tm_mon + interval.months;
        dest.tm_mday = tm->tm_mday + interval.days;
        dest.tm_hour = tm->tm_hour + interval.hours;
        dest.tm_min = tm->tm_min + interval.minutes;
        dest.tm_sec = tm->tm_sec + interval.seconds;

        t = mktime(&dest);
        return *this;
    }

    df_date_t operator+(const df_interval_t& interval) const {
        return df_date_t(*this) += interval;
    }




    df_date_t& operator-=(const df_interval_t& interval) {
        struct tm _tm;
        struct tm* tm = gmtime(&_tm, &t);

        struct tm dest{};
        dest.tm_year = tm->tm_year - interval.years;
        dest.tm_mon = tm->tm_mon - interval.months;
        dest.tm_mday = tm->tm_mday - interval.days;
        dest.tm_hour = tm->tm_hour - interval.hours;
        dest.tm_min = tm->tm_min - interval.minutes;
        dest.tm_sec = tm->tm_sec - interval.seconds;

        t = mktime(&dest);
        return *this;
    }

    df_date_t operator-(const df_interval_t& interval) const {
        return df_date_t(*this) -= interval;
    }






    // == converting ==

    operator time_t() const {
        return t DF_ENV_IS_MSVC(- 2209075200LL);
    }


    // == formatting ==

    const char* to_gmt_cstr(const char* fmt = DEFAULT_FORMAT, time_t offset = 0) const {
        struct tm _tm;
        time_t t_adjected = t + offset;
        struct tm* tm = gmtime(&_tm, &t_adjected);

        strftime(DF_DATE_FORMATTING_BUFFER, fmt, tm);
        return DF_DATE_FORMATTING_BUFFER;
    }

    const char* to_local_cstr(const char* fmt = DEFAULT_FORMAT, time_t offset = 0) const {
        return to_gmt_cstr(fmt, get_time_zone() * DF_HOUR);
    }

    

    std::string to_gmt_string(const char* fmt = DEFAULT_FORMAT, time_t offset = 0) const {
        return to_gmt_cstr(fmt, offset);
    }

    std::string to_local_string(const char* fmt = DEFAULT_FORMAT) const {
        return to_gmt_cstr(fmt, get_time_zone() * DF_HOUR);
    }


    // == std::cout ==

    friend std::ostream& operator<<(std::ostream& stream, const df_date_t& date) {
        return stream << date.to_gmt_cstr();
    }
};


#endif // DF_DATE_VERSION