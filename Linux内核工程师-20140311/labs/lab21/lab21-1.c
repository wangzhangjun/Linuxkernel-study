/*
 * Priorities and Timeslices
 *
 * Construct a program that is cpu-intensive, and takes at least 5-10
 * seconds to run under normal conditions.
 *
 * Feed it a nice increment (which can range from -20 to +19) as an
 * argument (you'll have to be superuser to feed negative increments)
 * and have the program call nice() to change its priority.
 *
 * Have the program, as its last step, open the file
 * /proc/[pid]/schedstat, and read in the three integer values, which
 * correspond to:
 *
 *    The number of jiffies spent while holding the cpu.
 *
 *    The number of jiffies spent sleeping.
 *
 *    The number of times the process was scheduled in.
 *
 * Note: for this to work the CONFIG_SCHEDSTATS option has to be set
 * in the kernel configuration file.
 *
 * From these values compute the average time slice in jiffies (you
 * can convert to time according to the value of HZ) the process was
 * granted while holding the cpu.
 *
 * You may want to separately compute the time executing with the use
 * of gettimeofday().
 *
 * For best results you should start off a bunch of instances with
 * different priorities simultaneously, so they have to schedule
 * against each other.  It would also be a good idea to keep the CPU
 * busy with other work, such as by doing a kernel compile.
 *
 * In the 2.6.23 kernel the units for scheduling statistics were
 * changed to TSC values, rather than jiffies.  Thus you'll have to
 * convert values to times by normalizing with the CPU frequency,
 * rather than the HZ value.  In the case of a variable speed CPU you
 * can not do this exactly, as the frequency is dynamic.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>

#define JMAX (400*100000)

#define GET_ELAPSED_TIME(tv1,tv2) ( \
  (double)( (tv2.tv_sec - tv1.tv_sec) \
            + .000001 * (tv2.tv_usec - tv1.tv_usec)))

double do_something (void)
{
    int j;
    double x = 0.0;
    struct timeval tv1, tv2;
    gettimeofday (&tv1, NULL);
    for (j = 0; j < JMAX; j++)
        x += 1.0 / (exp ((1 + x * x) / (2 + x * x)));
    gettimeofday (&tv2, NULL);
    return GET_ELAPSED_TIME (tv1, tv2);
}

int main (int argc, char *argv[])
{
    int niceval = 0, nsched;
    /* for kernels less than 2.6.21, this is HZ
       for tickless kernels this must be the MHZ rate
       e.g, for 2.6 GZ scale = 2600000000 */
    long scale = 1000;

    long ticks_cpu, ticks_sleep;
    pid_t pid;
    FILE *fp;
    char fname[256];
    double elapsed_time, timeslice, t_cpu, t_sleep;

    if (argc > 1)
        niceval = atoi (argv[1]);
    pid = getpid ();

    if (argc > 2)
        scale = atoi (argv[2]);

    /* give a chance for other tasks to queue up */
    sleep (3);

    sprintf (fname, "/proc/%d/schedstat", pid);
    /*    printf ("Fname = %s\n", fname); */

    if (!(fp = fopen (fname, "r"))) {
        printf ("Failed to open stat file\n");
        exit (-1);
    }
    if (nice (niceval) == -1 && niceval != -1) {
        printf ("Failed to set nice to %d\n", niceval);
        exit (-1);
    }
    elapsed_time = do_something ();

    fscanf (fp, "%ld %ld %d", &ticks_cpu, &ticks_sleep, &nsched);
    t_cpu = (float)ticks_cpu / scale;
    t_sleep = (float)ticks_sleep / scale;
    timeslice = t_cpu / (double)nsched;
    printf ("\nnice=%3d time=%8g secs pid=%5d"
            "  t_cpu=%8g  t_sleep=%8g  nsched=%5d"
            "  avg timeslice = %8g\n",
            niceval, elapsed_time, pid, t_cpu, t_sleep, nsched, timeslice);
    fclose (fp);

    exit (0);
}
