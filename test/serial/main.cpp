#include <iostream>

#include <opensbv/serial/Serialrw.h>

#include <unistd.h>

int main() {
    std::cout << "Hello, World!" << std::endl;

    SerialRW serial("/dev/ttySAC4","/dev/ttySAC3", 38400);

    std::string writed = "blablabla";
    std::string read;

    serial.Write(writed);

    std::cout << "write: " << writed << std::endl;

//    read = serial.Read(10);

    std::cout << "read: " << serial.Read(10) << std::endl;

    return 0;
}