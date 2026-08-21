// FROM BOOK pages 43, 45

char *get_temp(void) {
    static int max_temp = 0;
    static char tempname[MAXIDLEN];

    max_temp++;
    sprintf(tempname, "Temp_%d", max_temp);
    check_id(tempname);
    return tempname;
}

expr_rec process_literal(void) {
    expr_rec t;
    t.kind = LITERALEXPR;
    (void) sscanf(token_buffer, "%d", &t.val);
    return t;
}