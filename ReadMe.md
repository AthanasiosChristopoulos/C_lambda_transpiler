How to run:

0) Prerequisite dependencies:
```bash
sudo apt install flex 
sudo apt install bison
sudo apt install dos2unix
```
1) Either execute on linux or via windows wsl
2) Go to /bash/
3) choose whichever program you want to run:
    Choices:
        ./run_lexer.sh => only lexical analysis, in more detail
        ./run_transpiler.sh => transpile into C99 as well


## Known Issues: 

When on windows + wsl, you need to fix the with CRLF line terminators
```bash
sed -i 's/\r$//' bash/*.sh
chmod +x bash/*.sh
# Then execute:
cd bash
./run_transpiler.sh
```




	
#=====================================================================================================================================================
#=====================================================================================================================================================

Pattern Matches:

	%*s => Skip the First Word (for example in: int readInteger(), skip "int")
		Explaination: %s matches a single word, but * tells sscanf, to ignore this word
	%99[^ (] => Capture Function Name
		Explaination: %99[ ... ] => Custom character set matching (up to 99 characters), [^ (] => match everything, except (^) space and '('

#=====================================================================================================================================================
#=====================================================================================================================================================

Lexer Documentation:

	@defmacro[ \t]+{ID}[ \t]+{STRING}  => [ \t] means one or more spaces or tab characters (since [] and [ \t] (has space and \t))

	int expecting_unary = 1; => Indicates whether the next operator should be treated as unary (-5 (unary) vs. 4 - 5 (operator)).
	Rules for setting expecting_unary:
		0) At the start, we are by fault: expecting_unary = 1
		1) At the start of an expression or after certain symbols {(, [, =, etc.}, expecting_unary = 1.
		2) After a number, variable or closing bracket )/], expecting_unary = 0.
		3) After handling the unary case, expecting_unary is reset accordingly.


#=====================================================================================================================================================
#=====================================================================================================================================================

Bison Documentation:

	yylval.str = strdup(yytext); and  yylval.str = strdup("int");  => String content that lexer returns and will actually be printed

	extern int line_num; => extern means function or variable is defined in an external file (for example in the lexer)

	Bison declarations:
		Type (also called non-terminals): expr, is like a flag/variable that will get pattern, not an actuall character.
			  							  Needs to be defined in the "Grammar Rules", as expr: ...
		
		Token (also called terminals): the variable for actuall characters, read and returned by lexer. You can apply priority operators to them 

	Errors:
		
		bison -d -v tuc_transpiler.y (Verbose output to find the conflicts)

		shift/reduce:
			Bison had to choose between shifting (reading more input) or reducing (applying a grammar rule), 
			and it resolved these conflicts automatically by defaulting to shifting. 
			Shift/reduce conflicts occur when the parser has multiple valid options. Bison does not reduce everything at once
		
		reduce/reduce:
			Bison found two or more possible reductions (grammar rules that could apply), but it couldn't decide which one to use. 
			(We can also have possible shifts). When it sees IDENTIFIER ',', should it reduce IDENTIFIER as func_args or continue parsing it as expr?

		Action                  Stack Content
		Shift a                 ID
		Shift +                 ID +
		Shift b                 ID + ID
		Shift *                 ID + ID *
		Shift c                 ID + ID * ID
		Reduce ID -> expr (c)   ID + ID * expr
		Reduce expr * expr      ID + expr
		Reduce expr + expr      expr

		5 expr: POSINT • => (5 refers to rule 5 in the grammar ... Instead, these numbers are automatically assigned by Bison when it processes the rules in order. 
							 When Bison generates the parser, it assigns numbers to grammar rules in the order they appear.)
						 => The dot (•) represents the current position of the parser in processing the rule, essentially where the conflict arose
		19 variable_definition: IDENTIFIER '[' POSINT • ']' ':' KW_VARIABLE_TYPE
		Choice:
			Option1: ']'  shift, and go to state 47
			Option2: ']'       [reduce using rule 5 (expr)]
				=> The lookahead token is ']', and that's exactly where the conflict happens.
		$default  reduce using rule 5 (expr)

	Priority:
		expr:
			POSINT   { printf("Number: %s\n", $1); }
		| REAL       { printf("Real: %s\n", $1); }"

		Order of Rules: In Bison, the order of the rules does not establish precedence or priority between different types (like POSINT or REAL). 
		Instead, it determines the order in which they are considered during parsing. The one that gets chosen is the highest that can be matched

		priority is also established by Lexer:
		{CALCULATION} { append_to_current_line(yytext); yylval.str = strdup(yytext); return CALC_OP; }, higher priority
		{UNARY} { append_to_current_line(yytext); yylval.str = strdup(yytext); return UNARY_OP; }

		Precedence:

			%precedence ']'		(Lowest Priority)
			%precedence POSINT	(Highest Priority)
			Since %precedence POSINT is declared last, it has a higher precedence than ']'.

			Declares the precedence level of a token without specifying associativity.

		%nonassoc (Non-Associative Precedence):

			Declares the precedence level of a token and prevents it from being used associatively.

	Left, Right: (οριζουν προτεραιοτητα και προσεταιριστικοτητα (εχει νοημα στις πραξεις))

		προσεταιριστικοτητα (associativity): 
		Left => (x+y)+z
		Right => x+(y+z)

		προτεραιοτητα:
		%left '<' '>' '=' "!=" "<=" ">="  	(Lowest Priority)
		%left '+' '-'
		%left '*' '/' 						(Highest Priority)