class IF_ID
{
public :
    IF_ID();
    void incPC();
    void addToPC(int offest);
    void storeInstruction(char* inst);
    int getPC();
    char* getInstruction();
private:
    int PC = 0;
    char *insruction = "00000000000000000000000000000000";
};

IF_ID::IF_ID()
{

}

void IF_ID::incPC()
{
    PC = PC + 4;
}

void IF_ID::storeInstruction(char* inst)
{
    insruction = inst;
}

void IF_ID::addToPC(int offest)
{
    PC = PC + offest;
}

int IF_ID::getPC()
{
    return PC;
}

char* IF_ID::getInstruction()
{
    return insruction;
}
