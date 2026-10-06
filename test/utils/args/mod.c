#include <wetman/utils/test/macro.h>

#include "ut_parser.c"

void registerUtilArgsTests(void)
{
    REGISTER_TEST(ArgsParserTest_Positional);
    REGISTER_TEST(ArgsParserTest_OptionBeforePositional);
    REGISTER_TEST(ArgsParserTest_OptionAfterPositional);
    REGISTER_TEST(ArgsParserTest_OptionRepeatedKeepsLastValue);
    REGISTER_TEST(ArgsParserTest_PositionsDeclaredOutOfOrder);
    REGISTER_TEST(ArgsParserTest_MissingOptionValue);
    REGISTER_TEST(ArgsParserTest_UnknownOption);
    REGISTER_TEST(ArgsParserTest_UnexpectedArgument);
    REGISTER_TEST(ArgsParserTest_UnexpectedArgumentWithoutSpec);
    REGISTER_TEST(ArgsParserTest_MissingRequiredPositional);
    REGISTER_TEST(ArgsParserTest_MissingRequiredOption);
    REGISTER_TEST(ArgsParserTest_ReparseResetsRuntimeInfo);
}
