%{
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
int body_has_return_type_temp = 0;


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

%}

%debug

%union
{
  char* str;
}

%token <str> IDENTIFIER POSINT STRING DECIMAL REAL
%token <str> KW_IF KW_ELSE KW_ENDIF KW_IN KW_OF 
%token <str> KW_WHILE KW_ENDWHILE KW_FOR KW_ENDFOR KW_BREAK KW_CONTINUE
%token <str> KW_TRUE KW_FALSE KW_VAR KW_INT KW_REAL KW_FUNC KW_BEGIN 
%token <str> KW_DEF KW_ENDDEF KW_COMP KW_ENDCOMP KW_MACRO
%token <str> KW_RETURN KW_MAIN KW_FUNC_RETURN
%token <str> KW_INTEGER KW_SCALAR KW_STR KW_CONST KW_BOOLEAN

%type <str> KW_VARIABLE_TYPE VALUE NUMBERS
%type <str> expr arithmetic_expr_non_empty variable_definition variable_definition_comp parameter_definition variable_list variable_list_comp declaration_comp
%type <str> function_definition body arguments return_type 
%type <str> func_call func_args 
%type <str> complicated_types function_definition_comp function_definition_section return_type_comp use_comp_variable 
%type <str> use_of_comp_variable use_comp_variable_with_period
%type <str> commands stmts else_statement step loop_thing
%type <str> macro input 

%start program


%right <str> EXPONENTIATION_OP 
%left <str> ASSIGNMENT_OP  // Lower precedence (evaluated later)
%left <str> CALC_OP        // Higher precedence (evaluated first)
%right <str> UNARY_OP             
%left <str> RELATIONAL_OP    
%right <str> LOGICAL_NOT_OP       
%left <str> LOGICAL_AND_OR_OP     

%right KW_PLUS
%right KW_MINUS
%left KW_MUL
%left KW_DIV
%left KW_MOD

%left KW_LESSEQUAL
%left KW_GREATEREQUAL
%left KW_LESSTHAN
%left KW_GREATHAN
%left KW_INEQ

%left KW_AND
%left KW_OR
%right KW_NOT

%left KW_EQUAL

%right KW_EXPONENT
%left ';' 
%precedence POSINT
%precedence IDENTIFIER
%precedence ')'
%precedence ']'
%precedence '('
%precedence '['
%right '#'

%%

//==============================================================================================================================================================
//==============================================================================================================================================================

program:
  input { print_evaluation($1); } 

input:  
    %empty                                        {$$ = "";} //@@@@@@@@@
  | input variable_definition ';'                 { $$ = template("%s%s\n", $1, $2);}
  | input expr ';'                                { assignment_made = 0;  $$ = template("%s%s;\n", $1, $2);}
  | input function_definition ';'                 { assignment_made = 0; $$ = template("%s%s\n", $1, $2);}
  | input complicated_types ';'                   { assignment_made = 0;  $$ = template("%s%s\n", $1, $2);} 
  | input commands ';'                            {  $$ = template("%s%s\n", $1, $2);} 
  | input macro                                   {  $$ = template("%s%s\n", $1, $2);} 
;

//==============================================================================================================================================================
// macros:

macro:
    KW_MACRO IDENTIFIER STRING {
        char *id = $2; 
        char *value = $3;
        add_macro(id, value);  
    }
  | KW_MACRO IDENTIFIER expr {
        char *id = $2;  
        char *value = $3; 

        add_macro(id, value);
    }
;


//==============================================================================================================================================================
// if-while-for:

