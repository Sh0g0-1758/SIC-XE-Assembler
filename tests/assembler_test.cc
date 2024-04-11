#include <bits/stdc++.h>
#include <gtest/gtest.h>
using namespace std;

bool compareFiles(const string &filePath1, const string &filePath2) {
    ifstream file1(filePath1);
    ifstream file2(filePath2);
    if (!file1.is_open() || !file2.is_open()) {
        return false;
    }
    string line1, line2;
    while (getline(file1, line1) and getline(file2, line2)) {
        if (line1 != line2) {
            return false;
        }
    }
    getline(file2, line2);
    return (file1.eof() && file2.eof());
}

TEST(Program1_Test, CompareFiles) {
    string filePath1 = "./../../Output/test_program1_expected.txt";
    string filePath2 = "./../../Output/test_program1_generated.txt";
    ASSERT_TRUE(compareFiles(filePath1, filePath2));
}

TEST(Program2_Test, CompareFiles) {
    string filePath1 = "./../../Output/test_program2_expected.txt";
    string filePath2 = "./../../Output/test_program2_generated.txt";
    ASSERT_TRUE(compareFiles(filePath1, filePath2));
}

TEST(Program3_Test, CompareFiles) {
    string filePath1 = "./../../Output/test_program3_expected.txt";
    string filePath2 = "./../../Output/test_program3_generated.txt";
    ASSERT_TRUE(compareFiles(filePath1, filePath2));
}

TEST(Program4_Test, CompareFiles) {
    string filePath1 = "./../../Output/test_program4_expected.txt";
    string filePath2 = "./../../Output/test_program4_generated.txt";
    ASSERT_TRUE(compareFiles(filePath1, filePath2));
}

TEST(ErrorProgram_Test, CompareFiles) {
    string filePath1 = "./../../Output/error_expected.txt";
    string filePath2 = "./../../Output/error_generated.txt";
    ASSERT_TRUE(compareFiles(filePath1, filePath2));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
