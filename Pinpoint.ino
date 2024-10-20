/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-web-bluetooth/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pSensorCharacteristic = NULL;
BLECharacteristic* pLedCharacteristic = NULL;

// Pin characteristic definitions
BLECharacteristic* pBrakeCharacteristic = NULL;           // Brake
BLECharacteristic* pBlinkleftCharacteristic = NULL;       // Blink Left
BLECharacteristic* pBlinkrightCharacteristic = NULL;      // Blink Right
BLECharacteristic* pReverselightCharacteristic = NULL;    // Revese Light
BLECharacteristic* pRearfoglightCharacteristic = NULL;    // Rear Fog Light
BLECharacteristic* pIgnitionCharacteristic = NULL;        // +12V Ignition


bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
int OldBrakeSignal = 0;

// Output pin 
const int ledPin = 8; // Use the appropriate GPIO pin for your setup

// Input Pin (Pulled down)
const int BrakePin = 7;  // input for Brake Signal (Pin-6) in 13-pin connector


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID                      "34103d05-6633-4a0c-bd4a-0b238d203a45"
#define SENSOR_CHARACTERISTIC_UUID        "19b10001-e8f2-537e-4f6c-d104768a1214"
#define LED_CHARACTERISTIC_UUID           "19b10002-e8f2-537e-4f6c-d104768a1214"

// 13-pin connector
#define PINSTATUS_UUID                    "26c90000-9477-4182-80be-f95c67bf7df4"
#define BRAKELIGHT_CHARACTERISTIC_UUID    "26c90001-9477-4182-80be-f95c67bf7df4"
#define BLINKLEFT_CHARACTERISTIC_UUID     "26c90002-9477-4182-80be-f95c67bf7df4"
#define BLINKRIGHT_CHARACTERISTIC_UUID    "26c90003-9477-4182-80be-f95c67bf7df4"
#define REVERSELIGHT_CHARACTERISTIC_UUID  "26c90004-9477-4182-80be-f95c67bf7df4"
#define REARFOGLIGHT_CHARACTERISTIC_UUID  "26c90005-9477-4182-80be-f95c67bf7df4"
#define IGNITION_CHARACTERISTIC_UUID      "26c90006-9477-4182-80be-f95c67bf7df4"


class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pLedCharacteristic) {
    String value = pLedCharacteristic->getValue();
    if (value.length() > 0) {
      Serial.print("Characteristic event, written: ");
      Serial.println(static_cast<int>(value[0])); // Print the integer value

      int receivedValue = static_cast<int>(value[0]);
      if (receivedValue == 1) {
        digitalWrite(ledPin, HIGH);
      } else {
        digitalWrite(ledPin, LOW);
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(BrakePin, INPUT_PULLDOWN);    // GPIO Input to read Brake Signal


  // Create the BLE Device
  BLEDevice::init("TestPoint");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create Sensor BLE Characteristic
  pSensorCharacteristic = pService->createCharacteristic(
                      SENSOR_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pSensorCharacteristic->addDescriptor(new BLE2902());


  // Create the ON button BLE Characteristic
  pLedCharacteristic = pService->createCharacteristic(
                      LED_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE   // Write to LED from Browser 
                    );
  pLedCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();



/* ---------- Create Connector Pin Status ---------------------------------- */

  BLEService *pService2 = pServer->createService(PINSTATUS_UUID);

  // Create the Brake_signal Charectaristic
  pBrakeCharacteristic = pService2->createCharacteristic(
                      BRAKELIGHT_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pBrakeCharacteristic->addDescriptor(new BLE2902());


  // Create the Blink Left Signal Characteristic
  pBlinkleftCharacteristic = pService2->createCharacteristic(
                      BLINKLEFT_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pBlinkleftCharacteristic->addDescriptor(new BLE2902());  

  // Create the Blinkk Right Signal Characteristic
  pBlinkrightCharacteristic = pService2->createCharacteristic(
                      BLINKRIGHT_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pBlinkrightCharacteristic->addDescriptor(new BLE2902());  

  // Create the Blinkk Right Signal Characteristic
  pReverselightCharacteristic = pService2->createCharacteristic(
                      REVERSELIGHT_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pReverselightCharacteristic->addDescriptor(new BLE2902());  

  // Create the Rear Fog Light Characteristic
  pRearfoglightCharacteristic = pService2->createCharacteristic(
                      REARFOGLIGHT_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pRearfoglightCharacteristic->addDescriptor(new BLE2902());  

  // Create +12V Ignition Characteristic
  pIgnitionCharacteristic = pService2->createCharacteristic(
                      IGNITION_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pIgnitionCharacteristic->addDescriptor(new BLE2902());


  // start service for Connector pins
  pService2->start();


  // Register the callback for the ON button characteristic
  pLedCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  
  
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  // notify changed value
  if (deviceConnected) {

    // Sensor
    pSensorCharacteristic->setValue(String(value).c_str());
    pSensorCharacteristic->notify();
     value++;
    Serial.print("New value notified: ");
    Serial.println(value);

    
    // add port read for each signal in the 13-pin connector

    // Brake signal
    int BrakeSignal;
    BrakeSignal = digitalRead(BrakePin);

    pBrakeCharacteristic->setValue(String(BrakeSignal).c_str());
    pBrakeCharacteristic->notify();

    Serial.print("Brake status notified: ");
    Serial.println(BrakeSignal);
   

    delay(3000); // 3000 = 3ms!! not seconds      bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
  }


  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    Serial.println("Device disconnected.");
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("Start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
    Serial.println("Device Connected");
  }
}