#include<string>
using namespace std;

class ID_EX
{
public:
    ID_EX();
    void storeReadData1(int data);
    void storeReadData2(int data);
    void storeSign_ext(int sign);
    void storeRs(int rs);
    void storeRt(int rt);
    void storeRd(int rd);
    void storeControl(char* con);
    void storeFunc(char* f);
    int getReadData1();
    int getReadData2();
    int getSign_ext();
    int getRs();
    int getRt();
    int getRd();
    char* getControl();
    char* getFunc();
private:
    int readData1 = 0;
    int readData2 = 0;
    int sign_ext = 0;
    int Rs = 0;
    int Rt = 0;
    int Rd = 0;
    char* control = "000000000";
    char* func = "000000";
};

ID_EX::ID_EX()
{

}

void ID_EX::storeReadData1(int data)
{
    readData1 = data;
}

void ID_EX::storeReadData2(int data)
{
    readData2 = data;
}


void ID_EX::storeSign_ext(int sign)
{
    sign_ext = sign;
}


void ID_EX::storeRs(int rs)
{
    Rs = rs;
}


void ID_EX::storeRt(int rt)
{
    Rt = rt;
}

void ID_EX::storeRd(int rd)
{
    Rd = rd;
}

void ID_EX::storeControl(char* con)
{
    control = con;
}

void ID_EX::storeFunc(char* f)
{
    func = f;
}

int ID_EX::getReadData1()
{
    return readData1;
}

int ID_EX::getReadData2()
{
    return readData2;
}

int ID_EX::getSign_ext()
{
    return sign_ext;
}

int ID_EX::getRs()
{
    return Rs;
}

int ID_EX::getRt()
{
    return Rt;
}

int ID_EX::getRd()
{
    return Rd;
}

char* ID_EX::getControl()
{
    return control;
}

char* ID_EX::getFunc()
{
    return func;
}
