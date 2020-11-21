#include <sys/ioctl.h>   
#include <sys/types.h>   
#include <sys/stat.h>   
#include <fcntl.h>   
#include <linux/rtc.h>   
#include <errno.h>   
#include <stdio.h>   
#include <stdlib.h>   
#include <sys/time.h>   
#include <unistd.h>   
#define FREQ  32768
#define USEC_PER_SECOND 1000000   
typedef int MILLSEC;  
  
int g_fd = 0;  
   
int calc_cnt(MILLSEC millseconds )  
{  
   return (int)(millseconds * 1000.0 / USEC_PER_SECOND * FREQ ); //add 0.5 to meet precision in common   
//    return (int)(millseconds * 1000.0  );    
} 
void rtc_open()  
{  
    g_fd = open ("/dev/rtc", O_RDONLY);  
    if(g_fd < 0)  
    {     
        perror("open");  
        exit(errno);  
    }  
    printf("opened.\n");  
}

void set_freq()  
{  
    /*Set the freq*/  
    if(ioctl(g_fd, RTC_IRQP_SET, FREQ ) < 0)  
    {  
        perror("ioctl(RTC_IRQP_SET)");  
        close(g_fd);  
        exit(errno);  
    }  
    /* Enable periodic interrupts */  
    if(ioctl(g_fd, RTC_PIE_ON, 0) < 0)  
    {  
        perror("ioctl(RTC_PIE_ON)");  
        close(g_fd);  
        exit(errno);  
    }  
}


void rtc_task()  
{  
    printf("start counting...\n");  
    struct timeval tvs,tve;  
    unsigned long i = 0;  
    unsigned long data = 0;  
    while(1)  
    {  
        int time_to_wait;  
        printf("please input time to wait in millsecond, -1 to break\n");  
        scanf("%d",&time_to_wait);  
        if(time_to_wait < 0)  
            break;  
        //calc how many times to loop   
        int cnt = calc_cnt(time_to_wait);  
	printf("count = %d\n",cnt);
        gettimeofday( &tvs , 0 );  
        for(i = 0; i < cnt; i++)  
        {  
            if(read(g_fd, &data, sizeof(unsigned long)) < 0)  
            {  
                perror("read");  
                ioctl(g_fd, RTC_PIE_OFF, 0);  
                close(g_fd);  
                exit(errno);  
            }  
        }  
        gettimeofday(&tve,0);  
        printf("[%ld]timer usec\n" , (tve.tv_sec - tvs.tv_sec) * 1000000LL + (tve.tv_usec-tvs.tv_usec));  
    }  
    /* Disable periodic interrupts */  
    ioctl(g_fd, RTC_PIE_OFF, 0);  
    close(g_fd);  
}  
  
int main(int argc, char* argv[])  
{  
    rtc_open();  
    set_freq();  
    rtc_task();  
    return 0;  
}  
 
 

