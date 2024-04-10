#ifndef UTILS_H
#define UTILS_H

#include <bits/stdc++.h>
using namespace std;
#include "def.h"
#include "debug.h"

using VariantType = variant<formatOne, formatTwo, formatThree, formatFour, formatData>;

bool is_digits(const string &str)
{
    return all_of(str.begin(), str.end(), ::isdigit);
}

bool custom_sort(Instruction a, Instruction b)
{
    return a.address < b.address;
}

void handle_comments(string &line)
{
    size_t found = line.find('.');
    if (found != string::npos)
    {
        line = line.substr(0, found);
    }
}

pair<string, string> get_registers(const string &inputString)
{
    istringstream iss(inputString);
    string token1, token2;
    getline(iss, token1, ',');
    getline(iss, token2);
    return {token1, token2};
}

string charToHex(char c)
{
    stringstream ss;
    ss << hex << setw(2) << setfill('0') << static_cast<int>(c);
    return ss.str();
}

string stringToHex(const string &input)
{
    string hexString;
    for (char c : input)
    {
        hexString += charToHex(c);
    }
    return hexString;
}

string trim(const string &str)
{
    string result = str;
    result.erase(result.begin(), find_if(result.begin(), result.end(), [](unsigned char ch)
                                         { return !isspace(ch); }));
    result.erase(find_if(result.rbegin(), result.rend(), [](unsigned char ch)
                         { return !isspace(ch); })
                     .base(),
                 result.end());
    return result;
}

string intToHex(int value)
{
    stringstream stream;
    stream << hex << value;
    return stream.str();
}

int hexToInt(const string &hexString)
{
    stringstream stream(hexString);
    int value;
    stream >> hex >> value;
    return value;
}

vector<string> getExpressionTokens(const string &expression)
{
    vector<string> tokens;
    istringstream iss(expression);
    string token;

    while (iss >> token)
    {
        if (token.find('*') != std::string::npos || token.find('/') != std::string::npos)
        {
            cerr << "Invalid expression." << endl;
            exit(0);
        }
        size_t pos = 0;
        while ((pos = token.find_first_of("+-", pos)) != string::npos)
        {
            tokens.push_back(token.substr(0, pos));
            tokens.push_back(token.substr(pos, 1));
            token.erase(0, pos + 1);
            pos = 0;
        }
        if (!token.empty())
        {
            tokens.push_back(token);
        }
    }

    vector<string> finalTokens;
    for (const auto &token : tokens)
    {
        if (token != " " and token != "")
            finalTokens.push_back(token);
    }

    return finalTokens;
}

void printVariant(const VariantType &v)
{
    if (holds_alternative<formatOne>(v))
    {
        debug(get<formatOne>(v))
    }
    else if (holds_alternative<formatTwo>(v))
    {
        debug(get<formatTwo>(v))
    }
    else if (holds_alternative<formatThree>(v))
    {
        debug(get<formatThree>(v))
    }
    else if (holds_alternative<formatFour>(v))
    {
        debug(get<formatFour>(v))
    }
    else if (holds_alternative<formatData>(v))
    {
        debug(get<formatData>(v))
    }
}

unordered_map<string, Register> registerMap = {
    {"A", Register::A},
    {"X", Register::X},
    {"L", Register::L},
    {"B", Register::B},
    {"S", Register::S},
    {"T", Register::T},
    {"F", Register::F},
    {"PC", Register::PC},
    {"SW", Register::SW}};

Register stringToRegister(const string &str)
{
    // helper function to convert the string to register
    auto it = registerMap.find(str);
    if (it != registerMap.end())
    {
        return it->second;
    }
    else
    {
        cerr << "Invalid register string" << endl;
        exit(0);
    }
}

#endif