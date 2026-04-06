%{
  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include <ctype.h>

  int line_num = 1;
  #define LINE_BUFFER_SIZE 1024
  #define MAX_MACROS 100

  //=================================================================================
  
  void print_token(const char *token_type, const char *value) {
      printf("%-18s %-30s\n", token_type, value);
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

ID          [a-zA-Z][0-9a-zA-Z_]*    
DIGIT       [0-9]
NUMBER      {DIGIT}+
DEC         ("."{NUMBER})|({NUMBER}("."{NUMBER}?)?)
REAL_NUM    {DEC}([eE][+-]?{NUMBER})?
STRING      \"([^\"\\\n]|\\.)*\"

EXPONENTIATION   "**"
UNARY            [+-]
CALCULATION      [+\-*\/%]
RELATIONAL       [<>]=?|==|!=
LOGICAL_NOT      not
LOGICAL_AND_OR   (and|or)
ASSIGNMENT       "="|"+="|"-="|"*="|"/="|"%="|":="

%%

@defmacro[ \t]+{ID}[ \t]+(.+) {

    char id[LINE_BUFFER_SIZE], value[LINE_BUFFER_SIZE];

    sscanf(yytext, "@defmacro %s %[^\n]", id, value); // %s => id, %[^\n] (this means everything up to a line break) => value 
  
    add_macro(id, value);  
  
}

if        { print_token("KEYWORD_IF:", yytext); }
then      { print_token("KEYWORD_THEN:", yytext); }
else      { print_token("KEYWORD_ELSE:", yytext); }
endif     { print_token("KEYWORD_ENDIF:", yytext); }

while     { print_token("KEYWORD_WHILE:", yytext); }
endwhile  { print_token("KEYWORD_ENDWHILE:", yytext); }
for       { print_token("KEYWORD_FOR:", yytext); }
endfor    { print_token("KEYWORD_ENDFOR:", yytext); }
break     { print_token("KEYWORD_BREAK:", yytext); }
continue  { print_token("KEYWORD_CONTINUE:", yytext); }

in        { print_token("KEYWORD_IN:", yytext); }
of        { print_token("KEYWORD_OF:", yytext); }

true      { print_token("KEYWORD_TRUE:", yytext); }
false     { print_token("KEYWORD_FALSE:", yytext); }

var       { print_token("KEYWORD_VAR:", yytext); }
real      { print_token("KEYWORD_REAL:", yytext); }
begin     { print_token("KEYWORD_BEGIN:", yytext); }
func      { print_token("KEYWORD_FUNC:", yytext); }

integer   { print_token("KEYWORD_INTEGER:", "integer"); }
scalar    { print_token("KEYWORD_SCALAR:", "scalar"); }
str       { print_token("KEYWORD_STR:", yytext); }
const     { print_token("KEYWORD_CONST:", yytext); }
bool      { print_token("KEYWORD_BOOLEAN:", yytext); }

def       { print_token("KEYWORD_DEF:", yytext); }
enddef    { print_token("KEYWORD_ENDDEF:", yytext); }
comp      { print_token("KEYWORD_COMP:", yytext); }
endcomp   { print_token("KEYWORD_ENDCOMP:", yytext); }

return    { print_token("KEYWORD_RETURN:", yytext); }
main      { print_token("KEYWORD_MAIN:", yytext); }

->                    { print_token("KEYWORD_FUNC_RETURN:", yytext); }

"("                   { print_token("SYMBOL_LPAREN:", "("); }
")"                   { print_token("SYMBOL_RPAREN:", ")"); }
"["                   { print_token("SYMBOL_LBRACK:", "["); }
"]"                   { print_token("SYMBOL_RBRACK:", "]"); }

{EXPONENTIATION}      { print_token("EXPONENTIATION_OP:", yytext); }
{CALCULATION}         { print_token("CALC_OP:", yytext); }
{UNARY}               { print_token("UNARY_OP:", yytext); }
{RELATIONAL}          { print_token("RELATIONAL_OP:", yytext); }
{LOGICAL_NOT}         { print_token("LOGICAL_NOT_OP:", yytext); }
{LOGICAL_AND_OR}      { print_token("LOGICAL_AND_OR_OP:", yytext); }
{ASSIGNMENT}          { print_token("ASSIGNMENT_OP:", yytext); }

{NUMBER}       { print_token("POSINT:", yytext); }
{REAL_NUM}     { print_token("REAL:", yytext); }

{ID} {
    const char *replacement = get_macro(yytext);
    if (replacement) {
        print_token("CONST_STRING:", replacement);
    } else {
        print_token("IDENTIFIER:", yytext);
    }
}

{STRING}       { print_token("STRING:", yytext); }

";"           { print_token("SYMBOL_SEMICOLON:", ";"); }
","           { print_token("SYMBOL_COMMA:", ","); }
":"           { print_token("SYMBOL_COLON:", ":"); }
"#"           { print_token("SYMBOL_HASH:", "#"); }


"--"[^\n]*    /* eat */
"//"[^\n]*    /* eat */

[ \r\t]+      /* eat */

\n            { ++line_num; }
<<EOF>>       exit(0);

.             { printf("Rejected\n"); exit(EXIT_FAILURE); }

%%
