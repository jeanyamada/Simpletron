/* 
 * File:   main.c
 * Author: jean
 *
 * Created on 19 de Junho de 2019, 10:19
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/*tokens*/
#define IDENT 1
#define CONSTA 2
#define ADD_OP 3
#define SUB_OP 4
#define MULT_OP 5
#define DIV_OP 6
#define LEFT_PAREN 7
#define RIGHT_PAREN 8
#define LARGER_OP 9
#define LESS_OP 10
#define ATTRIB_OP 11
#define EQUAL_OP 12
#define DIFF_OP 13
#define LARGER_EQUAL_OP 21
#define LESS_EQUAL_OP 22

/*reserved*/
#define PRINT_RES 14
#define LET_RES 15
#define INPUT_RES 16
#define REM_RES 17
#define GOTO_RES 18
#define IF_RES 19
#define END_RES 20

#define MEMORY_INSTALLED 100
#define MEMORY_TEMP 70

/*operand*/
#define READ 10
#define WRITE 11
#define LOAD 20
#define STORE 21
#define ADD 30
#define SUBTRACT 31
#define DIVIDE 32
#define MULTIPLY 33
#define BRANCH 40
#define BRANCHNEG 41
#define BRANCHZERO 42
#define HALT 43
#define FINISH -99999
#define MEMORY_INSTALLED 100
#define EMPTY 0
#define INTEGER 9999

typedef struct regs {
    /*registrador do acumulador*/
    int accumulator;

    /*registrar o local na memória que contém a instrução que está sendo executada*/
    int instructionCounter;


    int instructionRegister;

    /*indicar a operação que está sendo executada no momento*/
    int operationCode;

    int operand;

} s_regs;

typedef struct TableEntry {
    int symbol;
    char type;
    int location;
} tableEntry;

struct stackNode {
    char data;
    struct stackNode *nextPtr;
};

typedef struct stackNode StackNode;
typedef StackNode *StackNodePtr;

struct stackNodePos {
    int data;
    struct stackNodePos *nextPtr;
};

typedef struct stackNodePos StackNodePos;
typedef StackNodePos *StackNodePtrPos;


char infix[100], postfix[100];

void initialize();
void dump_simpletron();
void dump_simpletron_in_file();

void execute();
void write();
unsigned read();
void load();
void store();
unsigned add();
unsigned subtract();
unsigned multiply();
void branch();
void branchneg();
void branchzero();
unsigned divide();
unsigned halt();

s_regs r;


bool firstPass();

void convertToPostfix(StackNodePtr *topPtr);
int isOperator(char c);
int precedence(char operator1, char operator2);
void push(StackNodePtr *topPtr, char value);
char pop(StackNodePtr *topPtr);
char stackTop(StackNodePtr topPtr);
int isEmpty(StackNodePtr topPtr);
void printStack(StackNodePtr topPtr);
int isDigit(char c);


void pushPos(StackNodePtrPos *topPtr, int value);
int popPos(StackNodePtrPos *topPtr);
int stackTopPos(StackNodePtrPos topPtr);
int isEmptyPos(StackNodePtrPos topPtr);
void printStackPos(StackNodePtrPos topPtr);
void evaluatePostfixExpression(StackNodePtrPos *topPtr);


bool openFile(char *str, char *mode);
bool closeFile();


bool isNumber(char *str);

void insertNumberLineTableSymbol(int line, int index);
void insertIdTableSymbol(char id);
void insertConstantTableSymbol(int constant);
int searchTableSymbol(int id, char type);
void printTableSymbol();

/*produções*/
bool cmd();
bool stmt();
bool print_cmd();
bool rem();
bool input();
bool expr();
bool term();
bool factor();
bool if_cmd();
bool goto_cmd(int branch);
bool let();

bool syntaxAnalysis();
bool lexicalAnalysis();
bool classificationLexeme();

bool getToken();
bool getNextToken();


int postionLineFile [100];

void loadFile();
void printCode();


