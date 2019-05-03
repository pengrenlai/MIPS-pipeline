class MEM_WB
{
public:
    MEM_WB();
    void storeReadData(int data);
    void storeALUout(int out);
    void storeControl(char* con);
    void storeRdes(int rdes);
    int getReadData();
    int getALUout();
    char* getControl();
    int getRdes();
private:
    int readData = 0;
    int ALUout = 0;
    char* control = "00";;
    int Rdes = 0;
};

MEM_WB::MEM_WB()
{

}

void MEM_WB::storeReadData(int data)
{
    readData = data;
}

void MEM_WB::storeALUout(int out)
{
    ALUout = out;
}

void MEM_WB::storeControl(char* con)
{
    control = con;
}

void MEM_WB::storeRdes(int rdes)
{
    Rdes = rdes;
}

int MEM_WB::getReadData()
{
    return readData;
}

int MEM_WB::getALUout()
{
    return ALUout;
}

char* MEM_WB::getControl()
{
    return control;
}

int MEM_WB::getRdes()
{
    return Rdes;
}
