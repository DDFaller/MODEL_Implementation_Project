/*
 * MACHADO CARNEIRO FALLER Daniel (21400117)
 * LY Laura (21500152)
 * CRIVELLARO Federico (21502450)
 */

#include <stdio.h>
#include <sys/time.h>

/*
 * Global variables to store the start time and the last measured elapsed time.
 * They are static so they are only visible inside this file.
 */
static double start_time = 0.0;
static double last_elapsed = 0.0;

/*
 * Internal helper: returns current wall-clock time in seconds
 * with microsecond resolution, using gettimeofday().
 */
static double wallclock_time(void) {
    struct timeval tmp_time;
    gettimeofday(&tmp_time, NULL);
    return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6);
}

/*
 * start_timer
 * ------------
 * Stores the current time in the global variable `start_time`.
 * Call this right before the code you want to measure.
 */
void start_timer(void) {
    start_time = wallclock_time();
}

/*
 * end_timer
 * ---------
 * Computes the elapsed time since the last call to start_timer()
 * and stores it in the global variable `last_elapsed`.
 * Call this right after the code you want to measure.
 */
void end_timer(void) {
    double now = wallclock_time();
    last_elapsed = now - start_time;
}

/*
 * save_timer
 * ----------
 * Returns the last measured elapsed time (in seconds).
 * You should call end_timer() before using this, otherwise
 * it will return the previous measurement.
 */
double save_timer(void) {
    return last_elapsed;
}

/*
 * print_timer
 * -----------
 * Prints the last measured elapsed time to stdout.
 * This is just a convenience function; you can also use save_timer()
 * and print it yourself if you need custom formatting.
 */
void print_timer(void) {
    printf("Elapsed time: %f seconds\n", last_elapsed);
}

