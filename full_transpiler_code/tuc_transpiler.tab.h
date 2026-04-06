/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_TUC_TRANSPILER_TAB_H_INCLUDED
# define YY_YY_TUC_TRANSPILER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IDENTIFIER = 258,              /* IDENTIFIER  */
    POSINT = 259,                  /* POSINT  */
    STRING = 260,                  /* STRING  */
    DECIMAL = 261,                 /* DECIMAL  */
    REAL = 262,                    /* REAL  */
    KW_IF = 263,                   /* KW_IF  */
    KW_ELSE = 264,                 /* KW_ELSE  */
    KW_ENDIF = 265,                /* KW_ENDIF  */
    KW_IN = 266,                   /* KW_IN  */
    KW_OF = 267,                   /* KW_OF  */
    KW_WHILE = 268,                /* KW_WHILE  */
    KW_ENDWHILE = 269,             /* KW_ENDWHILE  */
    KW_FOR = 270,                  /* KW_FOR  */
    KW_ENDFOR = 271,               /* KW_ENDFOR  */
    KW_BREAK = 272,                /* KW_BREAK  */
    KW_CONTINUE = 273,             /* KW_CONTINUE  */
    KW_TRUE = 274,                 /* KW_TRUE  */
    KW_FALSE = 275,                /* KW_FALSE  */
    KW_VAR = 276,                  /* KW_VAR  */
    KW_INT = 277,                  /* KW_INT  */
    KW_REAL = 278,                 /* KW_REAL  */
    KW_FUNC = 279,                 /* KW_FUNC  */
    KW_BEGIN = 280,                /* KW_BEGIN  */
    KW_DEF = 281,                  /* KW_DEF  */
    KW_ENDDEF = 282,               /* KW_ENDDEF  */
    KW_COMP = 283,                 /* KW_COMP  */
    KW_ENDCOMP = 284,              /* KW_ENDCOMP  */
    KW_MACRO = 285,                /* KW_MACRO  */
    KW_RETURN = 286,               /* KW_RETURN  */
    KW_MAIN = 287,                 /* KW_MAIN  */
    KW_FUNC_RETURN = 288,          /* KW_FUNC_RETURN  */
    KW_INTEGER = 289,              /* KW_INTEGER  */
    KW_SCALAR = 290,               /* KW_SCALAR  */
    KW_STR = 291,                  /* KW_STR  */
    KW_CONST = 292,                /* KW_CONST  */
    KW_BOOLEAN = 293,              /* KW_BOOLEAN  */
    EXPONENTIATION_OP = 294,       /* EXPONENTIATION_OP  */
    ASSIGNMENT_OP = 295,           /* ASSIGNMENT_OP  */
    CALC_OP = 296,                 /* CALC_OP  */
    UNARY_OP = 297,                /* UNARY_OP  */
    RELATIONAL_OP = 298,           /* RELATIONAL_OP  */
    LOGICAL_NOT_OP = 299,          /* LOGICAL_NOT_OP  */
    LOGICAL_AND_OR_OP = 300,       /* LOGICAL_AND_OR_OP  */
    KW_PLUS = 301,                 /* KW_PLUS  */
    KW_MINUS = 302,                /* KW_MINUS  */
    KW_MUL = 303,                  /* KW_MUL  */
    KW_DIV = 304,                  /* KW_DIV  */
    KW_MOD = 305,                  /* KW_MOD  */
    KW_LESSEQUAL = 306,            /* KW_LESSEQUAL  */
    KW_GREATEREQUAL = 307,         /* KW_GREATEREQUAL  */
    KW_LESSTHAN = 308,             /* KW_LESSTHAN  */
    KW_GREATHAN = 309,             /* KW_GREATHAN  */
    KW_INEQ = 310,                 /* KW_INEQ  */
    KW_AND = 311,                  /* KW_AND  */
    KW_OR = 312,                   /* KW_OR  */
    KW_NOT = 313,                  /* KW_NOT  */
    KW_EQUAL = 314,                /* KW_EQUAL  */
    KW_EXPONENT = 315              /* KW_EXPONENT  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 901 "tuc_transpiler.y"

  char* str;

#line 128 "tuc_transpiler.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_TUC_TRANSPILER_TAB_H_INCLUDED  */
