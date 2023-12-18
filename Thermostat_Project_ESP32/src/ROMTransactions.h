#ifndef ROMTRANSACTION_H
#define ROMTRANSACTION_H
#include <EEPROM.h>

class ROMTransactions {
public:
    ROMTransactions(int eeprom_size);
    void WriteInt(int val, int addr);
    int ReadInt(int addr);
private:
    int size;
};

#endif