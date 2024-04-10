#ifndef PASS2_H
#define PASS2_H

#include <bits/stdc++.h>
using namespace std;
#include "utils.h"
#include "def.h"
#include "debug.h"

void pass2(vector<Instruction> &INSTRUCTIONS, vector<VariantType> &OBJCODE, int &LOCCTR, map<string, int> &LITTAB, map<string, int> &SYMBOL_TABLE, bool &NOBASE, vector<string> &MRECORDS)
{
    for (auto it : INSTRUCTIONS)
    {
        if (it.format == Format::ONE)
        {
            LOCCTR += 1;
            formatOne obj;
            obj.opcode = it.opcode.code;
            OBJCODE.push_back(obj);
        }
        else if (it.format == Format::TWO)
        {
            LOCCTR += 2;
            auto REG = get_registers(it.data);
            formatTwo obj;
            obj.opcode = it.opcode.code;
            obj.r1 = stringToRegister(REG.first);
            obj.r2 = stringToRegister(REG.second);
            OBJCODE.push_back(obj);
        }
        else if (it.format == Format::THREE)
        {
            LOCCTR += 3;
            formatThree obj;
            if (it.opcode.code == 0x4C)
            {
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
            if (it.data[0] == '#')
            {
                it.data = it.data.substr(1);
                obj.n = false;
                obj.i = true;
            }
            else if (it.data[0] == '@')
            {
                it.data = it.data.substr(1);
                obj.n = true;
                obj.i = false;
            }
            else
            {
                obj.n = true;
                obj.i = true;
            }
            size_t found = it.data.find(",X");
            if (found != string::npos)
            {
                it.data = it.data.substr(0, found);
                obj.x = true;
            }
            else
            {
                obj.x = false;
            }
            if (obj.i and !obj.n)
            {
                if (SYMBOL_TABLE.find(it.data) != SYMBOL_TABLE.end())
                {
                    obj.displacement = SYMBOL_TABLE[it.data];
                }
                else
                {
                    obj.displacement = stoi(it.data);
                }
            }
            else if (SYMBOL_TABLE[it.data] - LOCCTR <= 0xFFF and SYMBOL_TABLE[it.data] - LOCCTR >= 0)
            {
                obj.p = true;
                obj.b = false;
                obj.displacement = SYMBOL_TABLE[it.data] - LOCCTR;
            }
            else if (SYMBOL_TABLE[it.data] - SYMBOL_TABLE["BASE"] < 4095 and SYMBOL_TABLE[it.data] - SYMBOL_TABLE["BASE"] > 0)
            {
                obj.p = false;
                obj.b = true;
                obj.displacement = SYMBOL_TABLE[it.data] - SYMBOL_TABLE["BASE"];
            }
            else if (LITTAB[it.data] - LOCCTR <= 0xFF and
                     LITTAB[it.data] - LOCCTR >= 0)
            {
                obj.p = true;
                obj.b = false;
                obj.displacement = LITTAB[it.data] - LOCCTR;
            }
            else if (LITTAB[it.data] - SYMBOL_TABLE["BASE"] < 4095 and
                     LITTAB[it.data] - SYMBOL_TABLE["BASE"] > 0 and !NOBASE)
            {
                obj.p = false;
                obj.b = true;
                obj.displacement = LITTAB[it.data] - SYMBOL_TABLE["BASE"];
            }
            else
            {
                debug(it.data) debug(LITTAB[it.data]) debug(LOCCTR) cout
                    << "Displacement can't be reached from PC or BASE." << endl;
                exit(0);
            }
            OBJCODE.push_back(obj);
        }
        else if (it.format == Format::FOUR)
        {
            string addr = intToHex(LOCCTR + 1);
            while (addr.size() != 6)
            {
                addr = '0' + addr;
            }
            addr = 'M' + addr + "05";
            MRECORDS.push_back(addr);
            LOCCTR += 4;
            formatFour obj;
            obj.opcode = it.opcode.code;
            obj.e = true;
            if (it.data[0] == '#')
            {
                it.data = it.data.substr(1);
                obj.n = false;
                obj.i = true;
            }
            else if (it.data[0] == '@')
            {
                it.data = it.data.substr(1);
                obj.n = true;
                obj.i = false;
            }
            else
            {
                obj.n = true;
                obj.i = true;
            }
            size_t found = it.data.find(",X");
            if (found != string::npos)
            {
                it.data = it.data.substr(0, found);
                obj.x = true;
            }
            else
            {
                obj.x = false;
            }
            obj.b = false;
            obj.p = false;
            if (obj.i and !obj.n)
            {
                if (SYMBOL_TABLE.find(it.data) != SYMBOL_TABLE.end())
                {
                    obj.address = SYMBOL_TABLE[it.data];
                }
                else
                {
                    obj.address = stoi(it.data);
                }
            }
            else if (SYMBOL_TABLE[it.data] > 0 and SYMBOL_TABLE[it.data] < 0xFFFFF)
            {
                obj.address = SYMBOL_TABLE[it.data];
            }
            else
            {
                cout << "Invalid Symbol or Address out of range." << endl;
                exit(0);
            }
            OBJCODE.push_back(obj);
        }
        else if (it.format == Format::DATA)
        {
            if (it.type == DataType::WORD)
            {
                LOCCTR += 3;
                formatData obj;
                obj.value = stoi(it.data);
                obj.reserved = false;
                obj.word = true;
                OBJCODE.push_back(obj);
            }
            else if (it.type == DataType::BYTE)
            {
                formatData obj;
                if (it.data[0] == 'X')
                {
                    LOCCTR += ((it.data.length() - 3) / 2);
                    if (it.data.length() - 3 == 4)
                    {
                        obj.loc_ctr = true;
                    }
                    obj.value = hexToInt(it.data.substr(2, it.data.length() - 3));
                }
                else if (it.data[0] == 'C')
                {
                    LOCCTR += (it.data.length() - 3);
                    string temp = it.data.substr(2, it.data.length() - 3);
                    obj.value = hexToInt(stringToHex(temp));
                }
                obj.reserved = false;
                obj.word = false;
                OBJCODE.push_back(obj);
            }
            else if (it.type == DataType::RESW)
            {
                LOCCTR += 3 * stoi(it.data);
                formatData obj;
                obj.value = 3 * stoi(it.data);
                obj.reserved = true;
                obj.word = true;
                OBJCODE.push_back(obj);
            }
            else if (it.type == DataType::RESB)
            {
                LOCCTR += stoi(it.data);
                formatData obj;
                obj.value = stoi(it.data);
                obj.reserved = true;
                obj.word = false;
                OBJCODE.push_back(obj);
            }
        }
    }
}

#endif
