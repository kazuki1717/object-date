# object date (c++)

to handle date by smart object in c++

| programmer  | かずき     |
| ----------- | ---------- |
| version     | 1.2.0      |
| last update | 2026-01-24 |

## features

1. parse and format date
2. move date
3. std::cout supportting
4. customize parse / format support
5. C std based date calculating

## sample

```cpp
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

```

the output would be

```
parsed date1: 2026-01-21 16:00:00
parsed date2: 2026-01-21 16:00:00
customize output format: 2026/01/21 16:00:00
create interval_t: interval_t(3 years, 1 months, 4 days, 1 hours, 5 mintues, 9 seconds)
forwarded date: 2029-02-25 17:05:09
backwarded date: 2026-01-21 16:00:00
week keyword used: 2026-02-04 16:00:00
```