commands:

    KW_IF '(' expr ')' ':' { indentation_level++; is_a_loop = 0; is_an_if = 1;} 
    stmts <str>
    {
        $$ = concatenate2(indent_statements($7, indentation_level), "}\n");
        indentation_level--;
    }
    else_statement KW_ENDIF {
        $$ = template("%s (%s) {\n%s%s\n", $1, $3, $8, $9);
        is_an_if = 0;
    }
    //==============================================================================================================================================================

    | KW_WHILE '(' expr ')' ':' {indentation_level++; is_a_loop = 1; is_an_if = 0;}
    stmts <str>
    {
        $$ = indent_statements($7, indentation_level);
        indentation_level--;
    }
    KW_ENDWHILE {
        $$ = template("%s (%s) {\n%s%s}\n", $1, $3, $8);
        is_a_loop = 0;
    }
    //==============================================================================================================================================================
    // for loop

    | KW_FOR IDENTIFIER KW_IN '[' arithmetic_expr_non_empty ':' arithmetic_expr_non_empty step ']' ':' <str>
    {
        if(for_has_step == 1){
            $$ = template("for (int %s = %s; %s < %s; %s =+ %s) {\n", $2, $5, $2, $7, $2, $8); 
        } else {
            $$ = template("for (int %s = %s; %s < %s; %s++) {\n", $2, $5, $2, $7, $2); 
        }
        indentation_level++; is_a_loop = 1; is_an_if = 0;
    }
    stmts <str>
    {
        $$ = indent_statements($12, indentation_level);
        indentation_level--;
    }
    KW_ENDFOR {

        $$ = template("%s%s}\n", $11, $13);
        is_a_loop = 0;
    }

    //==============================================================================================================================================================
    // Malloc:

    | IDENTIFIER ASSIGNMENT_OP '[' expr KW_FOR IDENTIFIER ':' POSINT ']' ':' KW_VARIABLE_TYPE  
    {
        char* newType = $11;
        $$ = template("%s* %s =(%s*)malloc(%s*sizeof(%s));\nfor (int %s = 0; %s < %s; ++%s) {\n  %s[%s] = %s;\n}\n", 
                        newType, $1,newType,$8,     newType,        $6,     $6,  $8,  $6,       $1,$6,   $4);

    }
    //==============================================================================================================================================================
    // Malloc with In Keyword: 

    | IDENTIFIER ASSIGNMENT_OP '[' expr KW_FOR IDENTIFIER ':' KW_VARIABLE_TYPE KW_IN IDENTIFIER KW_OF POSINT ']' ':' KW_VARIABLE_TYPE
    {
        char* newType = $15; 
        char* s2 = template("%s[%s_i]",$10,$10);
        char* modifiedExpr = replace_identifier_in_expr($4, $6, s2);

        $$ = template(
            "%s* %s = (%s*)malloc(%s * sizeof(%s));\nfor (%s %s_i = 0; %s_i < %s; ++%s_i) {\n   %s[%s_i] = %s;\n}\n",
            newType,$1, newType, $12, newType,            $8, $10,      $10, $12,    $10,    $1, $10, modifiedExpr
        );
    }
    ;

//==============================================================================================================================================================
// Else: 

else_statement:
  %empty                                            { $$ = ""; }
  | KW_ELSE ':' <str>                               { $$ = template("%s {\n", $1); indentation_level++;}
  stmts 
  {               
      $$ = template("%s%s}", $3, indent_statements($4, indentation_level));
      indentation_level--;
  }

step:
    %empty                              { for_has_step = 0; $$ = ""; }
  | ':' arithmetic_expr_non_empty       { for_has_step = 1; $$ = template("%s", $2);}
;


//==============================================================================================================================================================
// Statement:

stmts:
    %empty                                        { $$ = ""; }
  | stmts loop_thing ';'      { if (is_a_loop != 1 && is_an_if != 1 ) {  yyerror("This isnt a loop or if"); YYABORT;} $$ = template("%s%s;\n", $1, $2);} 

  | stmts variable_definition ';'                 { $$ = template("%s%s\n", $1, $2); }
  | stmts expr ';'                                { assignment_made = 0; $$ = template("%s%s;\n", $1, $2); }
  | stmts commands ';'                            { $$ = template("%s%s", $1, $2); } 
  | stmts KW_RETURN expr ';'                      { body_has_return_type_temp = 1; $$ = template("%s%s %s;\n", $1, $2, $3); }

loop_thing:
  KW_BREAK|KW_CONTINUE;

//==============================================================================================================================================================
//==============================================================================================================================================================
//==============================================================================================================================================================
// Comp: 

complicated_types:

  KW_COMP IDENTIFIER { comp_name = $2; inside_comp = 1; createNewComp(comp_name); indentation_level++;}
  ':' declaration_comp function_definition_section KW_ENDCOMP 
  { indentation_level--;
    char* function_definition_prints = getCompFunctionsAsString(comp_name);   
    char* function_names = getCompFunctionNamesAsString(comp_name);
    $$ = template("typedef struct %s {\n%s%s} %s;\n\n%s%s ctor_%s = {%s};\n", $2, $5, $6, comp_name, function_definition_prints, comp_name, comp_name, function_names); 
    inside_comp = 0;
  }

