# C Lambda Transpiler

A university/compiler-construction project that implements a small **Lambda-inspired language** using **Flex** and **Bison**, with support for:

- **Lexical analysis** (tokenization)
- **Parsing / syntax analysis**
- **Transpilation to C99**
- **Compilation and execution** of the generated C output

The project can be run on **Linux** or on **Windows via WSL**.

---

## How to Run

### 0) Prerequisite dependencies

Install the required tools inside Linux / WSL:

```bash
sudo apt update
sudo apt install -y build-essential flex bison libfl-dev dos2unix
```

These provide:

- `flex` → lexer generation
- `bison` → parser generation
- `gcc` / build tools → compilation
- `libfl-dev` → Flex runtime library (`-lfl`)
- `dos2unix` → useful for fixing Windows line endings in shell scripts

---

### 1) Run environment

You can run the project either:

- directly on **Linux**
- or on **Windows using WSL**

---

### 2) Go to the script directory

```bash
cd bash
```

---

### 3) Choose what you want to run

#### Lexer only
Runs lexical analysis in more detail.

```bash
./run_lexer.sh
```

#### Full transpiler
Parses the source program and transpiles it into **C99**.

```bash
./run_transpiler.sh
```

---

### 4) Choose the `.la` input file

Select the input file by editing the `INPUT_FILE` variable inside the corresponding shell script.

#### For lexer mode
Edit:

```bash
bash/run_lexer.sh
```

Example:

```bash
INPUT_FILE="test_lexer.la"
```

The selected file should exist inside:

```text
only_lexer_code/
```

#### For transpiler mode
Edit:

```bash
bash/run_transpiler.sh
```

Example:

```bash
INPUT_FILE="correct2.la"
```

The selected file should exist inside:

```text
full_transpiler_code/
```

> The input file must be written in the **Lambda language** used by this project.

---

### 5) Output behavior

#### If you run the lexer
The program performs lexical analysis and prints the tokenized output.

#### If you run the transpiler
The project will:

1. generate the parser and lexer
2. compile the transpiler
3. run the transpiler on the selected `.la` file
4. generate an `output.c` file
5. compile and execute the generated C program (unless skipped)

The generated C file will be created in:

```text
full_transpiler_code/output.c
```

---

## Known Issues

### Windows + WSL line-ending issue

If the shell scripts were edited on Windows, they may contain **CRLF line endings**, which can cause this error:

```bash
cannot execute: required file not found
```

Fix it with:

```bash
sed -i 's/\r$//' bash/*.sh
chmod +x bash/*.sh
```

Then run normally:

```bash
cd bash
./run_transpiler.sh
```

---


# Lambda Transpiler — Notes and Theory

This document summarizes the most useful theory and implementation notes behind the project, especially around **Flex**, **Bison**, parsing conflicts, token precedence, and a few C parsing helpers used inside the transpiler.

---

## 1) Useful Pattern Matching Notes

Some parsing helpers in C use `sscanf()` patterns to extract parts of strings such as function declarations.

### `%*s` — Skip the first word

Example:

```c
sscanf("int readInteger()", "%*s %99[^ (]", name);
```

### Meaning
- `%s` matches a single word
- `*` tells `sscanf()` to **read it but ignore it**

### Why it is useful
This is useful when the first token is a type and you only want the function name.

#### Example
Input:

```c
int readInteger()
```

Pattern:

```c
%*s
```

Effect:
- reads `"int"`
- ignores it

---

### `%99[^ (]` — Capture a function name

### Meaning
- `%99[...]` means: capture up to 99 characters
- `[^ (]` means: accept everything **except**
  - a space
  - `(`

### Why it is useful
This extracts a function name without including return types or argument parentheses.

#### Example
Input:

```c
int readInteger()
```

Pattern:

```c
%99[^ (]
```

Result:

```c
readInteger
```

---

## 2) Lexer Notes (Flex)

The lexer is responsible for reading source code and converting it into **tokens** that the parser can understand.

---

### Macro spacing pattern

Example rule:

```lex
@defmacro[ \t]+{ID}[ \t]+{STRING}
```

### Meaning
- `[ \t]+` means:
  - one or more spaces
  - or tab characters

This is useful when matching language constructs that require spacing between parts.

---

## 3) Handling Unary Operators

One important lexer detail in this project is distinguishing:

- **unary minus** → `-5`
- **binary subtraction** → `4 - 5`

To support that, the lexer uses:

