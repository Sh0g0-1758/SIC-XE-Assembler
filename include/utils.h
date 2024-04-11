#ifndef UTILS_H
#define UTILS_H

#include <bits/stdc++.h>
using namespace std;
#include "../tools/color/color.h"
#include "debug.h"
#include "def.h"

// colored output
const Color::Modifier red(Color::FG_RED);
const Color::Modifier def(Color::FG_DEFAULT);
const Color::Modifier green(Color::FG_GREEN);
const Color::Modifier blue(Color::FG_BLUE);
const Color::Modifier cyan(Color::FG_CYAN);

using VariantType =
    variant<formatOne, formatTwo, formatThree, formatFour, formatData>;

void save_error_msg(string str) {
    cout << red << "Assembler Encountered an Error. Diagnostics are stored in"
         << def << blue << " error_generated.txt\n"
         << def;
    string File_Name = "./../Output/error_generated.txt";
    ofstream outputFile(File_Name);

    if (!outputFile.is_open()) {
        cout << red << "Failed to open the file." << def << endl;
        exit(0);
    }
    outputFile << str << endl;
    outputFile.close();
}

bool is_digits(const string &str) {
    return all_of(str.begin(), str.end(), ::isdigit);
}

void handle_comments(string &line) {
    size_t found = line.find('.');
    if (found != string::npos) {
        line = line.substr(0, found);
    }
}

pair<string, string> get_registers(const string &inputString) {
    istringstream iss(inputString);
    string token1, token2;
    getline(iss, token1, ',');
    getline(iss, token2);
    return {token1, token2};
}

bool check_lit_validity(string &lit,
                        vector<pair<string, int>> &LIT_INTERMEDIATE) {
    for (auto it : LIT_INTERMEDIATE) {
        if (it.first == lit.substr(1)) {
            return false;
        }
    }
    return true;
}

string charToHex(char c) {
    stringstream ss;
    ss << hex << setw(2) << setfill('0') << static_cast<int>(c);
    return ss.str();
}

string stringToHex(const string &input) {
    string hexString;
    for (char c : input) {
        hexString += charToHex(c);
    }
    return hexString;
}

string trim(const string &str) {
    string result = str;
    result.erase(result.begin(),
                 find_if(result.begin(), result.end(),
                         [](unsigned char ch) { return !isspace(ch); }));
    result.erase(find_if(result.rbegin(), result.rend(),
                         [](unsigned char ch) { return !isspace(ch); })
                     .base(),
                 result.end());
    return result;
}

void remove_whitespaces(string &str) {
    size_t start = str.find_first_not_of(" \t\n\r");

    if (start == string::npos) {
        str.clear();
        return;
    }
    size_t end = str.find_last_not_of(" \t\n\r");
    str = str.substr(start, end - start + 1);
}

string intToHex(int value) {
    stringstream stream;
    stream << hex << value;
    return stream.str();
}

int hexToInt(const string &hexString) {
    stringstream stream(hexString);
    int value;
    stream >> hex >> value;
    return value;
}

string getLastThreeCharacters(const string &str) {
    if (str.length() >= 3) {
        return str.substr(str.length() - 3);
    } else {
        return str;
    }
}

void removeNewlines(string &str) {
    size_t lastNonNewline = str.find_last_not_of("\n");
    if (lastNonNewline != string::npos) {
        str = str.substr(0, lastNonNewline + 1);
    } else {
        str.clear();
    }
}

string get_file_name(string filePath) {
    size_t lastSlashPos = filePath.find_last_of('/');
    if (lastSlashPos == string::npos) {
        cout << red << "Invalid file path." << def << endl;
        exit(0);
    }
    string extractedString = filePath.substr(
        lastSlashPos + 1, filePath.find(".txt") - lastSlashPos - 1);
    return extractedString;
}

vector<string> getExpressionTokens(const string &expression) {
    vector<string> tokens;
    istringstream iss(expression);
    string token;

    while (iss >> token) {
        if (token.find('*') != string::npos ||
            token.find('/') != string::npos) {
            string err_msg = "Invalid expression: " + expression;
            save_error_msg(err_msg);
            exit(0);
        }
        size_t pos = 0;
        while ((pos = token.find_first_of("+-", pos)) != string::npos) {
            tokens.push_back(token.substr(0, pos));
            tokens.push_back(token.substr(pos, 1));
            token.erase(0, pos + 1);
            pos = 0;
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    vector<string> finalTokens;
    for (const auto &token : tokens) {
        if (token != " " and token != "")
            finalTokens.push_back(token);
    }

    return finalTokens;
}

void printVariant(const VariantType &v) {
    if (holds_alternative<formatOne>(v)) {
        debug(get<formatOne>(v))
    } else if (holds_alternative<formatTwo>(v)) {
        debug(get<formatTwo>(v))
    } else if (holds_alternative<formatThree>(v)) {
        debug(get<formatThree>(v))
    } else if (holds_alternative<formatFour>(v)) {
        debug(get<formatFour>(v))
    } else if (holds_alternative<formatData>(v)) {
        debug(get<formatData>(v))
    }
}

unordered_map<string, Register> registerMap = {
    {"A", Register::A}, {"X", Register::X},   {"L", Register::L},
    {"B", Register::B}, {"S", Register::S},   {"T", Register::T},
    {"F", Register::F}, {"PC", Register::PC}, {"SW", Register::SW}};

Register stringToRegister(const string &str) {
    // helper function to convert the string to register
    if (str.empty()) {
        auto it = registerMap.find("A");
        return it->second;
    }
    auto it = registerMap.find(str);
    if (it != registerMap.end()) {
        return it->second;
    } else {
        string err_msg = "Invalid register string: " + str;
        save_error_msg(err_msg);
        exit(0);
    }
}

#endif
