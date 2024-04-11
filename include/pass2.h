#ifndef PASS2_H
#define PASS2_H

#include <bits/stdc++.h>
using namespace std;
#include "debug.h"
#include "def.h"
#include "utils.h"

void pass2(vector<Instruction> &INSTRUCTIONS, vector<VariantType> &OBJCODE,
           int &LOCCTR, map<string, int> &LITTAB,
           map<string, int> &SYMBOL_TABLE, bool &NOBASE,
           vector<string> &MRECORDS, int &BLOCK_NUMBER,
           map<int, int> &BLOCK_LOCCTR, map<string, bool> &SYMBOL_FLAG) {
    for (auto it : INSTRUCTIONS) {
        // Identify the presence of a new block
        if (it.new_block) {
            BLOCK_LOCCTR[BLOCK_NUMBER] = LOCCTR;
            LOCCTR = BLOCK_LOCCTR[it.block];
            BLOCK_NUMBER = it.block;
            // To ensure that a new Text Record is created each time a new block
            // is encountered
            formatData obj;
            obj.value = 0;
            obj.reserved = true;
            obj.word = false;
            obj.LOCCTR = LOCCTR;
            OBJCODE.push_back(obj);
        } else if (it.format == Format::ONE) {
            formatOne obj;
            obj.LOCCTR = LOCCTR;
            LOCCTR += 1;
            obj.opcode = it.opcode.code;
            OBJCODE.push_back(obj);
        } else if (it.format == Format::TWO) {
            formatTwo obj;
            obj.LOCCTR = LOCCTR;
            LOCCTR += 2;
            auto REG = get_registers(it.data);
            obj.opcode = it.opcode.code;
            obj.r1 = stringToRegister(REG.first);
            obj.r2 = stringToRegister(REG.second);
            OBJCODE.push_back(obj);
        } else if (it.format == Format::THREE) {
            formatThree obj;
            obj.LOCCTR = LOCCTR;
            LOCCTR += 3;
            if (it.opcode.code == 0x4C) {
                obj.n = true;
                obj.i = true;
                obj.x = false;
                obj.b = false;
                obj.p = false;
                obj.e = false;
                obj.opcode = it.opcode.code;
                obj.displacement = 0;
                OBJCODE.push_back(obj);
                continue;
            }
            obj.e = false;
            obj.opcode = it.opcode.code;
            if (it.data[0] == '#') {
                it.data = it.data.substr(1);
                obj.n = false;
                obj.i = true;
                obj.p = false;
                obj.b = false;
            } else if (it.data[0] == '@') {
                it.data = it.data.substr(1);
                obj.n = true;
                obj.i = false;
            } else {
                obj.n = true;
                obj.i = true;
            }
            size_t found = it.data.find(",X");
            if (found != string::npos) {
                it.data = it.data.substr(0, found);
                obj.x = true;
            } else {
                obj.x = false;
            }
            if (obj.i and !obj.n) {
                if (SYMBOL_TABLE.find(it.data) != SYMBOL_TABLE.end()) {
                    obj.displacement = SYMBOL_TABLE[it.data];
                } else {
                    obj.displacement = stoi(it.data);
                }
            } else if (SYMBOL_TABLE.find(it.data) != SYMBOL_TABLE.end()) {
                if (SYMBOL_TABLE[it.data] - LOCCTR < 2047 and
                    SYMBOL_TABLE[it.data] - LOCCTR >= -2048) {
                    obj.p = true;
                    obj.b = false;
                    obj.displacement = SYMBOL_TABLE[it.data] - LOCCTR;
                } else if (SYMBOL_TABLE[it.data] - SYMBOL_TABLE["BASE"] <
                               4095 and
                           SYMBOL_TABLE[it.data] - SYMBOL_TABLE["BASE"] >= 0 and
                           !NOBASE) {
                    obj.p = false;
                    obj.b = true;
                    obj.displacement =
                        SYMBOL_TABLE[it.data] - SYMBOL_TABLE["BASE"];
                }
            } else if (LITTAB.find(it.data) != LITTAB.end()) {
                if (LITTAB[it.data] - LOCCTR < 2047 and
                    LITTAB[it.data] - LOCCTR >= -2048) {
                    obj.p = true;
                    obj.b = false;
                    obj.displacement = LITTAB[it.data] - LOCCTR;
                } else if (LITTAB[it.data] - SYMBOL_TABLE["BASE"] < 4095 and
                           LITTAB[it.data] - SYMBOL_TABLE["BASE"] >= 0 and
                           !NOBASE) {
                    obj.p = false;
                    obj.b = true;
                    obj.displacement = LITTAB[it.data] - SYMBOL_TABLE["BASE"];
                }
            } else {
                string err_msg =
                    "Displacement can't be reached from PC or BASE.";
                save_error_msg(err_msg);
                exit(0);
            }
            OBJCODE.push_back(obj);
        } else if (it.format == Format::FOUR) {
            string addr = intToHex(LOCCTR + 1);
            while (addr.size() != 6) {
                addr = '0' + addr;
            }
            addr = 'M' + addr + "05";
            formatFour obj;
            obj.LOCCTR = LOCCTR;
            LOCCTR += 4;
            obj.opcode = it.opcode.code;
            obj.e = true;
            if (it.data[0] == '#') {
                it.data = it.data.substr(1);
                obj.n = false;
                obj.i = true;
            } else if (it.data[0] == '@') {
                it.data = it.data.substr(1);
                obj.n = true;
                obj.i = false;
            } else {
                obj.n = true;
                obj.i = true;
            }
            size_t found = it.data.find(",X");
            if (found != string::npos) {
                it.data = it.data.substr(0, found);
                obj.x = true;
            } else {
                obj.x = false;
            }
            obj.b = false;
            obj.p = false;
            if (obj.i and !obj.n) {
                if (SYMBOL_TABLE.find(it.data) != SYMBOL_TABLE.end()) {
                    if (SYMBOL_FLAG[it.data]) {
                        MRECORDS.push_back(addr);
                    }
                    obj.address = SYMBOL_TABLE[it.data];
                } else {
                    obj.address = stoi(it.data);
                }
            } else if (SYMBOL_TABLE.find(it.data) != SYMBOL_TABLE.end()) {
                if (SYMBOL_TABLE[it.data] >= 0 and
                    SYMBOL_TABLE[it.data] < 0xFFFFF) {
                    obj.address = SYMBOL_TABLE[it.data];
                }
                MRECORDS.push_back(addr);
            } else {
                string err_msg = "Invalid Symbol or Address out of range.";
                save_error_msg(err_msg);
                exit(0);
            }
            OBJCODE.push_back(obj);
        } else if (it.format == Format::DATA) {
            if (it.type == DataType::WORD) {
                formatData obj;
                obj.LOCCTR = LOCCTR;
                LOCCTR += 3;
                obj.value = stoi(it.data);
                obj.reserved = false;
                obj.word = true;
                OBJCODE.push_back(obj);
            } else if (it.type == DataType::BYTE) {
                formatData obj;
                obj.LOCCTR = LOCCTR;
                if (it.data[0] == 'X') {
                    LOCCTR += ((it.data.length() - 3) / 2);
                    if (it.data.length() - 3 == 4) {
                        obj.loc_ctr = true;
                    }
                    obj.value =
                        hexToInt(it.data.substr(2, it.data.length() - 3));
                } else if (it.data[0] == 'C') {
                    LOCCTR += (it.data.length() - 3);
                    string temp = it.data.substr(2, it.data.length() - 3);
                    obj.value = hexToInt(stringToHex(temp));
                }
                obj.reserved = false;
                obj.word = false;
                OBJCODE.push_back(obj);
            } else if (it.type == DataType::RESW) {
                formatData obj;
                obj.LOCCTR = LOCCTR;
                LOCCTR += 3 * stoi(it.data);
                obj.value = 3 * stoi(it.data);
                obj.reserved = true;
                obj.word = true;
                OBJCODE.push_back(obj);
            } else if (it.type == DataType::RESB) {
                formatData obj;
                obj.LOCCTR = LOCCTR;
                LOCCTR += stoi(it.data);
                obj.value = stoi(it.data);
                obj.reserved = true;
                obj.word = false;
                OBJCODE.push_back(obj);
            }
        }
    }
}

#endif
