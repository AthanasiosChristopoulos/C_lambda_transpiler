/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "tuc_transpiler.y"

    #include <stdio.h>
    #include <string.h>
    #include "cgen.h"
    #include <ctype.h>
    #include <stdbool.h>
    #include <stdlib.h>
    #include <regex.h>

    #define MAX_FUNCTIONS 100
    #define MAX_LINE_LENGTH 1024
    #define MAX_LINES 1000
    #define BUFFER_SIZE 1024  
    #define MAX_FUNCTIONS 100
    #define MAX_NAME_LENGTH 50

    // int yydebug = 0;
    int yydebug = 1; 

    extern int parse_error;
    extern int yylex(void); // use the lexer, lex.yy.cc has already been generated
    extern int line_num;
    extern void add_macro(const char *id, const char *value);

    void print_evaluation(char* result);
    int match_return_variable_type(char* result);
    bool is_integer(const char* str);

    int used_lambdalib_function = 0;
    char *included_functionnames[MAX_FUNCTIONS];
    int num_of_included_functions = 0;

    char *lines[MAX_LINES];
    int current_line_index = 0;
    int main_has_been_defined = 0;

    int for_has_step = 0;
    char* function_name_temp;
    char* variable_name_temp;
    int first_variable = 1;
    char* comp_definition_variable_type;

    char* func_call_object;

//==========================================================================================================================
//=========================================================================================================================
void store_line(const char *text) {
  
    if (current_line_index < MAX_LINES) {

        char temp[BUFFER_SIZE];
        strncpy(temp, text, BUFFER_SIZE - 1);
        temp[BUFFER_SIZE - 1] = '\0'; 

        char *newline_pos = strchr(temp, '\n');
        if (newline_pos) {
            *newline_pos = '\0';
        }

        lines[current_line_index] = strdup(temp);
        current_line_index++;
    }
}

  void free_lines() {
      for (int i = 0; i < current_line_index; i++) {
          free(lines[i]);
      }
  }

//=========================================================================================================================

char* concatenate2(const char* str1, const char* str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char* result = (char*)malloc(len1 + len2 + 1); // +1 for null terminator

    if (!result) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

char* concatenate4(const char *arg1, const char *arg2, const char *arg3, const char *arg4) {
    size_t total_length = strlen(arg1) + strlen(arg2) + strlen(arg3) + strlen(arg4) + 1;
    char *result = (char*)malloc(total_length);

    if (!result) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    strcpy(result, arg1);
    strcat(result, arg2);
    strcat(result, arg3);
    strcat(result, arg4);

    return result;
}

//==========================================================================================================================    

int func_needs_return_type = 0; 
int body_has_return_type = 0;

int num_of_args_in_function = 0; 
int num_of_args_in_function_in_func_call = 0;

typedef struct {
    const char* name;
    int has_arguments;
    int temp_arg_count;
} DefinedFunction;

DefinedFunction *functions = NULL;
size_t num_functions = 0;

void increase_count(const char* name) {

    for (size_t i = 0; i < num_functions; i++) {
        if (strcmp(functions[i].name, name) == 0) {
            functions[i].temp_arg_count++;
            return;
        }
    }
    printf("Function not found to increase count, for function: %s\n", name);
}

int get_temp_arg_count(const char* name) {
    for (size_t i = 0; i < num_functions; i++) {
        if (strcmp(functions[i].name, name) == 0) {
            return functions[i].temp_arg_count;
        }
    }
    printf("Function not found to get count.\n");
    return -1; 
}

void set_count_to_zero(const char* name) {
    for (size_t i = 0; i < num_functions; i++) {
        if (strcmp(functions[i].name, name) == 0) {
            functions[i].temp_arg_count = 0;
            return;
        }
    }
    printf("Function not found to reset count.\n");
}

//=========================================================================================================================

void add_function(const char *name, int has_arguments) {

    functions = realloc(functions, (num_functions + 1) * sizeof(DefinedFunction));
    if (!functions) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    DefinedFunction *new_func = &functions[num_functions];
    new_func->name = strdup(name);  
    new_func->has_arguments = has_arguments;
    new_func->temp_arg_count = 0;
    num_functions++;
}

//=========================================================================================================================

int find_included_function(const char *functionname) {
    for (int i = 0; i < num_of_included_functions; i++) {
        if (strcmp(included_functionnames[i], functionname) == 0) {
            return 1; 
        }
    }
    return 0; 
}
//=========================================================================================================================

const DefinedFunction* find_function(const char* name) {

    for (size_t i = 0; i < num_functions; i++) {
        if (strcmp(functions[i].name, name) == 0) {
            return &functions[i];
        }
    }
    printf("Function not found\n");
    return NULL; 
}

void free_functions() {
    for (size_t i = 0; i < num_functions; i++) {
        free((void*)functions[i].name);
    }
    free(functions);
}

//=========================================================================================================================
// Function Call Stack

typedef struct StackNode {
    char* function_name;
    struct StackNode* next;
} StackNode;

typedef struct {
    StackNode* head;
} FunctionStack;

FunctionStack function_stack = { NULL };

void push(char* function_name) {
    StackNode* new_node = (StackNode*)malloc(sizeof(StackNode));
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed for stack node\n");
        exit(1);
    }
    new_node->function_name = strdup(function_name);
    new_node->next = function_stack.head;
    function_stack.head = new_node;
}

char* peek() {
    if (function_stack.head) {
        return function_stack.head->function_name;
    }
    return NULL;
}

void pop() {
    if (!function_stack.head) return;

    StackNode* temp = function_stack.head;
    function_stack.head = function_stack.head->next;

    free((void*)temp->function_name);
    free(temp);
}

void clear_stack() {
    while (function_stack.head) {
        pop();
    }
}

//=========================================================================================================================
//=========================================================================================================================
// Include Functions in lambdalib.h

int count_args(const char *line) {
    int count = 0;
    const char *p = strchr(line, '(');
    if (p) {
        p++;  
        while (*p == ' ' || *p == '\t') {p++;}
        if (strstr(p, "...")) {
            return 99; 
        }
        if (*p == ')') { return 0; }
        while (*p != ')' && *p != '\0') {
            if (*p == ',') {
                count++;
            }
            p++;
        } 
        count++;
    }
    return count;
}

//=========================================================================================================================

int is_function_definition(const char *line, char *func_name) {
    regex_t regex;
    regmatch_t matches[2];
    const char *pattern = "^(\\s*(void|int|float|char|double|long|short|unsigned|signed|bool|_Bool|\\w+\\*?\\s+)+[a-zA-Z_][a-zA-Z0-9_]*\\s*\\([^)]*\\)\\s*\\{)|^\\s*#define\\s+[a-zA-Z_][a-zA-Z0-9_]*\\s*\\([^)]*\\)\\s*[^\\{]*$";
    
    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        return 0;
    }
    
    int result = regexec(&regex, line, 1, matches, 0); // If a match is found, matches[0] contains the start and end positions.
    regfree(&regex);
    
    if (result == 0) { // Extract the Function Name
        int start = matches[0].rm_so;
        int end = matches[0].rm_eo - 1; // Ignore '('
        strncpy(func_name, line + start, end - start);
        func_name[end - start] = '\0';
        return 1;
    }
    return 0;
}

//=========================================================================================================================
//=========================================================================================================================
//=========================================================================================================================

void extractFunctionName(const char *func_name, char *output) {

    const char *start = func_name;
    while (!isspace(*start) && *start != '\0') {
        start++; 
    }
    while (isspace(*start)) {
        start++; 
    }
    
    int i = 0;
    while (*start != '(' && *start != '\0' && i < 99) {
        output[i++] = *start++;
    }
    output[i] = '\0';
}

//=========================================================================================================================

void extractFunctionNames(const char *code, char function_names[MAX_FUNCTIONS][MAX_NAME_LENGTH], int *count) {

    const char *ptr = code;
    *count = 0;

    while (*ptr != '\0') {  
        ptr = strstr(ptr, "double ");
        if (ptr != NULL) {
            ptr += 6;
        } else {
            ptr = strstr(ptr, "void ");
            if (ptr != NULL) {
                ptr += 4; 
            }
        }
        while (*ptr && isspace(*ptr)) {
            ptr++;
        }
        char name[MAX_NAME_LENGTH] = {0};
        int i = 0;

        while (*ptr && (isalnum(*ptr) || *ptr == '_')) { 
            name[i++] = *ptr++;
            if (i >= MAX_NAME_LENGTH - 1) break;  
        }
        name[i] = '\0';
        if (i > 0 && *count < MAX_FUNCTIONS) {
            strcpy(function_names[*count], name);
            (*count)++;
        }
    }
}
//=========================================================================================================================

void include_functions()  {

    FILE *file = fopen("lambdalib.h", "r");
    if (!file) {
        perror("Error opening file");
        return;
    }
    char line[MAX_LINES];

    while (fgets(line, MAX_LINES, file)) {
        char func_name[100];
        char output[100];

        if (is_function_definition(line, func_name)) {
            extractFunctionName(func_name, output);
            strcpy(func_name, output);
            int num_args = count_args(line);
            add_function(func_name, num_args);
            included_functionnames[num_of_included_functions] = strdup(func_name);
            num_of_included_functions++;
        }
    }
    fclose(file);
    return;
}

//=========================================================================================================================
//=========================================================================================================================
//=========================================================================================================================
// comp_upper:

int inside_comp= 0;

#define MAX_FUNCTIONS 100

typedef struct FunctionNode {
    char *function_comp;
    char *function_name;
    struct FunctionNode *next;
} FunctionNode;

typedef struct VariableNode {
    char *variable_name;
    struct VariableNode *next;
} VariableNode;

typedef struct VariableDefinitionsNode {
    char *variable_definition_name;
    struct VariableDefinitionsNode *next;
} VariableDefinitionsNode;

typedef struct CompFunctionMap {
    char *comp_name;
    FunctionNode *functions;  
    VariableNode *variables;
    VariableDefinitionsNode *variables_definitions;
    struct CompFunctionMap *next;
} CompFunctionMap;
#define HASH_TABLE_SIZE 100

CompFunctionMap *compFunctionTable[HASH_TABLE_SIZE] = { NULL };

//=========================================================================================================================

unsigned int hash(const char *str) {
    unsigned int hash = 0;
    while (*str) hash = (hash * 31) + *str++;
    return hash % HASH_TABLE_SIZE;
}

void createNewComp(const char *comp_name) {
    unsigned int index = hash(comp_name);
    CompFunctionMap *entry = compFunctionTable[index];
    entry = (CompFunctionMap *)malloc(sizeof(CompFunctionMap));
    entry->comp_name = strdup(comp_name);
    entry->functions = NULL;  
    entry->variables = NULL; 
    entry->variables_definitions = NULL; 
    entry->next = compFunctionTable[index];
    compFunctionTable[index] = entry;
}

int is_comp_type(const char *comp_name) {
    unsigned int index = hash(comp_name);
    CompFunctionMap *entry = compFunctionTable[index];
    while (entry) {
        if (strcmp(entry->comp_name, comp_name) == 0) {
            return 1;
        }
        entry = entry->next;
    }
    return 0;
}
//=========================================================================================================================
// Comp Variable Definition:

void add_comp_variable_definition(const char *comp_name, const char *variable_definition_name) {
    unsigned int index = hash(comp_name);
    CompFunctionMap *comp = compFunctionTable[index];
    VariableDefinitionsNode *var_def = comp->variables_definitions;

    while (var_def) {
        if (strcmp(var_def->variable_definition_name, variable_definition_name) == 0) {
            return; // Variable definition already exists
        }
        var_def = var_def->next;
    }
    var_def = (VariableDefinitionsNode *)malloc(sizeof(VariableDefinitionsNode));
    var_def->variable_definition_name = strdup(variable_definition_name);
    var_def->next = comp->variables_definitions;
    comp->variables_definitions = var_def;
}

void add_comp_variable_definitions(const char* comp_name, const char *input) {
    char buffer[256];  
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    char *token = strtok(buffer, ",");
    while (token != NULL) {
        while (*token == ' ') token++;
        add_comp_variable_definition(comp_name, token);
        token = strtok(NULL, ",");
    }
}

int is_comp_variable_definition(const char *comp_name, const char *variable_definition_name) {
    unsigned int index = hash(comp_name);
    CompFunctionMap *comp = compFunctionTable[index];
    VariableDefinitionsNode *var_def = comp->variables_definitions;
    while (var_def) {
        if (strcmp(var_def->variable_definition_name, variable_definition_name) == 0) {
            return 1; // Found
        }
        var_def = var_def->next;
    }
    return 0; // Not found
}

