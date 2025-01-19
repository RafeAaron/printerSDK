#include "printer.cpp"

int main()
{

    Printer printer(0, (char*)"66:32:35:A0:34:23");

    
    if(!printer.connectToDevice()){
        cout << "There was an error connecting to the device" << endl;
        return -1;
    }

    //printer.printHeading((unsigned char*)"Invoice Number: 39", 18, false, false, false, false, false);

    int width = 160;
    int height = 200;

    unsigned char data[width * height];

    for(int e = 0; e < (width / 8) * height; e++)
    {
        data[e] = 0xFF;
    }

    //printer.defineNVLogo(128, 32, data);

    //printer.printGratingLogo(data, width, height);

    unsigned char message[9];
    
    message[0] = 'H';
    message[1] = 'E';
    message[2] = 'L';
    message[3] = 'L';
    message[4] = 'O';
    message[5] = ',';
    message[6] = ' ';
    message[7] = 'W';
    message[8] = 'O';

    unsigned char barCodeData[12];

    for(int a = 0; a < 12; a++)
    {
        barCodeData[a] = 0x32;
    }
    
    //printer.setHeightOfBarCode(100);

    //printer.printBarCode(barCodeData, 12);

    printer.printQRCode(8, 0, 9, message);

    printer.disconnectFromDevice();

    return 0;
}