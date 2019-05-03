#include<iostream>
#include<fstream>
#include <string.h>
#include <stdlib.h>
#include "IF_ID.h"
#include "ID_EX.h"
#include "EX_MEM.h"
#include "MEM_WB.h"
#include "RegisterFile.h"
#include "DataMemory.h"
using namespace std;

void determineField(char* inst); // 切割instruction，決定各個欄位
char* getControl(char* op);     // 取得control signal
char* getALUOp(char* control); // 取得ALUOp
int getALUControl(char* ALUOp, char* func); // 決定運算方式
char* getCuttedControl(char* control, int length, int start); // 切割control
int getFoward(int id_exReg, int ex_memReg, int mem_wbReg, char ex_memWrite, char mem_wbWrite);//決定foward訊號，0:ID/EX(00), 1:MEM/WB(01), 2:EX/MEM(10)
// hazard detection unit
int getStallSignal(int id_rs, int id_rt, int lw_rt, char memRead); // stall signal, 1:stall

char *op, *rs, *rt, *rd, *addr_imm, *func; // for stage 2 and 3
const char *beq = "000100";
const char *lw = "100011";
const char *ori = "001101";
const char *addi = "001000";
const char *R_type = "000000";

const char *init = "00000000000000000000000000000000000000";

int main(){

    char *line = new char[32];
    ifstream fin("Branchazard.txt");
    ofstream fout("branchResult.txt");

    op = new char[6];
    rs = new char[5];
    rt = new char[5];
    rd = new char[5];
    addr_imm = new char[16];
    func = new char[6];

    IF_ID mIF_ID;
    ID_EX mID_EX;
    EX_MEM mEX_MEM;
    MEM_WB mMEM_WB;

    RegisterFile registerFile;
    DataMerory dataMemory;

    int CC = 1;//clock cycle
    int endC = 0; // end flag

    int readData1, readData2;// for stage 2

    int operand1,operand2; // for stage 3
    int ALUOut;
    int destination;

    int writeData; // for stage 5，can serve as input of forwarding unit

    bool stall = false;

    // forwarding input, record the values of MEM/WB register temporarily
    char* conForForwarding = new char[2];
    int RdesForForwarding;

    // for branch
    int IF_IDFlush = 0;
    int branchTaken = 0;
    int offest = 0;

    while(true)
    {
//stage 5
        conForForwarding = mMEM_WB.getControl();
        RdesForForwarding = mMEM_WB.getRdes();

        if(mMEM_WB.getControl()[0] == '1')// RegWrite
        {
            if(mMEM_WB.getControl()[1] == '0') // MemtoReg
            {
                writeData = mMEM_WB.getALUout();
            }
            else
            {
                writeData = mMEM_WB.getReadData();
            }

            registerFile.writeData(mMEM_WB.getRdes(), writeData);
        }

//stage 4
        if(mEX_MEM.getControl()[1] == '0')
        {
            mMEM_WB.storeReadData(0);
        }
        else
        {
            int data = dataMemory.readData(mEX_MEM.getALUout());
            mMEM_WB.storeReadData(data);
        }

        mMEM_WB.storeALUout(mEX_MEM.getALUout());

        mMEM_WB.storeControl(getCuttedControl(mEX_MEM.getControl(), 2, 3));

        mMEM_WB.storeRdes(mEX_MEM.getRdes());

//stage 3

        // 處理fowarding的邏輯，不要從MEM/WB直接取值，因為MEM/WB已經被改變了

        switch(getFoward(mID_EX.getRs(), mEX_MEM.getRdes(), RdesForForwarding, mEX_MEM.getControl()[3], conForForwarding[0]))
               {
                case 0:
                    operand1 = mID_EX.getReadData1();
                    break;
                case 1:
                    operand1 = writeData; // 使用尚未被stage 4影響過的值
                    break;
                case 2:
                    operand1 = mEX_MEM.getALUout();
                    break;
               }


        switch(getFoward(mID_EX.getRt(), mEX_MEM.getRdes(), RdesForForwarding, mEX_MEM.getControl()[3], conForForwarding[0]))
                {
                case 0:
                    if(mID_EX.getControl()[3] == '0') // ALUSrc決定第二個運算元
                    {
                        operand2 = mID_EX.getReadData2();
                    }
                    else
                    {
                        operand2 = mID_EX.getSign_ext();
                    }
                    break;
                case 1:
                    operand2 = writeData;
                    break;
                case 2:
                    operand2 = mEX_MEM.getALUout();
                    break;
                }

        switch(getALUControl(getALUOp(mID_EX.getControl()), mID_EX.getFunc())) // 依據ALUControl決定運算方式
        {
        case 0: // Add
            ALUOut = operand1 + operand2;
            break;
        case 1: // Sub
            ALUOut = operand1 - operand2;
            break;
        case 2: // And
            ALUOut = operand1 & operand2;
            break;
        case 3: // Or
            ALUOut = operand1 | operand2;
            break;
        case -1:
            size_t l;
            l = strcmp(op, addi);
            if(l == 0)
            {
                ALUOut = operand1 + operand2;
            }

            l = strcmp(op, ori);
            if(l == 0)
            {
                ALUOut = operand1 | operand2;
            }
            break;
        }

        mEX_MEM.storeALUout(ALUOut);

        if(mID_EX.getControl()[0] == '0') // RegDst
        {
            destination = mID_EX.getRt();
            mEX_MEM.setDestination(0);
        }
        else
        {
            destination = mID_EX.getRd();
            mEX_MEM.setDestination(1);
        }
        mEX_MEM.storeRdes(destination);

        mEX_MEM.storeWriteData(mID_EX.getReadData2());

        mEX_MEM.storeControl(getCuttedControl(mID_EX.getControl(), 5, 4));

//stage 2
        if(IF_IDFlush == 1) // flush if branch taken
        {
            mIF_ID.storeInstruction("00000000000000000000000000000000");
            IF_IDFlush = 0;
        }

        determineField(mIF_ID.getInstruction());

        int demRs = strtol(rs, NULL, 2);
        int demRt = strtol(rt, NULL, 2);
        int demRd = strtol(rd, NULL, 2);

        if(getStallSignal(demRs, demRt, mID_EX.getRt(), mID_EX.getControl()[5]) == 1) // stall
        {
            stall = true; // stall PC and IF/ID
            // flush
            mID_EX.storeControl("000000000");
            mID_EX.storeFunc("000000");
            mID_EX.storeReadData1(0);
            mID_EX.storeReadData2(0);
            mID_EX.storeRs(0);
            mID_EX.storeRt(0);
            mID_EX.storeRd(0);
            mID_EX.storeSign_ext(0);
        }
        else
        {

            readData1 = registerFile.readData(demRs);
            readData2 = registerFile.readData(demRt);

            mID_EX.storeReadData1(readData1);
            mID_EX.storeReadData2(readData2);
            mID_EX.storeRs(demRs);
            mID_EX.storeRt(demRt);
            mID_EX.storeRd(demRd);
            mID_EX.storeSign_ext(strtol(addr_imm, NULL, 2));
            mID_EX.storeControl(getControl(op));
            mID_EX.storeFunc(func);
        }

// stage 1

    if(!stall)
    {
        if(branchTaken == 0)
        {
            if(!fin.eof())
            {
                fin.getline(line, 33);
            }
            else
            {
                strncpy(line, init, 32);
                line[32] = '\0';
                endC++;
            }

                mIF_ID.storeInstruction(line);
                mIF_ID.incPC();
        }
        else
        {
            mIF_ID.incPC();
            mIF_ID.addToPC(offest * 4);
            for(int i = 0 ; i < offest ; i++)
            {
                fin.getline(line, 33);
            }
            mIF_ID.storeInstruction(line);
            branchTaken = 0;
        }

    }
    else
    {
        stall = false;
    }

//print or output to the file

        fout<<"CC "<<CC<<":"<<endl<<endl;
        fout<<"Register:"<<endl;
        for(int i = 0 ; i < 9 ; i++)
        {
            fout<<"$"<<i<<": "<<registerFile.readData(i)<<"   ";
            if(i % 3 == 2)
                fout<<endl;
        }
        fout<<endl;
        fout<<"Data memory:"<<endl;
        for(int i = 0 ; i <= 16 ; i += 4)
        {
            if(i < 10)
                fout<<"0"<<i<<":     "<<dataMemory.readData(i)<<endl;
            else
                fout<<i<<":     "<<dataMemory.readData(i)<<endl;
        }
        fout<<endl;
        fout<<"IF/ID :"<<endl;
        fout<<"PC		"<<mIF_ID.getPC()<<endl;
        fout<<"Instruction	"<<mIF_ID.getInstruction()<<endl;

        fout<<endl;
        fout<<"ID/EX :"<<endl;
        fout<<"ReadData1	"<<mID_EX.getReadData1()<<endl;
        fout<<"ReadData2	"<<mID_EX.getReadData2()<<endl;
        fout<<"sign_ext	"<<mID_EX.getSign_ext()<<endl;
        fout<<"Rs		"<<mID_EX.getRs()<<endl;
        fout<<"Rt		"<<mID_EX.getRt()<<endl;
        fout<<"Rd		"<<mID_EX.getRd()<<endl;
        fout<<"Control signals	"<<mID_EX.getControl()<<endl;

        fout<<endl;
        fout<<"EX/MEM :"<<endl;
        fout<<"ALUout		"<<mEX_MEM.getALUout()<<endl;
        fout<<"WriteData	"<<mEX_MEM.getWriteData()<<endl;
        if(mEX_MEM.getDestination() == 0)
            fout<<"Rt		"<<mEX_MEM.getRdes()<<endl;
        else
            fout<<"Rd		"<<mEX_MEM.getRdes()<<endl;
        fout<<"Control signals	"<<mEX_MEM.getControl()<<endl;

        fout<<endl;
        fout<<"MEM/WB :"<<endl;
        fout<<"ReadData	"<<mMEM_WB.getReadData()<<endl;
        fout<<"ALUout		"<<mMEM_WB.getALUout()<<endl;
        fout<<"Control signals	"<<mMEM_WB.getControl()<<endl;
        fout<<"================================================================="<<endl;
//處理分支
        size_t cmp;
        cmp = strcmp(op, beq);
        if(cmp == 0)
        {
            if(readData1 == readData2)
            {
                IF_IDFlush = 1;
                branchTaken = 1;
                offest = strtol(addr_imm, NULL, 2);
            }
        }
//

        if(endC == 4)
            break;
        CC++;

    }

    return 0;
}