char* get_comp_type_definition(char* variable_name) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        CompFunctionMap *entry = compFunctionTable[i];        
        while (entry) {  // Missing this loop in your code
            VariableDefinitionsNode *var_def = entry->variables_definitions;
            while (var_def) {
                if (strcmp(var_def->variable_definition_name, variable_name) == 0) {
                    return entry->comp_name; // Found in VariableDefinitionsNode
                }
                var_def = var_def->next;
            }
            entry = entry->next;
        }
    }
    return NULL; // Variable not found
}


//=========================================================================================================================
// variable_comp_upper:

void add_comp_variable(const char *comp_name, const char *variable_name) {
    unsigned int index = hash(comp_name);
    CompFunctionMap *comp = compFunctionTable[index];
    VariableNode *var = comp->variables;

    if(first_variable == 0){
        while (var) {
            if (strcmp(var->variable_name, variable_name) == 0) {
                return; // Variable already exists
            }
            var = var->next;
        }
        first_variable = 0;
    }
    var = (VariableNode *)malloc(sizeof(VariableNode));
    var->variable_name = strdup(variable_name);
    var->next = comp->variables;
    comp->variables = var;
}

void add_comp_variables(const char* comp_name, const char *input) {
    char buffer[256];  
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    char *token = strtok(buffer, ",");
    while (token != NULL) {
        while (*token == ' ') token++;
        add_comp_variable(comp_name,token);
        token = strtok(NULL, ",");
    }
}

int is_comp_variable(const char *comp_name, const char *variable_name) {
    unsigned int index = hash(comp_name);
    CompFunctionMap *comp = compFunctionTable[index];
    VariableNode *var = comp->variables;
    while (var) {
        if (strcmp(var->variable_name, variable_name) == 0) {
            return 1; // Found
        }
        var = var->next;
    }
    return 0; // Not found
}

char* get_comp_type(char* variable_name) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        CompFunctionMap *entry = compFunctionTable[i];
        while (entry) {
            VariableNode *var = entry->variables;
            while (var) {
                if (strcmp(var->variable_name, variable_name) == 0) {
                    return entry->comp_name; // Return the component name that owns this variable
                }
                var = var->next;
            }
            entry = entry->next;
        }
    }
    return NULL; // Variable not found
}

//=========================================================================================================================
// function_comp_upper:

void addFunctionToComp(const char *comp_name, const char *function_follow_comp, const char *func_name) {

    unsigned int index = hash(comp_name);

    CompFunctionMap *entry = compFunctionTable[index];
    while (entry) {
        if (strcmp(entry->comp_name, comp_name) == 0) {
            FunctionNode *newNode = (FunctionNode *)malloc(sizeof(FunctionNode));
            newNode->function_comp = strdup(function_follow_comp);
            newNode->function_name = strdup(func_name); 
            newNode->next = entry->functions;
            entry->functions = newNode;
            return;
        }
        entry = entry->next;
    }
}
//=========================================================================================================================

int is_comp_function(const char *comp_name, const char *function_name) {
    unsigned int index = hash(comp_name);
    CompFunctionMap *entry = compFunctionTable[index];

    while (entry) {
        if (strcmp(entry->comp_name, comp_name) == 0) {
            FunctionNode *function_current = entry->functions;
            while (function_current) {
                if (strcmp(function_current->function_name, function_name) == 0) {
                    return 1; // Function found
                }
                function_current = function_current->next;
            }
            return 0; 
        }
        entry = entry->next;
    }

    return 0;
}

int is_comp_function_simple(const char *function_name) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        CompFunctionMap *entry = compFunctionTable[i];
        
        while (entry) {
            FunctionNode *funcNode = entry->functions;
            while (funcNode) {
                if (strcmp(funcNode->function_name, function_name) == 0) {
                    return 1; // Function found
                }
                funcNode = funcNode->next;
            }
            entry = entry->next;
        }
    }
    return 0; // Function not found
}
//=========================================================================================================================

char *getCompFunctionsAsString(const char *comp_name) {
    unsigned int index = hash(comp_name);
    CompFunctionMap *entry = compFunctionTable[index];

    while (entry) {
        if (strcmp(entry->comp_name, comp_name) == 0) {
            size_t totalSize = 1;
            FunctionNode *fn = entry->functions;
            while (fn) {
                totalSize += strlen(fn->function_comp) + 2; 
                fn = fn->next;
            }
            char *result = (char *)malloc(totalSize);
            if (!result) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }
            result[0] = '\0'; 

            fn = entry->functions;
            while (fn) {
                strcat(result, fn->function_comp);
                strcat(result, "\n"); 
                fn = fn->next;
            }
            return result; 
        }
        entry = entry->next;
    }

    return strdup("");
}

//=========================================================================================================================

char *getCompFunctionNamesAsString(const char *comp_name) {
    unsigned int index = hash(comp_name);
    CompFunctionMap *entry = compFunctionTable[index];

    while (entry) {
        if (strcmp(entry->comp_name, comp_name) == 0) {
            size_t totalSize = 1; 
            FunctionNode *fn = entry->functions;
            while (fn) {
                totalSize += strlen(fn->function_name) * 2 + 6; 
                fn = fn->next;
            }
            char *result = (char *)malloc(totalSize);
            if (!result) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }
            result[0] = '\0';

            fn = entry->functions;
            while (fn) {
                strcat(result, ".");
                strcat(result, fn->function_name); 
                strcat(result, " = ");
                strcat(result, fn->function_name);
                strcat(result, ", ");
                fn = fn->next;
            }

            result[strlen(result) - 2] = '\0';

            return result;
        }
        entry = entry->next;
    }
    return strdup("");
}

//==========================================================================================================================
// Comp Stack FIFO:

typedef struct CompVariableNode {
    char* comp_name;
    struct CompVariableNode* next;
} CompVariableNode;

typedef struct {
    CompVariableNode* head;
    CompVariableNode* tail;  // New addition to keep track of the end of the queue
} CompStack;

CompStack comp_stack = { NULL, NULL };

// Push to the end (tail) of the queue
void push_comp(char* comp_name) {
    CompVariableNode* new_node = (CompVariableNode*)malloc(sizeof(CompVariableNode));
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed for stack node\n");
        exit(1);
    }
    new_node->comp_name = strdup(comp_name);
    new_node->next = NULL;

    if (comp_stack.tail) {
        comp_stack.tail->next = new_node;  // Link the old tail to the new node
    } else {
        comp_stack.head = new_node;  // If the queue was empty, new node is also the head
    }
    comp_stack.tail = new_node;  // Update the tail to the new node
}

char* peek_comp() {
    if (comp_stack.head) {
        return comp_stack.head->comp_name;
    }
    return NULL;
}

void pop_comp() {
    if (!comp_stack.head) return;

    CompVariableNode* temp = comp_stack.head;
    comp_stack.head = comp_stack.head->next;

    if (!comp_stack.head) { 
        comp_stack.tail = NULL;
    }

    free(temp->comp_name);
    free(temp);
}

void clear_stack_comp() {
    while (comp_stack.head) {
        pop();
    }
}

int validate_comp_stack(char* init_comp) {
    char* comp_name = get_comp_type_definition(init_comp);

    while (comp_stack.head) {
       char* variable_name_temp = peek_comp();
        // printf("Evaluating: %s, %s\n", comp_name, variable_name_temp);
        if(is_comp_variable(comp_name, variable_name_temp) == 0){ // if its not comp variable, then method should fail
            return 0;
        }
        // printf("Result: %d\n", is_comp_variable_definition(comp_name, variable_name_temp));
        comp_name = get_comp_type_definition(variable_name_temp);
        pop_comp();  
    }
    return 1;
}


//==========================================================================================================================
//==========================================================================================================================
//==========================================================================================================================
//==========================================================================================================================
// Other Stuff:

int indentation_level = 0;
int assignment_made = 0;
char* comp_name;

char* indent_statements(const char* statements, int level) {
    int rrrr = 0;
    sstream S;
    ssopen(&S);
    if(level >= 1){
      level = 1;
    } 
    const char* current = statements;
    int is_new_line = 1;  

    while (*current != '\0') {
        if (is_new_line) {
            
            for (int i = 0; i < level; i++) {
                fprintf(S.stream, "    "); 
                rrrr++;
            }
            is_new_line = 0;  
        }
        fputc(*current, S.stream);
        if (*current == '\n') {
            is_new_line = 1;  
        }
        current++;
    }
    char* result = ssvalue(&S);
    ssclose(&S);
    return result;
}

//==========================================================================================================================
//==========================================================================================================================

  typedef struct {
      const char *original;
      const char *transformed;
  } TypeMapping;

  TypeMapping typeMappings[] = {
      {"KW_SCALAR", "double"},
      {"KW_INTEGER", "int"},
      {"KW_STR", "char*"},
      {"KW_BOOLEAN", "int"},
  };

//==========================================================================================================================

  char* transform_type(const char *identifier) {
      for (int i = 0; i < sizeof(typeMappings) / sizeof(typeMappings[0]); i++) {
          if (strcmp(identifier, typeMappings[i].original) == 0) {
              return (char*)typeMappings[i].transformed;
          }
      }
      return NULL; 
  }

//==========================================================================================================================

  char* handle_variable_type(const char *inputType) {
      return transform_type(inputType);
  }

  int is_a_loop = 0;
  int is_an_if = 0;
//==========================================================================================================================
// Strings:

char* replace_identifier_in_expr(const char* expr, const char* oldVar, const char* newVar) {
    char* result = strdup(expr); 
    char* pos = strstr(result, oldVar); 
    
    if (pos) {
        size_t newLen = strlen(newVar);
        size_t oldLen = strlen(oldVar);
        char* newExpr = (char*)malloc(strlen(result) + newLen - oldLen + 1);
        
        strncpy(newExpr, result, pos - result); 
        strcpy(newExpr + (pos - result), newVar); 
        strcat(newExpr, pos + oldLen); 
        
        free(result);
        return newExpr;
    }
    
    return result;
}

//==========================================================================================================================

int contains_substring(const char *s1, const char *s2) {
    if (strstr(s1, s2) != NULL) {
        return 1; 
    }
    return 0;  
}


//==========================================================================================================================
//==========================================================================================================================
//==========================================================================================================================


