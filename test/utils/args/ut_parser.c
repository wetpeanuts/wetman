#include <wetman/utils/args/parser.h>
#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/test/macro.h>


static Args __Args_NewPositionalSpec(void)
{
    Args args = { .len = 2 };

    args.args[0] = (Arg) {
        .shortForm   = Str_CreateEmpty(),
        .fullForm    = Str_CreateEmpty(),
        .required    = TRUE,
        .position    = 0,
        .value       = Str_CreateEmpty(),
        .initialized = FALSE,
    };

    args.args[1] = (Arg) {
        .shortForm   = Str_FromCStr("-w"),
        .fullForm    = Str_FromCStr("--workspace"),
        .required    = FALSE,
        .position    = ARG_POSITION_NONE,
        .value       = Str_CreateEmpty(),
        .initialized = FALSE,
    };

    return args;
}

TEST(ArgsParserTest_Positional)
{
    Args args = __Args_NewPositionalSpec();

    char* argv[] = { "wetman", "get", "5" };
    ArgsParser parser;
    ArgsParser_Init(&parser, &args);
    ArgsParser_Parse(&parser, 3, argv, 2);

    ASSERT_EQ(parser.status, ARGS_PARSE_STATUS_OK);
    EXPECT(args.args[0].initialized);
    EXPECT(Str_EqCStr(args.args[0].value, "5"));
    EXPECT(!args.args[1].initialized);
}

TEST(ArgsParserTest_OptionBeforePositional)
{
    Args args = __Args_NewPositionalSpec();

    char* argv[] = { "wetman", "get", "-w", "3", "5" };
    ArgsParser parser;
    ArgsParser_Init(&parser, &args);
    ArgsParser_Parse(&parser, 5, argv, 2);

    ASSERT_EQ(parser.status, ARGS_PARSE_STATUS_OK);
    EXPECT(Str_EqCStr(args.args[1].value, "3"));
    EXPECT(Str_EqCStr(args.args[0].value, "5"));
}

TEST(ArgsParserTest_OptionAfterPositional)
{
    Args args = __Args_NewPositionalSpec();

    char* argv[] = { "wetman", "get", "5", "--workspace", "3" };
    ArgsParser parser;
    ArgsParser_Init(&parser, &args);
    ArgsParser_Parse(&parser, 5, argv, 2);

    ASSERT_EQ(parser.status, ARGS_PARSE_STATUS_OK);
    EXPECT(Str_EqCStr(args.args[0].value, "5"));
    EXPECT(Str_EqCStr(args.args[1].value, "3"));
}

TEST(ArgsParserTest_OptionRepeatedKeepsLastValue)
{
    Args args = __Args_NewPositionalSpec();

    char* argv[] = { "wetman", "get", "5", "-w", "3", "--workspace", "4" };
    ArgsParser parser;
    ArgsParser_Init(&parser, &args);
    ArgsParser_Parse(&parser, 7, argv, 2);

    ASSERT_EQ(parser.status, ARGS_PARSE_STATUS_OK);
    EXPECT(Str_EqCStr(args.args[0].value, "5"));
    EXPECT(Str_EqCStr(args.args[1].value, "4"));
}

TEST(ArgsParserTest_PositionsDeclaredOutOfOrder)
{
    Args args = { .len = 2 };

    args.args[0] = (Arg) {
        .shortForm   = Str_CreateEmpty(),
        .fullForm    = Str_CreateEmpty(),
        .required    = TRUE,
        .position    = 1,
        .value       = Str_CreateEmpty(),
        .initialized = FALSE,
    };

    args.args[1] = (Arg) {
        .shortForm   = Str_CreateEmpty(),
        .fullForm    = Str_CreateEmpty(),
        .required    = TRUE,
        .position    = 0,
        .value       = Str_CreateEmpty(),
        .initialized = FALSE,
    };

    char* argv[] = { "wetman", "get", "first", "second" };
    ArgsParser parser;
    ArgsParser_Init(&parser, &args);
    ArgsParser_Parse(&parser, 4, argv, 2);

    ASSERT_EQ(parser.status, ARGS_PARSE_STATUS_OK);
    EXPECT(Str_EqCStr(args.args[1].value, "first"));
    EXPECT(Str_EqCStr(args.args[0].value, "second"));
}

