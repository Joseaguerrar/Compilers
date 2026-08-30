/**
 * @file compile_helper.h
 * @brief Assembly and linking of the program produced by the compiler.
 */

#ifndef COMPILE_HELPER_H
#define COMPILE_HELPER_H

/**
 * @brief Assembles and links a generated assembly file into an executable.
 *
 * The file is passed to @c gcc with the @c -no-pie option, which is required
 * because the generated code refers to its variables through absolute
 * addresses. The resulting executable is always named
 * @c program_mostro_binary.
 *
 * @param s_filename Path to the assembly file to be assembled and linked.
 * @return 0 if the executable was produced, or 1 if failure.
 */
int compile_program(const char *s_filename);

#endif
