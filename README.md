# object date (c++)

to handle date by smart object in c++

| programmer  | かずき     |
| ----------- | ---------- |
| version     | 1.3.0      |
| last update | 2026-02-28 |

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
    // == get current time (GMT+0) ==

    df_date_t now;
    std::cout << "current time (GMT+0): " << now << "\n"
            << "current time (local): " << now.to_local_time_string() << "\n\n";
  

    // == parse date string ==

	df_date_t date1("2026-01-21 16:00:00");
	df_date_t date2("2026/01/21 16:00", "%Y/%m/%d %H:%M");

	std::cout << "parsed date1: " << date1 << "\n"
            << "parsed date2: " << date2 << "\n"
            << "customize output format: " << date2.to_string("%Y/%m/%d %H:%M:%S") << "\n\n";    // customize output format


    // == adject date ==

	df_interval_t interval("3 year 1 month 4 day 1 hour 5 min 9 sec");
    date1 += interval;
    date2 -= interval;

	std::cout << "create interval_t: " << interval << "\n"
	        << "move front: " << date1 << "\n"
            << "move back: " << date2 << "\n\n";

  

    df_interval_t two_week("2 week");   // 2 week == 14 day
	date1 += df_interval_t("2 week");

	std::cout << "week keyword used: " << date1 << "\n\n";

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