//==============================================================================================================================================================
// variable_declaration_comp:

declaration_comp:
    %empty                                                  { $$ = ""; }  
  | declaration_comp variable_definition_comp ';'           { $$ = template("%s  %s;\n", $1, $2); }
;

variable_definition_comp:
    variable_list_comp ':' KW_VARIABLE_TYPE                                     { if(is_comp_type($3)){  add_comp_variable_definitions($3, $1);}
                                                                                  add_comp_variables(comp_name, $1); $$ = template("%s %s", $3, $1);}

  | '#' IDENTIFIER '[' POSINT ']' ':' KW_VARIABLE_TYPE                          { if(is_comp_type($7)){add_comp_variable_definitions($7, $2);} add_comp_variable(comp_name, $2); 
                                                                                  $$ = template("%s %s[%s]", $7, $2, $4 ); }   

  | '#' IDENTIFIER '[' ']' ':' KW_VARIABLE_TYPE                                 { if(is_comp_type($6)){ add_comp_variable_definitions($6, $2);} add_comp_variable(comp_name, $2);
                                                                                  $$ = template("%s[]: %s", $2, $6 ); }

  | '#' KW_CONST IDENTIFIER ASSIGNMENT_OP VALUE ':' KW_VARIABLE_TYPE            { if(is_comp_type($7)){ add_comp_variable_definitions($7, $3);} add_comp_variable(comp_name, $3);
                                                                                  $$ = template("%s %s %s = %s", $2, $7, $3, $5); }
;

variable_list_comp:
  '#' IDENTIFIER                                    { $$ = template("%s", $2); }
  | variable_list_comp ',' '#' IDENTIFIER           { $$ = template("%s, %s", $1, $4); }
;

//==============================================================================================================================================================
// function_comp:


function_definition_section:
    %empty                                                      { $$ = ""; }  
 | function_definition_section function_definition_comp ';'     {$$ = template("%s  %s;\n", $1, $2);}
;

function_definition_comp:
  KW_DEF IDENTIFIER '(' arguments ')' return_type_comp ':' body KW_ENDDEF
  {
    if (func_needs_return_type != body_has_return_type) {  
      yyerror("Return statement missing from body"); 
      YYABORT;
    }
    $$ = template("%s (*%s)(struct %s *self%s%s)", $6, $2, comp_name, strlen($4) ? ", " : "", $4);
    func_needs_return_type = 0;
    body_has_return_type = 0;

    add_function($2, num_of_args_in_function);  
    num_of_args_in_function = 0;

    char* function_follow_comp = template("%s %s(struct %s *self%s%s) {\n%s}\n", $6, $2, comp_name,strlen($4) == 0 ? "":", " , $4, indent_statements($8, indentation_level) );
    addFunctionToComp(comp_name, function_follow_comp, $2);
  }
;

return_type_comp:
   %empty                               { $$ = "void"; } 
 | KW_FUNC_RETURN KW_VARIABLE_TYPE      { func_needs_return_type = 1; $$ = template("%s", $2); }
;

//==============================================================================================================================================================
// function call:

func_call:
    IDENTIFIER '(' {push($1);} func_args ')' 
    {  
        const DefinedFunction* func = find_function($1);
        function_name_temp = $1;

        if(find_included_function($1) == 1) {used_lambdalib_function = 1;}

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
            $$ = template("%s(%s)", $1, $4);
        }
        else if(inside_comp == 1) { $$ = template("%s(&self->%s%s%s)", $1, func_call_object, ($4 && strlen($4) != 0) ? ", " : "", $4); }
        else { $$ = template("%s(&%s%s%s)", $1, func_call_object, ($4 && strlen($4) != 0) ? ", " : "", $4); }

        set_count_to_zero(function_name); 
        pop();
    }
;

func_args:
    %empty { $$ = ""; } 
  | expr                            { increase_count(peek()); $$ = template("%s", $1); }
  | STRING                          { increase_count(peek()); $$ = template("%s", $1); }
  | func_args ',' expr              { increase_count(peek()); $$ = template("%s, %s", $1, $3); }
  | func_args ',' STRING            { increase_count(peek()); $$ = template("%s, %s", $1, $3); }
;
//==============================================================================================================================================================
// expressions:

