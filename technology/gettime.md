# kenel中获取当前时间
```
//内核中获取当前时间
int get_current_time_str(char *output)
{
    struct timex  txc;
    struct rtc_time tm;

    do_gettimeofday(&(txc.time));
    txc.time.tv_sec -= sys_tz.tz_minuteswest * 60;
    rtc_time_to_tm(txc.time.tv_sec,&tm);

    return sprintf(output, "%04d-%02d-%02d %02d:%02d:%02d"
        ,tm.tm_year+1900
        ,tm.tm_mon+1
        ,tm.tm_mday
        ,tm.tm_hour
        ,tm.tm_min
        ,tm.tm_sec);
}
```