```c
int expecting_unary = 1;
```

### Purpose
This variable tracks whether the next operator should be interpreted as a **unary operator** or a normal arithmetic operator.

---

### Rules for `expecting_unary`

#### `expecting_unary = 1`
Use unary interpretation when:

1. At the **start of an expression**
2. After symbols such as:
   - `(`
   - `[`
   - `=`
   - similar “expression-starting” tokens

#### `expecting_unary = 0`
Use binary/operator interpretation when the previous token is something that can end an expression, such as:

- a number
- an identifier / variable
- `)`
- `]`

### Example

#### Unary case
```text
-5
```

The `-` should be treated as **unary**.

#### Binary case
```text
4 - 5
```

The `-` should be treated as **subtraction**.

This is a common lexer design trick when the same symbol can represent two different grammatical meanings.

---

## 4) Bison Notes (Parser Theory)

Bison is responsible for taking the token stream from Flex and checking whether it matches the grammar of the language.

---

## 5) `yylval` and Token Values

Example:

```c
yylval.str = strdup(yytext);
```

or

```c
yylval.str = strdup("int");
```

### Meaning
`yylval` is the value that the lexer passes to Bison together with a token.

### Why it matters
It allows tokens to carry actual string content, such as:

- identifiers
- numbers
- keywords
- operators

So the parser can use or print the exact matched value.

---

## 6) `extern` Variables

Example:

```c
extern int line_num;
```

### Meaning
`extern` means the variable is **defined in another file**, but can be used here.

### Why it matters
This is useful when the lexer and parser share state, such as:

- current line number
- current token information
- parser context flags

---

## 7) Tokens vs Non-Terminals in Bison

This is one of the most important parser concepts.

---

### Tokens (terminals)

Tokens are the actual units returned by the lexer.

Examples:
- `IDENTIFIER`
- `POSINT`
- `REAL`
- `CALC_OP`

These correspond to actual text read from the input.

---

### Non-terminals

Non-terminals are grammar symbols used by Bison to describe **structures** in the language.

Example:

```bison
expr:
    POSINT
  | REAL
  | expr '+' expr
;
```

Here:

- `POSINT` and `REAL` are **tokens**
- `expr` is a **non-terminal**

### Key idea
A non-terminal is not a literal character or word in the input.

It is a **grammar concept**, such as:

- expression
- statement
- function definition
- variable declaration

---

## 8) Debugging Grammar Conflicts

To inspect parser conflicts, use:

```bash
bison -d -v tuc_transpiler.y
```

This generates a verbose parser report, which is extremely useful for debugging grammar ambiguity.

---

## 9) Shift / Reduce Conflicts

A **shift/reduce conflict** happens when Bison is unsure whether it should:

- **shift** → read more input
- **reduce** → apply a grammar rule now

### Bison's default behavior
Bison usually resolves this by **shifting**.

### Why this happens
It occurs when the grammar allows more than one valid interpretation at a given point.

### Important intuition
Bison does **not** reduce everything immediately.  
It waits when continuing to read input might lead to a better match.

---

### Example idea

Suppose the parser has seen:

```text
a + b * c
```

Possible parser actions:

```text
Action                  Stack Content
Shift a                 ID
Shift +                 ID +
Shift b                 ID + ID
Shift *                 ID + ID *
Shift c                 ID + ID * ID
Reduce ID -> expr (c)   ID + ID * expr
Reduce expr * expr      ID + expr
Reduce expr + expr      expr
```

### Why this matters
The parser must delay reduction long enough to preserve operator precedence correctly.

---

## 10) Reduce / Reduce Conflicts

A **reduce/reduce conflict** happens when Bison sees **two or more grammar rules** that could both be reduced at the same point.

### Why this is worse
This usually means the grammar is structurally ambiguous and needs cleanup.

### Example idea
If Bison sees:

```text
IDENTIFIER ,
```

it may not know whether to reduce `IDENTIFIER` as:

- an expression
- a function argument
- some other grammar category

---

## 11) Reading Bison Conflict Output

Example conflict snippet:

```text
5 expr: POSINT •
19 variable_definition: IDENTIFIER '[' POSINT • ']' ':' KW_VARIABLE_TYPE
```

### What the rule numbers mean
The number before the rule (such as `5` or `19`) is the internal rule number assigned by Bison based on the order of grammar definitions.

---

### What the dot (`•`) means
The dot shows the parser’s **current position** in the rule.

Example:

