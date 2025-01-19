#include "printer.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

using namespace std;

Printer::Printer(int connectionMethod, char* bluetoothAddress)
{

    if(connectionMethod == 0)
    {
        this->bluetoothAddress = bluetoothAddress;
        sockaddr_rc addr = {0};

        int mySocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

        addr.rc_family = AF_BLUETOOTH;
        addr.rc_channel = (uint8_t) 1;
        str2ba(bluetoothAddress, &addr.rc_bdaddr);

        this->bluetoothAddressTechnical = addr;
        this->bluetoothSocket = mySocket;

        cout << "Attributes initialized fully" << endl;
    }

    else{
        cout << "To be made later" << endl;
    }
}

bool Printer::connectToDevice()
{
    bool connected = false;

    if(this->connectedToSocket)
    {
        return connected;
    }

    if(connect(this->bluetoothSocket, (sockaddr *)&this->bluetoothAddressTechnical, sizeof(sockaddr)) == 0)
    {

        cout << "Connected To Bluetooth Device" << endl;

        connected = true;
        this->connectedToSocket = true;
        return connected;

    }
    else{
        cout << "Failed To Connect to Bluetooth Device" << endl;
        return connected;
    }
}

bool sendDataToPrinter(unsigned char* data, int dataLength, bool connectedToSocket, int bluetoothSocket)
{

if(!connectedToSocket)
{
    cout << "First connect to a bluetooth device" << endl;
    return false;
}
else{

    cout << "Data being sent to the printer" << endl;
    for(int e = 0; e < dataLength; e++){
        cout << (int)data[e] << " ";
        if(e == dataLength - 1) cout << endl;
    }

    //int commandSuccess = send(this->bluetoothSocket, command, commandLength, 0);
    int success = send(bluetoothSocket, data, dataLength, 0);

    if(success == -1){
        cout << "There was an error sending data to the printer" << endl;
        return false;
    }else{
        cout << "Number of bytes sent to device: " << success <<" bytes" << endl;
    }
}

    return true;
}

bool Printer::disconnectFromDevice()
{

    if(this->connectedToSocket)
    {
        close(this->bluetoothSocket);
        this->bluetoothAddressTechnical = {0};
        this->bluetoothSocket = -1;
        this->connectedToSocket = false;

        cout << "Successfully disconnected from device" << endl;
        
        return true;
    }
    else{

        cout << "The device is not currently connected" << endl;
        return false;
    }
}

bool Printer::printHeading(unsigned char* data, int dataLength, bool bold, bool characterFontSelection, bool doubleHeight, bool doubleWidth, bool underline)
{

    uint8_t printMode = 0;
    if(underline)
    {
        printMode += 128;
    }

    cout << (int)printMode << " ";

    if(doubleWidth)
    {
        printMode += 32;
    }
    cout << (int)printMode << " ";

    if(doubleHeight)
    {
        printMode += 16;
    }
    cout << (int)printMode << " ";

    if(bold)
    {
        printMode += 8;
    }
    cout << (int)printMode << " ";

    if(characterFontSelection)
    {
        printMode += 1;
    }
    cout << (int)printMode << endl;

    unsigned char printModeCommand[4];
    printModeCommand[0] = this->lineFeed;
    printModeCommand[1] = selectPrintMode.beginning;
    printModeCommand[2] = selectPrintMode.middle;
    printModeCommand[3] = (unsigned char)printMode;

    if(!sendDataToPrinter(printModeCommand, 4, this->connectedToSocket, this->bluetoothSocket))
    {
        cout << "Print Mode not set. Error" << endl;
        return false;
    }
    else{
        return sendDataToPrinter(data, dataLength, this->connectedToSocket, this->bluetoothSocket);
    } 

}