bool error = false;

FILE *inputFile;
tableEntry tableSymbol[100];

int memory[MEMORY_INSTALLED];
void printMemory();

int indexTableSymbol = 0;
int indexMemoryTop = 0;
int indexMemoryBot = 0;
int line = 0;

char *lexeme;
int token, nextToken;
char c;

void createSMLInput(char id);
void createSMLPrint();

int main(int argc, char **argv) {


    if (argc == 2) {

        if (openFile(argv[1], "r")) {

            loadFile();

            memset(memory, 0, sizeof (int) * MEMORY_INSTALLED);

            if (firstPass()) {

                printTableSymbol();

                printMemory();

                closeFile();

            }

        }

    }

    return 0;
}

bool firstPass() {
    return syntaxAnalysis();
}

bool syntaxAnalysis() {
    return cmd();
}

bool lexicalAnalysis() {

    int state = 0;

    if (!feof(inputFile)) {


        if (lexeme != NULL) {
            free(lexeme);
            lexeme = NULL;
        }

        lexeme = malloc(sizeof (char) * 100);

        memset(lexeme, '\0', sizeof (char) * 100);

        while (state != 4 && fscanf(inputFile, "%c", &c) != EOF) {

            switch (state) {

                case 0:
                    if (c >= 'a' && c <= 'z') {
                        lexeme[0] = c;
                        state = 1;
                    } else if (c >= '0' && c <= '9') {
                        lexeme[0] = c;
                        state = 2;
                    } else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')') {
                        lexeme[0] = c;
                        state = 4;
                    } else if (c == '=' || c == '!' || c == '>' || c == '<') {
                        lexeme[0] = c;
                        state = 3;
                    }
                    break;

                case 1:
                    if (c < 'a' || c > 'z') {

                        state = 4;
                    } else {
                        lexeme[strlen(lexeme)] = c;
                    }
                    break;
                case 2:
                    if (c < '0' || c > '9') {
                        state = 4;
                    } else {
                        lexeme[strlen(lexeme)] = c;
                    }
                    break;

                case 3:
                    if (c == '=') {
                        lexeme[1] = c;
                    }
                    state = 4;
                    break;
            }
        }

        return classificationLexeme();
    } else {
        token = -1;
    }
    return false;
}

bool getToken() {

    return lexicalAnalysis();

}

bool getNextToken() {

    long int position = ftell(inputFile);

    if (!lexicalAnalysis()) {
        return false;
    }

    fseek(inputFile, position, SEEK_SET);

    return true;
}

bool classificationLexeme() {
    if (strcmp(lexeme, "let") == 0) {
        token = LET_RES;
    } else if (strcmp(lexeme, "rem") == 0) {
        token = REM_RES;
    } else if (strcmp(lexeme, "print") == 0) {
        token = PRINT_RES;
    } else if (strcmp(lexeme, "input") == 0) {
        token = INPUT_RES;
    } else if (strcmp(lexeme, "goto") == 0) {
        token = GOTO_RES;
    } else if (strcmp(lexeme, "if") == 0) {
        token = IF_RES;
    } else if (strcmp(lexeme, "end") == 0) {
        token = END_RES;
    } else if (strcmp(lexeme, "+") == 0) {
        token = ADD_OP;
    } else if (strcmp(lexeme, "-") == 0) {
        token = SUB_OP;
    } else if (strcmp(lexeme, "*") == 0) {
        token = MULT_OP;
    } else if (strcmp(lexeme, "/") == 0) {
        token = DIV_OP;
    } else if (strcmp(lexeme, "<") == 0) {
        token = LESS_OP;
    } else if (strcmp(lexeme, ">") == 0) {
        token = LARGER_OP;
    } else if (strcmp(lexeme, "==") == 0) {
        token = EQUAL_OP;
    } else if (strcmp(lexeme, "!=") == 0) {
        token = DIFF_OP;
    } else if (strcmp(lexeme, "=") == 0) {
        token = ATTRIB_OP;
    } else if (strcmp(lexeme, "(") == 0) {
        token = LEFT_PAREN;
    } else if (strcmp(lexeme, ")") == 0) {
        token = RIGHT_PAREN;
    } else if (strcmp(lexeme, ">=") == 0) {
        token = LARGER_EQUAL_OP;
    } else if (strcmp(lexeme, "<=") == 0) {
        token = LESS_EQUAL_OP;
    } else if (isNumber(lexeme)) {
        token = CONSTA;
    } else if (strlen(lexeme) == 1 && (lexeme[0] >= 'a' && lexeme[0] <= 'z')) {
        token = IDENT;
    } else {
        printf("\nerro léxico na linha %d: lexema '%s' não pertence a linguagem\n", line, lexeme);
        return false;
    }
    return true;
}

