#include "timer_log.h"

#include <string.h>

/* 
 * Static file pointer used internally by the timer log module.
 * It is only visible inside this file.
 */
static FILE *timer_log_file = NULL;

/*
 * Initializes the CSV log file.
 * The file is opened in write mode and a header line is written.
 * Example header:
 *   function_name,poly_size,time_seconds
 */
int timer_log_init(const char *filename) {
    if (filename == NULL) {
        return -1;
    }

    timer_log_file = fopen(filename, "w");
    if (timer_log_file == NULL) {
        return -1;
    }

    /* Write CSV header */
    fprintf(timer_log_file, "function_name,poly_size,time_seconds\n");
    fflush(timer_log_file);

    return 0;
}

int timer_log_init_with_k(const char *filename) {
    if (filename == NULL) {
        return -1;
    }

    timer_log_file = fopen(filename, "w");
    if (timer_log_file == NULL) {
        return -1;
    }

    /* Write CSV header */
    fprintf(timer_log_file, "function_name,poly_size,time_seconds,k\n");
    fflush(timer_log_file);

    return 0;
}

/*
 * Writes a single row to the CSV file.
 * Each call appends one line with the given data.
 */
void timer_log_write_cutoff(const char *func_name, int poly_size, double time_seconds, int cutoff) {
    if (timer_log_file == NULL) {
        /* If the file is not initialized, there is nothing we can do. */
        return;
    }

    if (func_name == NULL) {
        func_name = "unknown";
    }

    /* CSV line: function_name,poly_size,time_seconds */
    fprintf(timer_log_file, "%s,%d,%.9f,%d\n", func_name, poly_size, time_seconds, cutoff);
    fflush(timer_log_file);
}


/*
 * Writes a single row to the CSV file.
 * Each call appends one line with the given data.
 */
void timer_log_write(const char *func_name, int poly_size, double time_seconds) {
    if (timer_log_file == NULL) {
        /* If the file is not initialized, there is nothing we can do. */
        return;
    }

    if (func_name == NULL) {
        func_name = "unknown";
    }

    /* CSV line: function_name,poly_size,time_seconds */
    fprintf(timer_log_file, "%s,%d,%.9f\n", func_name, poly_size, time_seconds);
    fflush(timer_log_file);
}


/*
 * Closes the CSV file and resets the internal pointer.
 */
void timer_log_close(void) {
    if (timer_log_file != NULL) {
        fclose(timer_log_file);
        timer_log_file = NULL;
    }
}
