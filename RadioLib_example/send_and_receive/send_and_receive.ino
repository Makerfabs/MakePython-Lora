
/*
Create time :   2022/8/15
Author      :   Vincent
Descrption  :

    A demo for communicating with the standard Makerfabs LoraNode (MaLora).
*/
// Use Lora library:   RadioLib Version 4.6.0
#include <RadioLib.h>

// MakePython has 2 piece of SX127X
// Lora Module A
#define DIO0 35
#define DIO1 39

#define LORA_RST 2
#define LORA_CS 25

#define SPI_MOSI 13
#define SPI_MISO 12
#define SPI_SCK 14

// #define FREQUENCY 433.0
// #define FREQUENCY 868.0
#define FREQUENCY 915.0

#define BANDWIDTH 125.0
#define SPREADING_FACTOR 9
#define CODING_RATE 7
#define OUTPUT_POWER 10
#define PREAMBLE_LEN 8
#define GAIN 0

// **433 use SX127X, 868 and 915 use sx1276, they are not same lora module.**
// Freq 433 Mhz
// SX127X radio = new Module(LORA_CS, DIO0, LORA_RST, DIO1, SPI, SPISettings());

// Freq 868 or 915 Mhz
SX1276 radio = new Module(LORA_CS, DIO0, LORA_RST, DIO1, SPI, SPISettings());

int count = 0;

void setup()
{
    Serial.begin(115200);
    Serial.print(F("[SX127X] Initializing ... "));

    // Must init SPI
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    int state = radio.begin(FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SX127X_SYNC_WORD, OUTPUT_POWER, PREAMBLE_LEN, GAIN);

    if (state == ERR_NONE)
    {
        Serial.println(F("success!"));
    }
    else
    {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true)
            ;
    }
}

void loop()
{
    lora_send_and_receive();
}

String command_text[4] = {"IDXDEBUGACT001PARAM000000#",
                          "IDXDEBUGACT000PARAM000000#",
                          "IDXDEBUGACT001PARAM000000#",
                          "IDXDEBUGACT000PARAM000000#"};

void lora_send_and_receive()
{
    Serial.print(F("[SX127X] Transmitting packet ... "));

    Serial.print("          " + command_text[count % 4]);

    int state = radio.transmit(command_text[count++ % 4]);

    if (state == ERR_NONE)
    {
        // the packet was successfully transmitted
        Serial.println(F(" success!"));

        // print measured data rate
        Serial.print(F("[SX127X] Datarate:\t"));
        Serial.print(radio.getDataRate());
        Serial.println(F(" bps"));
    }
    else if (state == ERR_PACKET_TOO_LONG)
    {
        // the supplied packet was longer than 256 bytes
        Serial.println(F("too long!"));
    }
    else if (state == ERR_TX_TIMEOUT)
    {
        // timeout occurred while transmitting packet
        Serial.println(F("timeout!"));
    }
    else
    {
        // some other error occurred
        Serial.print(F("failed, code "));
        Serial.println(state);
    }

    String str;
    state = radio.receive(str);

    if (state == ERR_NONE)
    {
        // packet was successfully received
        Serial.println(F("success!"));

        // print the data of the packet
        Serial.print(F("[SX127X] Data:\t\t\t"));
        Serial.println(str);

        // print the RSSI (Received Signal Strength Indicator)
        // of the last received packet
        Serial.print(F("[SX127X] RSSI:\t\t\t"));
        Serial.print(radio.getRSSI());
        Serial.println(F(" dBm"));

        // print the SNR (Signal-to-Noise Ratio)
        // of the last received packet
        Serial.print(F("[SX127X] SNR:\t\t\t"));
        Serial.print(radio.getSNR());
        Serial.println(F(" dB"));

        // print frequency error
        // of the last received packet
        Serial.print(F("[SX127X] Frequency error:\t"));
        Serial.print(radio.getFrequencyError());
        Serial.println(F(" Hz"));
    }
    else if (state == ERR_RX_TIMEOUT)
    {
        // timeout occurred while waiting for a packet
        // Serial.println(F("timeout!"));
    }
    else if (state == ERR_CRC_MISMATCH)
    {
        // packet was received, but is malformed
        Serial.println(F("CRC error!"));
    }
    else
    {
        // some other error occurred
        Serial.print(F("failed, code "));
        Serial.println(state);
    }

    // wait for a second before transmitting again
    delay(3000);
}