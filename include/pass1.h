#ifndef PASS1_H
#define PASS1_H

#include <bits/stdc++.h>
using namespace std;
#include "debug.h"
#include "def.h"
#include "utils.h"

#define data_directive()                                                       \
    {                                                                          \
        instruction.address = LOCCTR;                                          \
        instruction.data = tokens[2];                                          \
        instruction.format = Format::DATA;                                     \
        instruction.opcode.code = 0;                                           \
        instruction.opcode.format = Format::DATA;                              \
        instruction.new_block = false;                                         \
    }

void pass1(string line, bool &NOBASE,
           vector<pair<string, int>> &LIT_INTERMEDIATE,
           map<string, int> &LITTAB, int &LOCCTR,
           vector<Instruction> &INSTRUCTIONS, map<string, int> &SYMBOL_TABLE,
           map<string, bool> &SYMBOL_FLAG, int &START_ADDRESS, bool &ORG,
           vector<string> &MRECORDS, map<string, Opcode> &OPTAB, string &BASE,
           int &prevLOCCTR, map<string, pair<int, int>> &BLOCK_TABLE,
           int &BLOCK_NUMBER, string &CURR_BLOCK_NAME, int &TOTAL_BLOCKS,
           map<int, string> &BLOCK_NAMES, map<string, int> &SYMBOL_BLOCK,
           map<string, int> &LIT_BLOCK, string File_Name, bool &has_end,
           bool &has_rsub) {
    Instruction instruction;
    vector<string> tokens;
    istringstream iss(line);
    string token;
    while (iss >> token) {
        tokens.push_back(trim(token));
    }
    if (tokens[0] == "USE") {
        BLOCK_TABLE[CURR_BLOCK_NAME].second = LOCCTR;
        if (tokens.size() == 1) {
            BLOCK_NUMBER = 0;
            CURR_BLOCK_NAME = "DEFAULT";
        } else {
            if (BLOCK_TABLE.find(tokens[1]) != BLOCK_TABLE.end()) {
                BLOCK_NUMBER = BLOCK_TABLE[tokens[1]].first;
            } else {
                TOTAL_BLOCKS++;
                BLOCK_TABLE[tokens[1]] = {TOTAL_BLOCKS, 0};
                BLOCK_NUMBER = TOTAL_BLOCKS;
                BLOCK_NAMES[BLOCK_NUMBER] = tokens[1];
            }
            CURR_BLOCK_NAME = tokens[1];
        }
        LOCCTR = BLOCK_TABLE[CURR_BLOCK_NAME].second;
        instruction.new_block = true;
        instruction.block = BLOCK_NUMBER;
        INSTRUCTIONS.push_back(instruction);
        return;
    }
    if (tokens[0] == "NOBASE") {
        NOBASE = true;
        return;
    }
    if (tokens[0] == "LTORG") {
        for (auto it : LIT_INTERMEDIATE) {
            LITTAB[it.first] = LOCCTR;
            LIT_BLOCK[it.first] = BLOCK_NUMBER;
            instruction.address = LOCCTR;
            instruction.data = it.first;
            instruction.format = Format::DATA;
            instruction.opcode.code = 0;
            instruction.opcode.format = Format::DATA;
            instruction.type = DataType::BYTE;
            LOCCTR += it.second;
            instruction.new_block = false;
            INSTRUCTIONS.push_back(instruction);
        }
        LIT_INTERMEDIATE.clear();
        return;
    }
    if (tokens[0] == "RSUB") {
        tokens.insert(tokens.begin(), "");
        tokens.push_back("");
    }
    if (tokens.size() > 1 and tokens[1] == "RSUB") {
        tokens.push_back("");
    }
    if (tokens.size() == 2) {
        tokens.insert(tokens.begin(), "");
    }
    if (tokens.size() == 1) {
        string err_msg = "Invalid line: " + line;
        save_error_msg(err_msg, File_Name);
        exit(0);
    }
    if (tokens[1] == "END") {
        if (SYMBOL_TABLE.find(tokens[2]) == SYMBOL_TABLE.end() or
            SYMBOL_TABLE[tokens[2]] != START_ADDRESS) {
            string err_msg = "Invalid program end: " + tokens[2];
            save_error_msg(err_msg, File_Name);
            exit(0);
        }
        has_end = true;
        return;
    }
    if (!tokens[0].empty()) {
        if (SYMBOL_TABLE.find(tokens[0]) != SYMBOL_TABLE.end()) {
            string err_msg = "Duplicate symbol: " + tokens[0];
            save_error_msg(err_msg, File_Name);
            exit(0);
        }
        if (tokens[1] == "EQU") {
            vector<string> equ_toks;
            if (tokens[2] != "*") {
                equ_toks = getExpressionTokens(tokens[2], File_Name);
            }
            if (tokens[2] == "*") {
                SYMBOL_TABLE[tokens[0]] = LOCCTR;
                SYMBOL_BLOCK[tokens[0]] = BLOCK_NUMBER;
            } else if (equ_toks.size() != 1) {
                // Give Expr value here
                if (equ_toks[0] == "+" or equ_toks[0] == "-") {
                    string err_msg = "INVALID STARTING EQU ARGUMENT.";
                    save_error_msg(err_msg, File_Name);
                    exit(0);
                } else if (SYMBOL_TABLE.find(equ_toks[0]) ==
                           SYMBOL_TABLE.end()) {
                    string err_msg = "INVALID EQU ARGUMENT.";
                    save_error_msg(err_msg, File_Name);
                    exit(0);
                }
                int value = SYMBOL_TABLE[equ_toks[0]];
                bool absolute = false;
                for (ll i = 1; i < equ_toks.size(); i++) {
                    if (equ_toks[i] == "+") {
                        if (!absolute) {
                            if (SYMBOL_TABLE.find(equ_toks[i + 1]) !=
                                    SYMBOL_TABLE.end() or
                                !is_digits(equ_toks[i + 1])) {
                                string err_msg = "INVALID EQU ARGUMENT.";
                                save_error_msg(err_msg, File_Name);
                                exit(0);
                            } else {
                                value = value + stoi(equ_toks[i + 1]);
                                absolute = false;
                            }
                        } else {
                            if (SYMBOL_TABLE.find(equ_toks[i + 1]) !=
                                SYMBOL_TABLE.end()) {
                                value = value + SYMBOL_TABLE[equ_toks[i + 1]];
                                absolute = false;
                            } else if (is_digits(equ_toks[i + 1])) {
                                value = value + stoi(equ_toks[i + 1]);
                                absolute = true;
                            } else {
                                string err_msg = "INVALID EQU ARGUMENT.";
                                save_error_msg(err_msg, File_Name);
                                exit(0);
                            }
                        }
                    } else if (equ_toks[i] == "-") {
                        if (absolute) {
                            if (SYMBOL_TABLE.find(equ_toks[i + 1]) !=
                                SYMBOL_TABLE.end()) {
                                string err_msg = "INVALID EQU ARGUMENT.";
                                save_error_msg(err_msg, File_Name);
                                exit(0);
                            } else if (is_digits(equ_toks[i + 1])) {
                                value = value - stoi(equ_toks[i + 1]);
                            } else {
                                string err_msg = "INVALID EQU ARGUMENT.";
                                save_error_msg(err_msg, File_Name);
                                exit(0);
                            }
                        } else {
                            if (SYMBOL_TABLE.find(equ_toks[i + 1]) !=
                                SYMBOL_TABLE.end()) {
                                value = value - SYMBOL_TABLE[equ_toks[i + 1]];
                                absolute = true;
                            } else if (is_digits(equ_toks[i + 1])) {
                                value = value - stoi(equ_toks[i + 1]);
                                absolute = false;
                            } else {
                                string err_msg = "INVALID EQU ARGUMENT.";
                                save_error_msg(err_msg, File_Name);
                                exit(0);
                            }
                        }
                    }
                    i++;
                }
                SYMBOL_TABLE[tokens[0]] = value;
                if (absolute) {
                    SYMBOL_FLAG[tokens[0]] = false;
                    SYMBOL_BLOCK[tokens[0]] = -1;
                } else {
                    // Generate Modification Record
                    SYMBOL_BLOCK[tokens[0]] = BLOCK_NUMBER;
                    string addr = intToHex(LOCCTR);
                    while (addr.size() != 6) {
                        addr = '0' + addr;
                    }
                    string val_str =
                        intToHex(intToHex(SYMBOL_TABLE[tokens[0]]).size());
                    if (val_str.size() == 1) {
                        val_str = '0' + val_str;
                    }
                    MRECORDS.push_back("M" + addr + val_str);
                }
            } else if (equ_toks.size() == 1 and
                       SYMBOL_TABLE.find(tokens[2]) != SYMBOL_TABLE.end()) {
                SYMBOL_TABLE[tokens[0]] = SYMBOL_TABLE[tokens[2]];
                SYMBOL_BLOCK[tokens[0]] = BLOCK_NUMBER;
                SYMBOL_FLAG[tokens[0]] = true;
                if (SYMBOL_FLAG[tokens[2]]) {
                    // Generate Modification Record
                    string addr = intToHex(LOCCTR);
                    while (addr.size() != 6) {
                        addr = '0' + addr;
                    }
                    string val_str =
                        intToHex(intToHex(SYMBOL_TABLE[tokens[0]]).size());
                    if (val_str.size() == 1) {
                        val_str = '0' + val_str;
                    }
                    MRECORDS.push_back("M" + addr + val_str);
                }
            } else if (is_digits(tokens[2])) {
                SYMBOL_TABLE[tokens[0]] = stoi(tokens[2]);
                SYMBOL_BLOCK[tokens[0]] = BLOCK_NUMBER;
                SYMBOL_FLAG[tokens[0]] = false;
            } else {
                string err_msg = "INVALID EQU ARGUMENT.";
                save_error_msg(err_msg, File_Name);
                exit(0);
            }
            return;
        } // early return, hence no else
        SYMBOL_TABLE[tokens[0]] = LOCCTR;
        SYMBOL_BLOCK[tokens[0]] = BLOCK_NUMBER;
        SYMBOL_FLAG[tokens[0]] = true;
    }
    if (tokens[1] == "ORG") {
        if (!ORG) {
            ORG = true;
            prevLOCCTR = LOCCTR;
            if (SYMBOL_TABLE.find(tokens[2]) != SYMBOL_TABLE.end()) {
                LOCCTR = SYMBOL_TABLE[tokens[2]];
            } else {
                string err_msg = "Invalid ORG argument: " + tokens[2];
                save_error_msg(err_msg, File_Name);
                exit(0);
            }
            LOCCTR = SYMBOL_TABLE[tokens[2]];
            return;
        } else {
            LOCCTR = prevLOCCTR;
            ORG = false;
        }
    }
    if (tokens[1] == "BASE") {
        if (tokens[2] == "*") {
            string temp = intToHex(LOCCTR);
            while (temp.size() < 4) {
                temp = '0' + temp;
            }
            BASE = "*" + temp;
        } else {
            BASE = tokens[2];
        }
        return;
    }
    if (tokens[1] == "WORD") {
        data_directive();
        instruction.type = DataType::WORD;
        LOCCTR += 3;
        INSTRUCTIONS.push_back(instruction);
        return;
    } else if (tokens[1] == "RESW") {
        data_directive();
        instruction.type = DataType::RESW;
        LOCCTR += 3 * stoi(tokens[2]);
        INSTRUCTIONS.push_back(instruction);
        return;
    } else if (tokens[1] == "RESB") {
        data_directive();
        instruction.type = DataType::RESB;
        LOCCTR += stoi(tokens[2]);
        INSTRUCTIONS.push_back(instruction);
        return;
    } else if (tokens[1] == "BYTE") {
        data_directive();
        instruction.type = DataType::BYTE;
        if (tokens[2][0] == 'X') {
            LOCCTR += (tokens[2].length() - 3) / 2;
        } else if (tokens[2][0] == 'C') {
            LOCCTR += tokens[2].length() - 3;
        } else {
            string err_msg = "Invalid byte declaration: " + tokens[2];
            save_error_msg(err_msg, File_Name);
            exit(0);
        }
        instruction.new_block = false;
        INSTRUCTIONS.push_back(instruction);
        return;
    } else if (tokens[1][0] == '+') {
        if (OPTAB.find(tokens[1].substr(1)) == OPTAB.end()) {
            string err_msg = "Invalid opcode: " + tokens[1].substr(1);
            save_error_msg(err_msg, File_Name);
            exit(0);
        }
    } else if (OPTAB.find(tokens[1]) == OPTAB.end()) {
        string err_msg = "Invalid opcode: " + tokens[1];
        save_error_msg(err_msg, File_Name);
        exit(0);
    }
    if (tokens[1][0] == '+') {
        instruction.format = Format::FOUR;
        tokens[1] = tokens[1].substr(1);
    } else {
        instruction.format = OPTAB[tokens[1]].format;
    }
    instruction.opcode = OPTAB[tokens[1]];
    instruction.address = LOCCTR;
    // Checking for literals
    if (tokens[2][0] == '=') {
        if (LITTAB.find(tokens[2]) == LITTAB.end() and
            check_lit_validity(tokens[2], LIT_INTERMEDIATE)) {
            string tmp;
            if (tokens[2][1] == 'C') {
                tmp = tokens[2].substr(1, tokens[2].length() - 1);
                LIT_INTERMEDIATE.push_back({tmp, tmp.size() - 3});
                instruction.data = tmp;
            } else if (tokens[2][1] == 'X') {
                tmp = tokens[2].substr(1, tokens[2].length() - 1);
                LIT_INTERMEDIATE.push_back({tmp, (tmp.size() - 3) / 2});
                instruction.data = tmp;
            } else if (tokens[2][1] == '*') {
                tmp = intToHex(LOCCTR);
                while (tmp.size() != 4) {
                    tmp = '0' + tmp;
                }
                tmp = "X'" + tmp + "'";
                LIT_INTERMEDIATE.push_back({tmp, (tmp.size() - 3) / 2});
                instruction.data = tmp;
            } else {
                string err_msg = "Invalid literal declaration: " + tokens[2];
                save_error_msg(err_msg, File_Name);
                exit(0);
            }
        } else if (!check_lit_validity(tokens[2], LIT_INTERMEDIATE)) {
            instruction.data = tokens[2].substr(1, tokens[2].length() - 1);
        }
    } else {
        instruction.data = tokens[2];
    }
    if (instruction.format == Format::ONE) {
        LOCCTR += 1;
    } else if (instruction.format == Format::TWO) {
        LOCCTR += 2;
    } else if (instruction.format == Format::THREE) {
        LOCCTR += 3;
    } else if (instruction.format == Format::FOUR) {
        LOCCTR += 4;
    }
    instruction.block = BLOCK_NUMBER;
    instruction.new_block = false;
    INSTRUCTIONS.push_back(instruction);
}

#endif
