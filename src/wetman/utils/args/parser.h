#ifndef WETMAN_UTILS_ARGS_PARSER_H
#define WETMAN_UTILS_ARGS_PARSER_H

#include <wetman/utils/args/args.h>

#include <stdio.h>


typedef enum {
    ARGS_PARSE_STATUS_OK = 0,
    ARGS_PARSE_STATUS_UNKNOWN_OPTION,
    ARGS_PARSE_STATUS_MISSING_OPTION_VALUE,
    ARGS_PARSE_STATUS_UNEXPECTED_ARGUMENT,
    ARGS_PARSE_STATUS_MISSING_REQUIRED_ARGUMENT,
} ArgsParseStatus;

// Parses argv tokens into the runtime part of an Args spec.
// Callers build the spec (meta info), then create a parser over it.
typedef struct {
    Args*           args;    // spec being parsed, runtime info is filled in place
    ArgsParseStatus status;
    Str             token;   // offending token, empty if none
    const Arg*      arg;     // offending or missing arg, NULL if none
} ArgsParser;

void ArgsParser_Init(ArgsParser* self, Args* args);

// Fills runtime arg info from argv, starting at tokenIndex.
// Runtime info is reset before parsing, so a spec can be parsed repeatedly.
// On failure, self->status describes the error and parsing stops.
void ArgsParser_Parse(ArgsParser* self, int argc, char** argv, i32 tokenIndex);

void ArgsParser_PrintError(FILE* out, const ArgsParser* self);

#endif // WETMAN_UTILS_ARGS_PARSER_H