bool isNumber(char *str) {
    int i;

    for (i = 0; i < ((int) strlen(str)); i++) {
        if (!isDigit(str[i])) {
            return false;
        }
    }
    return true;
}

bool openFile(char *str, char *mode) {
    inputFile = fopen(str, mode);

    if (inputFile != NULL) {
        return true;
    }

    return false;
}

bool closeFile() {
    if (fclose(inputFile) == 0) {
        return true;
    }

    return false;
}

void insertNumberLineTableSymbol(int line, int index) {
    tableSymbol[indexTableSymbol].symbol = line;
    tableSymbol[indexTableSymbol].type = 'L';
    tableSymbol[indexTableSymbol].location = index;

    indexTableSymbol++;
}

void insertIdTableSymbol(char id) {
    if (searchTableSymbol(id, 'V') == -1) {
        tableSymbol[indexTableSymbol].symbol = id;
        tableSymbol[indexTableSymbol].type = 'V';
        tableSymbol[indexTableSymbol].location = 99 - indexMemoryBot;

        indexMemoryBot++;
        indexTableSymbol++;
    }
}

void insertConstantTableSymbol(int constant) {
    if (searchTableSymbol(constant, 'C') == -1) {
        tableSymbol[indexTableSymbol].symbol = constant;
        tableSymbol[indexTableSymbol].type = 'C';
        tableSymbol[indexTableSymbol].location = 99 - indexMemoryBot;

        memory[tableSymbol[indexTableSymbol].location] = constant;

        indexMemoryBot++;
        indexTableSymbol++;
    }
}

int searchTableSymbol(int id, char type) {
    int i;

    for (i = 0; i < indexTableSymbol; i++) {
        if (tableSymbol[i].symbol == id && tableSymbol[i].type == type) {
            return i;
        }
    }
    return -1;
}

void printTableSymbol() {
    int i;
    printf("\n*** Tabela de Simbolo: ***\n");
    for (i = 0; i < indexTableSymbol; i++) {
        if (tableSymbol[i].type == 'V') {
            printf("symbol: %c\ntype: %c\nlocation: %d\n\n", tableSymbol[i].symbol, tableSymbol[i].type, tableSymbol[i].location);
        } else {
            printf("symbol: %d\ntype: %c\nlocation: %d\n\n", tableSymbol[i].symbol, tableSymbol[i].type, tableSymbol[i].location);
        }
    }

}

/*produções*/
bool cmd() {

    if (!stmt()) {
        return false;
    }
    if (token == END_RES) {
        r.instructionRegister = 4300;
        execute();
    } else {
        printf("\nerro de sintaxe: esperava o fim do programa com o comando end\n");
        return false;
    }

    return true;
}

