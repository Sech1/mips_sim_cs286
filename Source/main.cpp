#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <iomanip>
#include <fstream>

using namespace std;

//struct mipsR {
//    int opCode;
//    int rs;
//    int rt;
//    int offSet;
//};
//
//struct mipsI {
//    int opCode;
//    int s;
//    int t;
//    int imm;
//};
//
//struct mipsJ {
//    int opCode;
//    int target;
//};

int numberOfInstructions = 0;
int *mipsInstructions;
int *mipsMemory;
int *data;
int *registers;
int *dataAddr;
char *inputFile;
int dataSize = 0;

void runSimulation(char *argv, char *inputFile);

int main(int argc, char *argv[]) {

    if (argc > 1) {

        string userOutput = argv[4];
        string dis = "_dis.txt";
        string file = userOutput + dis;

        ofstream outputFile(file);

        unsigned test;
        unsigned numFail;
        unsigned numOp;
        unsigned numOffset;
        unsigned const mask = 4227858432;
        unsigned const maskFail = 0x80000000;
        unsigned const maskOff = 0x3F;

        int startAddress = 96;

        bool breakTest = false;

        inputFile = argv[2];

        char buffer[4];
        int i;
        char *iPtr;
        iPtr = (char *) (void *) &i;

        int FD = open(inputFile, O_RDONLY);

        int amt = 4;
        while (amt != 0) {
            amt = read(FD, buffer, 4);
            if (amt == 4) {

                iPtr[0] = buffer[3];
                iPtr[1] = buffer[2];
                iPtr[2] = buffer[1];
                iPtr[3] = buffer[0];

                numFail = ((i & maskFail) >> 31);
                numOp = ((i & mask) >> 26);
                numOffset = ((i & maskOff));

                unsigned failBit = (numFail);
                outputFile << std::bitset<1>(failBit) << " ";
                outputFile << std::bitset<31>(i) << "   ";
                //  cout << hex << i << "   ";
                outputFile.width(3);
                outputFile << startAddress << "   ";
            }

            if (breakTest == false) {


                if (numFail == 0) {     //FAILBIT

                    outputFile << "Invalid Instruction";
                    outputFile << endl;

                    numberOfInstructions++;

                } else if ((numOp == 32) & (numOffset == 32)) {     //ADD INSTRUCTION

                    unsigned const r1Mask = 0x3E00000;
                    unsigned const r2Mask = 0x1F0000;
                    unsigned const r3Mask = 0xF800;
                    unsigned r1 = ((i & r1Mask) >> 21);
                    unsigned r2 = ((i & r2Mask) >> 16);
                    unsigned r3 = ((i & r3Mask) >> 11);

                    outputFile << "ADD ";
                    outputFile.width(4);
                    outputFile << "R" << r3 << ", ";
                    outputFile << "R" << r1 << ", ";
                    outputFile << "R" << r2 << "  ";
                    outputFile << endl;

                    numberOfInstructions++;

                } else if (numOp == 40) {     //ADDI INSTRUCTION

                    unsigned const r1Mask = 0x3E00000;
                    unsigned const r2Mask = 0x1F0000;
                    int const immMask = 0xFFFF;
                    unsigned r1 = ((i & r1Mask) >> 21);
                    unsigned r2 = ((i & r2Mask) >> 16);
                    int imm = (i & immMask);
                    int immPrint = imm;

                    if (imm >= 65535) {
                        immPrint = (imm - 65536);
                    }

                    outputFile << "ADDI ";
                    outputFile.width(3);
                    outputFile << "R" << r2 << ", ";
                    outputFile << "R" << r1 << ", ";
                    outputFile << "#" << immPrint << " ";
                    outputFile << endl;

                    numberOfInstructions++;

                } else if (numOp == 9) {     //ADDU INSTRUCTION (not used)

                    outputFile << "ADDIU -- Add immediate unsigned (no overflow)";
                    outputFile << endl;

                } else if (numOp == 35) {     //LW INSTRUCTION

                    unsigned const r1Mask = 0x1F0000;
                    unsigned const r2Mask = 0x3E00000;
                    unsigned const offSetMask = 0xFFFF;
                    unsigned r1 = ((i & r1Mask) >> 16);
                    unsigned r2 = ((i & r2Mask) >> 21);
                    unsigned offSet = (i & offSetMask);

                    outputFile << "LW";
                    outputFile.width(6);
                    outputFile << "R" << r1 << ", ";
                    outputFile << offSet << "(";
                    outputFile << "R" << r2 << ")";
                    outputFile << endl;

                    numberOfInstructions++;

                } else if (numOp == 43) {     //SW INSTRUCTION

                    unsigned const r1Mask = 0x1F0000;
                    unsigned const r2Mask = 0x3E00000;
                    unsigned const offSetMask = 0xFFFF;
                    unsigned r1 = ((i & r1Mask) >> 16);
                    unsigned r2 = ((i & r2Mask) >> 21);
                    unsigned offSet = (i & offSetMask);

                    outputFile << "SW";
                    outputFile.width(6);
                    outputFile << "R" << r1 << ", ";
                    outputFile << offSet << "(";
                    outputFile << "R" << r2 << ")";
                    outputFile << endl;

                    numberOfInstructions++;

                } else if (numOp == 33) {     //BLITZ INSTRUCTION

                    unsigned const r1Mask = 0x3E00000;
                    unsigned const offSetMask = 0xFFFF;
                    unsigned r1 = ((i & r1Mask) >> 21);
                    unsigned offSet = ((i & offSetMask) * 4);

                    outputFile << "BLTZ";
                    outputFile.width(4);
                    outputFile << "R" << r1 << ", ";
                    outputFile << "#" << offSet;
                    outputFile << endl;

                    numberOfInstructions++;

                } else if (numOp == 132) {     //BEQ INSTRUCTION

                    unsigned const r1Mask = 0x3E00000;
                    unsigned const r2Mask = 0x1F0000;
                    unsigned const offMask = 0xFFFF;
                    unsigned r1 = ((i & r1Mask) >> 16);
                    unsigned r2 = ((i & r2Mask) >> 11);
                    unsigned offSet = (i & offMask);

                    outputFile << "BEQ";
                    outputFile.width(5);
                    outputFile << "R" << r1 << ", ";
                    outputFile << "R" << r2 << ", ";
                    outputFile << offSet;
                    outputFile << endl;

                    numberOfInstructions++;

                } else if ((numOp == 32) & (numOffset == 0)) {     //NOP
                    unsigned const nopMask = 0x3FFFFC0;
                    unsigned nopTest = (i & nopMask);
                    if (nopTest == 0) {
                        outputFile << "NOP";
                        outputFile << endl;

                        numberOfInstructions++;
                    } else {                                      //SLL INSTRUCTION

                        unsigned const r1Mask = 0x1F0000;
                        unsigned const r2Mask = 0xF800;
                        unsigned const offMask = 0x7C0;
                        unsigned r1 = ((i & r1Mask) >> 16);
                        unsigned r2 = ((i & r2Mask) >> 11);
                        unsigned offSet = ((i & offMask) >> 6);

                        outputFile << "SLL ";
                        outputFile.width(4);
                        outputFile << "R" << r2 << ", ";
                        outputFile << "R" << r1 << ", ";
                        outputFile << "#" << offSet << "  ";
                        outputFile << endl;

                        numberOfInstructions++;
                    }
                } else if ((numOp == 32) & (numOffset == 34)) {   //SUB INSTRUCTION

                    unsigned const r1Mask = 0x3E00000;
                    unsigned const r2Mask = 0x1F0000;
                    unsigned const r3Mask = 0xF800;
                    unsigned r1 = ((i & r1Mask) >> 21);
                    unsigned r2 = ((i & r2Mask) >> 16);
                    unsigned r3 = ((i & r3Mask) >> 11);

                    outputFile << "SUB ";
                    outputFile.width(4);
                    outputFile << "R" << r3 << ", ";
                    outputFile << "R" << r1 << ", ";
                    outputFile << "R" << r2 << "  ";
                    outputFile << endl;

                    numberOfInstructions++;

                } else if (numOp == 34) {                     //JUMP/J INSTRUCTION

                    unsigned const jMask = 0x3FFFFFF;
                    unsigned const jTest = (i & jMask);
                    unsigned const jAddress = ((jTest * 4));

                    outputFile << "J";
                    outputFile.width(6);
                    outputFile << "   " << "#" << jAddress;
                    outputFile << endl;

                    numberOfInstructions++;

                } else if ((numOp == 32) & (numOffset == 13)) { //BREAK

                    outputFile << "BREAK";
                    outputFile << endl;

                    breakTest = true;

                    numberOfInstructions++;

                } else if ((numOffset == 2) & (numOp == 32)) { //SRL INSTRUCTION

                    unsigned const r1Mask = 0x1F0000;
                    unsigned const r2Mask = 0xF800;
                    unsigned const offMask = 0x7C0;
                    unsigned r1 = ((i & r1Mask) >> 16);
                    unsigned r2 = ((i & r2Mask) >> 11);
                    unsigned offSet = ((i & offMask) >> 6);

                    outputFile << "SRL ";
                    outputFile.width(4);
                    outputFile << "R" << r2 << ", ";
                    outputFile << "R" << r1 << ", ";
                    outputFile << "#" << offSet << "  ";
                    outputFile << endl;

                    numberOfInstructions++;

                } else if ((numOp == 32) & (numOffset == 8)) { //JR INSTRUCTION

                    unsigned const jrMask = 0x3E00000;
                    unsigned jrAddress = ((i & jrMask) >> 21);

                    outputFile << "JR ";
                    outputFile.width(5);
                    outputFile << "R" << jrAddress;
                    outputFile << endl;

                    numberOfInstructions++;

                } else if ((numOp == 60) & (numOffset == 2)) { //MULT INSTRUCTION

                    unsigned const r1Mask = 0x3E00000;
                    unsigned const r2Mask = 0x1F0000;
                    unsigned const r3Mask = 0xF800;
                    unsigned r1 = ((i & r1Mask) >> 21);
                    unsigned r2 = ((i & r2Mask) >> 16);
                    unsigned r3 = ((i & r3Mask) >> 11);

                    outputFile << "MULT";
                    outputFile.width(4);
                    outputFile << "R" << r3 << ", ";
                    outputFile << "R" << r1 << ", ";
                    outputFile << "R" << r2 << "  ";
                    outputFile << endl;

                    numberOfInstructions++;

                } else if ((numOp == 32) & (numOffset == 10)) { //MOVZ INSTRUCTION

                    unsigned const r1Mask = 0x3E00000;
                    unsigned const r2Mask = 0x1F0000;
                    unsigned const r3Mask = 0xF800;
                    unsigned r1 = ((i & r1Mask) >> 21);
                    unsigned r2 = ((i & r2Mask) >> 16);
                    unsigned r3 = ((i & r3Mask) >> 11);

                    outputFile << "MOVZ";
                    outputFile.width(4);
                    outputFile << "R" << r3 << ", ";
                    outputFile << "R" << r1 << ", ";
                    outputFile << "R" << r2 << "  ";
                    outputFile << endl;

                    numberOfInstructions++;

                }
            } else {
                outputFile << i;
                outputFile << endl;
                dataSize++;
            }
            startAddress += 4;
        }
        runSimulation(argv[4], inputFile);
    }
    return 0;
}