bool Printer::printGratingLogo(unsigned char* monochromeData, int width, int height)
{

    int remainder = 0;

    if(width > 168 || height > 200 || width < 1 || height < 100)
    {
        if(width > 168) cout << "Width is too big. Keep between 1 - 168" << endl;
        if(width < 1) cout << "Width is too small. Keep between 1 - 168" << endl;
        if(height < 1) cout << "Height is too small. Keep between 1 - 200" << endl;
        if(height > 200) cout << "Height is too big. Keep between 1 - 200" << endl;

        return false;
    }

    int totalSize = (width / 8) * height;

    unsigned char dataToPrint[totalSize];

    for(int a= 0; a < totalSize; a++)
    {
        dataToPrint[a] = monochromeData[a];
    }

    unsigned printingCommand[8];

    cout << "Width: " << (int)(width / 8) << ", Height: " << (int)height << endl;

    printingCommand[0] = 0x1D;
    printingCommand[1] = 0x76;
    printingCommand[2] = 0x30;
    printingCommand[3] = 0x00;
    printingCommand[4] = width / 8;
    printingCommand[5] = 0x00;
    printingCommand[6] = height;
    printingCommand[7] = 0x00;

    for(int b = 0; b < 8; b++)
    {
        cout << (int)printingCommand[b] << " ";
    }
    
    cout << endl;

    int finalSize;
    
    finalSize = totalSize + 8;

    unsigned char dataFinal[finalSize];

    for(int a = 0; a < finalSize; a++)
    {
        if(a < 8)
        {
            dataFinal[a] = printingCommand[a];
        }else{
            dataFinal[a] = dataToPrint[a - 8];
        }
    }

    sendDataToPrinter(dataFinal, finalSize, this->connectedToSocket, this->bluetoothSocket);

    cout << endl;

    return true;
}

bool Printer::printQRCode(unsigned char magnification, unsigned char ecLevel, uint16_t dataLength, unsigned char* data)
{

    int finalSize = 7 + dataLength;

    unsigned char printQRCodeCommand[finalSize];

    printQRCodeCommand[0] = QRCode.beginning;
    printQRCodeCommand[1] = QRCode.middle;

    printQRCodeCommand[2] = 0x00;
    printQRCodeCommand[3] = ecLevel;

    printQRCodeCommand[4] = magnification;
    printQRCodeCommand[5] = dataLength;

    for(int i = 0; i < dataLength; i++)
    {
        printQRCodeCommand[7 + i] = data[i];
    }

    for(int a = 0; a < finalSize; a++)
    {
        cout << (int)printQRCodeCommand[a] << " ";
    }

    cout << endl;

    if(!sendDataToPrinter(printQRCodeCommand, finalSize, this->connectedToSocket, this->bluetoothSocket))
    {
        cout << endl << "Data failed to be sent to printer" << endl;
        return false;
    }
    else{
        cout << endl << "Data successfully sent to the printer" << endl;
    }

    return true;

    }

bool Printer::printBarCode(unsigned char* data, int lengthOfData)
{

    unsigned char barCodeData[35];

    barCodeData[0] = 0x55;
    barCodeData[1] = 0x50;
    barCodeData[2] = 0x43;
    barCodeData[3] = 0x5F;
    barCodeData[4] = 0x41;
    barCodeData[5] = 0x0a;
    barCodeData[6] = 0;
    barCodeData[7] = 0;
    barCodeData[8] = 0;
    barCodeData[9] = 0;
    barCodeData[10] = 0;
    barCodeData[11] = 0;
    barCodeData[12] = 0x1d;
    barCodeData[13] = 0x66;
    barCodeData[14] = 0x00;
    barCodeData[15] = 0x1d;
    barCodeData[16] = 0x48;
    barCodeData[17] = 0x02;
    barCodeData[18] = 0x1d;
    barCodeData[19] = 0x6b;
    barCodeData[20] = 0x41;
    barCodeData[21] = 0x0c;
    barCodeData[22] = 0x32;
    barCodeData[23] = 0x32;
    barCodeData[24] = 0x32;
    barCodeData[25] = 0x32;
    barCodeData[26] = 0x32;
    barCodeData[27] = 0x32;
    barCodeData[28] = 0x32;
    barCodeData[29] = 0x32;
    barCodeData[30] = 0x32;
    barCodeData[31] = 0x32;
    barCodeData[32] = 0x32;
    barCodeData[33] = 0x32;
    barCodeData[34] = 0x65;

    if(lengthOfData != 12)
    {
        cout << "Please provide 12 bytes of data" << endl;
        return false;
    }

    for(int i = 0; i < lengthOfData; i++)
    {
        barCodeData[22 + i] = data[i];
    }

    for(int a = 0; a < 35; a++)
    {
        cout << (int)barCodeData[a] << " ";
    }

    cout << endl;

    if(!sendDataToPrinter(barCodeData, 35, this->connectedToSocket, this->bluetoothSocket))
    {
        cout << "Command couldn't be sent to printer" << endl;
        return false;
    }else
    {
        cout << "Data successfully sent to the printer" << endl;
    }

    return true;
}

