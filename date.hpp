#pragma once

#ifndef _DF_DATE_HPP_
#define _DF_DATE_HPP_ "c++ 1.2.3"


#include <string.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>

#include <string>
#include <ostream>




constexpr size_t DF_DATE_FORMATTING_BUFFER_LENGTH = 256;
char DF_DATE_FORMATTING_BUFFER[DF_DATE_FORMATTING_BUFFER_LENGTH + 1] = {0};

constexpr time_t DF_MINUTE = 60;
constexpr time_t DF_HOUR = 60 * DF_MINUTE;
constexpr time_t DF_DAY = 24 * DF_HOUR;

constexpr static int DF_ACCUMULATION_MONTH_DAYS[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 366};
constexpr static char DF_MONTHS_FULL_NAMES[12][12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
constexpr static char DF_WEEKDAYS_NAMES[7][10] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};





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


time_t df_mktime(const struct tm* tm) {
    time_t seconds = tm->tm_sec + tm->tm_min * DF_MINUTE + tm->tm_hour * DF_HOUR;

    // convert yday / month to second
    int year = tm->tm_year;
    int yday = tm->tm_yday;
    int mon = tm->tm_mon;
    int mday = tm->tm_mday;

    if (yday != 0) {
        seconds += (yday - (yday > 0)) * DF_DAY;
    }
    else {
        if (mon < 0 || mon > 12) {
            year += mon / 12;
            mon %= 12;
            if (mon < 0) {
                year -= 1;
                mon = 12 + mon;
            }
        }

        // convert month to yday to seconds
        yday += DF_ACCUMULATION_MONTH_DAYS[mon] + (mday - (mday > 0));

        if (year%4 == 0 && mon > 2) {
            yday += 1;
        }

        seconds += yday * DF_DAY;
    }


    // convert year to seconds

    int year_noleaped = year % 4;

    seconds += (year - year_noleaped) * (365.25 * DF_DAY) + year_noleaped * 365 * DF_DAY;

    return seconds;
}


struct tm* df_gmtime(struct tm* tm, const time_t* t) {
    DF_ENV_IS_MSVC(
        time_t less = t[0];

        // == date ==
        
        tm->tm_year = less / (365.25 * DF_DAY);
        less = less % time_t(365.25 * DF_DAY) + (tm->tm_year % 4) * DF_DAY / 4;

        tm->tm_yday = less / DF_DAY + 1;
        less %= DF_DAY;

        // find month and mday
        for (int i = 1; i < 13; i++) {
            if (tm->tm_yday <= DF_ACCUMULATION_MONTH_DAYS[i]) {
                tm->tm_mon = i - 1;
                tm->tm_mday = tm->tm_yday - DF_ACCUMULATION_MONTH_DAYS[tm->tm_mon];
                break;
            }
        }

        // == time ==

        tm->tm_hour = less / DF_HOUR;
        less %= DF_HOUR;

        tm->tm_min = less / DF_MINUTE;
        tm->tm_sec = less % DF_MINUTE;

        return tm;
    )
    DF_ENV_NOT_MSVC(
        return gmtime(t);
    )
}



size_t df_strftime(char* buf, const char* fmt, const struct tm* tm) {
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
                out = df_strncpy_s(out, 128, DF_MONTHS_FULL_NAMES[tm->tm_mon], strlen(DF_MONTHS_FULL_NAMES[tm->tm_mon]));
                continue;
            case 'b':
                out = df_strncpy_s(out, 128, DF_MONTHS_FULL_NAMES[tm->tm_mon], 3);
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
                out = df_strncpy_s(out, 128, DF_WEEKDAYS_NAMES[tm->tm_wday], strlen(DF_WEEKDAYS_NAMES[tm->tm_wday]));
                continue;
            case 'a':
                out = df_strncpy_s(out, 128, DF_WEEKDAYS_NAMES[tm->tm_wday], 3);
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
                out += df_strftime(out, "%a %b %d %H:%M:%S %Y", tm);
                continue;
            case 'x':
                out += df_strftime(out, "%d/%m/%y", tm);
                continue;
            case 'X':
                out += df_strftime(out, "%H:%M:%S", tm);
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
    
    const char* c_str(char* buffer = DF_DATE_FORMATTING_BUFFER, size_t buffer_size = DF_DATE_FORMATTING_BUFFER_LENGTH) const {
        snprintf(buffer, buffer_size,
            "df_interval_t(%d years, %d months, %d days, %d hours, %d mintues, %d seconds)",
            years, months, days,
            hours, minutes, seconds
        );
        return buffer;
    }

    operator std::string() const {
        return std::string(c_str());
    }


    // == std::cout ==

    friend std::ostream& operator<<(std::ostream& stream, const df_interval_t interval) {
        return stream << interval.c_str();
    }
};




