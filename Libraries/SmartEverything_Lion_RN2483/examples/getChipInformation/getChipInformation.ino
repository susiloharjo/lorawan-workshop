/*
    SmarteEveryting Lion RN2483 Library - getChipInformation

    This example shows how to retrieve information from the Microchip RN2483 Module

    created 19 Dec 2015
    by Seve (seve@ioteam.it)

    This example is in the public domain
    https://github.com/axelelettronica/sme-rn2483-library

    More information on RN2483 available here:
    http://www.microchip.com/wwwproducts/en/RN2483
    
 */
#include <Arduino.h>
#include <rn2483.h>

char buffer[100];

void setup() {

    loraDbg = false;      // Set to 'true' to dump RN2483 TX/RX traces
     
    Serial.begin(115200);
    lora.begin();
    delay(100);

    while (!Serial) {
        ;
    }

    // first time get from Hw

}

void loop() {
    errE ok;
    const char *s;
    uint32_t state;
    char answer;
    
    // get SwVersion
    Serial.println("/***********************************************************/");
    Serial.println("/******************* Module Information ********************/");
    Serial.println("/***********************************************************/");

    Serial.println("\n ---- System --- ");
    Serial.print("FW Version      : ");
    Serial.println(lora.sysGetVersion());
    Serial.print("Vdd is          : ");
    Serial.print(lora.sysGetVdd());
    Serial.println(" mV");
    Serial.print("hwEUI is        : ");
    Serial.println(lora.sysGetHwEUI());
            
    Serial.println("\n ---- Mac --- ");
    s = lora.getMacAppEUI();
    Serial.print("MAC APP-EUI     : ");
    Serial.println(s);

    s = lora.sendRawCmdAndAnswer("mac get deveui");
    Serial.print("MAC DEV-EUI     : ");
    Serial.println(s);
    s = lora.sendRawCmdAndAnswer("mac get devaddr");
    Serial.print("MAC DEV-ADDR    : ");
    Serial.println(s);    

    s = lora.sendRawCmdAndAnswer("mac get adr");
    Serial.print("MAC Adaptive DR : ");
    Serial.println(s);

    s = lora.sendRawCmdAndAnswer("mac get ar");
    Serial.print("MAC Auto Reply  : ");
    Serial.println(s);
   
    state = lora.macGetStatus();
    Serial.print("MAC Status Mask : ");
    Serial.println(state, HEX);  
    
    Serial.println("\n ---- Radio --- ");
    Serial.print("Radio Mode      : ");
    radioModeE radio = lora.radioGetMode();
    if (radio == LoRa) {
        Serial.println("LoRa");
    } else if (radio == FSK) {
        Serial.println("FSK");
    }
    s = lora.radioGetPwr();
    Serial.print("Raio Get Power  : "); 
    Serial.println(s);
   
    Serial.println("\n/***********************************************************/\n");
    // Read the EEprom
    Serial.print("get EEProm (0x300) : ");
    Serial.println(lora.sysGetUserEEprom(0x300), HEX);
    Serial.print("get EEProm (0x310): ");
    Serial.println(lora.sysGetUserEEprom(0x310), HEX);
    Serial.print("get EEProm (0x3FF): ");
    Serial.println(lora.sysGetUserEEprom(0x3FF), HEX);

    // Read Radio modulation Mode

    Serial.print("\nCurrent Radio Mode     : ");   
    radio = lora.radioGetMode();
    if (radio == LoRa) {
        Serial.println("LoRa");
    } else if (radio == FSK) {
        Serial.println("FSK");
    } 

    Serial.println("\n You can triggers between (L)ora and (F)sk radio modes, press (Q) to quit");
    answer='a';

    do {
        delay(100);

        if (Serial.available()) {
            answer = Serial.read();
        }
    } while ((answer != 'L') && (answer != 'F') &&
             (answer != 'l') && (answer != 'f') &&
             (answer != 'Q') && (answer != 'q'));

    if (('L' == answer) || ('l' == answer)) {
        lora.radioSetMode(LoRa);
    } else if (('F' == answer) || ('f' == answer)) {
        lora.radioSetMode(FSK);
    } else {
        Serial.print("Exiting ...\n");
    }
    Serial.print("\New Radio Mode     : ");   
    radio = lora.radioGetMode();
    if (radio == LoRa) {
        Serial.println("LoRa");
    } else if (radio == FSK) {
        Serial.println("FSK");
    } 
    
    //endless loop
    while(1){
        ;
    }
}


