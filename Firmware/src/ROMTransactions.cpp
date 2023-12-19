#include "ROMTransactions.h"

ROMTransactions::ROMTransactions(int eeprom_size) {
    EEPROM.begin(eeprom_size);
}

void ROMTransactions::WriteInt(int val, int addr) {
    byte byte0 = val >> 8;
  
    EEPROM.write(addr, byte0);

    byte byte1 = val & 0xFF;
    EEPROM.write(addr + 1, byte1);
    EEPROM.commit();
}

int ROMTransactions::ReadInt(int addr) {
    byte byte0 = EEPROM.read(addr);
    byte byte1 = EEPROM.read(addr + 1);
    return (byte0 << 8) + byte1;
}