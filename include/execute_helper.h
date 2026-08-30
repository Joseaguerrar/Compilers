/**
 * @file execute_helper.h
 * @brief Execution of the program produced by the compiler.
 */

#ifndef EXECUTE_HELPER_H
#define EXECUTE_HELPER_H

/**
 * @brief Runs an executable located in the current working directory.
 *
 * @param binary_filename Name of the executable to be run.
 * @return 0 if the program terminated with a status of zero, or 1 otherwise.
 */
int execute_program(const char *binary_filename);

#endif
