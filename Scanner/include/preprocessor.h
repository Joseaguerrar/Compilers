#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stddef.h>

/*
 * Preprocesses input_filename and creates a temporary file.
 *
 * temp_filename receives the generated temporary filename.
 *
 * Returns:
 *     1 on success.
 *     0 on fatal error.
 */
int preprocess_file(
    const char *input_filename,
    char *temp_filename,
    size_t temp_filename_size,
    int *lexical_errors,
    int *preprocessor_errors
);

#endif