%{
    #include <stdio.h>
    #include <string.h>  
    #include "cgen.h"
    #include "tuc_transpiler.tab.h"
  
    int line_num = 1; 
    #define LINE_BUFFER_SIZE 1024
    #define MAX_MACROS 100

    char current_line[LINE_BUFFER_SIZE];
    
    //=================================================================================

    void append_to_current_line(const char *str) {

        if (strlen(current_line) + strlen(str) < LINE_BUFFER_SIZE - 1) {
            strcat(current_line, str);

        } else {
            fprintf(stderr, "Warning: Not enough space to append to current line.\n");
        }
    }

    //=================================================================================

    void clear_current_line() {
        current_line[0] = '\0';  // Setting the first character to '\0' effectively makes the string empty, 
                                 //C functions will only see an empty string ("") (for example strlen), because they stop at '\0'.
    }

    //=================================================================================

    int expecting_unary = 1; 
    int was_unary = 0; 
    int inside_def_macro = 0;

    void reverse_unary(){
        if (expecting_unary) {
            expecting_unary = 0; 
        } else {
            expecting_unary = 1;
        }
        if(was_unary){
            expecting_unary = 0;
        }
    }

  //=================================================================================
  
  typedef struct {

      char identifier[LINE_BUFFER_SIZE];
      char replacement[LINE_BUFFER_SIZE];

  } Macro;

  Macro macros[MAX_MACROS];

  int macro_count = 0;

  void add_macro(const char *id, const char *value) {

      for (int i = 0; i < macro_count; i++) {
          if (strcmp(macros[i].identifier, id) == 0) {
              strcpy(macros[i].replacement, value);  
              return;
          }
      }
      if (macro_count < MAX_MACROS) {
          strcpy(macros[macro_count].identifier, id);
          strcpy(macros[macro_count].replacement, value);
          macro_count++;
      }
  }
  //=================================================================================
  
  const char* get_macro(const char *id) {
  
      for (int i = macro_count - 1; i >= 0; i--) {
          if (strcmp(macros[i].identifier, id) == 0) {
              return macros[i].replacement;
          }
      }
      return NULL; 
  }

  //=================================================================================

%}

ID      [a-zA-Z][0-9a-zA-Z_]*    
DIGIT   [0-9]
NUMBER  {DIGIT}+
DEC     ("."{NUMBER})|({NUMBER}("."{NUMBER}?)?)
REAL_NUM    {DEC}([eE][+-]?{NUMBER})?
STRING    \"([^\"\\\n]|\\.)*\" 

EXPONENTIATION   "**"
UNARY            [+-]
CALCULATION      [+\-*\/%]
RELATIONAL       [<>]=?|==|!=|>=|<=
LOGICAL_NOT      not
LOGICAL_AND_OR   (and|or)
ASSIGNMENT       "="|"+="|"-="|"*="|"/="|"%="|":="

%%

if        { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_IF; }
else      { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_ELSE; }
endif     { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_ENDIF; }

while     { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_WHILE; }
endwhile  { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_ENDWHILE; }
for       { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_FOR; }
endfor    { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_ENDFOR; }
break     { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_BREAK; }
continue  { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_CONTINUE; }

in        { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_IN; }
of        { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_OF; }

True      { append_to_current_line(yytext); yylval.str = strdup("1"); return KW_TRUE; }
False     { append_to_current_line(yytext); yylval.str = strdup("0"); return KW_FALSE; }

var       { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_VAR; }
real      { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_REAL; }
begin     { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_BEGIN; }
func      { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_FUNC; }

integer     { append_to_current_line("int"); yylval.str = strdup("int"); return KW_INTEGER; }
scalar      { append_to_current_line("double"); yylval.str = strdup("double"); return KW_SCALAR; } 
str         { append_to_current_line("StringType"); yylval.str = strdup("StringType"); return KW_STR; } 
const       { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_CONST; }
bool        { append_to_current_line("int"); yylval.str = strdup("int"); return KW_BOOLEAN; }

def         { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_DEF; }
enddef      { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_ENDDEF; }
comp        { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_COMP; } 
endcomp     { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_ENDCOMP; } 
@defmacro   { inside_def_macro = 1; append_to_current_line(""); yylval.str = strdup(""); return KW_MACRO; } 

return      { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_RETURN; } 
main        { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_MAIN; } 

->          { append_to_current_line(yytext); yylval.str = strdup(yytext); return KW_FUNC_RETURN; } 

"("           { expecting_unary = 1; append_to_current_line("("); return '('; }
")"           { expecting_unary = 0; append_to_current_line(")"); return ')'; }
"["           { expecting_unary = 1; append_to_current_line("["); return '['; }
"]"           { expecting_unary = 0; append_to_current_line("]"); return ']'; }

{EXPONENTIATION}        { append_to_current_line(yytext); yylval.str = strdup(yytext); return EXPONENTIATION_OP; }

{CALCULATION} {
    append_to_current_line(yytext);
    yylval.str = strdup(yytext);
    if (expecting_unary) {
        expecting_unary = 0;  
        was_unary = 1;
        return UNARY_OP;
    } else {
        expecting_unary = 1;
        return CALC_OP;
    }
}


{RELATIONAL}        { expecting_unary = 1; append_to_current_line(yytext); yylval.str = strdup(yytext); return RELATIONAL_OP; }
{LOGICAL_NOT}       { append_to_current_line(yytext); yylval.str = strdup(yytext); return LOGICAL_NOT_OP; }
{LOGICAL_AND_OR}    { if(strcmp(yytext, "and") == 0) {append_to_current_line("&&"); yylval.str = strdup("&&");}
                      else {append_to_current_line("||"); yylval.str = strdup("||");} return LOGICAL_AND_OR_OP; }
{ASSIGNMENT}        { expecting_unary = 1;append_to_current_line(yytext); yylval.str = strdup(yytext); return ASSIGNMENT_OP; }

{NUMBER}       {reverse_unary(); append_to_current_line(yytext); yylval.str = strdup(yytext); return POSINT; }
{REAL_NUM}     {reverse_unary(); append_to_current_line(yytext); yylval.str = strdup(yytext); return REAL; }

{ID} {
    if(inside_def_macro == 0){
        reverse_unary();
    }
    const char *replacement = get_macro(yytext);
    if (replacement) {
        append_to_current_line(yytext);
        yylval.str = strdup(replacement);
        return IDENTIFIER;
    } else {
        append_to_current_line(yytext);
        yylval.str = strdup(yytext);
        return IDENTIFIER;
    }
}

{STRING}       { if(inside_def_macro == 0){reverse_unary();}; append_to_current_line(yytext); yylval.str = strdup(yytext); return STRING; }


";"           { expecting_unary = 1; append_to_current_line(";"); return ';'; }
","           { append_to_current_line(","); return ','; }
":"           { expecting_unary = 1; append_to_current_line(":"); return ':'; }
"#"           { append_to_current_line("#"); expecting_unary = 1; return '#'; }
"."           { append_to_current_line("."); expecting_unary = 1; return '.'; }


"--"[^\n]*    expecting_unary = 1;
"//"[^\n]*    /* eat line comments */

[ \r\t]+      { append_to_current_line(" "); }
\n            { expecting_unary = 1; inside_def_macro = 0; ++line_num; clear_current_line(); } 
<<EOF>>       return EOF;


%% 
