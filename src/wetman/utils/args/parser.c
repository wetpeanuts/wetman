#include <wetman/utils/args/parser.h>

#include <wetman/utils/data_struct/str.h>

#include <stdio.h>


static Arg* __ArgsParser_FindOption(Args* args, const char* token)
{
    for (u32 i = 0; i < args->len; ++i) {
        Arg* arg = &args->args[i];
        if (arg->position != ARG_POSITION_NONE) {
            continue;
        }
        if (Str_EqCStr(arg->shortForm, token) ||
                Str_EqCStr(arg->fullForm, token)) {
            return arg;
        }
    }
    return NULL;
}

static Arg* __ArgsParser_FindPosition(Args* args, i32 position)
{
    for (u32 i = 0; i < args->len; ++i) {
        Arg* arg = &args->args[i];
        if (arg->position == position) {
            return arg;
        }
    }
    return NULL;
}

void ArgsParser_Init(ArgsParser* self, Args* args)
{
    self->args   = args;
    self->status = ARGS_PARSE_STATUS_OK;
    self->token  = Str_CreateEmpty();
    self->arg    = NULL;
}

void ArgsParser_Parse(ArgsParser* self, int argc, char** argv, i32 tokenIndex)
{
    Args* args = self->args;

    self->status = ARGS_PARSE_STATUS_OK;
    self->token  = Str_CreateEmpty();
    self->arg    = NULL;

    Args_Reset(args);

    i32 nextPosition = 0;

    for (i32 i = tokenIndex; i < argc; ++i) {
        const char* token = argv[i];

        Arg* option = __ArgsParser_FindOption(args, token);
        if (option != NULL) {
            if (i + 1 >= argc) {
                self->status = ARGS_PARSE_STATUS_MISSING_OPTION_VALUE;
                self->token  = Str_FromCStr(token);
                self->arg    = option;
                return;
            }

            option->value       = Str_FromCStr(argv[i + 1]);
            option->initialized = TRUE;
            i += 1;
            continue;
        }

        if (token[0] == '-') {
            self->status = ARGS_PARSE_STATUS_UNKNOWN_OPTION;
            self->token  = Str_FromCStr(token);
            return;
        }

        Arg* positional = __ArgsParser_FindPosition(args, nextPosition);
        if (positional == NULL) {
            self->status = ARGS_PARSE_STATUS_UNEXPECTED_ARGUMENT;
            self->token  = Str_FromCStr(token);
            return;
        }

        positional->value       = Str_FromCStr(token);
        positional->initialized = TRUE;
        nextPosition += 1;
    }

    for (u32 i = 0; i < args->len; ++i) {
        Arg* arg = &args->args[i];
        if (arg->required && !arg->initialized) {
            self->status = ARGS_PARSE_STATUS_MISSING_REQUIRED_ARGUMENT;
            self->arg    = arg;
            return;
        }
    }
}

void ArgsParser_PrintError(FILE* out, const ArgsParser* self)
{
    switch (self->status) {
        case ARGS_PARSE_STATUS_UNKNOWN_OPTION:
            fprintf(out, "Unknown option: %.*s\n",
                    (int)self->token.len, self->token.data);
            break;
        case ARGS_PARSE_STATUS_MISSING_OPTION_VALUE:
            fprintf(out, "Missing value for option: %.*s\n",
                    (int)self->token.len, self->token.data);
            break;
        case ARGS_PARSE_STATUS_UNEXPECTED_ARGUMENT:
            fprintf(out, "Unexpected argument: %.*s\n",
                    (int)self->token.len, self->token.data);
            break;
        case ARGS_PARSE_STATUS_MISSING_REQUIRED_ARGUMENT:
            if (self->arg == NULL) {
                break;
            }
            if (self->arg->position != ARG_POSITION_NONE) {
                fprintf(out, "Missing required argument\n");
                break;
            }
            Str name = self->arg->fullForm.len > 0
                     ? self->arg->fullForm
                     : self->arg->shortForm;
            fprintf(out, "Missing required option: %.*s\n",
                    (int)name.len, name.data);
            break;
        case ARGS_PARSE_STATUS_OK:
            break;
    }
}
