#include <gtest/gtest.h>
#include "argparse.h"

namespace {

TEST(ArgParseTest, EmptyParser) {
    CommandLineArgumentParser parser;

    EXPECT_EQ(parser.numNamedArguments(), 0);
    EXPECT_EQ(parser.numUnnamedArguments(), 0);
}

TEST(ArgParseTest, OnlyShortNamedArgs) {
    CommandLineArgumentParser parser;

    int arg_int = 0;
    float arg_float = 0.0f;
    unsigned int arg_uint = 0u;
    std::string arg_str;
    std::string arg_cb = "";
    auto cb = [&arg_cb](const std::string & s) { arg_cb = s; };

    parser.addArgument('i', "", arg_int);
    parser.addArgument('f', "", arg_float);
    parser.addArgument('u', "", arg_uint);
    parser.addArgument('s', "", arg_str);
    parser.addArgument('c', "", cb);

    EXPECT_EQ(parser.numNamedArguments(), 5);
    EXPECT_EQ(parser.numUnnamedArguments(), 0);

    const char * argv[] = {
        "prog",
        "-i", "-17",
        "-f", "3.14",
        "-u", "42",
        "-s", "hello",
        "-c", "cb",
        nullptr
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    parser.parse(argc, (char**) argv); 

    EXPECT_EQ(arg_int, -17);
    EXPECT_FLOAT_EQ(arg_float, 3.14);
    EXPECT_EQ(arg_uint, 42);
    EXPECT_EQ(arg_str, "hello");
    EXPECT_EQ(arg_cb, "cb");
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