TEST(ArgsParserTest_MissingOptionValue)
{
    Args args = __Args_NewPositionalSpec();

    char* argv[] = { "wetman", "get", "5", "-w" };
    ArgsParser parser;
    ArgsParser_Init(&parser, &args);
    ArgsParser_Parse(&parser, 4, argv, 2);

    ASSERT_EQ(parser.status, ARGS_PARSE_STATUS_MISSING_OPTION_VALUE);
    EXPECT(Str_EqCStr(parser.token, "-w"));
    EXPECT_EQ(parser.arg, (const Arg*)&args.args[1]);
}

TEST(ArgsParserTest_UnknownOption)
{
    Args args = __Args_NewPositionalSpec();

    char* argv[] = { "wetman", "get", "--bogus", "5" };
    ArgsParser parser;
    ArgsParser_Init(&parser, &args);
    ArgsParser_Parse(&parser, 4, argv, 2);

    ASSERT_EQ(parser.status, ARGS_PARSE_STATUS_UNKNOWN_OPTION);
    EXPECT(Str_EqCStr(parser.token, "--bogus"));
}

TEST(ArgsParserTest_UnexpectedArgument)
{
    Args args = __Args_NewPositionalSpec();

    char* argv[] = { "wetman", "get", "5", "6" };
    ArgsParser parser;
    ArgsParser_Init(&parser, &args);
    ArgsParser_Parse(&parser, 4, argv, 2);

    ASSERT_EQ(parser.status, ARGS_PARSE_STATUS_UNEXPECTED_ARGUMENT);
    EXPECT(Str_EqCStr(parser.token, "6"));
}

TEST(ArgsParserTest_UnexpectedArgumentWithoutSpec)
{
    Args args = { .len = 0 };

    char* argv[] = { "wetman", "list", "5" };
    ArgsParser parser;
    ArgsParser_Init(&parser, &args);
    ArgsParser_Parse(&parser, 3, argv, 2);

    ASSERT_EQ(parser.status, ARGS_PARSE_STATUS_UNEXPECTED_ARGUMENT);
    EXPECT(Str_EqCStr(parser.token, "5"));
}

TEST(ArgsParserTest_MissingRequiredPositional)
{
    Args args = __Args_NewPositionalSpec();

    char* argv[] = { "wetman", "get" };
    ArgsParser parser;
    ArgsParser_Init(&parser, &args);
    ArgsParser_Parse(&parser, 2, argv, 2);

    ASSERT_EQ(parser.status, ARGS_PARSE_STATUS_MISSING_REQUIRED_ARGUMENT);
    EXPECT_EQ(parser.arg, (const Arg*)&args.args[0]);
}

TEST(ArgsParserTest_MissingRequiredOption)
{
    Args args = __Args_NewPositionalSpec();
    args.args[1].required = TRUE;

    char* argv[] = { "wetman", "get", "5" };
    ArgsParser parser;
    ArgsParser_Init(&parser, &args);
    ArgsParser_Parse(&parser, 3, argv, 2);

    ASSERT_EQ(parser.status, ARGS_PARSE_STATUS_MISSING_REQUIRED_ARGUMENT);
    EXPECT_EQ(parser.arg, (const Arg*)&args.args[1]);
}

TEST(ArgsParserTest_ReparseResetsRuntimeInfo)
{
    Args args = __Args_NewPositionalSpec();

    char* firstArgv[] = { "wetman", "get", "5", "-w", "3" };
    ArgsParser parser;
    ArgsParser_Init(&parser, &args);
    ArgsParser_Parse(&parser, 5, firstArgv, 2);
    ASSERT_EQ(parser.status, ARGS_PARSE_STATUS_OK);
    ASSERT(Str_EqCStr(args.args[0].value, "5"));

    char* secondArgv[] = { "wetman", "get", "7" };
    ArgsParser_Parse(&parser, 3, secondArgv, 2);

    ASSERT_EQ(parser.status, ARGS_PARSE_STATUS_OK);
    EXPECT(Str_EqCStr(args.args[0].value, "7"));
    EXPECT(!args.args[1].initialized);
}
