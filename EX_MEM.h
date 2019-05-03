class EX_MEM
{
public:
    EX_MEM();
    void storeALUout(int out);
    void storeWriteData(int data);
    void storeRdes(int rdes);
    void storeControl(char* con);
    void setDestination(int des);
    int getALUout();
    int getWriteData();
    int getRdes();
    char* getControl();
    int getDestination();
private:
    int ALUout = 0;
    int writeData = 0;
    int Rdes = 0;          //¥Nªídesitination
    int destination = 0; // 0:Rt, 1:Rd
    char* control = "00000";
};

EX_MEM::EX_MEM()
{

}

void EX_MEM::storeALUout(int out)
{
    ALUout = out;
}

void EX_MEM::storeWriteData(int data)
{
    writeData = data;
}

void EX_MEM::storeRdes(int rdes)
{
    Rdes = rdes;
}

void EX_MEM::storeControl(char* con)
{
    control = con;
}

void EX_MEM::setDestination(int des)
{
    destination = des;
}

int EX_MEM::getALUout()
{
    return ALUout;
}

int EX_MEM::getWriteData()
{
    return writeData;
}

int EX_MEM::getRdes()
{
    return Rdes;
}

char* EX_MEM::getControl()
{
    return control;
}

int EX_MEM::getDestination()
{
    return destination;
}
