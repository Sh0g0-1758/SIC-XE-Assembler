#ifndef DEF_H
#define DEF_H

#include <string>
#include <unordered_map>

enum Register { A, X, L, B, S, T, F, PC, SW };

enum Format {
    ONE,
    TWO,
    THREE,
    FOUR,
    DATA,
};

enum DataType {
    WORD,
    BYTE,
    RESB,
    RESW,
};

struct Opcode {
    int code;
    Format format;
};

struct Instruction {
    Opcode opcode;
    std::string data;
    Format format;
    DataType type;
    int address;
    int block;
    bool new_block;
};

struct formatOne {
    int opcode;
    int LOCCTR;
};

struct formatTwo {
    int opcode;
    Register r1;
    Register r2;
    int LOCCTR;
};

struct formatThree {
    int opcode;
    bool n;
    bool i;
    bool x;
    bool b;
    bool p;
    bool e;
    int displacement;
    int LOCCTR;
};

struct formatFour {
    int opcode;
    bool n;
    bool i;
    bool x;
    bool b;
    bool p;
    bool e;
    int address;
    int LOCCTR;
};

struct formatData {
    int value;
    bool reserved;
    bool word;
    bool loc_ctr;
    int LOCCTR;
};

std::string SPACE = "\n<======================================================="
                    "==============>\n";

#endif
