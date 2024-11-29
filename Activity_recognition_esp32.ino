



/* Edge Impulse ingestion SDK
 * Copyright (c) 2022 EdgeImpulse Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/* Includes ---------------------------------------------------------------- */
#include <ActivityRecognitionn_inferencing.h>
#include <DFRobot_LIS2DW12.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <stdio.h>
#include <string.h>
#include "SPIFFS.h"
#include "FS.h"


#define SERVICE_UUID "020c6211-64f6-4e6d-82e8-c2c1391f75fa"
#define CHARACTERISTIC_UUID_TX "020c6213-64f6-4e6d-82e8-c2c1391f75fa"

//When using I2C communication, use the following program to construct an object by DFRobot_LIS2DW12_I2C
/*!
 * @brief Constructor 
 * @param pWire I2c controller
 * @param addr  I2C address(0x18/0x19)
 */
//DFRobot_LIS2DW12_I2C acce(&Wire,0x18);
DFRobot_LIS2DW12_I2C acce;
unsigned long startTime;

//Bluetooth
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
int txValue = 0;
File dataFile;
File dataFileAcc;
BLEServer *ppServer;



class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    pServer->getAdvertising()->start();
  }
};


float features[90] = {
  //     // copy raw features here (for example from the 'Live classification' page)
  //     // see https://docs.edgeimpulse.com/docs/running-your-impulse-arduino
  //  -542.0,-757,499,-512.0,-466,320,-512.0,-466,320,-608.0,-462,422,-608.0,-462,422,-454.0,-871,462,-454.0,-871,462,-646.0,-1058,317,-451.0,-555,77,-451.0,-555,77,-546.0,-478,143,-1115.0,-1339,631,-1115.0,-1339,631,-932.0,-1315,646,-445.0,-818,416,-701.0,-518,192,-701.0,-518,192,-375.0,-536,82,-877.0,-665,451,-877.0,-776,542,-1243.0,-1352,662,-1243.0,-1352,683,-409.0,-769,347,-398.0,-553,182,-367.0,-593,108,-367.0,-593,108,-1035.0,-1246,515,-1035.0,-1246,515,-1377.0,-1215,635,-551.0,-930,626

};

/**
 * @brief      Copy raw feature data in out_ptr
 *             Function called by inference library
 *
 * @param[in]  offset   The offset
 * @param[in]  length   The length
 * @param      out_ptr  The out pointer
 *
 * @return     0
 */
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
  memcpy(out_ptr, features + offset, length * sizeof(float));
  return 0;
}

void print_inference_result(ei_impulse_result_t result);

/**
 * @brief      Arduino setup function
 */
void setup() {
  Serial.begin(115200);
  delay(100);
  while (!acce.begin()) {
    Serial.println("Communication failed, check the connection and I2C address setting when using I2C communication.");
    delay(1000);
  }
  Serial.print("chip id : ");
  Serial.println(acce.getID(), HEX);
  //Chip soft reset
  acce.softReset();
  //Set whether to collect data continuously
  acce.continRefresh(true);

  acce.setDataRate(DFRobot_LIS2DW12::eRate_50hz);
  acce.setRange(DFRobot_LIS2DW12::e2_g);
  acce.setFilterPath(DFRobot_LIS2DW12::eLPF);
  acce.setFilterBandwidth(DFRobot_LIS2DW12::eRateDiv_4);
  acce.setPowerMode(DFRobot_LIS2DW12::eContLowPwrLowNoise2_14bit);

  //Create the BLE Device
  BLEDevice::init("ESP32");

  //Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  ppServer = pServer;
  pServer->setCallbacks(new MyServerCallbacks());

  //Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  //Create the BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_NOTIFY);

  //BLE2902 nedded to notify
  pCharacteristic->addDescriptor(new BLE2902());


  //Start the service
  pService->start();

  //Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting for a client connection notify...");

  if(!SPIFFS.begin(true)){
    Serial.println("Błąd inicjalizacji SPIFFS!");
    return;
  }


    //Otwarcie pliku "dane.txt" w trybie zapisu, co spowoduje wyczyszczenie jego zawartości!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // dataFile = SPIFFS.open("/dane.txt", "w");
  // if (!dataFile) {
  //   Serial.println("Błąd otwarcia pliku 'dane.txt'!");
  //   return;
  // }
  // dataFile.close();
  // Serial.println("Zawartość pliku 'dane.txt' została wyczyszczona.");


//Usunięcie pliku "dane.txt", jeśli istnieje!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // if (SPIFFS.exists("/dane.txt")) {
  //   SPIFFS.remove("/dane.txt");
  //   Serial.println("Plik 'dane.txt' został usunięty.");
  // }

  // if (SPIFFS.exists("/daneAcc.txt")) {
  //   SPIFFS.remove("/daneAcc.txt");
  //   Serial.println("Plik 'daneAcc.txt' został usunięty.");
  // }

  // // Otwarcie pliku w trybie dopisywania
  // dataFile = SPIFFS.open("/dane.txt", "a");
  // if(!dataFile){
  //   Serial.println("Błąd otwarcia pliku!");
  //   return;
  // }

  //   // Otwarcie pliku w trybie dopisywania
  // dataFileAcc = SPIFFS.open("/daneAcc.txt", "a");
  // if(!dataFileAcc){
  //   Serial.println("Błąd otwarcia pliku!");
  //   return;
  // }
  
}
/**
 * @brief      Arduino main function
 */
