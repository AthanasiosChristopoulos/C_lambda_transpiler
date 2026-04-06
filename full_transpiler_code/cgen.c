#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgen.h"

int parse_error = 0;
extern int line_num;
#define LINE_BUFFER_SIZE 1024

extern char current_line[LINE_BUFFER_SIZE];

#define MAX_LINES 1000

extern char *lines[MAX_LINES];
extern int current_line_index;

void ssopen(sstream *S) { S->stream = open_memstream(&S->buffer, &S->bufsize); }

char *ssvalue(sstream *S) {
  fflush(S->stream);
  return S->buffer;
}

void ssclose(sstream *S) { fclose(S->stream); }

char *replaceChar(char* const source, char toBeReplaced, char replacer) {
	for (int i = 0; i < strlen(source); ++i) {
		if (source[i] == toBeReplaced) {
			source[i] = replacer;
		}
	}
	return source;
}

char *template(const char *pat, ...) {
  sstream S;
  ssopen(&S);

  va_list arg;
  va_start(arg, pat);
  vfprintf(S.stream, pat, arg);
  va_end(arg);

  char *ret = ssvalue(&S);
  ssclose(&S);
  return ret;
}

char *template_with_indent(int level, const char *pat, ...) {
    sstream S;
    ssopen(&S);

    // Add indentation to the stream
    for (int i = 0; i < level; i++) {
        fprintf(S.stream, "  ");  // Using two spaces for each indentation level
    }

    // Format the provided pattern and arguments
    va_list arg;
    va_start(arg, pat);
    vfprintf(S.stream, pat, arg);
    va_end(arg);

    char *ret = ssvalue(&S);
    ssclose(&S);
    return ret;
}

  void yyerror(char const *pat, ...) {
    fprintf(stderr, "\n");

    va_list arg;
    if (line_num > 0 && line_num <= current_line_index) {
        fprintf(stderr, "Syntax error in line %d: %s   (%s)\n", line_num, lines[line_num - 1], pat);
    } else {
        fprintf(stderr, "Line content not found.\n");  // error happens but at which line ?
    }  

    va_start(arg, pat);
    // vfprintf(stderr, pat, arg);
    va_end(arg);

    yyerror_count++;
    parse_error = 1;
    exit(EXIT_FAILURE);
  }


int yyerror_count = 0;

const char *c_prologue =
    "#include \"lambdalib.h\"\n"
    "\n";
