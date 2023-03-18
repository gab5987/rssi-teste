#include "common.hpp"
#include "ClassMonitor.hpp"

BLECharacteristic *pCharacteristic;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define DESCRIPTOR_UUID		"e8f45c7e-8be5-4918-bb84-467d3fd354aa"
 
Monitor monitor;
 
 
void setup() {
	Serial.begin(9600);
 
	BLEDevice::init("Esp-32");
 	BLEServer *pServer = BLEDevice::createServer();
	pServer->setCallbacks(&monitor);
 
	BLEService *pService = pServer->createService(SERVICE_UUID);
 
	pCharacteristic = pService->createCharacteristic(
		CHARACTERISTIC_UUID,
		BLECharacteristic::PROPERTY_READ   |
		BLECharacteristic::PROPERTY_WRITE  |
		BLECharacteristic::PROPERTY_NOTIFY |
		BLECharacteristic::PROPERTY_INDICATE
	);
 
	// https://www.bluetooth.com/specifications/specs/gatt-specification-supplement/
	pCharacteristic->addDescriptor(new BLEDescriptor(DESCRIPTOR_UUID));
 
	pService->start();
 	pServer->getAdvertising()->start();
	Serial.println("Waiting for incoming connections...");
}
 

void loop() {
	static const uint32_t REFRESH_DELAY = 1000;
	static uint32_t next_detection;

	uint32_t current_time = millis();

	if (Monitor::connection_id != -1) {
		if (current_time - next_detection >= REFRESH_DELAY) {
			next_detection += REFRESH_DELAY;

			auto value = monitor.get_value();
			pCharacteristic->setValue(&value, sizeof(value));
			pCharacteristic->notify();
	 
			// Request RSSI from the remote address
			if (monitor.get_rssi() != ESP_OK)
				Serial.println("RSSI request failed");
		}
	}
}