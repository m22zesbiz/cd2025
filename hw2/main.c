#include <stdio.h>
#include <string.h>

/*— VERY TINY SCANNER for digits, +, (, ), end —*/
typedef enum { TOK_NUM,
               TOK_PLUS,
               TOK_LP,
               TOK_RP,
               TOK_END,
               TOK_ERR } Token;
static char buf[128];
static int pos;
static char lexeme[32];
static Token tok;

void nextToken() {
    int i = 0;
    char c;
    while ((c = buf[pos]) == ' ' || c == '\t' || c == '\n') pos++;
    if ((c = buf[pos]) == '\0') {
        tok = TOK_END;
        return;
    }
    if (c >= '0' && c <= '9') {
        tok = TOK_NUM;
        while ((c = buf[pos]) >= '0' && c <= '9') lexeme[i++] = c, pos++;
        lexeme[i] = 0;
        return;
    }
    pos++;
    switch (c) {
        case '+':
            tok = TOK_PLUS;
            lexeme[0] = '+';
            lexeme[1] = 0;
            break;
        case '(':
            tok = TOK_LP;
            lexeme[0] = '(';
            lexeme[1] = 0;
            break;
        case ')':
            tok = TOK_RP;
            lexeme[0] = ')';
            lexeme[1] = 0;
            break;
        default:
            tok = TOK_ERR;
            lexeme[0] = c;
            lexeme[1] = 0;
    }
}

void match(Token want) {
    if (tok != want) {
        fprintf(stderr, "parse error: expected %d got '%s'\n", want, lexeme);
    }
    nextToken();
}

void ind(int n) {
    while (n-- > 0) putchar(' ');
}

/*— OFFSETS into those literal strings —*/
#define OFF_S_E 5
#define OFF_S_SP 6
#define OFF_Sp_S 7
#define OFF_E_S 6

void parse_S(int indent);
void parse_Sp(int indent);
void parse_E(int indent);

void parse_S(int indent) {
    ind(indent);
    printf("S -> ES'\n");
    /* call E under the 'E' in that line */
    parse_E(indent + OFF_S_E);
    /* call S' under the 'S' in that line */
    parse_Sp(indent + OFF_S_SP);
}

void parse_Sp(int indent) {
    if (tok == TOK_PLUS) {
        ind(indent);
        printf("S' -> +S\n");
        match(TOK_PLUS);
        /* call the next S under the 'S' in "+S" */
        parse_S(indent + OFF_Sp_S);
    } else {
        ind(indent);
        printf("S' -> ε\n");
    }
}

void parse_E(int indent) {
    if (tok == TOK_NUM) {
        ind(indent);
        printf("%s\n", lexeme);
        match(TOK_NUM);
    } else if (tok == TOK_LP) {
        ind(indent);
        printf("E -> (S)\n");
        match(TOK_LP);
        /* call the inner S under the 'S' in "(S)" */
        parse_S(indent + OFF_E_S);
        match(TOK_RP);
    } else {
        fprintf(stderr, "parse error: unexpected '%s'\n", lexeme);
    }
}

int main() {
    strcpy(buf, "(1+2+(3+4))+5");
    pos = 0;
    nextToken();
    parse_S(0);
    return 0;
}
