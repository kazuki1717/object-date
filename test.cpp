#include "date.hpp"
#include <iostream>


int main(int argc, char** argv) {
    std::cout << "object-date version: " << DF_DATE_VERSION << "\n\n";

    std::cout << "== make date by number ==\n";

    df_date_t zero;                     // zero, mean 1900-01-01 00:00:00
    df_date_t now = time(nullptr);      // get current time

    std::cout << "  zero:  gmt " << zero << ", local " << zero.to_local_cstr() << "\n"      // to_local_cstr() or to_local_string() to get local time string
            << "  now:   gmt " << now << ", local " << now.to_local_string() << "\n\n"    // gmt is default but you can also use to_gmt_cstr() or to_gmt_string()
    ;

    std::cout << "== parse date ==\n";

    df_date_t date1, date2, date3;
    try {
        date1 = df_date_t("2013-08-18 00:00:00");                   // parse date with default format, it is independent core `int df_date_t::strptime(date_str, fmt, struct tm* tm)`
        date2 = df_date_t("2009-05-17");                            // df_date_t will not care lessing part, except no meet one specifier
        date3 = df_date_t("2026/01/21 16:00", "%Y/%m/%d %H:%M");    // parse date with customize format

        // success: df_date_t("2026");
        // error: df_date_t("!@#$%");
        // error: df_date_t("hello world");
    }
    catch (df_date_t::parse_exception_t& e) {       // error here if no meet one specifier
        std::cerr << e.what() << "\n";
    }

    std::cout << "  date1: gmt " << date1 << ", local " << date1.to_local_cstr() << "\n"
            << "  date2: gmt " << date2 << ", local " << date2.to_local_string() << "\n"
            << "  date3: gmt " << date3 << ", local " << date3.to_local_string() << "\n\n"
    ;


    std::cout << "== create df_interval_t instance ==\n";

    df_interval_t interval_1("3 year 1 month 4 day 1 hour 5 min 9 sec");
    df_interval_t interval_2(3, 1, 4, 1, 5, 9);

    df_interval_t interval_3;
    interval_3.hour = 1;
    interval_3.minute = 5;
    interval_3.second = 9;

    df_interval_t two_week("2 week");   // 2 week == 14 day


    std::cout << "  interval_1: " << interval_1 << "\n"
            "  interval_2: " << interval_2 << "\n"
            "  interval_3: " << interval_3 << "\n"
            "  two_week: " << two_week << "\n\n"
    ;


    std::cout << "== shift date ==\n";

    date1 += interval_1;
    date2 -= interval_1;
    date3 += two_week;

    std::cout << "  date1 (future): " << date1 << "\n"
            << "  date2 (past): " << date2 << "\n\n"
            << "  date3 (+2 week): " << date3 << "\n\n";
    ;


    std::cout << "== get last day of month ==\n"
            << "  current month: " << df_date_t(now).get_month_last() << "\n"
            << "  future 3 month: " << df_date_t(now).get_month_last(3) << "\n"
            << "  past 3 month: " << df_date_t(now).get_month_last(-3) << "\n\n"
    ;

    return 0;
}

