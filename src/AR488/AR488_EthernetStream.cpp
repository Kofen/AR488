#include "AR488_EthernetStream.h"


EthernetStream::EthernetStream(byte* mac, IPAddress ip, uint16_t port)
    : server(port), mac(mac), ip(ip), port(port), lastActivityTime(0), timeout(10000) {}


void EthernetStream::begin() {
        

    Ethernet.init(7);
    if (ip == IPAddress(0, 0, 0, 0)) {
        // Use DHCP
        Ethernet.begin(mac);
    } 
    else {
        // Use static IP
        Ethernet.begin(mac, ip);
    }
    
    server.begin();
    lastActivityTime = millis();
}

void EthernetStream::checkClient() {
    if (client && !client.connected()) {
        client.stop();
        client = EthernetClient();
    }
    if (!client) {
        client = server.available();
        if (client) {
        }
    }
    if (client) {
        lastActivityTime = millis();
    }

}int EthernetStream::available() {
    checkClient();
    if (!client) {
        client = server.available();
    }
    if (client) {
        return client.available();
    }
    return 0;
}

int EthernetStream::read() {
    checkClient();
    if (client) {
        return client.read();
    }
    return -1;
}

int EthernetStream::peek() {
    checkClient();
    if (client) {
        return client.peek();
    }
    return -1;
}

void EthernetStream::flush() {
    if (client) {
        client.flush();
    }
}

size_t EthernetStream::write(uint8_t b) {
    if (client) {
        buffer += static_cast<char>(b);
        if (b == '\n') {
            client.print(buffer);
            buffer = "";
        }
        return 1;
    }
    return 0;
}

void EthernetStream::maintain() {
    unsigned long currentMillis = millis();
    if (client && (currentMillis - lastActivityTime > timeout)) {
        client.stop();
        client = EthernetClient();
    }
}
