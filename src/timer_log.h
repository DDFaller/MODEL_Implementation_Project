#ifndef TIMER_LOG_H
#define TIMER_LOG_H

/*
 * Simple helper to log timing information into a CSV file.
 * The CSV format is:
 *   function_name,poly_size,time_seconds
 */

#include <stdio.h>

/*
 * Initializes the CSV log file.
 * The file is opened in write mode ("w"), and a header line is written.
 * Returns 0 on success, -1 on failure.
 */
int timer_log_init(const char *filename);

/*
 * Writes one timing record to the CSV file.
 *  - func_name: name of the function/algorithm (e.g., "naive", "karatsuba")
 *  - poly_size: size of the polynomial (number of coefficients)
 *  - time_seconds: measured time in seconds
 */
void timer_log_write(const char *func_name, int poly_size, double time_seconds);

/*
 * Closes the CSV log file if it is open.
 */
void timer_log_close(void);

#endif /* TIMER_LOG_H */