expr:
  func_call                             { $$ = template("%s", $1);  } 
  | NUMBERS                             { $$ = $1; }
  | KW_FALSE                            { $$ = $1; }
  | KW_TRUE                             { $$ = $1; }
  | use_comp_variable                   { $$ = $1; }
  | IDENTIFIER                          { $$ = $1; }
  | IDENTIFIER '[' expr ']'             { $$ = template("%s[%s]", $1, $3); }
  | '(' expr ')'                        { $$ = template("(%s)", $2); }
  | expr CALC_OP expr                   {   if(strcmp($2, "%") == 0) {$$ = template("((int) %s %s (int) %s)", $1, $2, $3);} 
                                            else {$$ = template("%s %s %s", $1, $2, $3);}  }  
  | expr EXPONENTIATION_OP expr         { $$ = template("pow(%s, %s)", $1, $3); } 
  | expr RELATIONAL_OP expr             { $$ = template("%s %s %s", $1, $2, $3); }  
  | expr LOGICAL_AND_OR_OP expr         { $$ = template("%s %s %s", $1, $2, $3); }  
  | LOGICAL_NOT_OP expr                 { $$ = template("%s %s", $1, $2); }  
  | UNARY_OP expr                       { $$ = template("%s%s", $1, $2); }  
//====================================================================================
  | IDENTIFIER ASSIGNMENT_OP expr        
  {
      if (assignment_made == 1) {
          yyerror("Multiple assignments in one expression are not allowed");
          YYABORT; 
      }
      assignment_made = 1; 
      $$ = template("%s %s %s", $1, $2, $3); 
  }
  | IDENTIFIER '[' expr ']'  ASSIGNMENT_OP expr     
  {
      if (assignment_made == 1) {
          yyerror("Multiple assignments in one expression are not allowed");
          YYABORT; 
      }
      assignment_made = 1; 
      $$ = template("%s[%s] %s %s", $1, $3, $5, $6 ); 
  }
//====================================================================================
  | '#' IDENTIFIER ASSIGNMENT_OP expr     
  {     
      if (assignment_made == 1 || inside_comp == 0) {
          yyerror("Multiple assignments in one expression are not allowed or not in comp");
          YYABORT; 
      }
      assignment_made = 1; 
      $$ = template("self->%s %s %s", $2, $3, $4); 
  }
  | '#' IDENTIFIER '[' expr ']'  ASSIGNMENT_OP expr       
  {
      if (assignment_made == 1 || inside_comp == 0) {
          yyerror("Multiple assignments in one expression are not allowed or not in comp");
          YYABORT; 
      }
      assignment_made = 1; 
      $$ = template("self->%s[%s] %s %s", $2, $4, $6, $7 ); 
  }
//====================================================================================
// comp dots:

  | IDENTIFIER '.' {func_call_object = $1;} func_call
  {
      char* comp_name = get_comp_type_definition($1);
      if(is_comp_function(comp_name, function_name_temp) == 0){
          yyerror("Function used isnt assigned for this variable type");
          YYABORT; 
      }
      $$ = template("%s.%s", $1, $4); 
  }  
  | IDENTIFIER '[' expr ']' '.' {func_call_object = concatenate4($1,"[",$3,"]");} func_call  
  {     
      char* comp_name = get_comp_type_definition($1);
      if(is_comp_function(comp_name, function_name_temp) == 0){
          yyerror("Function used isnt assigned for this variable type");
          YYABORT; 
      }
      $$ = template("self->%s[%s].%s", $1, $3, $7); 
  }
  | '#' IDENTIFIER '.' {func_call_object = $2;} func_call  
  {     
      char* comp_name = get_comp_type_definition($2);
      if(is_comp_function(comp_name, function_name_temp) == 0){
          yyerror("Function used isnt assigned for this variable type");
          YYABORT; 
      }
      $$ = template("self->%s.%s", $2, $5); 
  }
  | '#' IDENTIFIER '[' expr ']' '.' {func_call_object = concatenate4($2,"[",$4,"]");} func_call  
  {     
      char* comp_name = get_comp_type_definition($2);
      if(is_comp_function(comp_name, function_name_temp) == 0){
          yyerror("Function used isnt assigned for this variable type");
          YYABORT; 
      }
      $$ = template("self->%s[%s].%s", $2, $4, $8); 
  }