class df_date_t {
  time_t t;

public:
    constexpr static char DEFAULT_FORMAT[] = "%Y-%m-%d %H:%M:%S";



    constexpr df_date_t(time_t t = time(nullptr) DF_ENV_IS_MSVC(+ 2208988800LL)) : t(t) {}

    df_date_t(const char* strdate, const char* fmt = DEFAULT_FORMAT) {
        parse_date(strdate, fmt);
    }

    df_date_t(const std::string& strdate, const char* fmt = DEFAULT_FORMAT) {
        parse_date(strdate.c_str(), fmt);
    }



    df_date_t& operator=(const char* strdate) {
        parse_date(strdate, DEFAULT_FORMAT);
        return *this;
    }



    // == parse ==

        // parse month
    size_t parse_month(const char* strmonth, size_t n, int* month) {
        for (int i = 0; i < 12; i++) {
            if (df_strncasecmp(strmonth, DF_MONTHS_FULL_NAMES[i], n) == 0) {
                *month = i;
                return strlen(DF_MONTHS_FULL_NAMES[i]);
            }
        }
        return 0;
    }

    // parse weekday
    size_t parse_weekday(const char* strweek, size_t n, int* week) {
        for (int i = 0; i < 7; i++) {
            if (df_strncasecmp(strweek, DF_WEEKDAYS_NAMES[i], n) == 0) {
                *week = i + 1;
                return strlen(DF_WEEKDAYS_NAMES[i]);
            }
        }
        return 0;
    }

    // self strptime for parsing time in win32
    // ! be causeful since this method will not complete all values
    int parse_time(const char* strdate, const char* fmt, struct tm* tm) {
        const char* fmt_start = fmt;
        int symbol, c;
        char buffer[10];
        int temp;

        // == reset tm values ==
        tm->tm_hour = 0;
        tm->tm_mon = 0;
        tm->tm_mday = 1;

        // == start parse ==

        while ((symbol = *fmt++) != 0) {
            // == filter ==
            // symbol is normal char: clear anything not suit to fmt
            if (symbol != '%') {
        filter:
                while ((c = *(strdate++)) && c != symbol);

                if (c == 0) {
                    break;
                }
                continue;
            }

            // == parse ==
            // if have known size, move strdate pointer and continue
            // if unknown size, continue to move by filter (!no savity)
            symbol = *(fmt++);

            switch (symbol) {
                // == date parsing ==
                case '%':
                    goto filter;
                case 'Y':
                    if (df_sscanf(strdate, "%04d", &tm->tm_year) == 0) {
                        return -1;
                    }

                    tm->tm_year -= 1900;
                    temp = 4;
                    goto label_pass;
                case 'y':
                    df_sscanf(strdate, "%02d", &tm->tm_year);
                    tm->tm_year += 100; // 2000 - 1900
                    temp = 2;
                    goto label_pass;
                case 'm':
                    df_sscanf(strdate, "%02d", &tm->tm_mon);
                    tm->tm_mon -= 1;
                    temp = 2;
                    goto label_pass;
                case 'B':
                    strdate += parse_month(strdate, 12, &tm->tm_mon);
                    continue;
                case 'b':
                    parse_month(strdate, 3, &tm->tm_mon);
                    temp = 3;
                    goto label_pass;
                case 'd':
                    df_sscanf(strdate, "%02d", &tm->tm_mday);
                    temp = 2;
                    goto label_pass;
                case 'e':
                    df_strncpy_s(buffer, sizeof(buffer), strdate, 2);
                    df_sscanf(buffer, "%d", &tm->tm_mday);
                    temp = 2;
                    goto label_pass;
                case 'j':
                    df_sscanf(strdate, "%03d", &tm->tm_yday);
                    temp = 3;
                    goto label_pass;
                case 'A':
                    strdate += parse_weekday(strdate, 12, &tm->tm_wday);
                    continue;
                case 'a':
                    parse_weekday(strdate, 3, &tm->tm_wday);
                    temp = 3;
                    goto label_pass;
                case 'u':
                    df_sscanf(strdate, "%01d", &tm->tm_wday);
                    temp = 1;
                    goto label_pass;
                case 'w':
                    df_sscanf(strdate, "%01d", &tm->tm_wday);
                    tm->tm_wday = 7 - tm->tm_wday;
                    temp = 1;
                    goto label_pass;
                
                // == time parsing ==
                case 'H':
                    if (df_sscanf(strdate, "%02d", &tm->tm_hour) == 0) {
                        return -1;
                    }
                    temp = 2;
                    goto label_pass;
                case 'l':
                    df_sscanf(strdate, "%02d", &temp);
                    tm->tm_hour = (tm->tm_hour + temp) % 24;
                    temp = 2;
                    goto label_pass;
                case 'p':
                    if (df_strncasecmp(strdate, "PM", 2) == 0) {
                        tm->tm_hour = (tm->tm_hour + 12) % 24;
                        strdate += 2;
                    }
                    continue;
                case 'M':
                    df_sscanf(strdate, "%02d", &tm->tm_min);
                    temp = 2;
                    goto label_pass;
                case 'S':
                    df_sscanf(strdate, "%02d", &tm->tm_sec);
                    temp = 2;
                    goto label_pass;
                case 'f':
                    continue;   // micro seconds (unusable)
                case 'c':
                    strdate += parse_time(strdate, "%a %b %d %H:%M:%S %Y", tm);
                    continue;
                case 'x':
                    strdate += parse_time(strdate, "%d/%m/%y", tm);
                    continue;
                case 'X':
                    strdate += parse_time(strdate, "%H:%M:%S", tm);
                    continue;
                    
            }

            continue;
        label_pass:
            do {
            if ((c = *(strdate++)) == 0) {
                return 0;
            }
            if (!isdigit(c)) {
                strdate--;
                break;
            }
            }
            while (--temp > 0);
        }

        return 0;
    }


