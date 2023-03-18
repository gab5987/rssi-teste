#ifndef RT_COMMON_HPP
#define RT_COMMON_HPP

#pragma once

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEAdvertisedDevice.h>
#include <Arduino.h>

template <typename T, size_t N>
void show_address(const T (&address)[N])
{
	Serial.print(address[0], HEX);
	for (uint8_t i = 1; i < N; i++)
		Serial.printf(":%02x", address[i]);
}

#endif