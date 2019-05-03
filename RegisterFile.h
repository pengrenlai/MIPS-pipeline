class RegisterFile
{
public:
    int readData(int rNumber);
    void writeData(int rNumber, int wData);
private:
    int $0 = 0;
    int $1 = 8;
    int $2 = 7;
    int $3 = 6;
    int $4 = 3;
    int $5 = 9;
    int $6 = 5;
    int $7 = 2;
    int $8 = 7;
};

int RegisterFile::readData(int rNumber)
{
    switch(rNumber)
    {
        case 0:
            return $0;
        case 1:
            return $1;
        case 2:
            return $2;
        case 3:
            return $3;
        case 4:
            return $4;
        case 5:
            return $5;
        case 6:
            return $6;
        case 7:
            return $7;
        case 8:
            return $8;
        default:
            return 0;
    }
}

void RegisterFile::writeData(int rNumber, int wData)
{
    switch(rNumber)
    {
        case 0:
            $0 = wData;
            break;
        case 1:
            $1 = wData;
            break;
        case 2:
            $2 = wData;
            break;
        case 3:
            $3 = wData;
            break;
        case 4:
            $4 = wData;
            break;
        case 5:
            $5 = wData;
            break;
        case 6:
            $6 = wData;
            break;
        case 7:
            $7 = wData;
            break;
        case 8:
            $8 = wData;
            break;
    }
}
