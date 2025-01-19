#include <cstdint>
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

struct {
    unsigned char beginning = 0x1B;
    unsigned char middle = 0x21;
} selectPrintMode;

struct{
    unsigned char beginning = 0x1D;
    unsigned char middle = 0x68;
} setBarcodeHeight;

struct{
    unsigned char beginning = 0x1D;
    unsigned char middle = 0x77;
} setBarcodeWidth;

struct{
    unsigned char beginning = 0x1B;
    unsigned char middle = 0x2D;
    unsigned char value1 = 0x00;
    unsigned char value2 = 0x01;
    unsigned char value3 = 0x02;
} setUnderline;

struct{
    unsigned char beginning = 0x1B;
    unsigned char middle = 0x32;
} defaultUnderline;

struct{
    unsigned char beginning = 0x1B;
    unsigned char middle = 0x33;
    uint8_t value = 1;
} lineSpacing;

struct{
    unsigned char beginning = 0x1B;
    unsigned char middle = 0x40;
}initializePrinter_t;

struct {
    unsigned char beginning = 0x1B;
    unsigned char middle = 0x56;
    uint8_t value1 = 0;
    uint8_t value2 = 1;
} clockwisePrinting;

struct{
    unsigned char beginning = 0x1B;
    unsigned char middle = 0x61;

    uint8_t leftAligned = 0;
    uint8_t centerAligned = 1;
    uint8_t rightAligned = 2;

} alignmentSettings;
    
struct {
    unsigned char beginning = 0x1D;
    unsigned char middle = 0x48;

    uint8_t dontPrint = 0;
    uint8_t aboveBarCode = 1;
    uint8_t bottomOfBarCode = 2;
    uint8_t aboveAndBelowBarCode = 3;
} setHRICharactersPosition;

struct {
    unsigned char beginning = 0x1D;
    unsigned char middle = 0x66;

    uint8_t selectFontA = 0;
    uint8_t selectFontB = 1;
} selectHRIFonts;

struct {
    unsigned char beginning = 0x1D;
    unsigned char middle = 0x67;

    uint8_t height = 0;
    uint8_t defaultHeight = 162;
} selectBarCodeHeight;

struct {
    unsigned char beginning = 0x1D;
    unsigned char middle = 0x77;

    uint8_t times2 = 2;
    uint8_t times3 = 3;
    uint8_t times4 = 4;
    uint8_t times5 = 5;
    uint8_t times6 = 6;
} selectBarCodeWidth;

struct {
    unsigned char beginning = 0x1D;
    unsigned char middle = 0x78;

    uint8_t value = 0;
} selectBarCodeLeftSpacing;

struct {
    unsigned char beginning = 0x1B;
    unsigned char middle = 0x5A;

    uint8_t version = 0;
    uint8_t ECLevel1 = 0;
    uint8_t ECLevel2 = 1;
    uint8_t ECLevel3 = 2;
    uint8_t ECLevel4 = 3;

    uint8_t componentType1 = 1;
    uint8_t componentType2 = 2;
    uint8_t componentType3 = 3;
    uint8_t componentType4 = 4;
    uint8_t componentType5 = 5;
    uint8_t componentType6 = 6;
    uint8_t componentType7 = 7;
    uint8_t componentType8 = 8;

    uint8_t lengthOfDataLow;
    uint8_t lengthOfDataHigh;

    unsigned char* dataForCode;
} QRCode;

struct {
    unsigned char beginning = 0x1D;
    unsigned char middle = 0x4C;
} setLeftMarginCommand;

class Printer{

    unsigned char lineFeed = 0x0A;
    uint8_t rightSideCharacterSpacing = -3;
    uint8_t lineOffsetLow = 0;
    uint8_t lineOffsetHigh = 0;
    uint8_t characterSet = 0;

    //Bluetooth Settings
    int port;
    char* bluetoothAddress;
    int bluetoothSocket;
    sockaddr_rc bluetoothAddressTechnical;
    bool connectedToSocket = false;

    public:
        Printer(int connectionMethod, char* bluetoothAddress);
        bool connectToDevice();
        bool disconnectFromDevice();
        bool printHeading(unsigned char* data, int dataLength, bool bold, bool characterFontSelection, bool doubleHeight, bool doubleWidth, bool underline);
        bool printQRCode(unsigned char component, unsigned char ecLevel, uint16_t ataLength, unsigned char* data);
        bool printGratingLogo(unsigned char* monochromeData, int width, int height);
        bool printBarCode(unsigned char* data, int lengthOfData);
        bool setHeightOfBarCode(uint8_t height);
        bool setWidthOfBarCode(uint8_t width);
        bool setLeftMargin(uint8_t margin);
        bool setAlignment(uint8_t value);
        bool intializePrinter();
};