    df_date_t& parse_date(const char* strdate, const char* fmt = DEFAULT_FORMAT) {
        struct tm tm{};
        if (parse_time(strdate, fmt, &tm) == 0) {
            t = df_mktime(&tm);
        }
        return *this;
    }



    // == incresce/reduce interval ==

    df_date_t& operator+=(time_t interval) {
        t += interval;
        return *this;
    }

    df_date_t& operator+(time_t interval) {
        t += interval;
        return *this;
    }



    df_date_t& operator+=(const df_interval_t& interval) {
        struct tm _tm;
        struct tm* tm = df_gmtime(&_tm, &t);

        struct tm dest{};
        dest.tm_year = tm->tm_year + interval.years;
        dest.tm_mon = tm->tm_mon + interval.months;
        dest.tm_mday = tm->tm_mday + interval.days;
        dest.tm_hour = tm->tm_hour + interval.hours;
        dest.tm_min = tm->tm_min + interval.minutes;
        dest.tm_sec = tm->tm_sec + interval.seconds;

        t = df_mktime(&dest);
        return *this;
    }

    df_date_t operator+(const df_interval_t& interval) const {
        return df_date_t(t) += interval;
    }




    df_date_t& operator-=(const df_interval_t& interval) {
        struct tm _tm;
        struct tm* tm = df_gmtime(&_tm, &t);

        struct tm dest{};
        dest.tm_year = tm->tm_year - interval.years;
        dest.tm_mon = tm->tm_mon - interval.months;
        dest.tm_mday = tm->tm_mday - interval.days;
        dest.tm_hour = tm->tm_hour - interval.hours;
        dest.tm_min = tm->tm_min - interval.minutes;
        dest.tm_sec = tm->tm_sec - interval.seconds;

        t = df_mktime(&dest);
        return *this;
    }

    df_date_t operator-(const df_interval_t& interval) const {
        return df_date_t(t) -= interval;
    }



    // == converting ==

    operator time_t() const {
        return t;
    }


    // == formatting ==

    const char* c_str(
            const char* fmt = DEFAULT_FORMAT,
            char* buffer = DF_DATE_FORMATTING_BUFFER,
            size_t buffer_size = DF_DATE_FORMATTING_BUFFER_LENGTH
    ) const {
        struct tm _tm;
        struct tm* tm = df_gmtime(&_tm, &t);

        df_strftime(buffer, fmt, tm);
        return buffer;
    }

    operator std::string() const {
        return c_str();
    }

    std::string to_string(const char* fmt = DEFAULT_FORMAT) const {
        return c_str();
    }


    // == std::cout ==

    friend std::ostream& operator<<(std::ostream& stream, const df_date_t& date) {
        return stream << date.c_str();
    }
};


#endif // _DF_DATE_HPP_