#line 968 "tuc_transpiler.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "tuc_transpiler.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENTIFIER = 3,                 /* IDENTIFIER  */
  YYSYMBOL_POSINT = 4,                     /* POSINT  */
  YYSYMBOL_STRING = 5,                     /* STRING  */
  YYSYMBOL_DECIMAL = 6,                    /* DECIMAL  */
  YYSYMBOL_REAL = 7,                       /* REAL  */
  YYSYMBOL_KW_IF = 8,                      /* KW_IF  */
  YYSYMBOL_KW_ELSE = 9,                    /* KW_ELSE  */
  YYSYMBOL_KW_ENDIF = 10,                  /* KW_ENDIF  */
  YYSYMBOL_KW_IN = 11,                     /* KW_IN  */
  YYSYMBOL_KW_OF = 12,                     /* KW_OF  */
  YYSYMBOL_KW_WHILE = 13,                  /* KW_WHILE  */
  YYSYMBOL_KW_ENDWHILE = 14,               /* KW_ENDWHILE  */
  YYSYMBOL_KW_FOR = 15,                    /* KW_FOR  */
  YYSYMBOL_KW_ENDFOR = 16,                 /* KW_ENDFOR  */
  YYSYMBOL_KW_BREAK = 17,                  /* KW_BREAK  */
  YYSYMBOL_KW_CONTINUE = 18,               /* KW_CONTINUE  */
  YYSYMBOL_KW_TRUE = 19,                   /* KW_TRUE  */
  YYSYMBOL_KW_FALSE = 20,                  /* KW_FALSE  */
  YYSYMBOL_KW_VAR = 21,                    /* KW_VAR  */
  YYSYMBOL_KW_INT = 22,                    /* KW_INT  */
  YYSYMBOL_KW_REAL = 23,                   /* KW_REAL  */
  YYSYMBOL_KW_FUNC = 24,                   /* KW_FUNC  */
  YYSYMBOL_KW_BEGIN = 25,                  /* KW_BEGIN  */
  YYSYMBOL_KW_DEF = 26,                    /* KW_DEF  */
  YYSYMBOL_KW_ENDDEF = 27,                 /* KW_ENDDEF  */
  YYSYMBOL_KW_COMP = 28,                   /* KW_COMP  */
  YYSYMBOL_KW_ENDCOMP = 29,                /* KW_ENDCOMP  */
  YYSYMBOL_KW_MACRO = 30,                  /* KW_MACRO  */
  YYSYMBOL_KW_RETURN = 31,                 /* KW_RETURN  */
  YYSYMBOL_KW_MAIN = 32,                   /* KW_MAIN  */
  YYSYMBOL_KW_FUNC_RETURN = 33,            /* KW_FUNC_RETURN  */
  YYSYMBOL_KW_INTEGER = 34,                /* KW_INTEGER  */
  YYSYMBOL_KW_SCALAR = 35,                 /* KW_SCALAR  */
  YYSYMBOL_KW_STR = 36,                    /* KW_STR  */
  YYSYMBOL_KW_CONST = 37,                  /* KW_CONST  */
  YYSYMBOL_KW_BOOLEAN = 38,                /* KW_BOOLEAN  */
  YYSYMBOL_EXPONENTIATION_OP = 39,         /* EXPONENTIATION_OP  */
  YYSYMBOL_ASSIGNMENT_OP = 40,             /* ASSIGNMENT_OP  */
  YYSYMBOL_CALC_OP = 41,                   /* CALC_OP  */
  YYSYMBOL_UNARY_OP = 42,                  /* UNARY_OP  */
  YYSYMBOL_RELATIONAL_OP = 43,             /* RELATIONAL_OP  */
  YYSYMBOL_LOGICAL_NOT_OP = 44,            /* LOGICAL_NOT_OP  */
  YYSYMBOL_LOGICAL_AND_OR_OP = 45,         /* LOGICAL_AND_OR_OP  */
  YYSYMBOL_KW_PLUS = 46,                   /* KW_PLUS  */
  YYSYMBOL_KW_MINUS = 47,                  /* KW_MINUS  */
  YYSYMBOL_KW_MUL = 48,                    /* KW_MUL  */
  YYSYMBOL_KW_DIV = 49,                    /* KW_DIV  */
  YYSYMBOL_KW_MOD = 50,                    /* KW_MOD  */
  YYSYMBOL_KW_LESSEQUAL = 51,              /* KW_LESSEQUAL  */
  YYSYMBOL_KW_GREATEREQUAL = 52,           /* KW_GREATEREQUAL  */
  YYSYMBOL_KW_LESSTHAN = 53,               /* KW_LESSTHAN  */
  YYSYMBOL_KW_GREATHAN = 54,               /* KW_GREATHAN  */
  YYSYMBOL_KW_INEQ = 55,                   /* KW_INEQ  */
  YYSYMBOL_KW_AND = 56,                    /* KW_AND  */
  YYSYMBOL_KW_OR = 57,                     /* KW_OR  */
  YYSYMBOL_KW_NOT = 58,                    /* KW_NOT  */
  YYSYMBOL_KW_EQUAL = 59,                  /* KW_EQUAL  */
  YYSYMBOL_KW_EXPONENT = 60,               /* KW_EXPONENT  */
  YYSYMBOL_61_ = 61,                       /* ';'  */
  YYSYMBOL_62_ = 62,                       /* ')'  */
  YYSYMBOL_63_ = 63,                       /* ']'  */
  YYSYMBOL_64_ = 64,                       /* '('  */
  YYSYMBOL_65_ = 65,                       /* '['  */
  YYSYMBOL_66_ = 66,                       /* '#'  */
  YYSYMBOL_67_ = 67,                       /* ':'  */
  YYSYMBOL_68_ = 68,                       /* ','  */
  YYSYMBOL_69_ = 69,                       /* '.'  */
  YYSYMBOL_YYACCEPT = 70,                  /* $accept  */
  YYSYMBOL_program = 71,                   /* program  */
  YYSYMBOL_input = 72,                     /* input  */
  YYSYMBOL_macro = 73,                     /* macro  */
  YYSYMBOL_commands = 74,                  /* commands  */
  YYSYMBOL_75_1 = 75,                      /* $@1  */
  YYSYMBOL_76_2 = 76,                      /* @2  */
  YYSYMBOL_77_3 = 77,                      /* $@3  */
  YYSYMBOL_78_4 = 78,                      /* @4  */
  YYSYMBOL_79_5 = 79,                      /* @5  */
  YYSYMBOL_80_6 = 80,                      /* @6  */
  YYSYMBOL_else_statement = 81,            /* else_statement  */
  YYSYMBOL_82_7 = 82,                      /* @7  */
  YYSYMBOL_step = 83,                      /* step  */
  YYSYMBOL_stmts = 84,                     /* stmts  */
  YYSYMBOL_loop_thing = 85,                /* loop_thing  */
  YYSYMBOL_complicated_types = 86,         /* complicated_types  */
  YYSYMBOL_87_8 = 87,                      /* $@8  */
  YYSYMBOL_declaration_comp = 88,          /* declaration_comp  */
  YYSYMBOL_variable_definition_comp = 89,  /* variable_definition_comp  */
  YYSYMBOL_variable_list_comp = 90,        /* variable_list_comp  */
  YYSYMBOL_function_definition_section = 91, /* function_definition_section  */
  YYSYMBOL_function_definition_comp = 92,  /* function_definition_comp  */
  YYSYMBOL_return_type_comp = 93,          /* return_type_comp  */
  YYSYMBOL_func_call = 94,                 /* func_call  */
  YYSYMBOL_95_9 = 95,                      /* $@9  */
  YYSYMBOL_func_args = 96,                 /* func_args  */
  YYSYMBOL_expr = 97,                      /* expr  */
  YYSYMBOL_98_10 = 98,                     /* $@10  */
  YYSYMBOL_99_11 = 99,                     /* $@11  */
  YYSYMBOL_100_12 = 100,                   /* $@12  */
  YYSYMBOL_101_13 = 101,                   /* $@13  */
  YYSYMBOL_use_of_comp_variable = 102,     /* use_of_comp_variable  */
  YYSYMBOL_use_comp_variable_with_period = 103, /* use_comp_variable_with_period  */
  YYSYMBOL_use_comp_variable = 104,        /* use_comp_variable  */
  YYSYMBOL_arithmetic_expr_non_empty = 105, /* arithmetic_expr_non_empty  */
  YYSYMBOL_function_definition = 106,      /* function_definition  */
  YYSYMBOL_107_14 = 107,                   /* @14  */
  YYSYMBOL_108_15 = 108,                   /* @15  */
  YYSYMBOL_109_16 = 109,                   /* @16  */
  YYSYMBOL_110_17 = 110,                   /* @17  */
  YYSYMBOL_arguments = 111,                /* arguments  */
  YYSYMBOL_parameter_definition = 112,     /* parameter_definition  */
  YYSYMBOL_return_type = 113,              /* return_type  */
  YYSYMBOL_body = 114,                     /* body  */
  YYSYMBOL_variable_definition = 115,      /* variable_definition  */
  YYSYMBOL_VALUE = 116,                    /* VALUE  */
  YYSYMBOL_KW_VARIABLE_TYPE = 117,         /* KW_VARIABLE_TYPE  */
  YYSYMBOL_NUMBERS = 118,                  /* NUMBERS  */
  YYSYMBOL_variable_list = 119             /* variable_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   431

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  70
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  50
/* YYNRULES -- Number of rules.  */
#define YYNRULES  136
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  297

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   315


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    66,     2,     2,     2,     2,
      64,    62,     2,     2,    68,     2,    69,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    67,    61,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    65,     2,    63,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   966,   966,   969,   970,   971,   972,   973,   974,   975,
     982,   987,  1001,  1003,  1001,  1013,  1015,  1013,  1027,  1036,
    1026,  1049,  1059,  1076,  1077,  1077,  1085,  1086,  1094,  1095,
    1097,  1098,  1099,  1102,  1102,  1111,  1111,  1124,  1125,  1129,
    1132,  1135,  1138,  1143,  1144,  1152,  1153,  1157,  1176,  1177,
    1184,  1184,  1214,  1215,  1216,  1217,  1218,  1224,  1225,  1226,
    1227,  1228,  1229,  1230,  1231,  1232,  1234,  1235,  1236,  1237,
    1238,  1240,  1249,  1259,  1268,  1280,  1280,  1289,  1289,  1298,
    1298,  1307,  1307,  1317,  1322,  1327,  1333,  1339,  1347,  1348,
    1349,  1353,  1357,  1359,  1366,  1367,  1368,  1369,  1370,  1371,
    1372,  1374,  1375,  1376,  1377,  1386,  1391,  1385,  1409,  1416,
    1408,  1435,  1436,  1437,  1441,  1442,  1443,  1447,  1448,  1453,
    1454,  1460,  1468,  1469,  1470,  1474,  1475,  1476,  1480,  1481,
    1482,  1483,  1484,  1495,  1496,  1500,  1501
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IDENTIFIER", "POSINT",
  "STRING", "DECIMAL", "REAL", "KW_IF", "KW_ELSE", "KW_ENDIF", "KW_IN",
  "KW_OF", "KW_WHILE", "KW_ENDWHILE", "KW_FOR", "KW_ENDFOR", "KW_BREAK",
  "KW_CONTINUE", "KW_TRUE", "KW_FALSE", "KW_VAR", "KW_INT", "KW_REAL",
  "KW_FUNC", "KW_BEGIN", "KW_DEF", "KW_ENDDEF", "KW_COMP", "KW_ENDCOMP",
  "KW_MACRO", "KW_RETURN", "KW_MAIN", "KW_FUNC_RETURN", "KW_INTEGER",
  "KW_SCALAR", "KW_STR", "KW_CONST", "KW_BOOLEAN", "EXPONENTIATION_OP",
  "ASSIGNMENT_OP", "CALC_OP", "UNARY_OP", "RELATIONAL_OP",
  "LOGICAL_NOT_OP", "LOGICAL_AND_OR_OP", "KW_PLUS", "KW_MINUS", "KW_MUL",
  "KW_DIV", "KW_MOD", "KW_LESSEQUAL", "KW_GREATEREQUAL", "KW_LESSTHAN",
  "KW_GREATHAN", "KW_INEQ", "KW_AND", "KW_OR", "KW_NOT", "KW_EQUAL",
  "KW_EXPONENT", "';'", "')'", "']'", "'('", "'['", "'#'", "':'", "','",
  "'.'", "$accept", "program", "input", "macro", "commands", "$@1", "@2",
  "$@3", "@4", "@5", "@6", "else_statement", "@7", "step", "stmts",
  "loop_thing", "complicated_types", "$@8", "declaration_comp",
  "variable_definition_comp", "variable_list_comp",
  "function_definition_section", "function_definition_comp",
  "return_type_comp", "func_call", "$@9", "func_args", "expr", "$@10",
  "$@11", "$@12", "$@13", "use_of_comp_variable",
  "use_comp_variable_with_period", "use_comp_variable",
  "arithmetic_expr_non_empty", "function_definition", "@14", "@15", "@16",
  "@17", "arguments", "parameter_definition", "return_type", "body",
  "variable_definition", "VALUE", "KW_VARIABLE_TYPE", "NUMBERS",
  "variable_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-219)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-136)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -219,    31,   114,  -219,   106,  -219,  -219,   -38,   -30,    40,
    -219,  -219,    10,    54,    66,    77,   263,   263,   263,    85,
    -219,    32,    58,  -219,   324,  -219,    64,    70,  -219,    44,
       2,  -219,   221,    39,   263,   263,   137,    99,   115,  -219,
      20,   142,    90,    96,   146,   291,    59,  -219,  -219,   263,
     263,   263,   263,  -219,  -219,  -219,    17,   195,   263,   202,
     210,   147,   133,   217,   208,   209,    92,  -219,   302,   309,
     140,   213,   156,   165,  -219,   358,   296,   263,   263,  -219,
     263,   263,    39,   358,    96,   146,  -219,  -219,  -219,  -219,
    -219,  -219,  -219,  -219,    46,  -219,    28,   358,   170,    17,
      -4,  -219,   157,  -219,   263,   263,    39,   175,   192,   295,
      95,    30,  -219,   197,  -219,  -219,  -219,  -219,   204,   202,
     247,   209,    12,   266,  -219,   274,    17,  -219,   263,  -219,
     202,   283,  -219,  -219,  -219,   256,    37,   109,   263,   263,
     263,   269,   359,   237,     7,    17,   273,   213,  -219,   242,
      17,     5,  -219,   244,  -219,   358,  -219,   202,   209,   243,
    -219,  -219,   -26,   288,   317,    80,   263,   263,   263,   263,
     295,   254,   264,  -219,    17,   268,  -219,  -219,    26,   281,
     122,    84,  -219,   263,    39,   149,  -219,    39,   231,   231,
     136,   358,   246,   328,   194,   277,   282,    17,  -219,  -219,
     189,  -219,   292,   372,  -219,    17,   311,   378,  -219,   321,
     202,   209,   154,   320,   373,  -219,  -219,  -219,   325,   380,
     326,   327,   329,   379,   295,   331,    17,  -219,  -219,   263,
     365,     8,   366,  -219,   402,   343,  -219,  -219,   263,   341,
     406,  -219,   344,   400,  -219,  -219,  -219,  -219,  -219,   345,
    -219,  -219,   335,  -219,   350,   347,   296,  -219,   213,   202,
      17,   403,  -219,  -219,  -219,   389,  -219,   351,    17,   352,
      33,  -219,   413,  -219,  -219,  -219,   231,    17,  -219,    17,
     387,   360,   231,   231,  -219,  -219,    17,   354,   355,   408,
    -219,  -219,    17,  -219,   398,  -219,  -219
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,    62,   133,   134,     0,     0,     0,
      60,    59,     0,     0,     0,     0,     0,     0,     0,     0,
       9,     0,     0,    57,     0,    61,     0,     0,    58,     0,
       0,    50,     0,    75,     0,     0,     0,     0,     0,    35,
       0,     0,    62,    70,    69,     0,    92,     8,     7,     0,
       0,     0,     0,     5,     6,     4,     0,     0,     0,    71,
      52,   133,     0,     0,     0,     0,    85,    88,     0,     0,
       0,   111,     0,     0,    10,    11,     0,     0,     0,    64,
       0,     0,    79,    66,    65,    67,    68,   132,   129,   128,
     130,   131,   121,   136,     0,    54,     0,    53,     0,     0,
      63,    91,     0,    76,     0,     0,     0,     0,     0,     0,
       0,     0,   112,     0,    37,   125,   127,   126,     0,    73,
       0,     0,    83,     0,    51,     0,     0,   123,     0,    77,
      86,     0,    90,    12,    15,    62,   133,   134,     0,     0,
       0,     0,     0,     0,     0,     0,   117,     0,   105,    45,
       0,    93,    80,     0,    56,    55,   122,    72,     0,     0,
      28,    28,    70,    69,     0,    92,     0,     0,     0,     0,
       0,     0,     0,   114,     0,     0,   113,    28,     0,     0,
       0,     0,   124,     0,    81,     0,    78,     0,    13,    16,
      64,    99,    65,    67,    68,    26,     0,     0,   118,   108,
     119,   106,    43,     0,    38,     0,     0,     0,    36,     0,
      74,     0,    84,     0,     0,    89,    33,    34,     0,    23,
       0,     0,     0,     0,     0,     0,     0,   116,    28,     0,
       0,     0,     0,    39,     0,     0,    46,    82,     0,     0,
       0,    32,     0,     0,    29,    31,    30,    17,    27,     0,
     115,   109,     0,   107,     0,     0,     0,    44,   111,    87,
       0,     0,    24,    14,    18,     0,    28,     0,     0,     0,
       0,    21,     0,    28,    28,   110,   120,     0,    41,     0,
      48,     0,    25,    19,    40,    42,     0,     0,     0,     0,
      49,    28,     0,    20,     0,    22,    47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -219,  -219,  -219,  -219,   424,  -219,  -219,  -219,  -219,  -219,
    -219,  -219,  -219,  -219,  -158,  -219,  -219,  -219,  -219,  -219,
    -219,  -219,  -219,  -219,   -61,  -219,  -219,    -2,  -219,  -219,
    -219,  -219,   -75,  -105,  -219,  -152,  -219,  -219,  -219,  -219,
    -219,   169,   284,  -219,  -218,   426,   173,   -91,  -219,  -219
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    20,   218,   160,   219,   161,   223,   274,
     289,   243,   273,   225,   200,   220,    22,    73,   149,   179,
     180,   181,   209,   287,    23,    60,    96,   221,    65,   158,
     121,   211,    66,    67,    25,   143,    26,   177,   230,   228,
     265,   111,   112,   175,   201,   222,   118,    92,    28,    29
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      24,   132,   188,   189,   103,    42,     5,   122,   127,     6,
     251,   171,   254,    37,    43,    44,    45,    51,   195,    52,
      87,    10,    11,    42,     5,    74,    34,     6,    59,   202,
      63,     3,    68,    69,    35,   156,   128,  -104,    75,    10,
      11,  -104,    38,    36,    16,   183,    17,    83,    84,    85,
      86,    88,    89,    90,   173,    91,    94,    39,    97,   182,
     152,   123,    16,   203,    17,   129,    18,    58,    19,    40,
     172,   255,   248,   294,   184,    59,    63,   105,   119,   120,
      41,   106,   215,   198,    18,    49,    19,    50,    46,    51,
     124,    52,   146,    47,   214,   280,   125,   186,   147,    80,
     -94,   147,   130,   131,   -94,    64,   227,   142,   276,   212,
     207,    56,    57,   208,   233,   282,   283,     4,     5,    48,
      80,     6,     7,   155,    81,    54,   157,     8,    82,     9,
      77,    55,   104,    10,    11,   250,   162,   163,   164,    51,
      12,    52,    13,   -97,    14,    81,    30,   -97,    70,    82,
     237,    15,    87,   213,    31,    78,    16,   105,    17,    33,
     144,   106,   145,    71,   191,   192,   193,   194,   142,   271,
      31,    32,   -95,  -135,  -135,    33,   -95,   278,    18,    72,
      19,   210,    76,    88,    89,    90,   284,    91,   285,   205,
     206,    52,     4,     5,   238,   290,     6,     7,    93,   -98,
      99,   295,     8,   -98,     9,   109,   216,   217,    10,    11,
      98,   101,   102,    42,     5,    95,   110,     6,   113,   105,
     229,    31,   142,   106,    42,    61,    15,   252,     6,    10,
      11,    16,   114,    17,     4,     5,   259,   126,     6,     7,
      10,    11,   133,    50,     8,    51,     9,    52,   216,   217,
      10,    11,    16,    18,    17,    19,    49,  -102,    50,   134,
      51,  -102,    52,    16,   148,    17,    42,     5,    15,   153,
       6,   150,   165,    16,    18,    17,    19,    42,     5,   154,
     100,     6,    10,    11,    62,    18,    49,    19,    50,    51,
      51,    52,    52,    10,    11,    18,    77,    19,   135,   136,
     115,   116,   137,   117,   170,    16,   174,    17,   178,  -100,
     151,   185,   187,  -100,    10,    11,    16,   196,    17,   -96,
      31,    78,    49,   -96,    50,    33,    51,    18,    52,    19,
      49,   197,    50,    52,    51,   199,    52,   138,    18,   139,
      19,    49,   204,    50,   224,    51,   159,    52,    49,   226,
      50,  -103,    51,    79,    52,  -103,    49,   231,    50,   140,
      51,   141,    52,    49,   107,    50,    49,    51,    50,    52,
      51,   108,    52,    52,    49,   232,    50,   234,    51,   190,
      52,   235,   236,   239,   240,    53,   241,   244,   245,   242,
     246,  -101,   253,   247,   249,  -101,   266,    49,   166,    50,
     167,    51,   168,    52,   169,   257,   256,   258,   260,   261,
     263,   262,   264,   267,   268,   272,   275,   281,   277,   279,
     286,   291,   292,   288,   293,   296,    21,   270,    27,   269,
       0,   176
};