void determineField(char* inst)
{

    strncpy(op, inst, 6);
    op[6] = '\0';

    size_t loc;

    loc = (strcmp(op, beq))&(strcmp(op, lw))&(strcmp(op, ori))&(strcmp(op, addi));
    if(loc == 0)
    {
        strncpy(rs, inst + 6, 5);
        strncpy(rt, inst + 11, 5);
        strncpy(addr_imm, inst + 16, 16);
        addr_imm[16] = '\0';
        strncpy(rd, init, 5);
        strncpy(func, init, 6);
        return;
    }

    loc = strcmp(op, R_type);
    if(loc == 0)
    {
        strncpy(rs, inst + 6, 5);
        strncpy(rt, inst + 11, 5);
        strncpy(rd, inst + 16, 5);
        strncpy(func, inst + 26, 6);
        func[6] = '\0';
        strncpy(addr_imm, init, 16);
        addr_imm[16] = '\0';
        return;
    }

    return;
}

char* getControl(char* op)
{
    char* control = new char[9];
    size_t loc;

    loc = (strcmp(op, "000000")) | (strcmp(rs, "00000")) | (strcmp(rt, "00000")) |
     (strcmp(rd, "00000"))| (strcmp(func, "000000")) | (strcmp(addr_imm, "0000000000000000")); // 先確認是否有指令，避免與R-type搞混，32個0表示沒有指令
    if(loc == 0)
    {
        control = "000000000";
        return control;
    }

    loc = strcmp(op, R_type);
    if(loc == 0)
    {
        control = "110000010";
        return control;
    }

    loc = strcmp(op, lw);
    if(loc == 0)
    {
        control = "000101011";
        return control;
    }

    loc = strcmp(op, beq);
    if(loc == 0)
    {
        control = "001010000";
        return control;
    }

    loc = (strcmp(op, ori))&(strcmp(op, addi));
    if(loc == 0)
    {
        control = "010100010";
        return control;
    }

}