//====================================================================================
  | '#' IDENTIFIER '.' use_of_comp_variable  
  {     
    if(validate_comp_stack($2) == 0) { yyerror("Variable used isnt assigned for this variable type"); YYABORT; }
    $$ = template("self->%s.%s", $2, $4); 
  }
  | '#' IDENTIFIER '[' expr ']' '.' use_of_comp_variable 
  {   
    if(validate_comp_stack($2) == 0) { yyerror("Variable used isnt assigned for this variable type"); YYABORT; }
    $$ = template("self->%s[%s].%s", $2, $4, $7); 
  }
  | IDENTIFIER '.' use_of_comp_variable
  {     
    if(validate_comp_stack($1) == 0) { yyerror("Variable used isnt assigned for this variable type"); YYABORT; }
    if(inside_comp == 1) {$$ = template("self->%s.%s", $1, $3); }
    else {$$ = template("%s.%s", $1, $3); }
  }  
  | IDENTIFIER '.' use_of_comp_variable ASSIGNMENT_OP expr
  {     
    if(validate_comp_stack($1) == 0) { yyerror("Variable used isnt assigned for this variable type"); YYABORT; }
    if(inside_comp == 1) {$$ = template("self->%s.%s = %s", $1, $3, $5); }
    else {$$ = template("%s.%s = %s", $1, $3, $5); }
  }  
  | '#' IDENTIFIER '[' expr ']' '.' use_of_comp_variable ASSIGNMENT_OP expr
  {
    if(validate_comp_stack($2) == 0) { yyerror("Variable used isnt assigned for this variable type"); YYABORT; }
    $$ = template("self->%s[%s].%s = %s", $2, $4, $7, $9); 
  }
;

use_of_comp_variable:
    use_comp_variable_with_period                                               {  $$ = template("%s", $1); } 
  | use_of_comp_variable '[' expr ']' '.' use_comp_variable_with_period         { $$ = template("%s[%s].%s", $1, $3, $6); }
  | use_of_comp_variable '.' use_comp_variable_with_period                      { $$ = template("%s.%s", $1, $3); }
;

use_comp_variable_with_period:
  '#' IDENTIFIER                        { push_comp($2); $$ = template("%s", $2);}
;

use_comp_variable:
  '#' IDENTIFIER                        { if (inside_comp == 0 || !(is_comp_variable(comp_name, $2))) {yyerror("Variable, not in comp"); YYABORT;} 
                                          else { $$ = template("self->%s", $2);} }
  | '#' IDENTIFIER '[' expr ']'         { if (inside_comp == 0 || !(is_comp_variable(comp_name, $2))) {yyerror("Variable, not in comp"); YYABORT;} 
                                          else { $$ = template("self->%s[%s]", $2, $4);} }
;

//==============================================================================================================================================================

arithmetic_expr_non_empty:
    POSINT                              { $$ = $1; }
  | REAL                                { $$ = $1; }
  | IDENTIFIER                          { $$ = $1; }
  | '#' IDENTIFIER                      { if(inside_comp == 0){yyerror("Not allowed outside comp");YYABORT;} $$ = template("self->%s", $2); }
  | '(' expr ')'                        { $$ = template("(%s)", $2); }
  | expr EXPONENTIATION_OP expr         { $$ = template("pow(%s, %s)", $1, $3); }
  | expr CALC_OP expr                   {   if(strcmp($2, "%") == 0) {$$ = template("(int) %s %s (int) %s", $1, $2, $3);} 
                                            else {$$ = template("%s %s %s", $1, $2, $3);}  }  
  | expr RELATIONAL_OP expr             { $$ = template("%s %s %s", $1, $2, $3); }
  | expr LOGICAL_AND_OR_OP expr         { $$ = template("%s %s %s", $1, $2, $3); }
  | LOGICAL_NOT_OP expr                 { $$ = template("%s %s", $1, $2); }
  | UNARY_OP expr                       { $$ = template("%s%s", $1, $2); }
;

//==============================================================================================================================================================
// Function Definition Main:

function_definition:

  KW_DEF KW_MAIN'('')' ':' <str> 
{
    indentation_level++;
    $$ = template("int %s() {\n", $2);
}
  body <str>
{      
    $$ = indent_statements($7, indentation_level);
    indentation_level--;
}
  KW_ENDDEF
{     
    if (func_needs_return_type != body_has_return_type) {  
      yyerror("Return statement missing from body"); 
      YYABORT;
    }
    $$ = template("%s%s}\n", $6, $8);
}
;