bool if_cmd() {
    StackNodePtr ptrInfix = NULL;
    StackNodePtrPos ptrPostFix = NULL;
    int auxToken;
    long int position;

    memset(infix, '\0', sizeof (char) * 100);
    memset(postfix, '\0', sizeof (char) * 100);

    if (!expr()) {
        return false;
    }

    convertToPostfix(&ptrInfix);
    evaluatePostfixExpression(&ptrPostFix);


    r.instructionRegister = 2000 + MEMORY_TEMP;

    execute();

    r.instructionRegister = 2100 + MEMORY_TEMP + 1;

    execute();

    if (token == LARGER_OP || token == LESS_OP || token == EQUAL_OP || token == DIFF_OP || token == LESS_EQUAL_OP || token == LARGER_EQUAL_OP) {

        auxToken = token;

        if (!getToken()) {
            return false;
        }

        memset(infix, '\0', sizeof (char) * 100);
        memset(postfix, '\0', sizeof (char) * 100);


        if (!expr()) {
            return false;
        }

        convertToPostfix(&ptrInfix);
        evaluatePostfixExpression(&ptrPostFix);


        r.instructionRegister = 2000 + MEMORY_TEMP;

        execute();

        r.instructionRegister = 2100 + MEMORY_TEMP + 2;

        execute();

        position = ftell(inputFile);

        if (!getToken()) {
            return false;
        }

        if (token == GOTO_RES) {

            if (auxToken == EQUAL_OP) {

                r.instructionRegister = 2000 + MEMORY_TEMP + 1;

                execute();

                r.instructionRegister = 3100 + MEMORY_TEMP + 2;

                execute();

                if (!goto_cmd(4200)) {
                    return false;
                }
            } else if (auxToken == LARGER_OP) {

                r.instructionRegister = 2000 + MEMORY_TEMP + 1;

                execute();

                r.instructionRegister = 3100 + MEMORY_TEMP + 2;

                execute();

                if (!goto_cmd(4100)) {
                    return false;
                }
            } else if (auxToken == LESS_OP) {

                r.instructionRegister = 2000 + MEMORY_TEMP + 2;

                execute();

                r.instructionRegister = 3100 + MEMORY_TEMP + 1;

                execute();

                if (!goto_cmd(4100)) {
                    return false;
                }
            } else if (auxToken == LARGER_EQUAL_OP) {

                r.instructionRegister = 2000 + MEMORY_TEMP + 1;

                execute();

                r.instructionRegister = 3100 + MEMORY_TEMP + 2;

                execute();

                if (!goto_cmd(4200)) {
                    return false;
                }

                fseek(inputFile, position, SEEK_SET);

                if (!getToken()) {
                    return false;
                }

                if (!goto_cmd(4100)) {
                    return false;
                }

            } else if (auxToken == LESS_EQUAL_OP) {

                r.instructionRegister = 2000 + MEMORY_TEMP + 2;

                execute();

                r.instructionRegister = 3100 + MEMORY_TEMP + 1;

                execute();

                if (!goto_cmd(4100)) {
                    return false;
                }

                fseek(inputFile, position, SEEK_SET);

                if (!getToken()) {
                    return false;
                }


                if (!goto_cmd(4200)) {
                    return false;
                }

            } else if (auxToken == DIFF_OP) {

                r.instructionRegister = 2000 + MEMORY_TEMP + 1;

                execute();

                r.instructionRegister = 3100 + MEMORY_TEMP + 2;

                execute();

                if (!goto_cmd(4100)) {
                    return false;
                }

                fseek(inputFile, position, SEEK_SET);


                if (!getToken()) {
                    return false;
                }


                r.instructionRegister = 2000 + MEMORY_TEMP + 2;

                execute();

                r.instructionRegister = 3100 + MEMORY_TEMP + 1;

                execute();

                if (!goto_cmd(4100)) {
                    return false;
                }

            }
        } else {
            printf("\nerro de sintaxe na linha %d: esperava goto após expressão lógica\n", line);
            return false;
        }


    } else {
        printf("\nerro de sintaxe na linha %d: esperava um operador lógico\n", line);
        return false;
    }
    return true;
}

bool goto_cmd(int branch) {


    if (token == GOTO_RES) {

        if (!getToken()) {
            return false;
        }

        if (token == CONSTA) {

            r.instructionRegister = branch + atoi(lexeme);
            execute();

        } else {
            printf("\nerro de sintaxe na linha %d : esperava constante após goto\n", line);
            return false;
        }
    } else {
        return false;
    }

    return true;
}

