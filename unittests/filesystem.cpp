#include <gtest/gtest.h>
#include "filesystem.h"

namespace {

TEST(FileSystemTest, SplitPathFileDirectoryNoDirectorySpecified) {
    std::string path, file;

    std::tie(path, file) = filesystem::splitFileDirectory("file.txt");
    EXPECT_EQ(path, ".");
    EXPECT_EQ(file, "file.txt");
}

TEST(FileSystemTest, SplitPathFileDirectoryDot) {
    std::string path, file;

    std::tie(path, file) = filesystem::splitFileDirectory("./file.txt");
    EXPECT_EQ(path, ".");
    EXPECT_EQ(file, "file.txt");
}

TEST(FileSystemTest, SplitPathFileDirectoryDepthOne) {
    std::string path, file;

    std::tie(path, file) = filesystem::splitFileDirectory("one/file.txt");
    EXPECT_EQ(path, "one");
    EXPECT_EQ(file, "file.txt");
}

TEST(FileSystemTest, SplitPathFileDirectoryDepthTwo) {
    std::string path, file;

    std::tie(path, file) = filesystem::splitFileDirectory("one/two/file.txt");
    EXPECT_EQ(path, "one/two");
    EXPECT_EQ(file, "file.txt");
}

TEST(FileSystemTest, HasExtension) {
    EXPECT_TRUE(filesystem::hasExtension("foo.txt", "txt"));
    EXPECT_TRUE(filesystem::hasExtension("foo/bar/baz.txt", "txt"));
    EXPECT_TRUE(filesystem::hasExtension("foo.a", "a"));
    EXPECT_TRUE(filesystem::hasExtension("foo.obj", ""));
    EXPECT_TRUE(filesystem::hasExtension("", ""));

    EXPECT_FALSE(filesystem::hasExtension("foo.a", "b"));
    EXPECT_FALSE(filesystem::hasExtension("foo.stl", "obj"));
    EXPECT_FALSE(filesystem::hasExtension("", "obj"));
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

