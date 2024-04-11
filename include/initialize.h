#ifndef INITIALIZE_H
#define INITIALIZE_H

#include <bits/stdc++.h>
using namespace std;
#include "debug.h"
#include "def.h"
#include "utils.h"

void program_initialisation(string line, string &NAME, int &START_ADDRESS,
                            int &LOCCTR) {
    vector<string> tokens;
    istringstream iss(line);
    string token;
    while (iss >> token) {
        tokens.push_back(trim(token));
    }
    if (tokens[1] != "START") {
        string err_msg = "Invalid program start: " + tokens[1];
        save_error_msg(err_msg);
        exit(0);
    }
    NAME = tokens[0];
    START_ADDRESS = hexToInt(tokens[2]);
    LOCCTR = START_ADDRESS;
}

void pre_process(string line, map<string, Opcode> &OPTAB) {
    Opcode opcode;
    vector<string> tokens;
    istringstream iss(line);
    string token;
    while (getline(iss, token, '|')) {
        tokens.push_back(trim(token));
    }

    if (tokens.size() == 3) {
        opcode.code = hexToInt(tokens[2]);
        if (tokens[1] == "1") {
            opcode.format = Format::ONE;
        } else if (tokens[1] == "2") {
            opcode.format = Format::TWO;
        } else if (tokens[1] == "3/4") {
            opcode.format = Format::THREE;
        } else {
            string err_msg = "Invalid format value: " + tokens[1];
            save_error_msg(err_msg);
            exit(0);
        }
        OPTAB[tokens[0]] = opcode;
    } else {
        string err_msg =
            "Insufficient data values for opcode: " + tokens.size();
        save_error_msg(err_msg);
        exit(0);
    }
}

#endif
