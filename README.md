# object date (c++)

df_date_t is a part of my in-making repositories "Simply Dataframe C++" for handle date easier.


this date object using independent date calculation system base on unix time.h so that it will take less effects from different system.

| programmer  | かずき     |
| ----------- | ---------- |
| version     | 1.5.0      |
| last update | 2026-03-25 |

## features

1. parse and format date
2. adject date
3. std::cout supportting
4. customize parse / format support
5. C std based date calculating

## sample

```cpp
#include "date.hpp"
#include <iostream>

int main(int argc, char** argv) {
    std::cout << "object-date version: " << DF_DATE_VERSION << "\n\n";


    std::cout << "== make date by number ==\n";

    df_date_t zero;                 // zero, mean 1900-01-01 00:00:00
    df_date_t now = time(nullptr);  // get current time

    std::cout << "  zero:  gmt " << zero << ", local " << zero.to_local_cstr() << "\n"      // to_local_cstr() or to_local_string() to get local time string
            << "  now:   gmt " << now << ", local " << now.to_local_string() << "\n\n"    // gmt is default but you can also use to_gmt_cstr() or to_gmt_string()
    ;

    std::cout << "== parse date ==\n";

    df_date_t date1, date2, date3;
    try {
        date1 = df_date_t("2013-08-18 00:00:00");                 // parse date with default format, it is independent core `int df_date_t::strptime(date_str, fmt, struct tm* tm)`
        date2 = df_date_t("2009-05-17");                          // df_date_t will not care lessing part, except no meet one specifier
        date3 = df_date_t("2026/01/21 16:00", "%Y/%m/%d %H:%M");  // parse date with customize format

        // success: df_date_t("2026");
        // error: df_date_t("!@#$%");
        // error: df_date_t("hello world");
    }
    catch (df_failed_parse_date_exception_t& e) {       // error here if no meet one specifier
        std::cerr << e.what() << "\n";
    }

    std::cout << "  date1: gmt " << date1 << ", local " << date1.to_local_cstr() << "\n"
            << "  date2: gmt " << date2 << ", local " << date2.to_local_string() << "\n"
            << "  date3: gmt " << date3 << ", local " << date3.to_local_string() << "\n\n"
    ;

  
    // == adject date ==

    df_interval_t interval("3 year 1 month 4 day 1 hour 5 min 9 sec");
    date1 += interval;
    date2 -= interval;

    std::cout << "== adject date ==\n"
            << "  interval: " << interval << "\n"
            << "  date1 (future): " << date1 << "\n"
            << "  date2 (past): " << date2 << "\n\n"
    ;


    df_interval_t two_week("2 week");   // 2 week == 14 day
    date1 += df_interval_t("2 week");

    std::cout << "  date1 (future 2 week): " << date1 << "\n\n";


    std::cout << "== get last day of month ==\n"
            << "  current month: " << df_date_t(now).get_month_last() << "\n"
            << "  future 3 month: " << df_date_t(now).get_month_last(3) << "\n"
            << "  past 3 month: " << df_date_t(now).get_month_last(-3) << "\n"
    ;

    return 0;
}

```

**sample output**

```
object-date version: c++ 1.5.0 2026-03-25

== make date by number ==
  zero:  gmt 1900-01-01 00:00:00, local 1900-01-01 09:00:00
  now:   gmt 2026-03-25 10:29:15, local 2026-03-25 19:29:15

== parse date ==
  date1: gmt 2013-08-18 00:00:00, local 2013-08-18 09:00:00
  date2: gmt 2009-05-17 00:00:00, local 2009-05-17 09:00:00
  date3: gmt 2026-01-21 16:00:00, local 2026-01-22 01:00:00

== adject date ==
  interval: df_interval_t(3 year 1 month 4 day 1 hour 5 min 9 sec)
  date1 (future): 2016-09-22 01:05:09
  date2 (past): 2006-04-12 22:54:51

  date1 (future 2 week): 2016-10-06 01:05:09

== get last day of month ==
  current month: 2026-03-31 10:29:15
  future 3 month: 2026-06-30 10:29:15
  past 3 month: 2025-12-31 10:29:15

```