bool stmt() {

    do {
        if (!getToken()) {
            return false;
        }

        if (token != CONSTA) {
            printf("\nerro de sintaxe na linha %d: esperava constante no inicio da linha\n", line);
            return false;
        }

        if (!getToken()) {
            return false;
        }

        if (token == INPUT_RES) {
            if (!input()) {
                return false;
            }
        } else if (token == REM_RES) {
            if (!rem()) {
                return false;
            }
        } else if (token == PRINT_RES) {
            if (!print_cmd()) {
                return false;
            }
        } else if (token == IF_RES) {
            if (!if_cmd()) {
                return false;
            }
        } else if (token == GOTO_RES) {
            if (!goto_cmd(4000)) {
                return false;
            }
        } else if (token == LET_RES) {
            if (!let()) {
                return false;
            }
        } else if (token != END_RES) {
            printf("\nerro de sintaxe na linha %d: esperava comando apos constante de linha\n", line);
            return false;
        }

    } while (token != END_RES && token != -1);

    return true;
}

bool input() {

    int index;

    if (!getToken()) {
        return false;
    }

    if (token == IDENT) {

        insertIdTableSymbol(lexeme[0]);

        index = searchTableSymbol(lexeme[0], 'V');

        r.instructionRegister = 1000 + tableSymbol[index].location;

        execute();

    } else {

        printf("\nerro de sintaxe na linha %d: esperava um ID\n", line);
        return false;

    }
    return true;
}

bool rem() {

    while (c != '\n') {

        c = fgetc(inputFile);
    }
    return true;
}

bool print_cmd() {

    StackNodePtr ptrInfix = NULL;
    StackNodePtrPos ptrPostFix = NULL;

    memset(infix, '\0', sizeof (char) * 100);
    memset(postfix, '\0', sizeof (char) * 100);

    if (!expr()) {
        return false;
    }
    convertToPostfix(&ptrInfix);
    evaluatePostfixExpression(&ptrPostFix);

    r.instructionRegister = 1100 + MEMORY_TEMP;

    execute();

    return true;


}

bool let() {

    StackNodePtr ptrInfix = NULL;
    StackNodePtrPos ptrPostFix = NULL;
    int index;

    if (!getToken()) {
        return false;
    }

    if (token == IDENT) {


        insertIdTableSymbol(lexeme[0]);

        index = searchTableSymbol(lexeme[0], 'V');

        if (!getToken()) {
            return false;
        }

        if (token == ATTRIB_OP) {

            memset(infix, '\0', sizeof (char) * 100);
            memset(postfix, '\0', sizeof (char) * 100);

            if (!expr()) {
                return false;
            }
            convertToPostfix(&ptrInfix);

            evaluatePostfixExpression(&ptrPostFix);

            r.instructionRegister = 2000 + MEMORY_TEMP;

            execute();

            r.instructionRegister = 2100 + tableSymbol[index].location;

            execute();


        } else {
            printf("\nerro de sintaxe na linha %d: esperava símbolo '='\n", line);
            return false;
        }
    } else {
        printf("\nerro de sintaxe na linha %d: esperava um ID\n", line);
        return false;
    }
    return true;
}

bool expr() {
    bool value;

    if ((value = term())) {

        if (!getNextToken()) {
            return false;
        }

        while (token == ADD_OP || token == SUB_OP) {

            if (!getToken()) {
                return false;
            }

            strcat(infix, lexeme);

            value = term();
        }
    }

    return value;
}

bool term() {
    bool value;

    if ((value = factor())) {

        if (!getNextToken()) {
            return false;
        }

        while (token == MULT_OP || token == DIV_OP) {

            if (!getToken()) {
                return false;
            }

            strcat(infix, lexeme);

            value = factor();
        }
    }
    return value;

}

