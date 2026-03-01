#include "date.hpp"
#include <iostream>

int main(int argc, char** argv) {
    // == print df_date_t version ==
    std::cout << "df_date_t version: " << DF_DATE_VERSION << "\n\n";

    // == make df_date_t ==

    df_date_t zero;                 // zero, mean 1900-01-01 00:00:00
    df_date_t now = time(nullptr);  // get current time

    // parse

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


    std::cout << "== making df_date_t ==\n"
            << "  zero:  gmt " << zero << ", local " << zero.to_local_cstr() << "\n"      // to_local_cstr() or to_local_string() to get local time string
            << "  now:   gmt " << now << ", local " << now.to_local_string() << "\n\n"    // gmt is default but you can also use to_gmt_cstr() or to_gmt_string()

            << "  date1: gmt " << date1 << ", local " << date1.to_local_cstr() << "\n"
            << "  date2: gmt " << date2 << ", local " << date2.to_local_string() << "\n\n";

  
    // == adject date ==

    df_interval_t interval("3 year 1 month 4 day 1 hour 5 min 9 sec");
    date1 += interval;
    date2 -= interval;

    std::cout << "== adject date ==\n"
            << "  interval: " << interval << "\n"
	    << "  move front: " << date1 << "\n"
            << "  move back: " << date2 << "\n\n";

  

    df_interval_t two_week("2 week");   // 2 week == 14 day
    date1 += df_interval_t("2 week");

    std::cout << "week keyword used: " << date1 << "\n\n";

    return 0;
}