bool Printer::setHeightOfBarCode(uint8_t height)
{
    unsigned char changeBarcodeHeight[3];
    changeBarcodeHeight[0] = selectBarCodeHeight.beginning;
    changeBarcodeHeight[1] = selectBarCodeHeight.middle;

    changeBarcodeHeight[2] = height;
    
    if(height > 200)
    {
        cout << "Height value too large" << endl;
        return false;
    }

    if(height < 50)
    {
        cout << "Height value too low" << endl;
        return false;
    }

    if(!sendDataToPrinter(changeBarcodeHeight, 3, this->connectedToSocket, this->bluetoothSocket))
    {
        cout << "There was an error changing the barcode height" << endl;
        return false;
    }else{
        cout << "Barcode height changed" << endl;
        return true;
    }
}

bool Printer::setWidthOfBarCode(uint8_t width)
{
    unsigned char changeBarcodeWidth[3];
    changeBarcodeWidth[0] = selectBarCodeWidth.beginning;
    changeBarcodeWidth[1] = selectBarCodeWidth.middle;

    changeBarcodeWidth[2] = width;
    
    if(width > 6)
    {
        cout << "Command not recognised" << endl;
        return false;
    }

    if(width < 2)
    {
        cout << "Command not recognised" << endl;
        return false;
    }

    if(!sendDataToPrinter(changeBarcodeWidth, 3, this->connectedToSocket, this->bluetoothSocket))
    {
        cout << "There was an error changing the barcode height" << endl;
        return false;
    }else{
        cout << "Barcode height changed" << endl;

        return true;
    }
}

bool Printer::setLeftMargin(uint8_t margin)
{

    unsigned char leftMargin[4];

    leftMargin[0] = setLeftMarginCommand.beginning;
    leftMargin[1] = setLeftMarginCommand.middle;
    leftMargin[2] = margin;
    leftMargin[3] = 0;

    if(!sendDataToPrinter(leftMargin, 4, this->connectedToSocket, this->bluetoothSocket))
    {
        cout << "Failed to send command to printer" << endl;
        return false;
    }
    else{
        cout << "Success, left margin set to: " << margin << endl;
    }

    return true;
}

bool Printer::setAlignment(uint8_t value)
{

    if(value < 0)
    {
        cout << "Please input either: 0 or 1 or 2 " << endl;
        return false;
    }

    if(value > 2)
    {
        cout << "Please input either: 0 or 1 or 2 " << endl;
        return false;
    }

    unsigned char alignmentCommand[3];

    alignmentCommand[0] = alignmentSettings.beginning;
    alignmentCommand[1] = alignmentSettings.middle;
    alignmentCommand[2] = value;

    if(!sendDataToPrinter(alignmentCommand, 3, this->connectedToSocket, this->bluetoothSocket))
    {
        cout << "There was an error sending the data to the printer" << endl;
        return false;
    }
    else{
        cout << "Success! alignment sent" << endl;
    }

    return true;
}

bool Printer::intializePrinter()
{
    unsigned char initializePrinterCommand[2];

    initializePrinterCommand[0] = initializePrinter_t.beginning;
    initializePrinterCommand[1] = initializePrinter_t.middle;

    if(!sendDataToPrinter(initializePrinterCommand, 2, this->connectedToSocket, this->bluetoothSocket))
    {
        cout << "Failed to send command to printer. Is the printer initialized?" << endl;
        return false;
    }
    else{
        cout << "Success, Printer has been initialized" << endl;
    }

    return true;
}