bool factor() {

    if (!getToken()) {
        return false;
    }

    if (token == CONSTA) {

        insertConstantTableSymbol(atoi(lexeme));

        strcat(infix, lexeme);
    } else if (token == IDENT) {

        insertIdTableSymbol(lexeme[0]);


        strcat(infix, lexeme);
    } else if (token == LEFT_PAREN) {

        strcat(infix, lexeme);

        if (!expr()) {
            return false;
        }

        if (!getToken()) {
            return false;
        }

        if (token == RIGHT_PAREN) {

            strcat(infix, lexeme);
        } else {
            printf("\nerro de sintaxe na linha %d: esperava símbolo ')'\n", line);
            return false;
        }
    } else {
        printf("\nerro de sintaxe na linha %d: esperava um ID, constante ou '('\n", line);

        return false;
    }

    return true;
}

void printMemory() {
    int i;
    printf("\n*** MEMORY: ***\n");
    for (i = 0; i < MEMORY_INSTALLED; i++) {

        printf("\n%d : %d\n", i, memory[i]);
    }
}

/*conversão para SML*/

void convertToPostfix(StackNodePtr *topPtr) {
    int indexInfix = 0, indexPostfix = 0;
    char c;
    push(& (*topPtr), '(');

    infix[strlen(infix)] = ')';

    while (isEmpty(*topPtr)) {

        c = infix[indexInfix++];

        if (isalnum(c)) {
            do {
                postfix[indexPostfix++] = c;
                c = infix[indexInfix++];
            } while (isalnum(c));

            postfix[indexPostfix++] = ' ';
            indexInfix--;
        } else if (c == '(') {
            push(& (*topPtr), '(');
        } else if (isOperator(c)) {
            while (isOperator(stackTop(*topPtr)) && precedence(stackTop(*topPtr), c) > -1) {
                postfix[indexPostfix++] = pop(& (*topPtr));
                postfix[indexPostfix++] = ' ';
            }

            push(& (*topPtr), c);
        } else if (c == ')') {
            while (stackTop(*topPtr) != '(') {

                postfix[indexPostfix++] = pop(&(*topPtr));
                postfix[indexPostfix++] = ' ';
            }
            pop(&(*topPtr));
        }

    }

}

int isDigit(char c) {
    if (c >= 48 && c <= 57)

        return 1;
    return 0;
}

int isOperator(char c) {
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%')

        return 1;
    return 0;
}

void printStack(StackNodePtr topPtr) {

    StackNodePtr cpyPtr = topPtr;

    while (cpyPtr != NULL) {

        printf("%c\n", cpyPtr->data);
        cpyPtr = cpyPtr->nextPtr;
    }
}

int isEmpty(StackNodePtr topPtr) {
    if (topPtr == NULL)

        return 0;
    return 1;
}

char stackTop(StackNodePtr topPtr) {

    return topPtr->data;
}

char pop(StackNodePtr *topPtr) {
    char data = ' ';
    StackNodePtr cpyPtr = *topPtr;

    if (cpyPtr != NULL) {

        data = (*topPtr)->data;
        *topPtr = (StackNodePtr) (*topPtr)->nextPtr;

        free(cpyPtr);
    }

    return data;

}

void push(StackNodePtr *topPtr, char value) {

    if (*topPtr == NULL) {
        *topPtr = malloc(sizeof (StackNode*));
        (*topPtr)->data = value;
        (*topPtr)->nextPtr = NULL;
    } else {

        StackNodePtr newTopPtr = malloc(sizeof (StackNode*));

        newTopPtr->data = value;
        newTopPtr->nextPtr = (StackNode*) * topPtr;

        *topPtr = newTopPtr;
    }
}

int precedence(char operator1, char operator2) {
    if (operator1 == operator2)
        return 0;
    if (operator1 == '*' && operator2 == '/')
        return 0;
    if (operator1 == '/' && operator2 == '*')
        return 0;
    if (operator1 == '+' && operator2 == '-')
        return 0;
    if (operator1 == '-' && operator2 == '+')
        return 0;
    if (operator1 == '*' || operator1 == '/')

        return 1;

    return -1;
}