char* getALUOp(char* control)
{
    char* ALUOp = new char[2];

    strncpy(ALUOp, control + 1, 2);
    ALUOp[2] = '\0';

    return ALUOp;
}

int getALUControl(char* ALUOp, char* func)
{
    int ALUCon = -1;// Add:0,Sub:1,And:2,Or:3

    size_t loc, locFunc;

    loc = strcmp(ALUOp, "00");
    if(loc == 0) // Add
    {
        ALUCon = 0;
        return ALUCon;
    }

    loc = strcmp(ALUOp, "01");
    if(loc == 0) // Sub
    {
        ALUCon = 1;
        return ALUCon;
    }

    loc = strcmp(ALUOp, "10");
    if(loc == 0) // determine by func
    {
        locFunc = strcmp(func, "100000");
        if(locFunc == 0)
        {
            ALUCon = 0;
            return ALUCon;
        }

        locFunc = strcmp(func, "100010");
        if(locFunc == 0)
        {
            ALUCon = 1;
            return ALUCon;
        }

        locFunc = strcmp(func, "100100");
        if(locFunc == 0)
        {
            ALUCon = 2;
            return ALUCon;
        }

        locFunc = strcmp(func, "100101");
        if(locFunc == 0)
        {
            ALUCon = 3;
            return ALUCon;
        }
    }

    loc = strcmp(ALUOp, "11");
    if(loc == 0) // Or
    {
        ALUCon = 3;
        return ALUCon;
    }
}

char* getCuttedControl(char* control, int length, int start)
{
        char* con = new char[length];
        strncpy(con, control + start, length);
        con[length] = '\0';
        return con;
}

int getFoward(int id_exReg, int ex_memReg, int mem_wbReg, char ex_memWrite, char mem_wbWrite)
{
    int foward = 0;

    if(ex_memWrite == '1' && ex_memReg != 0 && ex_memReg == id_exReg)
        foward = 2;
    if(mem_wbWrite == '1' && mem_wbReg != 0 && mem_wbReg == id_exReg)
        foward = 1;

    return foward;
}

int getStallSignal(int id_rs, int id_rt, int lw_rt, char memRead)
{
    int stall = 0;
    if(memRead == '1' && (lw_rt == id_rs || lw_rt == id_rt))
        stall = 1;

    return stall;
}
