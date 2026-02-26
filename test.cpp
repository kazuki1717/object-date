#include "date.hpp"
#include <iostream>

int main(int argc, char** argv) {
    // parse date string by default format
	date_t date("2026-01-21 16:00:00");
	std::cout << "parsed date1: " << date << std::endl;

    // parse date string by customize format
	date = date_t("2026/01/21 16:00", "%Y/%m/%d %H:%M");
	std::cout << "parsed date2: " << date << std::endl;

    // customize output format
    std::cout << "customize output format: " << date.to_string("%Y/%m/%d %H:%M:%S") << std::endl;

    // forward/backward date
	interval_t interval("3 year 1 month 4 day 1 hour 5 min 9 sec");
	std::cout << "create interval_t: " << interval << std::endl;
    
    date += interval;
	std::cout << "forwarded date: " << date << std::endl;

    // move backward
    date -= interval;
    std::cout << "backwarded date: " << date << std::endl;

    // move forward 2*7 = 14 days
	date += interval_t("2 week");
	std::cout << "week keyword used: " << date << std::endl;

    return 0;
}