void loop() {
  // Zbieranie danych z czujnika
  for (int i = 0; i < 90; i += 3) {
    features[i] = acce.readAccX();
    features[i + 1] = acce.readAccY();
    features[i + 2] = acce.readAccZ();
    delay(100);
  }

  // Sprawdzenie poprawności rozmiaru danych
  if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
    ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
              EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
    delay(1000);
    return;
  }

  ei_impulse_result_t result = { 0 };

  // Przygotowanie sygnału do klasyfikacji
  signal_t features_signal;
  features_signal.total_length = sizeof(features) / sizeof(features[0]);
  features_signal.get_data = &raw_feature_get_data;

  // Uruchomienie klasyfikatora
  EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
  if (res != EI_IMPULSE_OK) {
    ei_printf("ERR: Failed to run classifier (%d)\n", res);
    return;
  }

  // Przygotowanie wyników klasyfikacji
  char txString[20]; // Zwiększ rozmiar, aby zmieścić "Unknown Activity"
  char valueString[10]; // Zwiększ rozmiar na wypadek dłuższych wartości
  char resultString[30]; // Zwiększ rozmiar, aby uwzględnić dodatkowy tekst
  float runValue = result.classification[0].value;
  float standValue = result.classification[1].value;
  float walkValue = result.classification[2].value;


  if(runValue< 0.5 && walkValue < 0.5 && standValue<0.5){
  strcpy(txString, "Unknown Activity");
  }
  else if (runValue > walkValue && runValue > standValue) {
    strcpy(txString, ei_classifier_inferencing_categories[0]);
    dtostrf(result.classification[0].value, 1, 4, valueString);
  }
  else if (walkValue > runValue && walkValue > standValue) {
    strcpy(txString, ei_classifier_inferencing_categories[2]);
    dtostrf(result.classification[2].value, 1, 4, valueString);
  }
  else {
    strcpy(txString, ei_classifier_inferencing_categories[1]);
    dtostrf(result.classification[1].value, 1, 4, valueString);
  }

// Przygotowanie ciągu wynikowego w zależności od klasyfikacji
if (strcmp(txString, "Unknown Activity") == 0) {
    strcpy(resultString, txString); // Bez "="
} else {
    strcpy(resultString, txString);
    strcat(resultString, "=");
    strcat(resultString, valueString);
}




  // Ustawienie wartości w charakterystyce
  pCharacteristic->setValue(resultString);

  // Wysyłanie powiadomienia tylko jeśli urządzenie jest połączone
  if (deviceConnected) {
    pCharacteristic->notify();
    Serial.println("Send value " + String(resultString) + "\n");
  }
  else {
    Serial.println("Lack GATT Connetion");
  }

  memset(resultString, 0, sizeof(resultString));
  Serial.println("Connected: " + String(deviceConnected));
  
  // Opcjonalnie: zapis do pliku
  /*
  // dataFile.print(millis()-startTime);
  // dataFile.print(", ");
  // dataFile.println(String(resultString));

  // dataFileAcc.print(millis()-startTime);
  // dataFileAcc.print(", ");
  // dataFileAcc.print(acce.readAccX());
  // dataFileAcc.print(", ");
  // dataFileAcc.print(acce.readAccY());
  // dataFileAcc.print(", ");
  // dataFileAcc.println(acce.readAccZ());
  */
}
  
  // else {
  //   Serial.println("Measurement stopped after 10 seconds");
  //  Serial.println("Dane zapisane!");
  // listFiles("/");
  // dataFile.close();
  // dataFileAcc.close();
  // // readDataFromFile("/dane.txt");
  // // readDataFromFile("/daneAcc.txt");       


  // delay(100000000);
  // }


void print_inference_result(ei_impulse_result_t result) {

  // Print how long it took to perform inference
  ei_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n",
            result.timing.dsp,
            result.timing.classification,
            result.timing.anomaly);

  // Print the prediction results (object detection)
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
  ei_printf("Object detection bounding boxes:\r\n");
  for (uint32_t i = 0; i < result.bounding_boxes_count; i++) {
    ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];
    if (bb.value == 0) {
      continue;
    }
    ei_printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n",
              bb.label,
              bb.value,
              bb.x,
              bb.y,
              bb.width,
              bb.height);
  }

  // Print the prediction results (classification)
#else
  ei_printf("Predictions:\r\n");
  for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    ei_printf("  %s: ", ei_classifier_inferencing_categories[i]);
    ei_printf("%.5f\r\n", result.classification[i].value);
  }
#endif

  // Print anomaly result (if it exists)
#if EI_CLASSIFIER_HAS_ANOMALY == 1
  ei_printf("Anomaly prediction: %.3f\r\n", result.anomaly);
#endif
}

void readDataFromFile(const char* filename) {
  // Otwarcie pliku w trybie odczytu
  File dataFile = SPIFFS.open(filename, "r");
  if(!dataFile){
    Serial.println("Błąd otwarcia pliku!");
    return;
  }
 // Odczyt danych z pliku i wyświetlenie ich w monitorze szeregowym
  Serial.println("Zawartość pliku 'dane.txt':");
  while(dataFile.available()) {
    Serial.write(dataFile.read());
  }
  Serial.println(); // Dodatkowy enter na końcu
  dataFile.close();
}

void listFiles(const char* dirname) {
  Serial.println("Lista plików w katalogu głównym:");
  
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  
  while (file) {
    Serial.print("  ");
    Serial.println(file.name());
    file = root.openNextFile();
  }
}