```text
expr: POSINT •
```

means:
- the parser has fully read `POSINT`
- it is deciding whether to reduce now or wait

---

### Example conflict interpretation

```text
19 variable_definition: IDENTIFIER '[' POSINT • ']' ':' KW_VARIABLE_TYPE
```

At this point, Bison may have a choice:

#### Option 1 — Shift
Continue reading:

```text
]
```

#### Option 2 — Reduce
Reduce:

```text
POSINT -> expr
```

This is exactly where precedence rules may be needed.

---

## 12) Rule Order vs Precedence

A very common misconception:

### Important
The **order of grammar rules** in Bison does **not** define arithmetic precedence.

For example:

```bison
expr:
    POSINT   { printf("Number: %s\n", $1); }
  | REAL     { printf("Real: %s\n", $1); }
;
```

This does **not** mean `POSINT` has higher precedence than `REAL`.

### What rule order actually affects
Rule order mainly affects:

- internal numbering
- some conflict resolution edge cases
- readability / organization

But **operator precedence** should be defined explicitly.

---

## 13) Precedence and Associativity

Bison allows you to define precedence and associativity to resolve grammar ambiguity.

---

### Precedence

Precedence determines **which operator binds more strongly**.

Example:

```bison
%left '<' '>' '=' "!=" "<=" ">="
%left '+' '-'
%left '*' '/'
```

### Meaning
From top to bottom:

- comparison operators = lowest priority
- `+` and `-` = medium priority
- `*` and `/` = highest priority

So:

```text
2 + 3 * 4
```

is parsed as:

```text
2 + (3 * 4)
```

not:

```text
(2 + 3) * 4
```

---

### Associativity

Associativity determines how operators of the **same precedence** group together.

---

#### Left associativity

```bison
%left '+' '-'
```

Means:

```text
x - y - z
```

becomes:

```text
(x - y) - z
```

---

#### Right associativity

```bison
%right '='
```

Means:

```text
a = b = c
```

becomes:

```text
a = (b = c)
```

---

### `%nonassoc`

Example:

```bison
%nonassoc SOME_TOKEN
```

### Meaning
This declares precedence **without allowing chaining**.

Useful when expressions like this should be illegal:

```text
a < b < c
```

if the grammar should not interpret that as a valid chained expression.

---

### `%precedence`

Example:

```bison
%precedence ']'
%precedence POSINT
```

### Meaning
This assigns precedence **without associativity**.

### Important detail
In Bison, declarations that appear **later** have **higher precedence**.

So in:

```bison
%precedence ']'
%precedence POSINT
```

`POSINT` has **higher precedence** than `]`.

This can be used to manually resolve specific grammar conflicts.

---

## 14) Lexer Rule Priority Also Matters

Some priority behavior is not only controlled by Bison — it also depends on how the lexer is written.

Example:

```lex
{CALCULATION} {
    append_to_current_line(yytext);
    yylval.str = strdup(yytext);
    return CALC_OP;
}
```

```lex
{UNARY} {
    append_to_current_line(yytext);
    yylval.str = strdup(yytext);
    return UNARY_OP;
}
```

### Important lexer rule
In Flex, when multiple rules can match:

1. the **longest match** wins
2. if the match length is equal, the **earlier rule in the file** wins

This means lexer rule ordering can directly affect how your grammar behaves later in Bison.

---

## 15) Practical Takeaways

If you are debugging this project, the most important ideas are:

- **Flex** decides how raw text becomes tokens
- **Bison** decides how tokens form valid language structures
- **Unary vs binary operators** often need explicit lexer state
- **Shift/reduce conflicts** are common and not always fatal
- **Reduce/reduce conflicts** usually mean the grammar needs redesign
- **Operator precedence** should be defined explicitly
- **Lexer rule ordering** can influence parser behavior
- `bison -v` is one of the most useful debugging tools for grammar development

---

## 16) Recommended Debug Commands

### Generate parser with verbose conflict report

```bash
bison -d -v full_transpiler_code/tuc_transpiler.y
```

### Generate lexer manually

```bash
flex -o generated/lex.yy.c full_transpiler_code/lexer.l
```

### Compile manually

```bash
gcc -o generated/mycompiler generated/lex.yy.c generated/tuc_transpiler.tab.c full_transpiler_code/cgen.c -Ifull_transpiler_code -lfl
```

---

This file is intended as a **cleaned-up reference / theory companion** for the project README and development notes.