void evaluatePostfixExpression(StackNodePtrPos *topPtr) {
    char aux[100];
    char *t;
    int x, y;


    t = strtok(postfix, " ");


    while (t != NULL) {

        strcpy(aux, t);

        if (isOperator(aux[0])) {

            x = popPos(&(*topPtr));
            y = popPos(&(*topPtr));

            r.instructionRegister = 2000 + x;

            execute();

            if (aux[0] == '+') {

                r.instructionRegister = 3000 + y;

                execute();
            } else if (aux[0] == '-') {

                r.instructionRegister = 3100 + y;

                execute();
            } else if (aux[0] == '*') {

                r.instructionRegister = 3300 + y;

                execute();
            } else if (aux[0] == '/') {

                r.instructionRegister = 3200 + y;

                execute();
            }

            r.instructionRegister = 2100 + MEMORY_TEMP;

            execute();

            pushPos(&(*topPtr), MEMORY_TEMP);

        } else {
            if (isNumber(aux)) {
                x = searchTableSymbol(atoi(aux), 'C');
            } else {
                x = searchTableSymbol(aux[0], 'V');
            }

            pushPos(&(*topPtr), tableSymbol[x].location);
        }
        t = strtok(NULL, " ");
    }

    if (!isEmptyPos(*topPtr)) {
        x = popPos(&(*topPtr));
        if (x != MEMORY_TEMP) {

            r.instructionRegister = 2000 + x;

            execute();

            r.instructionRegister = 2100 + MEMORY_TEMP;

            execute();
        }
    }
}

void printStackPos(StackNodePtrPos topPtr) {

    StackNodePtrPos cpyPtr = topPtr;

    while (cpyPtr != NULL) {

        printf("%d\n", cpyPtr->data);
        cpyPtr = cpyPtr->nextPtr;
    }
}

int isEmptyPos(StackNodePtrPos topPtr) {
    if (topPtr == NULL)

        return 1;
    return 0;
}

int stackTopPos(StackNodePtrPos topPtr) {

    return topPtr->data;
}

int popPos(StackNodePtrPos *topPtr) {
    int data = 0;
    StackNodePtrPos cpyPtr = *topPtr;

    if (cpyPtr != NULL) {

        data = (*topPtr)->data;
        *topPtr = (StackNodePtrPos) (*topPtr)->nextPtr;

        free(cpyPtr);
    }

    return data;

}

void pushPos(StackNodePtrPos *topPtr, int value) {

    if (*topPtr == NULL) {
        *topPtr = malloc(sizeof (StackNodePos*));
        (*topPtr)->data = value;
        (*topPtr)->nextPtr = NULL;
    } else {
        StackNodePtrPos newTopPtr = malloc(sizeof (StackNodePos*));

        newTopPtr->data = value;
        newTopPtr->nextPtr = (StackNodePos*) * topPtr;

        *topPtr = newTopPtr;
    }
}

void execute() {

    r.operationCode = r.instructionRegister / 100;
    r.operand = r.instructionRegister % 100;


    switch (r.operationCode) {
        case READ:
            if (read())
                ++r.instructionCounter;
            break;

        case WRITE:
            write();
            ++r.instructionCounter;
            break;

        case LOAD:
            load();
            ++r.instructionCounter;
            break;

        case STORE:
            store();
            ++r.instructionCounter;
            break;

        case ADD:
            if (add())
                ++r.instructionCounter;
            break;

        case SUBTRACT:
            if (subtract())
                ++r.instructionCounter;
            break;

        case DIVIDE:
            if (divide())
                ++r.instructionCounter;
            break;

        case MULTIPLY:
            if (multiply())
                ++r.instructionCounter;
            break;

        case BRANCH:
            branch();
            break;

        case BRANCHNEG:
            branchneg();
            break;
        case BRANCHZERO:
            branchzero();
            break;

        case HALT:
            if (!halt())
                ++r.instructionCounter;
            break;

        default:
            printf("\n*** INVALID OPCODE: %d ***\n", r.operationCode);

            break;
    }

}

