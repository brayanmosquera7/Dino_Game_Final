#ifndef BLE_CONTROLLER_H
#define BLE_CONTROLLER_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "Config.h"
#include "Point.h"

class BLEController {
private:
  BLEServer* server;
  BLECharacteristic* rxCharacteristic;
  volatile Command playerCommand;
  bool deviceConnected;
  static BLEController* instance;
  
  class ServerCallback : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      BLEController::instance->deviceConnected = true;
      Serial.println("Dispositivo conectado");
    }

    void onDisconnect(BLEServer* pServer) {
      BLEController::instance->deviceConnected = false;
      Serial.println("Dispositivo desconectado - Reiniciando advertising...");
      BLEDevice::startAdvertising();
    }
  };
  
  class CommandCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* characteristic) {
      uint8_t* data = characteristic->getData();
      if (characteristic->getLength() > 0) {
        Command cmd = (Command)data[0];
        
        if (cmd == CMD_JUMP) {
          BLEController::instance->playerCommand = CMD_JUMP;
          Serial.println("Comando recibido: SALTAR");
        }
      }
    }
  };

public:
  BLEController() : server(nullptr), rxCharacteristic(nullptr), 
                    playerCommand(CMD_NONE), deviceConnected(false) {
    instance = this;
  }
  
  void begin() {
    BLEDevice::init(BLE_NAME);
    
    server = BLEDevice::createServer();
    server->setCallbacks(new ServerCallback());
    
    BLEService* service = server->createService(SERVICE_UUID);
    
    rxCharacteristic = service->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE | 
      BLECharacteristic::PROPERTY_WRITE_NR
    );
    
    rxCharacteristic->addDescriptor(new BLE2902());
    rxCharacteristic->setCallbacks(new CommandCallback());
    
    service->start();
    
    BLEAdvertising* advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(SERVICE_UUID);
    advertising->setScanResponse(true);
    advertising->setMinPreferred(0x06);
    advertising->setMaxPreferred(0x12);
    
    BLEDevice::startAdvertising();
    
    Serial.println("BLE iniciado - Esperando conexion...");
  }
  
  Command getCommand() {
    Command cmd = playerCommand;
    playerCommand = CMD_NONE;
    return cmd;
  }
  
  bool isConnected() {
    return deviceConnected;
  }
};

BLEController* BLEController::instance = nullptr;

#endif
