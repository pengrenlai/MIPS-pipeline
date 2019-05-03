class DataMerory
{
public:
    int readData(int addr);
private:
    int _00 = 5;
    int _04 = 5;
    int _08 = 6;
    int _12 = 8;
    int _16 = 8;
};

int DataMerory::readData(int addr)
{
    switch(addr)
    {
    case 0:
        return _00;
    case 4:
        return _04;
    case 8:
        return _08;
    case 12:
        return _12;
    case 16:
        return _16;

    default:
        return 0;
    }
}