void write() {
    printf("\n%d\n", memory[r.operand]);

}

unsigned read() {
    int wrd = 0;

    printf("\nEntre com um número inteiro: ");
    scanf("%d", &wrd);

    if (wrd > INTEGER) {
        printf("\nreadtoobid.sml ***ABEND: entrada ilegal***\n");
        return 0;
    }

    if (wrd < -INTEGER) {
        printf("\nreadtoosmall.sml ***ABEND: ilegal input***\n");
        return 0;
    }

    memory[r.operand] = wrd;


    return 1;

}

void load() {
    r.accumulator = memory[r.operand];
}

void store() {
    memory[r.operand] = r.accumulator;
}

unsigned add() {
    if (r.accumulator + memory[r.operand]< -INTEGER) {
        printf("\nunderflow.add.sml ***ABEND: underflow ***\n");
        return 0;
    }
    if (r.accumulator + memory[r.operand] > INTEGER) {
        printf("\noverflow.add.sml ***ABEND: overflow ***\n");
        return 0;
    }

    r.accumulator += memory[r.operand];
    return 1;
}

unsigned subtract() {
    if (r.accumulator - memory[r.operand] < -INTEGER) {
        printf("\nunderflow.sub.sml ***ABEND: underflow ***\n");
        return 0;
    }
    if (r.accumulator - memory[r.operand] > INTEGER) {
        printf("\noverflow.sub.sml ***ABEND: overflow ***\n");
        return 0;
    }

    r.accumulator = memory[r.operand] - r.accumulator;

    return 1;
}

unsigned multiply() {
    if (r.accumulator * memory[r.operand] < -INTEGER) {
        printf("\nunderflow.mult.sml ***ABEND: underflow ***\n");
        return 0;
    }
    if (r.accumulator * memory[r.operand] > INTEGER) {
        printf("\noverflow.mult.sml ***ABEND: overflow ***\n");
        return 0;
    }

    r.accumulator *= memory[r.operand];
    return 1;
}

void branch() {
    int index = searchTableSymbol(r.operand, 'L');
    fseek(inputFile, tableSymbol[index].location, SEEK_SET);
}

void branchneg() {
    int index;
    if (r.accumulator < 0) {
        index = searchTableSymbol(r.operand, 'L');
        fseek(inputFile, tableSymbol[index].location, SEEK_SET);
    }
}

void branchzero() {
    int index;
    if (r.accumulator == 0) {
        index = searchTableSymbol(r.operand, 'L');
        fseek(inputFile, tableSymbol[index].location, SEEK_SET);
    }
}

unsigned divide() {

    if (memory[r.operand] == 0) {
        printf("\ndiv0.sm ***ABEND: attempted division by 0 ***l\n");
        return 0;
    }

    r.accumulator = memory[r.operand] / r.accumulator;
    return 1;
}

unsigned halt() {
    printf("\n*** Execução do Simpletron encerrada ***\n");
    return 0;
}

void initialize() {
    unsigned i = 0;
    memset(&r, 0, sizeof (r));
    for (i = 0; i < MEMORY_INSTALLED; ++i) memory[i] = EMPTY;

}

void loadFile() {
    char *tok;
    char aux [100];
    int index;

    memset(postionLineFile, 0, sizeof (int) * 100);

    do {
        index = ftell(inputFile);


        fgets(aux, 100 * sizeof (aux), inputFile);

        tok = strtok(aux, " ");

        if (isNumber(tok)) {
            if (searchTableSymbol(atoi(tok), 'L') == -1) {
                insertNumberLineTableSymbol(atoi(tok), index);
            }

        }

    } while (!feof(inputFile));

    rewind(inputFile);
}
