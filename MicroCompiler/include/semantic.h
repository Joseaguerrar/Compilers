/**
 * @file semantic.h
 * @brief Semantic records and action routines of the Micro compiler.
 *
 * The parser calls these action routines as soon as it recognizes a construct
 * that requires translation.
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

/** @brief Maximum length of an identifier, including the terminating null character. */
#define MAXIDLEN 33

/** @brief Fixed size buffer that stores an identifier or a generated temporary name. */
typedef char string[MAXIDLEN];

/**
 * @brief Semantic record describing an additive operator.
 *
 * @c PLUS denotes addition and @c MINUS denotes subtraction.
 */
typedef struct operator {
    enum op { PLUS, MINUS } operator;
} op_rec;

/**
 * @brief Classifies the kinds of expression an expr_rec may describe.
 *
 * @c IDEXPR denotes a user-declared variable, @c LITERALEXPR denotes a value
 * known at compile time, and @c TEMPEXPR denotes a compiler-generated
 * temporary holding an intermediate result.
 */
enum expr { IDEXPR, LITERALEXPR, TEMPEXPR };

/**
 * @brief Semantic record describing an expression.
 *
 * The @c kind field selects which member of the
 * anonymous union is valid. A literal carries its value; a variable or
 * a temporary carries the name of the memory location that holds it.
 */
typedef struct expression {
    enum expr kind; /**< Field selecting the valid union member. */
    union {
        string name; /**< Memory location, valid when @c kind is @c IDEXPR or @c TEMPEXPR. */
        int val; /**< Compile-time value, valid when @c kind is @c LITERALEXPR. */
    };

} expr_rec;

/**
 * @brief Produces the start of the target program.
 */
void start(void);

/**
 * @brief Produces the end of the target program and the storage declarations.
 */
void finish(void);

/**
 * @brief Produces the code that stores an expression into a variable.
 *
 * @param target Semantic record of the variable that receives the value.
 * @param source Semantic record of the expression being assigned.
 */
void assign(expr_rec target, expr_rec source);

/**
 * @brief Builds the semantic record of the add or minus operator just matched.
 *
 * @return An operator record set to @c PLUS or @c MINUS according to the
 *         current token of the parser.
 */
op_rec process_op(void);

/**
 * @brief Translates an infix addition or subtraction.
 *
 * When both operands are literals the result is folded at compile time and no
 * instruction is emitted. Otherwise a fresh temporary is allocated to hold the
 * result and the corresponding arithmetic instructions are generated.
 *
 * @param e1 Semantic record of the left operand.
 * @param op Semantic record of the operator.
 * @param e2 Semantic record of the right operand.
 * @return A literal record when the operation was folded, or a temporary
 *         record naming the location of the result.
 */
expr_rec gen_infix(expr_rec e1, op_rec op, expr_rec e2);

/**
 * @brief Translates the conditional expression @c (E1|E2|E3).
 *
 * The expression returns @p e2 when @p e1 is non-zero and @p e3 otherwise. When
 * the condition is a literal the branch is selected at compile time and no
 * instruction is emitted.
 *
 * @param e1 Semantic record of the condition.
 * @param e2 Semantic record of the branch selected when the condition is non-zero.
 * @param e3 Semantic record of the branch selected when the condition is zero.
 * @return The selected operand when the condition was folded, or a temporary
 *         record naming the location of the result.
 */
expr_rec gen_cond(expr_rec e1, expr_rec e2, expr_rec e3);

/**
 * @brief Produces the code that reads an integer into a variable.
 *
 * @param in_var Semantic record of the variable that receives the input value.
 */
void read_id(expr_rec in_var);

/**
 * @brief Builds the semantic record of the identifier just matched.
 *
 * The identifier is entered into the symbol table if it has not been seen
 * before.
 *
 * @return An expression record of kind @c IDEXPR naming the variable.
 */
expr_rec process_id(void);

/**
 * @brief Builds the semantic record of the integer literal just matched.
 *
 * @return An expression record of kind @c LITERALEXPR holding the value.
 */
expr_rec process_literal(void);

/**
 * @brief Produces the code that writes the value of an expression.
 *
 * @param out_expr Semantic record of the expression to be written.
 */
void write_expr(expr_rec out_expr);

#endif