//============================================================================================================================================
// function_definition:

| KW_DEF IDENTIFIER'(' arguments ')' return_type ':' <str>
{
    add_function($2, num_of_args_in_function);  
    num_of_args_in_function = 0;
    indentation_level++;
    $$ = template("%s %s(%s){\n", $6, $2, $4 );
}
  body <str>
{      
    $$ = indent_statements($9, indentation_level);
    indentation_level--;
}
  KW_ENDDEF
{     
    if (func_needs_return_type != body_has_return_type) {  
      yyerror("Return statement missing from body"); 
      YYABORT;
    }
    $$ = template("%s%s}\n", $8, $10);
    func_needs_return_type = 0;
    body_has_return_type = 0;
}
;

// Arguments of a function:

arguments: 
    %empty                                  { $$ = ""; } 
  | parameter_definition                    { num_of_args_in_function++; $$ = template("%s", $1); }             // int n, int k
  | arguments ',' parameter_definition      { num_of_args_in_function++; $$ = template("%s, %s", $1, $3); }
;

parameter_definition:  
   IDENTIFIER ':' KW_VARIABLE_TYPE                              { $$ = template("%s %s", $3, $1); }            
 | IDENTIFIER '[' POSINT ']' ':' KW_VARIABLE_TYPE               { $$ = template("%s* %s[%s]", $6, $1, $3 ); }   
 | IDENTIFIER '[' ']' ':' KW_VARIABLE_TYPE                      { $$ = template("%s* %s", $5, $1 ); }
; 

return_type:
   %empty { $$ = "void"; } 
 | KW_FUNC_RETURN KW_VARIABLE_TYPE      {   if(strcmp($2, "void") == 0) {func_needs_return_type = 0;} 
                                            else {func_needs_return_type = 1;} $$ = template("%s", $2); }
;

// body: 
//   stmts                     { body_has_return_type = 0; $$ = template("%s", $1); }
// | stmts KW_RETURN expr ';'  { body_has_return_type = 1; $$ = template("%s%s %s;\n", $1, $2, $3); }
// ;
body: 
    stmts                            {  if(body_has_return_type_temp == 1) {body_has_return_type = 1;} 
                                        else {body_has_return_type = 0;} body_has_return_type_temp = 0; $$ = template("%s", $1); }
  ;
//==============================================================================================================================================================
// Variable Definition: 

variable_definition:  
   variable_list ':' KW_VARIABLE_TYPE          // a, b: integer; => int a, b;            
   {  
        if(is_comp_type($3)){
            // create_compTypeVariables($3, $1);
            add_comp_variable_definitions($3, $1);
            $$ = template("%s %s = ctor_%s;", $3, $1, $3);        
        } else { $$ = template("%s %s;", $3, $1); }   
   }             
 | IDENTIFIER '[' POSINT ']' ':' KW_VARIABLE_TYPE                       { $$ = template("%s %s[%s];", $6, $1, $3 ); }       // grades[5]: scalar; => double grades[5];
 | IDENTIFIER '[' ']' ':' KW_VARIABLE_TYPE                              { $$ = template("%s[]: %s;", $5, $1 ); }
 | KW_CONST IDENTIFIER ASSIGNMENT_OP VALUE ':' KW_VARIABLE_TYPE         { $$ = template("%s %s %s = %s;", $1, $6, $2, $4); }
; 

VALUE:
    POSINT 
  | REAL
  | STRING
;

KW_VARIABLE_TYPE:
    KW_SCALAR 
  | KW_INTEGER          {$$ = $1; }
  | KW_STR 
  | KW_BOOLEAN
  | IDENTIFIER {
    if (is_comp_type($1) || strcmp($1, "void") == 0) {
        $$ = template("%s", $1);
    } else {
        yyerror("Unknown variable type");
        YYABORT;
    }
  }
;

NUMBERS:
    POSINT
  | REAL
;

variable_list:
    IDENTIFIER                      { $$ = template("%s", $1); }
  | variable_list ',' IDENTIFIER    { $$ = template("%s, %s", $1, $3); }
;

//==============================================================================================================================================================
//==============================================================================================================================================================
%%
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