void runSimulation(char *argv, char *inputFile) {

    bool breakTest = false;

    string userOutput = argv;
    string dis = "_sim.txt";
    string file = userOutput + dis;

    ofstream outputFile(file);

    int const registersCount = 32;

    mipsInstructions = new int[numberOfInstructions];
    mipsMemory = new int[numberOfInstructions];
    registers = new int[registersCount];
    data = new int[dataSize];
    dataAddr = new int[dataSize];
    int totalMem = (dataSize + numberOfInstructions);
    int startMem = 96;

    int arrayIndex = 0;
    int dataIndex = 0;
    int count = 0;

    char buffer[4];
    int i;
    char *iPtr;
    iPtr = (char *) (void *) &i;

    int FD = open(inputFile, O_RDONLY);

    int amt = 4;
    while (amt != 0) {
        amt = read(FD, buffer, 4);
        if (amt == 4) {

            iPtr[0] = buffer[3];
            iPtr[1] = buffer[2];
            iPtr[2] = buffer[1];
            iPtr[3] = buffer[0];
        }
        if (count < numberOfInstructions) {
            mipsInstructions[arrayIndex] = i;
            mipsMemory[arrayIndex] = startMem;
            startMem += 4;
            arrayIndex++;
            count++;
        } else {
            data[dataIndex] = i;
            dataAddr[dataIndex] = startMem;
            startMem += 4;
            dataIndex++;
        }
    }

    for (int l = 0; l < 32; ++l) {
        registers[i] = 0;
    }


    int currentMem = 0;
    unsigned currentInstruction;

    unsigned numFail;
    unsigned numOp;
    unsigned numOffset;
    unsigned const mask = 4227858432;
    unsigned const maskFail = 0x80000000;
    unsigned const maskOff = 0x3F;

    int cycleCount = 1;

    while (breakTest == false) {

        currentInstruction = mipsInstructions[currentMem];
        numFail = ((currentInstruction & maskFail) >> 31);
        numOp = ((currentInstruction & mask) >> 26);
        numOffset = ((currentInstruction & maskOff));

        if (numFail == 0) {     //FAILBIT
            currentMem++;
        } else {
            if ((numOp == 32) & (numOffset == 32)) {     //ADD INSTRUCTION

                unsigned const r1Mask = 0x3E00000;
                unsigned const r2Mask = 0x1F0000;
                unsigned const r3Mask = 0xF800;
                unsigned r1 = ((currentInstruction & r1Mask) >> 21);
                unsigned r2 = ((currentInstruction & r2Mask) >> 16);
                unsigned r3 = ((currentInstruction & r3Mask) >> 11);

                outputFile << "====================" << endl;
                outputFile << "cycle:" << cycleCount << " ";
                outputFile << mipsMemory[currentMem] << "     ";
                outputFile << "ADD ";
                outputFile.width(4);
                outputFile << "R" << r3 << ", ";
                outputFile << "R" << r1 << ", ";
                outputFile << "R" << r2 << "  ";
                outputFile << endl;

                int calc = (registers[r1] + registers[r2]);
                registers[r3] = calc;

                currentMem++;
                cycleCount++;

            } else if (numOp == 40) {     //ADDI INSTRUCTION

                unsigned const r1Mask = 0x3E00000;
                unsigned const r2Mask = 0x1F0000;
                int const immMask = 0xFFFF;
                unsigned r1 = ((currentInstruction & r1Mask) >> 21);
                unsigned r2 = ((currentInstruction & r2Mask) >> 16);
                int imm = (currentInstruction & immMask);
                int immPrint = imm;

                if (imm >= 65535) {
                    immPrint = (imm - 65536);
                }

                outputFile << "====================" << endl;
                outputFile << "cycle:" << cycleCount << " ";
                outputFile << mipsMemory[currentMem] << "     ";
                outputFile << "ADDI";
                outputFile.width(4);
                outputFile << "R" << r2 << ", ";
                outputFile << "R" << r1 << ", ";
                outputFile << "#" << immPrint << " ";
                outputFile << endl;

                registers[r2] = (registers[r1] + immPrint);

                currentMem++;
                cycleCount++;

            } else if (numOp == 9) {     //ADDU INSTRUCTION (not used)

            } else if (numOp == 35) {     //LW INSTRUCTION

                unsigned const r1Mask = 0x1F0000;
                unsigned const r2Mask = 0x3E00000;
                unsigned const offSetMask = 0xFFFF;
                unsigned r1 = ((currentInstruction & r1Mask) >> 16);
                unsigned r2 = ((currentInstruction & r2Mask) >> 21);
                unsigned offSet = (currentInstruction & offSetMask);

                outputFile << "====================" << endl;
                outputFile << "cycle:" << cycleCount << " ";
                outputFile << mipsMemory[currentMem] << "     ";
                outputFile << "LW";
                outputFile.width(6);
                outputFile << "R" << r1 << ", ";
                outputFile << offSet << "(";
                outputFile << "R" << r2 << ")";
                outputFile << endl;

                int swIndex = offSet;
                swIndex = (((swIndex - 96) / 4) - numberOfInstructions);
                registers[r1] = data[swIndex + (registers[r2]/4)];

                currentMem++;
                cycleCount++;

            } else if (numOp == 43) {     //SW INSTRUCTION

                unsigned const r1Mask = 0x1F0000;
                unsigned const r2Mask = 0x3E00000;
                unsigned const offSetMask = 0xFFFF;
                unsigned r1 = ((currentInstruction & r1Mask) >> 16);
                unsigned r2 = ((currentInstruction & r2Mask) >> 21);
                unsigned offSet = (currentInstruction & offSetMask);

                outputFile << "====================" << endl;
                outputFile << "cycle:" << cycleCount << " ";
                outputFile << mipsMemory[currentMem] << "     ";
                outputFile << "SW ";
                outputFile.width(5);
                outputFile << "R" << r1 << ", ";
                outputFile << offSet << "(";
                outputFile << "R" << r2 << ")";
                outputFile << endl;

                int swIndex = offSet;
                swIndex = (((swIndex - 96) / 4) - numberOfInstructions);
                swIndex = (swIndex + (registers[r2]/4));
                data[swIndex] = registers[r1];

                currentMem++;
                cycleCount++;

            } else if (numOp == 33) {     //BLITZ INSTRUCTION

                unsigned const r1Mask = 0x3E00000;
                unsigned const offSetMask = 0xFFFF;
                unsigned r1 = ((currentInstruction & r1Mask) >> 21);
                unsigned offSet = ((currentInstruction & offSetMask) * 4);

                outputFile << "====================" << endl;
                outputFile << "cycle:" << cycleCount << " ";
                outputFile << mipsMemory[currentMem] << "     ";
                outputFile << "BLTZ";
                outputFile.width(4);
                outputFile << "R" << r1 << ", ";
                outputFile << "#" << offSet;
                outputFile << endl;

                if (registers[r1] < 0) {
                    int calc = ((mipsMemory[currentMem] + 4) + offSet);
                    calc = ((calc - 96) / 4);
                    currentMem = calc;
                } else {
                    currentMem++;
                }
                cycleCount++;
            } else if (numOp == 132) {     //BEQ INSTRUCTION

                unsigned const r1Mask = 0x3E00000;
                unsigned const r2Mask = 0x1F0000;
                unsigned const offMask = 0xFFFF;
                unsigned r1 = ((currentInstruction & r1Mask) >> 16);
                unsigned r2 = ((currentInstruction & r2Mask) >> 11);
                unsigned offSet = (currentInstruction & offMask);

                outputFile << "====================" << endl;
                outputFile << "cycle:" << cycleCount << " ";
                outputFile << mipsMemory[currentMem] << "     ";
                outputFile << "BEQ";
                outputFile.width(5);
                outputFile << "R" << r1 << ", ";
                outputFile << "R" << r2 << ", ";
                outputFile << offSet;
                outputFile << endl;

                currentMem++;
                cycleCount++;

            } else if ((numOp == 32) & (numOffset == 0)) {     //NOP
                unsigned const nopMask = 0x3FFFFC0;
                unsigned nopTest = (currentInstruction & nopMask);

                if (nopTest == 0) {
                    outputFile << "====================" << endl;
                    outputFile << "cycle:" << cycleCount << " ";
                    outputFile << mipsMemory[currentMem] << "     ";
                    outputFile << "NOP";
                    outputFile << endl;

                    currentMem++;
                    cycleCount++;

                } else {                                      //SLL INSTRUCTION

                    unsigned const r1Mask = 0x1F0000;
                    unsigned const r2Mask = 0xF800;
                    unsigned const offMask = 0x7C0;
                    unsigned r1 = ((currentInstruction & r1Mask) >> 16);
                    unsigned r2 = ((currentInstruction & r2Mask) >> 11);
                    unsigned offSet = ((currentInstruction & offMask) >> 6);

                    outputFile << "====================" << endl;
                    outputFile << "cycle:" << cycleCount << " ";
                    outputFile << mipsMemory[currentMem] << "     ";
                    outputFile << "SLL ";
                    outputFile.width(4);
                    outputFile << "R" << r2 << ", ";
                    outputFile << "R" << r1 << ", ";
                    outputFile << "#" << offSet << "  ";
                    outputFile << endl;

                    unsigned calc = (unsigned int) (registers[r1] << offSet);
                    registers[r2] = calc;

                    currentMem++;
                    cycleCount++;

                }
            } else if ((numOp == 32) & (numOffset == 34)) {   //SUB INSTRUCTION

                unsigned const r1Mask = 0x3E00000;
                unsigned const r2Mask = 0x1F0000;
                unsigned const r3Mask = 0xF800;
                unsigned r1 = ((currentInstruction & r1Mask) >> 21);
                unsigned r2 = ((currentInstruction & r2Mask) >> 16);
                unsigned r3 = ((currentInstruction & r3Mask) >> 11);

                outputFile << "====================" << endl;
                outputFile << "cycle:" << cycleCount << " ";
                outputFile << mipsMemory[currentMem] << "     ";
                outputFile << "SUB ";
                outputFile.width(3);
                outputFile << "R" << r3 << ", ";
                outputFile << "R" << r1 << ", ";
                outputFile << "R" << r2 << "  ";
                outputFile << endl;

                int calc = (registers[r1] - registers[r2]);
                registers[r3] = calc;

                currentMem++;
                cycleCount++;

            } else if (numOp == 34) {                     //JUMP/J INSTRUCTION

                unsigned const jMask = 0x3FFFFFF;
                unsigned const jTest = (currentInstruction & jMask);
                unsigned const jAddress = ((jTest * 4));

                outputFile << "====================" << endl;
                outputFile << "cycle:" << cycleCount << " ";
                outputFile << mipsMemory[currentMem] << "     ";
                outputFile << "J ";
                outputFile.width(5);
                outputFile << "   " << "#" << jAddress;
                outputFile << endl;

                int destination = ((jAddress - 96) / 4);
                currentMem = destination;

                cycleCount++;

            } else if ((numOp == 32) & (numOffset == 13)) { //BREAK

                breakTest = true;

                outputFile << "====================" << endl;
                outputFile << "cycle:" << cycleCount << " ";
                outputFile << mipsMemory[currentMem] << "     ";
                outputFile << "BREAK";
                outputFile << endl;

                currentMem++;
                cycleCount++;

            } else if ((numOffset == 2) & (numOp == 32)) { //SRL INSTRUCTION

                unsigned const r1Mask = 0x1F0000;
                unsigned const r2Mask = 0xF800;
                unsigned const offMask = 0x7C0;
                unsigned r1 = ((currentInstruction & r1Mask) >> 16);
                unsigned r2 = ((currentInstruction & r2Mask) >> 11);
                unsigned offSet = ((currentInstruction & offMask) >> 6);

                outputFile << "====================" << endl;
                outputFile << "cycle:" << cycleCount << " ";
                outputFile << mipsMemory[currentMem] << "     ";
                outputFile << "SRL ";
                outputFile.width(4);
                outputFile << "R" << r2 << ", ";
                outputFile << "R" << r1 << ", ";
                outputFile << "#" << offSet << "  ";
                outputFile << endl;

                unsigned calc = (unsigned int) (registers[r1] >> offSet);
                registers[r2] = calc;

                currentMem++;
                cycleCount++;

            } else if ((numOp == 32) & (numOffset == 8)) { //JR INSTRUCTION

                unsigned const jrMask = 0x3E00000;
                unsigned jrAddress = ((currentInstruction & jrMask) >> 21);

                outputFile << "====================" << endl;
                outputFile << "cycle:" << cycleCount << " ";
                outputFile << mipsMemory[currentMem] << "     ";
                outputFile << "JR ";
                outputFile.width(5);
                outputFile << "R" << jrAddress;
                outputFile << endl;

                currentMem++;
                cycleCount++;

            } else if ((numOp == 60) & (numOffset == 2)) { //MULT INSTRUCTION

                unsigned const r1Mask = 0x3E00000;
                unsigned const r2Mask = 0x1F0000;
                unsigned const r3Mask = 0xF800;
                unsigned r1 = ((currentInstruction & r1Mask) >> 21);
                unsigned r2 = ((currentInstruction & r2Mask) >> 16);
                unsigned r3 = ((currentInstruction & r3Mask) >> 11);

                outputFile << "====================" << endl;
                outputFile << "cycle:" << cycleCount << " ";
                outputFile << mipsMemory[currentMem] << "     ";
                outputFile << "MULT";
                outputFile.width(4);
                outputFile << "R" << r3 << ", ";
                outputFile << "R" << r1 << ", ";
                outputFile << "R" << r2 << "  ";
                outputFile << endl;

                registers[r3] = (registers[r1]*registers[r2]);

                currentMem++;
                cycleCount++;

            } else if ((numOp == 32) & (numOffset == 10)) { //MOVZ INSTRUCTION

                unsigned const r1Mask = 0x3E00000;
                unsigned const r2Mask = 0x1F0000;
                unsigned const r3Mask = 0xF800;
                unsigned r1 = ((currentInstruction & r1Mask) >> 21);
                unsigned r2 = ((currentInstruction & r2Mask) >> 16);
                unsigned r3 = ((currentInstruction & r3Mask) >> 11);

                outputFile << "====================" << endl;
                outputFile << "cycle:" << cycleCount << " ";
                outputFile << mipsMemory[currentMem] << "     ";
                outputFile << "MOVZ";
                outputFile.width(4);
                outputFile << "R" << r3 << ", ";
                outputFile << "R" << r1 << ", ";
                outputFile << "R" << r2 << "  ";
                outputFile << endl;


                int calc2 = registers[r2];
                if (calc2 == 0) {
                    registers[r3] = registers[r1];
                }

                currentMem++;
                cycleCount++;

            }
            outputFile << endl;
            outputFile << "registers:" << endl;
            outputFile << "r00:";
            for (int j = 0; j < 8; ++j) {
                outputFile.width(4);
                outputFile << registers[j];
            }
            outputFile << endl;
            outputFile << "r08:";
            for (int j = 0; j < 8; ++j) {
                outputFile.width(4);
                outputFile << registers[j + 8];
            }
            outputFile << endl;
            outputFile << "r16:";
            for (int j = 0; j < 8; ++j) {
                outputFile.width(4);
                outputFile << registers[j + 16];
            }
            outputFile << endl;
            outputFile << "r24:";
            for (int j = 0; j < 8; ++j) {
                outputFile.width(4);
                outputFile << registers[j + 24];
            }
            outputFile << endl;
            outputFile << endl;
            outputFile << "data:";
            outputFile << endl;
            int printStop = (dataSize - 1);
            int dataIndex2 = 0;
            while (printStop > 0) {
                outputFile << dataAddr[dataIndex2] << ":";
                for (int j = 0; j < 8; ++j) {
                    if (printStop <= 0) {
                        break;
                    } else {
                        outputFile.width(4);
                        outputFile << data[dataIndex2];
                        dataIndex2++;
                        printStop--;
                    }
                }
                outputFile << endl;
            }
            outputFile << endl;
        }
    }
}