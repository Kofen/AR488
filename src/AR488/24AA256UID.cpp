#include "24AA256UID.h"
#include "AR488_ComPorts.h"

_24AA256UID::_24AA256UID(uint8_t address, bool pinswap, bool debug) : deviceAddress(address), pinswap(pinswap), debugEnabled(debug) {}

void _24AA256UID::begin() {
    //Wire.swap(pinswap);
    Wire.begin();
}

void _24AA256UID::getMACAddress(uint8_t* mac) {
    // Get MAC address EUI-48
    readBytes(0x7F7A, mac, 6);
    printDebug("MAC Address read.");
}

void _24AA256UID::getUniqueID(uint8_t* uid) {
    // Read unique ID 32 bit serial
    readBytes(0x7FFA, uid, 4);
    printDebug("Unique ID read.");
}

uint8_t _24AA256UID::readByte(uint16_t address) {
    Wire.beginTransmission(deviceAddress);
    Wire.write((address >> 8) & 0xFF);
    Wire.write(address & 0xFF);
    Wire.endTransmission();
    Wire.requestFrom(deviceAddress, 1);
    return Wire.available() ? Wire.read() : 0xFF; // Return 0xFF if no data is available
}

void _24AA256UID::writeByte(uint16_t address, uint8_t data) {
    Wire.beginTransmission(deviceAddress);
    Wire.write((address >> 8) & 0xFF);
    Wire.write(address & 0xFF);
    Wire.write(data);
    Wire.endTransmission();
    delay(10);
    printDebug("Byte written.");
}

void _24AA256UID::readBytes(uint16_t address, uint8_t* buffer, uint16_t length) {
    Wire.beginTransmission(deviceAddress);
    Wire.write((address >> 8) & 0xFF);
    Wire.write(address & 0xFF);
    Wire.endTransmission();
    Wire.requestFrom(deviceAddress, length);
    for (uint16_t i = 0; i < length && Wire.available(); i++) {
        buffer[i] = Wire.read();
    }
    printDebug("Bytes read.");
}

void _24AA256UID::writeBytes(uint16_t address, const uint8_t* buffer, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        writeByte(address + i, buffer[i]);
    }
    printDebug("Bytes written.");
}

void _24AA256UID::writePage(uint16_t address, const uint8_t* data, uint8_t length) {
    const uint8_t PAGE_SIZE = 32;
    if (length > PAGE_SIZE) {
        printDebug("Error: Length exceeds page size!");
        return;
    }

    uint16_t endAddress = address + length - 1;
    if ((address / PAGE_SIZE) != (endAddress / PAGE_SIZE)) {
        printDebug("Error: Write operation crosses page boundary!");
        return;
    }

    Wire.beginTransmission(deviceAddress);
    Wire.write((address >> 8) & 0xFF);
    Wire.write(address & 0xFF);
    for (uint8_t i = 0; i < length; i++) {
        Wire.write(data[i]);
    }
    Wire.endTransmission();
    delay(10);
    printDebug("Page written within boundary.");
}

void _24AA256UID::readPage(uint16_t address, uint8_t* data) {
    const uint8_t PAGE_SIZE = 32;
    if (address % PAGE_SIZE != 0) {
        printDebug("Error: Address is not page aligned!");
        return;
    }

    Wire.beginTransmission(deviceAddress);
    Wire.write((address >> 8) & 0xFF);
    Wire.write(address & 0xFF);
    Wire.endTransmission();
    Wire.requestFrom(deviceAddress, PAGE_SIZE);
    for (uint8_t i = 0; i < PAGE_SIZE && Wire.available(); i++) {
        data[i] = Wire.read();
    }
    printDebug("Page read. ");
}

void _24AA256UID::printDebug(const String& message) {
    if (debugEnabled) {
        DB_RAW_PRINTLN(message);
    }
}

