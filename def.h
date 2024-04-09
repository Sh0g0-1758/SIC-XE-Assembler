#include<string>
#include<unordered_map>

enum Register {
    A,
    X,
    L,
    B,
    S,
    T,
    F,
    PC,
    SW
};

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
};

struct formatOne {
    int opcode;
};

struct formatTwo {
    int opcode;
    Register r1;
    Register r2;
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
};

struct formatData {
    int value;
    bool reserved;
    bool word;
    bool loc_ctr;
};

std::string SPACE = "\n<=====================================================================>\n";