static const yytype_int16 yycheck[] =
{
       2,   106,   160,   161,    65,     3,     4,    82,    99,     7,
     228,     4,     4,     3,    16,    17,    18,    43,   170,    45,
       3,    19,    20,     3,     4,     5,    64,     7,    30,     3,
      32,     0,    34,    35,    64,   126,    40,    63,    40,    19,
      20,    67,    32,     3,    42,    40,    44,    49,    50,    51,
      52,    34,    35,    36,   145,    38,    58,     3,    60,   150,
     121,    15,    42,    37,    44,    69,    64,    65,    66,     3,
      63,    63,   224,   291,    69,    77,    78,    65,    80,    81,
       3,    69,   187,   174,    64,    39,    66,    41,     3,    43,
      62,    45,    62,    61,   185,    62,    68,   158,    68,    40,
      63,    68,   104,   105,    67,    66,   197,   109,   266,   184,
      26,    67,    68,    29,   205,   273,   274,     3,     4,    61,
      40,     7,     8,   125,    65,    61,   128,    13,    69,    15,
      40,    61,    40,    19,    20,   226,   138,   139,   140,    43,
      26,    45,    28,    63,    30,    65,    40,    67,    11,    69,
     211,    37,     3,     4,    64,    65,    42,    65,    44,    69,
      65,    69,    67,    64,   166,   167,   168,   169,   170,   260,
      64,    65,    63,    67,    68,    69,    67,   268,    64,    64,
      66,   183,    40,    34,    35,    36,   277,    38,   279,    67,
      68,    45,     3,     4,    40,   286,     7,     8,     3,    63,
      67,   292,    13,    67,    15,    65,    17,    18,    19,    20,
      63,     3,     3,     3,     4,     5,     3,     7,    62,    65,
      31,    64,   224,    69,     3,     4,    37,   229,     7,    19,
      20,    42,    67,    44,     3,     4,   238,    67,     7,     8,
      19,    20,    67,    41,    13,    43,    15,    45,    17,    18,
      19,    20,    42,    64,    44,    66,    39,    63,    41,    67,
      43,    67,    45,    42,    67,    44,     3,     4,    37,     3,
       7,    67,     3,    42,    64,    44,    66,     3,     4,     5,
      63,     7,    19,    20,    63,    64,    39,    66,    41,    43,
      43,    45,    45,    19,    20,    64,    40,    66,     3,     4,
       4,     5,     7,     7,    67,    42,    33,    44,    66,    63,
      63,    67,    69,    67,    19,    20,    42,    63,    44,    63,
      64,    65,    39,    67,    41,    69,    43,    64,    45,    66,
      39,    67,    41,    45,    43,    67,    45,    42,    64,    44,
      66,    39,    61,    41,    67,    43,    63,    45,    39,    67,
      41,    63,    43,    62,    45,    67,    39,    65,    41,    64,
      43,    66,    45,    39,    62,    41,    39,    43,    41,    45,
      43,    62,    45,    45,    39,     3,    41,    66,    43,    62,
      45,     3,    61,    63,    11,    61,    61,    61,    61,     9,
      61,    63,    27,    14,    63,    67,    61,    39,    39,    41,
      41,    43,    43,    45,    45,     3,    40,    64,    67,     3,
      10,    67,    67,    63,    67,    12,    27,     4,    67,    67,
      33,    67,    67,    63,    16,    27,     2,   258,     2,   256,
      -1,   147
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    71,    72,     0,     3,     4,     7,     8,    13,    15,
      19,    20,    26,    28,    30,    37,    42,    44,    64,    66,
      73,    74,    86,    94,    97,   104,   106,   115,   118,   119,
      40,    64,    65,    69,    64,    64,     3,     3,    32,     3,
       3,     3,     3,    97,    97,    97,     3,    61,    61,    39,
      41,    43,    45,    61,    61,    61,    67,    68,    65,    97,
      95,     4,    63,    97,    66,    98,   102,   103,    97,    97,
      11,    64,    64,    87,     5,    97,    40,    40,    65,    62,
      40,    65,    69,    97,    97,    97,    97,     3,    34,    35,
      36,    38,   117,     3,    97,     5,    96,    97,    63,    67,
      63,     3,     3,    94,    40,    65,    69,    62,    62,    65,
       3,   111,   112,    62,    67,     4,     5,     7,   116,    97,
      97,   100,   102,    15,    62,    68,    67,   117,    40,    69,
      97,    97,   103,    67,    67,     3,     4,     7,    42,    44,
      64,    66,    97,   105,    65,    67,    62,    68,    67,    88,
      67,    63,    94,     3,     5,    97,   117,    97,    99,    63,
      75,    77,    97,    97,    97,     3,    39,    41,    43,    45,
      67,     4,    63,   117,    33,   113,   112,   107,    66,    89,
      90,    91,   117,    40,    69,    67,    94,    69,    84,    84,
      62,    97,    97,    97,    97,   105,    63,    67,   117,    67,
      84,   114,     3,    37,    61,    67,    68,    26,    29,    92,
      97,   101,   102,     4,   117,   103,    17,    18,    74,    76,
      85,    97,   115,    78,    67,    83,    67,   117,   109,    31,
     108,    65,     3,   117,    66,     3,    61,    94,    40,    63,
      11,    61,     9,    81,    61,    61,    61,    14,   105,    63,
     117,   114,    97,    27,     4,    63,    40,     3,    64,    97,
      67,     3,    67,    10,    67,   110,    61,    63,    67,   116,
     111,   117,    12,    82,    79,    27,    84,    67,   117,    67,
      62,     4,    84,    84,   117,   117,    33,    93,    63,    80,
     117,    67,    67,    16,   114,   117,    27
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    70,    71,    72,    72,    72,    72,    72,    72,    72,
      73,    73,    75,    76,    74,    77,    78,    74,    79,    80,
      74,    74,    74,    81,    82,    81,    83,    83,    84,    84,
      84,    84,    84,    85,    85,    87,    86,    88,    88,    89,
      89,    89,    89,    90,    90,    91,    91,    92,    93,    93,
      95,    94,    96,    96,    96,    96,    96,    97,    97,    97,
      97,    97,    97,    97,    97,    97,    97,    97,    97,    97,
      97,    97,    97,    97,    97,    98,    97,    99,    97,   100,
      97,   101,    97,    97,    97,    97,    97,    97,   102,   102,
     102,   103,   104,   104,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   107,   108,   106,   109,   110,
     106,   111,   111,   111,   112,   112,   112,   113,   113,   114,
     114,   115,   115,   115,   115,   116,   116,   116,   117,   117,
     117,   117,   117,   118,   118,   119,   119
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     3,     3,     3,     3,     3,     2,
       3,     3,     0,     0,    10,     0,     0,     9,     0,     0,
      14,    11,    15,     0,     0,     4,     0,     2,     0,     3,
       3,     3,     3,     1,     1,     0,     7,     0,     3,     3,
       7,     6,     7,     2,     4,     0,     3,     9,     0,     2,
       0,     5,     0,     1,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     4,     3,     3,     3,     3,     3,     2,
       2,     3,     6,     4,     7,     0,     4,     0,     7,     0,
       5,     0,     8,     4,     7,     3,     5,     9,     1,     6,
       3,     2,     2,     5,     1,     1,     1,     2,     3,     3,
       3,     3,     3,     2,     2,     0,     0,     9,     0,     0,
      11,     0,     1,     3,     3,     6,     5,     0,     2,     1,
       5,     3,     6,     5,     6,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: input  */
#line 966 "tuc_transpiler.y"
        { print_evaluation((yyvsp[0].str)); }
#line 2308 "tuc_transpiler.tab.c"
    break;

  case 3: /* input: %empty  */
#line 969 "tuc_transpiler.y"
                                                  {(yyval.str) = "";}
#line 2314 "tuc_transpiler.tab.c"
    break;

  case 4: /* input: input variable_definition ';'  */
#line 970 "tuc_transpiler.y"
                                                  { (yyval.str) = template("%s%s\n", (yyvsp[-2].str), (yyvsp[-1].str));}
#line 2320 "tuc_transpiler.tab.c"
    break;

  case 5: /* input: input expr ';'  */
#line 971 "tuc_transpiler.y"
                                                  { assignment_made = 0;  (yyval.str) = template("%s%s;\n", (yyvsp[-2].str), (yyvsp[-1].str));}
#line 2326 "tuc_transpiler.tab.c"
    break;

  case 6: /* input: input function_definition ';'  */
#line 972 "tuc_transpiler.y"
                                                  { assignment_made = 0; (yyval.str) = template("%s%s\n", (yyvsp[-2].str), (yyvsp[-1].str));}
#line 2332 "tuc_transpiler.tab.c"
    break;

  case 7: /* input: input complicated_types ';'  */
#line 973 "tuc_transpiler.y"
                                                  { assignment_made = 0;  (yyval.str) = template("%s%s\n", (yyvsp[-2].str), (yyvsp[-1].str));}
#line 2338 "tuc_transpiler.tab.c"
    break;

  case 8: /* input: input commands ';'  */
#line 974 "tuc_transpiler.y"
                                                  {  (yyval.str) = template("%s%s\n", (yyvsp[-2].str), (yyvsp[-1].str));}
#line 2344 "tuc_transpiler.tab.c"
    break;

  case 9: /* input: input macro  */
#line 975 "tuc_transpiler.y"
                                                  {  (yyval.str) = template("%s%s\n", (yyvsp[-1].str), (yyvsp[0].str));}
#line 2350 "tuc_transpiler.tab.c"
    break;

  case 10: /* macro: KW_MACRO IDENTIFIER STRING  */
#line 982 "tuc_transpiler.y"
                               {
        char *id = (yyvsp[-1].str); 
        char *value = (yyvsp[0].str);
        add_macro(id, value);  
    }
#line 2360 "tuc_transpiler.tab.c"
    break;

  case 11: /* macro: KW_MACRO IDENTIFIER expr  */
#line 987 "tuc_transpiler.y"
                             {
        char *id = (yyvsp[-1].str);  
        char *value = (yyvsp[0].str); 

        add_macro(id, value);
    }
#line 2371 "tuc_transpiler.tab.c"
    break;

  case 12: /* $@1: %empty  */
#line 1001 "tuc_transpiler.y"
                           { indentation_level++; is_a_loop = 0; is_an_if = 1;}
#line 2377 "tuc_transpiler.tab.c"
    break;

  case 13: /* @2: %empty  */
#line 1003 "tuc_transpiler.y"
    {
        (yyval.str) = concatenate2(indent_statements((yyvsp[0].str), indentation_level), "}\n");
        indentation_level--;
    }
#line 2386 "tuc_transpiler.tab.c"
    break;

  case 14: /* commands: KW_IF '(' expr ')' ':' $@1 stmts @2 else_statement KW_ENDIF  */
#line 1007 "tuc_transpiler.y"
                            {
        (yyval.str) = template("%s (%s) {\n%s%s\n", (yyvsp[-9].str), (yyvsp[-7].str), (yyvsp[-2].str), (yyvsp[-1].str));
        is_an_if = 0;
    }
#line 2395 "tuc_transpiler.tab.c"
    break;

  case 15: /* $@3: %empty  */
#line 1013 "tuc_transpiler.y"
                                {indentation_level++; is_a_loop = 1; is_an_if = 0;}
#line 2401 "tuc_transpiler.tab.c"
    break;

  case 16: /* @4: %empty  */
#line 1015 "tuc_transpiler.y"
    {
        (yyval.str) = indent_statements((yyvsp[0].str), indentation_level);
        indentation_level--;
    }
#line 2410 "tuc_transpiler.tab.c"
    break;

  case 17: /* commands: KW_WHILE '(' expr ')' ':' $@3 stmts @4 KW_ENDWHILE  */
#line 1019 "tuc_transpiler.y"
                {
        (yyval.str) = template("%s (%s) {\n%s%s}\n", (yyvsp[-8].str), (yyvsp[-6].str), (yyvsp[-1].str));
        is_a_loop = 0;
    }
#line 2419 "tuc_transpiler.tab.c"
    break;

  case 18: /* @5: %empty  */
#line 1027 "tuc_transpiler.y"
    {
        if(for_has_step == 1){
            (yyval.str) = template("for (int %s = %s; %s < %s; %s =+ %s) {\n", (yyvsp[-8].str), (yyvsp[-5].str), (yyvsp[-8].str), (yyvsp[-3].str), (yyvsp[-8].str), (yyvsp[-2].str)); 
        } else {
            (yyval.str) = template("for (int %s = %s; %s < %s; %s++) {\n", (yyvsp[-8].str), (yyvsp[-5].str), (yyvsp[-8].str), (yyvsp[-3].str), (yyvsp[-8].str)); 
        }
        indentation_level++; is_a_loop = 1; is_an_if = 0;
    }
#line 2432 "tuc_transpiler.tab.c"
    break;

  case 19: /* @6: %empty  */
#line 1036 "tuc_transpiler.y"
    {
        (yyval.str) = indent_statements((yyvsp[0].str), indentation_level);
        indentation_level--;
    }
#line 2441 "tuc_transpiler.tab.c"
    break;

  case 20: /* commands: KW_FOR IDENTIFIER KW_IN '[' arithmetic_expr_non_empty ':' arithmetic_expr_non_empty step ']' ':' @5 stmts @6 KW_ENDFOR  */
#line 1040 "tuc_transpiler.y"
              {

        (yyval.str) = template("%s%s}\n", (yyvsp[-3].str), (yyvsp[-1].str));
        is_a_loop = 0;
    }
#line 2451 "tuc_transpiler.tab.c"
    break;

  case 21: /* commands: IDENTIFIER ASSIGNMENT_OP '[' expr KW_FOR IDENTIFIER ':' POSINT ']' ':' KW_VARIABLE_TYPE  */
#line 1050 "tuc_transpiler.y"
    {
        char* newType = (yyvsp[0].str);
        (yyval.str) = template("%s* %s =(%s*)malloc(%s*sizeof(%s));\nfor (int %s = 0; %s < %s; ++%s) {\n  %s[%s] = %s;\n}\n", 
                        newType, (yyvsp[-10].str),newType,(yyvsp[-3].str),     newType,        (yyvsp[-5].str),     (yyvsp[-5].str),  (yyvsp[-3].str),  (yyvsp[-5].str),       (yyvsp[-10].str),(yyvsp[-5].str),   (yyvsp[-7].str));

    }
#line 2462 "tuc_transpiler.tab.c"
    break;

  case 22: /* commands: IDENTIFIER ASSIGNMENT_OP '[' expr KW_FOR IDENTIFIER ':' KW_VARIABLE_TYPE KW_IN IDENTIFIER KW_OF POSINT ']' ':' KW_VARIABLE_TYPE  */
#line 1060 "tuc_transpiler.y"
    {
        char* newType = (yyvsp[0].str); 
        char* s2 = template("%s[%s_i]",(yyvsp[-5].str),(yyvsp[-5].str));
        char* modifiedExpr = replace_identifier_in_expr((yyvsp[-11].str), (yyvsp[-9].str), s2);

        (yyval.str) = template(
            "%s* %s = (%s*)malloc(%s * sizeof(%s));\nfor (%s %s_i = 0; %s_i < %s; ++%s_i) {\n   %s[%s_i] = %s;\n}\n",
            newType,(yyvsp[-14].str), newType, (yyvsp[-3].str), newType,            (yyvsp[-7].str), (yyvsp[-5].str),      (yyvsp[-5].str), (yyvsp[-3].str),    (yyvsp[-5].str),    (yyvsp[-14].str), (yyvsp[-5].str), modifiedExpr
        );
    }
#line 2477 "tuc_transpiler.tab.c"
    break;

  case 23: /* else_statement: %empty  */
#line 1076 "tuc_transpiler.y"
                                                    { (yyval.str) = ""; }
#line 2483 "tuc_transpiler.tab.c"
    break;

  case 24: /* @7: %empty  */
#line 1077 "tuc_transpiler.y"
                                                    { (yyval.str) = template("%s {\n", (yyvsp[-1].str)); indentation_level++;}
#line 2489 "tuc_transpiler.tab.c"
    break;

  case 25: /* else_statement: KW_ELSE ':' @7 stmts  */
#line 1079 "tuc_transpiler.y"
  {               
      (yyval.str) = template("%s%s}", (yyvsp[-1].str), indent_statements((yyvsp[0].str), indentation_level));
      indentation_level--;
  }
#line 2498 "tuc_transpiler.tab.c"
    break;

  case 26: /* step: %empty  */
#line 1085 "tuc_transpiler.y"
                                        { for_has_step = 0; (yyval.str) = ""; }
#line 2504 "tuc_transpiler.tab.c"
    break;

  case 27: /* step: ':' arithmetic_expr_non_empty  */
#line 1086 "tuc_transpiler.y"
                                        { for_has_step = 1; (yyval.str) = template("%s", (yyvsp[0].str));}
#line 2510 "tuc_transpiler.tab.c"
    break;

  case 28: /* stmts: %empty  */
#line 1094 "tuc_transpiler.y"
                                                  { (yyval.str) = ""; }
#line 2516 "tuc_transpiler.tab.c"
    break;

  case 29: /* stmts: stmts loop_thing ';'  */
#line 1095 "tuc_transpiler.y"
                              { if (is_a_loop != 1 && is_an_if != 1 ) {  yyerror("This isnt a loop or if"); YYABORT;} (yyval.str) = template("%s%s;\n", (yyvsp[-2].str), (yyvsp[-1].str));}
#line 2522 "tuc_transpiler.tab.c"
    break;

  case 30: /* stmts: stmts variable_definition ';'  */
#line 1097 "tuc_transpiler.y"
                                                  { (yyval.str) = template("%s%s\n", (yyvsp[-2].str), (yyvsp[-1].str)); }
#line 2528 "tuc_transpiler.tab.c"
    break;

  case 31: /* stmts: stmts expr ';'  */
#line 1098 "tuc_transpiler.y"
                                                  { assignment_made = 0; (yyval.str) = template("%s%s;\n", (yyvsp[-2].str), (yyvsp[-1].str)); }
#line 2534 "tuc_transpiler.tab.c"
    break;

  case 32: /* stmts: stmts commands ';'  */
#line 1099 "tuc_transpiler.y"
                                                  { (yyval.str) = template("%s%s", (yyvsp[-2].str), (yyvsp[-1].str)); }
#line 2540 "tuc_transpiler.tab.c"
    break;

  case 35: /* $@8: %empty  */
#line 1111 "tuc_transpiler.y"
                     { comp_name = (yyvsp[0].str); inside_comp = 1; createNewComp(comp_name); indentation_level++;}
#line 2546 "tuc_transpiler.tab.c"
    break;

  case 36: /* complicated_types: KW_COMP IDENTIFIER $@8 ':' declaration_comp function_definition_section KW_ENDCOMP  */
#line 1113 "tuc_transpiler.y"
  { indentation_level--;
    char* function_definition_prints = getCompFunctionsAsString(comp_name);   
    char* function_names = getCompFunctionNamesAsString(comp_name);
    (yyval.str) = template("typedef struct %s {\n%s%s} %s;\n\n%s%s ctor_%s = {%s};\n", (yyvsp[-5].str), (yyvsp[-2].str), (yyvsp[-1].str), comp_name, function_definition_prints, comp_name, comp_name, function_names); 
    inside_comp = 0;
  }
#line 2557 "tuc_transpiler.tab.c"
    break;

  case 37: /* declaration_comp: %empty  */
#line 1124 "tuc_transpiler.y"
                                                            { (yyval.str) = ""; }
#line 2563 "tuc_transpiler.tab.c"
    break;

  case 38: /* declaration_comp: declaration_comp variable_definition_comp ';'  */
#line 1125 "tuc_transpiler.y"
                                                            { (yyval.str) = template("%s  %s;\n", (yyvsp[-2].str), (yyvsp[-1].str)); }
#line 2569 "tuc_transpiler.tab.c"
    break;

  case 39: /* variable_definition_comp: variable_list_comp ':' KW_VARIABLE_TYPE  */
#line 1129 "tuc_transpiler.y"
                                                                                { if(is_comp_type((yyvsp[0].str))){  add_comp_variable_definitions((yyvsp[0].str), (yyvsp[-2].str));}
                                                                                  add_comp_variables(comp_name, (yyvsp[-2].str)); (yyval.str) = template("%s %s", (yyvsp[0].str), (yyvsp[-2].str));}
#line 2576 "tuc_transpiler.tab.c"
    break;

  case 40: /* variable_definition_comp: '#' IDENTIFIER '[' POSINT ']' ':' KW_VARIABLE_TYPE  */
#line 1132 "tuc_transpiler.y"
                                                                                { if(is_comp_type((yyvsp[0].str))){add_comp_variable_definitions((yyvsp[0].str), (yyvsp[-5].str));} add_comp_variable(comp_name, (yyvsp[-5].str)); 
                                                                                  (yyval.str) = template("%s %s[%s]", (yyvsp[0].str), (yyvsp[-5].str), (yyvsp[-3].str) ); }
#line 2583 "tuc_transpiler.tab.c"
    break;

  case 41: /* variable_definition_comp: '#' IDENTIFIER '[' ']' ':' KW_VARIABLE_TYPE  */
#line 1135 "tuc_transpiler.y"
                                                                                { if(is_comp_type((yyvsp[0].str))){ add_comp_variable_definitions((yyvsp[0].str), (yyvsp[-4].str));} add_comp_variable(comp_name, (yyvsp[-4].str));
                                                                                  (yyval.str) = template("%s[]: %s", (yyvsp[-4].str), (yyvsp[0].str) ); }
#line 2590 "tuc_transpiler.tab.c"
    break;

  case 42: /* variable_definition_comp: '#' KW_CONST IDENTIFIER ASSIGNMENT_OP VALUE ':' KW_VARIABLE_TYPE  */
#line 1138 "tuc_transpiler.y"
                                                                                { if(is_comp_type((yyvsp[0].str))){ add_comp_variable_definitions((yyvsp[0].str), (yyvsp[-4].str));} add_comp_variable(comp_name, (yyvsp[-4].str));
                                                                                  (yyval.str) = template("%s %s %s = %s", (yyvsp[-5].str), (yyvsp[0].str), (yyvsp[-4].str), (yyvsp[-2].str)); }
#line 2597 "tuc_transpiler.tab.c"
    break;

  case 43: /* variable_list_comp: '#' IDENTIFIER  */
#line 1143 "tuc_transpiler.y"
                                                    { (yyval.str) = template("%s", (yyvsp[0].str)); }
#line 2603 "tuc_transpiler.tab.c"
    break;

  case 44: /* variable_list_comp: variable_list_comp ',' '#' IDENTIFIER  */
#line 1144 "tuc_transpiler.y"
                                                    { (yyval.str) = template("%s, %s", (yyvsp[-3].str), (yyvsp[0].str)); }
#line 2609 "tuc_transpiler.tab.c"
    break;

  case 45: /* function_definition_section: %empty  */
#line 1152 "tuc_transpiler.y"
                                                                { (yyval.str) = ""; }
#line 2615 "tuc_transpiler.tab.c"
    break;

  case 46: /* function_definition_section: function_definition_section function_definition_comp ';'  */
#line 1153 "tuc_transpiler.y"
                                                                {(yyval.str) = template("%s  %s;\n", (yyvsp[-2].str), (yyvsp[-1].str));}
#line 2621 "tuc_transpiler.tab.c"
    break;

  case 47: /* function_definition_comp: KW_DEF IDENTIFIER '(' arguments ')' return_type_comp ':' body KW_ENDDEF  */
#line 1158 "tuc_transpiler.y"
  {
    if (func_needs_return_type != body_has_return_type) {  
      yyerror("Return statement missing from body"); 
      YYABORT;
    }
    (yyval.str) = template("%s (*%s)(struct %s *self%s%s)", (yyvsp[-3].str), (yyvsp[-7].str), comp_name, strlen((yyvsp[-5].str)) ? ", " : "", (yyvsp[-5].str));
    func_needs_return_type = 0;
    body_has_return_type = 0;

    add_function((yyvsp[-7].str), num_of_args_in_function);  
    num_of_args_in_function = 0;

    char* function_follow_comp = template("%s %s(struct %s *self%s%s) {\n%s}\n", (yyvsp[-3].str), (yyvsp[-7].str), comp_name,strlen((yyvsp[-5].str)) == 0 ? "":", " , (yyvsp[-5].str), indent_statements((yyvsp[-1].str), indentation_level) );
    addFunctionToComp(comp_name, function_follow_comp, (yyvsp[-7].str));
  }
#line 2641 "tuc_transpiler.tab.c"
    break;

  case 48: /* return_type_comp: %empty  */
#line 1176 "tuc_transpiler.y"
                                        { (yyval.str) = "void"; }
#line 2647 "tuc_transpiler.tab.c"
    break;

  case 49: /* return_type_comp: KW_FUNC_RETURN KW_VARIABLE_TYPE  */
#line 1177 "tuc_transpiler.y"
                                        { func_needs_return_type = 1; (yyval.str) = template("%s", (yyvsp[0].str)); }
#line 2653 "tuc_transpiler.tab.c"
    break;

  case 50: /* $@9: %empty  */
#line 1184 "tuc_transpiler.y"
                   {push((yyvsp[-1].str));}
#line 2659 "tuc_transpiler.tab.c"
    break;

  case 51: /* func_call: IDENTIFIER '(' $@9 func_args ')'  */
#line 1185 "tuc_transpiler.y"
    {  
        const DefinedFunction* func = find_function((yyvsp[-4].str));
        function_name_temp = (yyvsp[-4].str);

        if(find_included_function((yyvsp[-4].str)) == 1) {used_lambdalib_function = 1;}

        if (!func) {
            yyerror("Error: Function has not been defined.");
            YYABORT;
        } 
        char* function_name = peek();
        // ????
        // printf("get_temp_arg_count(function_name): %d, func->has_arguments: %d\n", get_temp_arg_count(function_name), func->has_arguments);
        // if (func->has_arguments != 99 && get_temp_arg_count(function_name) != func->has_arguments) {
        //     yyerror("Error: The function is defined as variadic.");
        //     YYABORT;
        // }
        if(is_comp_function_simple(function_name) == 0) {
            (yyval.str) = template("%s(%s)", (yyvsp[-4].str), (yyvsp[-1].str));
        }
        else if(inside_comp == 1) { (yyval.str) = template("%s(&self->%s%s%s)", (yyvsp[-4].str), func_call_object, ((yyvsp[-1].str) && strlen((yyvsp[-1].str)) != 0) ? ", " : "", (yyvsp[-1].str)); }
        else { (yyval.str) = template("%s(&%s%s%s)", (yyvsp[-4].str), func_call_object, ((yyvsp[-1].str) && strlen((yyvsp[-1].str)) != 0) ? ", " : "", (yyvsp[-1].str)); }

        set_count_to_zero(function_name); 
        pop();
    }
#line 2690 "tuc_transpiler.tab.c"
    break;

  case 52: /* func_args: %empty  */
#line 1214 "tuc_transpiler.y"
           { (yyval.str) = ""; }
#line 2696 "tuc_transpiler.tab.c"
    break;

  case 53: /* func_args: expr  */
#line 1215 "tuc_transpiler.y"
                                    { increase_count(peek()); (yyval.str) = template("%s", (yyvsp[0].str)); }
#line 2702 "tuc_transpiler.tab.c"
    break;

  case 54: /* func_args: STRING  */
#line 1216 "tuc_transpiler.y"
                                    { increase_count(peek()); (yyval.str) = template("%s", (yyvsp[0].str)); }
#line 2708 "tuc_transpiler.tab.c"
    break;

  case 55: /* func_args: func_args ',' expr  */
#line 1217 "tuc_transpiler.y"
                                    { increase_count(peek()); (yyval.str) = template("%s, %s", (yyvsp[-2].str), (yyvsp[0].str)); }
#line 2714 "tuc_transpiler.tab.c"
    break;

  case 56: /* func_args: func_args ',' STRING  */
#line 1218 "tuc_transpiler.y"
                                    { increase_count(peek()); (yyval.str) = template("%s, %s", (yyvsp[-2].str), (yyvsp[0].str)); }
#line 2720 "tuc_transpiler.tab.c"
    break;

  case 57: /* expr: func_call  */
#line 1224 "tuc_transpiler.y"
                                        { (yyval.str) = template("%s", (yyvsp[0].str));  }
#line 2726 "tuc_transpiler.tab.c"
    break;

  case 58: /* expr: NUMBERS  */
#line 1225 "tuc_transpiler.y"
                                        { (yyval.str) = (yyvsp[0].str); }
#line 2732 "tuc_transpiler.tab.c"
    break;

  case 59: /* expr: KW_FALSE  */
#line 1226 "tuc_transpiler.y"
                                        { (yyval.str) = (yyvsp[0].str); }
#line 2738 "tuc_transpiler.tab.c"
    break;

  case 60: /* expr: KW_TRUE  */
#line 1227 "tuc_transpiler.y"
                                        { (yyval.str) = (yyvsp[0].str); }
#line 2744 "tuc_transpiler.tab.c"
    break;

  case 61: /* expr: use_comp_variable  */
#line 1228 "tuc_transpiler.y"
                                        { (yyval.str) = (yyvsp[0].str); }
#line 2750 "tuc_transpiler.tab.c"
    break;

  case 62: /* expr: IDENTIFIER  */
#line 1229 "tuc_transpiler.y"
                                        { (yyval.str) = (yyvsp[0].str); }
#line 2756 "tuc_transpiler.tab.c"
    break;

  case 63: /* expr: IDENTIFIER '[' expr ']'  */
#line 1230 "tuc_transpiler.y"
                                        { (yyval.str) = template("%s[%s]", (yyvsp[-3].str), (yyvsp[-1].str)); }
#line 2762 "tuc_transpiler.tab.c"
    break;

  case 64: /* expr: '(' expr ')'  */
#line 1231 "tuc_transpiler.y"
                                        { (yyval.str) = template("(%s)", (yyvsp[-1].str)); }
#line 2768 "tuc_transpiler.tab.c"
    break;

  case 65: /* expr: expr CALC_OP expr  */
#line 1232 "tuc_transpiler.y"
                                        {   if(strcmp((yyvsp[-1].str), "%") == 0) {(yyval.str) = template("((int) %s %s (int) %s)", (yyvsp[-2].str), (yyvsp[-1].str), (yyvsp[0].str));} 
                                            else {(yyval.str) = template("%s %s %s", (yyvsp[-2].str), (yyvsp[-1].str), (yyvsp[0].str));}  }
#line 2775 "tuc_transpiler.tab.c"
    break;

  case 66: /* expr: expr EXPONENTIATION_OP expr  */
#line 1234 "tuc_transpiler.y"
                                        { (yyval.str) = template("pow(%s, %s)", (yyvsp[-2].str), (yyvsp[0].str)); }
#line 2781 "tuc_transpiler.tab.c"
    break;

  case 67: /* expr: expr RELATIONAL_OP expr  */
#line 1235 "tuc_transpiler.y"
                                        { (yyval.str) = template("%s %s %s", (yyvsp[-2].str), (yyvsp[-1].str), (yyvsp[0].str)); }
#line 2787 "tuc_transpiler.tab.c"
    break;

  case 68: /* expr: expr LOGICAL_AND_OR_OP expr  */
#line 1236 "tuc_transpiler.y"
                                        { (yyval.str) = template("%s %s %s", (yyvsp[-2].str), (yyvsp[-1].str), (yyvsp[0].str)); }
#line 2793 "tuc_transpiler.tab.c"
    break;

  case 69: /* expr: LOGICAL_NOT_OP expr  */
#line 1237 "tuc_transpiler.y"
                                        { (yyval.str) = template("%s %s", (yyvsp[-1].str), (yyvsp[0].str)); }
#line 2799 "tuc_transpiler.tab.c"
    break;

  case 70: /* expr: UNARY_OP expr  */
#line 1238 "tuc_transpiler.y"
                                        { (yyval.str) = template("%s%s", (yyvsp[-1].str), (yyvsp[0].str)); }
#line 2805 "tuc_transpiler.tab.c"
    break;

  case 71: /* expr: IDENTIFIER ASSIGNMENT_OP expr  */
#line 1241 "tuc_transpiler.y"
  {
      if (assignment_made == 1) {
          yyerror("Multiple assignments in one expression are not allowed");
          YYABORT; 
      }
      assignment_made = 1; 
      (yyval.str) = template("%s %s %s", (yyvsp[-2].str), (yyvsp[-1].str), (yyvsp[0].str)); 
  }
#line 2818 "tuc_transpiler.tab.c"
    break;

  case 72: /* expr: IDENTIFIER '[' expr ']' ASSIGNMENT_OP expr  */
#line 1250 "tuc_transpiler.y"
  {
      if (assignment_made == 1) {
          yyerror("Multiple assignments in one expression are not allowed");
          YYABORT; 
      }
      assignment_made = 1; 
      (yyval.str) = template("%s[%s] %s %s", (yyvsp[-5].str), (yyvsp[-3].str), (yyvsp[-1].str), (yyvsp[0].str) ); 
  }
#line 2831 "tuc_transpiler.tab.c"
    break;

  case 73: /* expr: '#' IDENTIFIER ASSIGNMENT_OP expr  */
#line 1260 "tuc_transpiler.y"
  {     
      if (assignment_made == 1 || inside_comp == 0) {
          yyerror("Multiple assignments in one expression are not allowed or not in comp");
          YYABORT; 
      }
      assignment_made = 1; 
      (yyval.str) = template("self->%s %s %s", (yyvsp[-2].str), (yyvsp[-1].str), (yyvsp[0].str)); 
  }
#line 2844 "tuc_transpiler.tab.c"
    break;

  case 74: /* expr: '#' IDENTIFIER '[' expr ']' ASSIGNMENT_OP expr  */
#line 1269 "tuc_transpiler.y"
  {
      if (assignment_made == 1 || inside_comp == 0) {
          yyerror("Multiple assignments in one expression are not allowed or not in comp");
          YYABORT; 
      }
      assignment_made = 1; 
      (yyval.str) = template("self->%s[%s] %s %s", (yyvsp[-5].str), (yyvsp[-3].str), (yyvsp[-1].str), (yyvsp[0].str) ); 
  }
#line 2857 "tuc_transpiler.tab.c"
    break;

  case 75: /* $@10: %empty  */
#line 1280 "tuc_transpiler.y"
                   {func_call_object = (yyvsp[-1].str);}
#line 2863 "tuc_transpiler.tab.c"
    break;

  case 76: /* expr: IDENTIFIER '.' $@10 func_call  */
#line 1281 "tuc_transpiler.y"
  {
      char* comp_name = get_comp_type_definition((yyvsp[-3].str));
      if(is_comp_function(comp_name, function_name_temp) == 0){
          yyerror("Function used isnt assigned for this variable type");
          YYABORT; 
      }
      (yyval.str) = template("%s.%s", (yyvsp[-3].str), (yyvsp[0].str)); 
  }
#line 2876 "tuc_transpiler.tab.c"
    break;

  case 77: /* $@11: %empty  */
#line 1289 "tuc_transpiler.y"
                                {func_call_object = concatenate4((yyvsp[-4].str),"[",(yyvsp[-2].str),"]");}
#line 2882 "tuc_transpiler.tab.c"
    break;

  case 78: /* expr: IDENTIFIER '[' expr ']' '.' $@11 func_call  */
#line 1290 "tuc_transpiler.y"
  {     
      char* comp_name = get_comp_type_definition((yyvsp[-6].str));
      if(is_comp_function(comp_name, function_name_temp) == 0){
          yyerror("Function used isnt assigned for this variable type");
          YYABORT; 
      }
      (yyval.str) = template("self->%s[%s].%s", (yyvsp[-6].str), (yyvsp[-4].str), (yyvsp[0].str)); 
  }
#line 2895 "tuc_transpiler.tab.c"
    break;

  case 79: /* $@12: %empty  */
#line 1298 "tuc_transpiler.y"
                       {func_call_object = (yyvsp[-1].str);}
#line 2901 "tuc_transpiler.tab.c"
    break;

  case 80: /* expr: '#' IDENTIFIER '.' $@12 func_call  */
#line 1299 "tuc_transpiler.y"
  {     
      char* comp_name = get_comp_type_definition((yyvsp[-3].str));
      if(is_comp_function(comp_name, function_name_temp) == 0){
          yyerror("Function used isnt assigned for this variable type");
          YYABORT; 
      }
      (yyval.str) = template("self->%s.%s", (yyvsp[-3].str), (yyvsp[0].str)); 
  }
#line 2914 "tuc_transpiler.tab.c"
    break;

  case 81: /* $@13: %empty  */
#line 1307 "tuc_transpiler.y"
                                    {func_call_object = concatenate4((yyvsp[-4].str),"[",(yyvsp[-2].str),"]");}
#line 2920 "tuc_transpiler.tab.c"
    break;

  case 82: /* expr: '#' IDENTIFIER '[' expr ']' '.' $@13 func_call  */
#line 1308 "tuc_transpiler.y"
  {     
      char* comp_name = get_comp_type_definition((yyvsp[-6].str));
      if(is_comp_function(comp_name, function_name_temp) == 0){
          yyerror("Function used isnt assigned for this variable type");
          YYABORT; 
      }
      (yyval.str) = template("self->%s[%s].%s", (yyvsp[-6].str), (yyvsp[-4].str), (yyvsp[0].str)); 
  }
#line 2933 "tuc_transpiler.tab.c"
    break;

  case 83: /* expr: '#' IDENTIFIER '.' use_of_comp_variable  */
#line 1318 "tuc_transpiler.y"
  {     
    if(validate_comp_stack((yyvsp[-2].str)) == 0) { yyerror("Variable used isnt assigned for this variable type"); YYABORT; }
    (yyval.str) = template("self->%s.%s", (yyvsp[-2].str), (yyvsp[0].str)); 
  }
#line 2942 "tuc_transpiler.tab.c"
    break;

  case 84: /* expr: '#' IDENTIFIER '[' expr ']' '.' use_of_comp_variable  */
#line 1323 "tuc_transpiler.y"
  {   
    if(validate_comp_stack((yyvsp[-5].str)) == 0) { yyerror("Variable used isnt assigned for this variable type"); YYABORT; }
    (yyval.str) = template("self->%s[%s].%s", (yyvsp[-5].str), (yyvsp[-3].str), (yyvsp[0].str)); 
  }
#line 2951 "tuc_transpiler.tab.c"
    break;

  case 85: /* expr: IDENTIFIER '.' use_of_comp_variable  */
#line 1328 "tuc_transpiler.y"
  {     
    if(validate_comp_stack((yyvsp[-2].str)) == 0) { yyerror("Variable used isnt assigned for this variable type"); YYABORT; }
    if(inside_comp == 1) {(yyval.str) = template("self->%s.%s", (yyvsp[-2].str), (yyvsp[0].str)); }
    else {(yyval.str) = template("%s.%s", (yyvsp[-2].str), (yyvsp[0].str)); }
  }
#line 2961 "tuc_transpiler.tab.c"
    break;

  case 86: /* expr: IDENTIFIER '.' use_of_comp_variable ASSIGNMENT_OP expr  */
#line 1334 "tuc_transpiler.y"
  {     
    if(validate_comp_stack((yyvsp[-4].str)) == 0) { yyerror("Variable used isnt assigned for this variable type"); YYABORT; }
    if(inside_comp == 1) {(yyval.str) = template("self->%s.%s = %s", (yyvsp[-4].str), (yyvsp[-2].str), (yyvsp[0].str)); }
    else {(yyval.str) = template("%s.%s = %s", (yyvsp[-4].str), (yyvsp[-2].str), (yyvsp[0].str)); }
  }
#line 2971 "tuc_transpiler.tab.c"
    break;

  case 87: /* expr: '#' IDENTIFIER '[' expr ']' '.' use_of_comp_variable ASSIGNMENT_OP expr  */
#line 1340 "tuc_transpiler.y"
  {
    if(validate_comp_stack((yyvsp[-7].str)) == 0) { yyerror("Variable used isnt assigned for this variable type"); YYABORT; }
    (yyval.str) = template("self->%s[%s].%s = %s", (yyvsp[-7].str), (yyvsp[-5].str), (yyvsp[-2].str), (yyvsp[0].str)); 
  }
#line 2980 "tuc_transpiler.tab.c"
    break;

  case 88: /* use_of_comp_variable: use_comp_variable_with_period  */
#line 1347 "tuc_transpiler.y"
                                                                                {  (yyval.str) = template("%s", (yyvsp[0].str)); }
#line 2986 "tuc_transpiler.tab.c"
    break;

  case 89: /* use_of_comp_variable: use_of_comp_variable '[' expr ']' '.' use_comp_variable_with_period  */
#line 1348 "tuc_transpiler.y"
                                                                                { (yyval.str) = template("%s[%s].%s", (yyvsp[-5].str), (yyvsp[-3].str), (yyvsp[0].str)); }
#line 2992 "tuc_transpiler.tab.c"
    break;

  case 90: /* use_of_comp_variable: use_of_comp_variable '.' use_comp_variable_with_period  */
#line 1349 "tuc_transpiler.y"
                                                                                { (yyval.str) = template("%s.%s", (yyvsp[-2].str), (yyvsp[0].str)); }
#line 2998 "tuc_transpiler.tab.c"
    break;

  case 91: /* use_comp_variable_with_period: '#' IDENTIFIER  */
#line 1353 "tuc_transpiler.y"
                                        { push_comp((yyvsp[0].str)); (yyval.str) = template("%s", (yyvsp[0].str));}
#line 3004 "tuc_transpiler.tab.c"
    break;

  case 92: /* use_comp_variable: '#' IDENTIFIER  */
#line 1357 "tuc_transpiler.y"
                                        { if (inside_comp == 0 || !(is_comp_variable(comp_name, (yyvsp[0].str)))) {yyerror("Variable, not in comp"); YYABORT;} 
                                          else { (yyval.str) = template("self->%s", (yyvsp[0].str));} }
#line 3011 "tuc_transpiler.tab.c"
    break;

  case 93: /* use_comp_variable: '#' IDENTIFIER '[' expr ']'  */
#line 1359 "tuc_transpiler.y"
                                        { if (inside_comp == 0 || !(is_comp_variable(comp_name, (yyvsp[-3].str)))) {yyerror("Variable, not in comp"); YYABORT;} 
                                          else { (yyval.str) = template("self->%s[%s]", (yyvsp[-3].str), (yyvsp[-1].str));} }
#line 3018 "tuc_transpiler.tab.c"
    break;

  case 94: /* arithmetic_expr_non_empty: POSINT  */
#line 1366 "tuc_transpiler.y"
                                        { (yyval.str) = (yyvsp[0].str); }
#line 3024 "tuc_transpiler.tab.c"
    break;

  case 95: /* arithmetic_expr_non_empty: REAL  */
#line 1367 "tuc_transpiler.y"
                                        { (yyval.str) = (yyvsp[0].str); }
#line 3030 "tuc_transpiler.tab.c"
    break;

  case 96: /* arithmetic_expr_non_empty: IDENTIFIER  */
#line 1368 "tuc_transpiler.y"
                                        { (yyval.str) = (yyvsp[0].str); }
#line 3036 "tuc_transpiler.tab.c"
    break;

  case 97: /* arithmetic_expr_non_empty: '#' IDENTIFIER  */
#line 1369 "tuc_transpiler.y"
                                        { if(inside_comp == 0){yyerror("Not allowed outside comp");YYABORT;} (yyval.str) = template("self->%s", (yyvsp[0].str)); }
#line 3042 "tuc_transpiler.tab.c"
    break;

  case 98: /* arithmetic_expr_non_empty: '(' expr ')'  */
#line 1370 "tuc_transpiler.y"
                                        { (yyval.str) = template("(%s)", (yyvsp[-1].str)); }
#line 3048 "tuc_transpiler.tab.c"
    break;

  case 99: /* arithmetic_expr_non_empty: expr EXPONENTIATION_OP expr  */
#line 1371 "tuc_transpiler.y"
                                        { (yyval.str) = template("pow(%s, %s)", (yyvsp[-2].str), (yyvsp[0].str)); }
#line 3054 "tuc_transpiler.tab.c"
    break;

  case 100: /* arithmetic_expr_non_empty: expr CALC_OP expr  */
#line 1372 "tuc_transpiler.y"
                                        {   if(strcmp((yyvsp[-1].str), "%") == 0) {(yyval.str) = template("(int) %s %s (int) %s", (yyvsp[-2].str), (yyvsp[-1].str), (yyvsp[0].str));} 
                                            else {(yyval.str) = template("%s %s %s", (yyvsp[-2].str), (yyvsp[-1].str), (yyvsp[0].str));}  }
#line 3061 "tuc_transpiler.tab.c"
    break;

  case 101: /* arithmetic_expr_non_empty: expr RELATIONAL_OP expr  */
#line 1374 "tuc_transpiler.y"
                                        { (yyval.str) = template("%s %s %s", (yyvsp[-2].str), (yyvsp[-1].str), (yyvsp[0].str)); }
#line 3067 "tuc_transpiler.tab.c"
    break;

  case 102: /* arithmetic_expr_non_empty: expr LOGICAL_AND_OR_OP expr  */
#line 1375 "tuc_transpiler.y"
                                        { (yyval.str) = template("%s %s %s", (yyvsp[-2].str), (yyvsp[-1].str), (yyvsp[0].str)); }
#line 3073 "tuc_transpiler.tab.c"
    break;

  case 103: /* arithmetic_expr_non_empty: LOGICAL_NOT_OP expr  */
#line 1376 "tuc_transpiler.y"
                                        { (yyval.str) = template("%s %s", (yyvsp[-1].str), (yyvsp[0].str)); }
#line 3079 "tuc_transpiler.tab.c"
    break;

  case 104: /* arithmetic_expr_non_empty: UNARY_OP expr  */
#line 1377 "tuc_transpiler.y"
                                        { (yyval.str) = template("%s%s", (yyvsp[-1].str), (yyvsp[0].str)); }
#line 3085 "tuc_transpiler.tab.c"
    break;

  case 105: /* @14: %empty  */
#line 1386 "tuc_transpiler.y"
{
    indentation_level++;
    (yyval.str) = template("int %s() {\n", (yyvsp[-3].str));
}
#line 3094 "tuc_transpiler.tab.c"
    break;

  case 106: /* @15: %empty  */
#line 1391 "tuc_transpiler.y"
{      
    (yyval.str) = indent_statements((yyvsp[0].str), indentation_level);
    indentation_level--;
}
#line 3103 "tuc_transpiler.tab.c"
    break;

  case 107: /* function_definition: KW_DEF KW_MAIN '(' ')' ':' @14 body @15 KW_ENDDEF  */
#line 1396 "tuc_transpiler.y"
{     
    if (func_needs_return_type != body_has_return_type) {  
      yyerror("Return statement missing from body"); 
      YYABORT;
    }
    (yyval.str) = template("%s%s}\n", (yyvsp[-3].str), (yyvsp[-1].str));
}
#line 3115 "tuc_transpiler.tab.c"
    break;

  case 108: /* @16: %empty  */
#line 1409 "tuc_transpiler.y"
{
    add_function((yyvsp[-5].str), num_of_args_in_function);  
    num_of_args_in_function = 0;
    indentation_level++;
    (yyval.str) = template("%s %s(%s){\n", (yyvsp[-1].str), (yyvsp[-5].str), (yyvsp[-3].str) );
}
#line 3126 "tuc_transpiler.tab.c"
    break;

  case 109: /* @17: %empty  */
#line 1416 "tuc_transpiler.y"
{      
    (yyval.str) = indent_statements((yyvsp[0].str), indentation_level);
    indentation_level--;
}
#line 3135 "tuc_transpiler.tab.c"
    break;

  case 110: /* function_definition: KW_DEF IDENTIFIER '(' arguments ')' return_type ':' @16 body @17 KW_ENDDEF  */
#line 1421 "tuc_transpiler.y"
{     
    if (func_needs_return_type != body_has_return_type) {  
      yyerror("Return statement missing from body"); 
      YYABORT;
    }
    (yyval.str) = template("%s%s}\n", (yyvsp[-3].str), (yyvsp[-1].str));
    func_needs_return_type = 0;
    body_has_return_type = 0;
}
#line 3149 "tuc_transpiler.tab.c"
    break;

  case 111: /* arguments: %empty  */
#line 1435 "tuc_transpiler.y"
                                            { (yyval.str) = ""; }
#line 3155 "tuc_transpiler.tab.c"
    break;

  case 112: /* arguments: parameter_definition  */
#line 1436 "tuc_transpiler.y"
                                            { num_of_args_in_function++; (yyval.str) = template("%s", (yyvsp[0].str)); }
#line 3161 "tuc_transpiler.tab.c"
    break;

  case 113: /* arguments: arguments ',' parameter_definition  */
#line 1437 "tuc_transpiler.y"
                                            { num_of_args_in_function++; (yyval.str) = template("%s, %s", (yyvsp[-2].str), (yyvsp[0].str)); }
#line 3167 "tuc_transpiler.tab.c"
    break;

  case 114: /* parameter_definition: IDENTIFIER ':' KW_VARIABLE_TYPE  */
#line 1441 "tuc_transpiler.y"
                                                                { (yyval.str) = template("%s %s", (yyvsp[0].str), (yyvsp[-2].str)); }
#line 3173 "tuc_transpiler.tab.c"
    break;

  case 115: /* parameter_definition: IDENTIFIER '[' POSINT ']' ':' KW_VARIABLE_TYPE  */
#line 1442 "tuc_transpiler.y"
                                                                { (yyval.str) = template("%s* %s[%s]", (yyvsp[0].str), (yyvsp[-5].str), (yyvsp[-3].str) ); }
#line 3179 "tuc_transpiler.tab.c"
    break;

  case 116: /* parameter_definition: IDENTIFIER '[' ']' ':' KW_VARIABLE_TYPE  */
#line 1443 "tuc_transpiler.y"
                                                                { (yyval.str) = template("%s* %s", (yyvsp[0].str), (yyvsp[-4].str) ); }
#line 3185 "tuc_transpiler.tab.c"
    break;

  case 117: /* return_type: %empty  */
#line 1447 "tuc_transpiler.y"
          { (yyval.str) = "void"; }
#line 3191 "tuc_transpiler.tab.c"
    break;

  case 118: /* return_type: KW_FUNC_RETURN KW_VARIABLE_TYPE  */
#line 1448 "tuc_transpiler.y"
                                        {   if(strcmp((yyvsp[0].str), "void") == 0) {func_needs_return_type = 0;} 
                                            else {func_needs_return_type = 1;} (yyval.str) = template("%s", (yyvsp[0].str)); }
#line 3198 "tuc_transpiler.tab.c"
    break;

  case 119: /* body: stmts  */
#line 1453 "tuc_transpiler.y"
                                     { body_has_return_type = 0; (yyval.str) = template("%s", (yyvsp[0].str)); }
#line 3204 "tuc_transpiler.tab.c"
    break;

  case 120: /* body: stmts KW_RETURN expr ';' stmts  */
#line 1454 "tuc_transpiler.y"
                                      { body_has_return_type = 1; (yyval.str) = template("%s%s %s;\n%s", (yyvsp[-4].str), (yyvsp[-3].str), (yyvsp[-2].str), (yyvsp[0].str)); }
#line 3210 "tuc_transpiler.tab.c"
    break;

  case 121: /* variable_definition: variable_list ':' KW_VARIABLE_TYPE  */
#line 1461 "tuc_transpiler.y"
   {  
        if(is_comp_type((yyvsp[0].str))){
            // create_compTypeVariables($3, $1);
            add_comp_variable_definitions((yyvsp[0].str), (yyvsp[-2].str));
            (yyval.str) = template("%s %s = ctor_%s;", (yyvsp[0].str), (yyvsp[-2].str), (yyvsp[0].str));        
        } else { (yyval.str) = template("%s %s;", (yyvsp[0].str), (yyvsp[-2].str)); }   
   }
#line 3222 "tuc_transpiler.tab.c"
    break;

  case 122: /* variable_definition: IDENTIFIER '[' POSINT ']' ':' KW_VARIABLE_TYPE  */
#line 1468 "tuc_transpiler.y"
                                                                        { (yyval.str) = template("%s %s[%s];", (yyvsp[0].str), (yyvsp[-5].str), (yyvsp[-3].str) ); }
#line 3228 "tuc_transpiler.tab.c"
    break;

  case 123: /* variable_definition: IDENTIFIER '[' ']' ':' KW_VARIABLE_TYPE  */
#line 1469 "tuc_transpiler.y"
                                                                        { (yyval.str) = template("%s[]: %s;", (yyvsp[0].str), (yyvsp[-4].str) ); }
#line 3234 "tuc_transpiler.tab.c"
    break;

  case 124: /* variable_definition: KW_CONST IDENTIFIER ASSIGNMENT_OP VALUE ':' KW_VARIABLE_TYPE  */
#line 1470 "tuc_transpiler.y"
                                                                        { (yyval.str) = template("%s %s %s = %s;", (yyvsp[-5].str), (yyvsp[0].str), (yyvsp[-4].str), (yyvsp[-2].str)); }
#line 3240 "tuc_transpiler.tab.c"
    break;

  case 129: /* KW_VARIABLE_TYPE: KW_INTEGER  */
#line 1481 "tuc_transpiler.y"
                        {(yyval.str) = (yyvsp[0].str); }
#line 3246 "tuc_transpiler.tab.c"
    break;

  case 132: /* KW_VARIABLE_TYPE: IDENTIFIER  */
#line 1484 "tuc_transpiler.y"
               {
    if (is_comp_type((yyvsp[0].str)) || strcmp((yyvsp[0].str), "void") == 0) {
        (yyval.str) = template("%s", (yyvsp[0].str));
    } else {
        yyerror("Unknown variable type");
        YYABORT;
    }
  }
#line 3259 "tuc_transpiler.tab.c"
    break;

  case 135: /* variable_list: IDENTIFIER  */
#line 1500 "tuc_transpiler.y"
                                    { (yyval.str) = template("%s", (yyvsp[0].str)); }
#line 3265 "tuc_transpiler.tab.c"
    break;

  case 136: /* variable_list: variable_list ',' IDENTIFIER  */
#line 1501 "tuc_transpiler.y"
                                    { (yyval.str) = template("%s, %s", (yyvsp[-2].str), (yyvsp[0].str)); }
#line 3271 "tuc_transpiler.tab.c"
    break;


#line 3275 "tuc_transpiler.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1506 "tuc_transpiler.y"

    void print_evaluation(char* result) {

        if (yyerror_count == 0) {
            //printf("===================================================== Expression evaluates to =====================================================\n%s\n", result);
            
            FILE *file = fopen("output.c", "w");
            if (file != NULL) {
                
                fprintf(file, "#include <stdio.h>\n");
                fprintf(file, "#include <stdlib.h>\n");  
                fprintf(file, "#include <math.h>\n");

                if(used_lambdalib_function == 1){
                    fprintf(file, "#include \"lambdalib.h\"\n");
                }
                
                fprintf(file, "%s\n", result);
                fclose(file);
            } else {
                fprintf(stderr, "Error opening output.c for writing.\n");
            }
        }
    }

//==============================================================================================================================================================

bool is_integer(const char* str) {
    if (str == NULL || *str == '\0') return false;
    if (*str == '-' || *str == '+') {
        str++;
    }
    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return false;
        }
        str++;
    }
    return true;
}

//==============================================================================================================================================================
//==============================================================================================================================================================

int main(int argc, char *argv[]) {
    // Check if the user has provided a file name as an argument
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        return 1;
    }

    // Use the file name passed as argument
    char* file_la = argv[1];
    FILE *file = fopen(file_la, "r");

    if (file == NULL) {
        perror("Could not open file");
        return 1;
    }
    char buffer[BUFFER_SIZE];
    include_functions();
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        store_line(buffer);
    }

    if (yyparse() == 0 && parse_error == 0) {
        printf("Your program is syntactically correct!\n");
    } else {
        printf("Rejected\n");
    }

    free_functions();
    free_lines();

    fclose(file);  // Close the file
    return 0;
}
