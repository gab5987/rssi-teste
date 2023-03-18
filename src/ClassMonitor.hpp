#include "common.hpp"

class Monitor: public BLEServerCallbacks {
public:
	static int16_t connection_id;

	static constexpr float reference_power  = -50; //rssi reffrence 
	static constexpr float distance_factor = 3.5; 
 
	uint8_t get_value() { return value++; }
	esp_err_t get_rssi() { return esp_ble_gap_read_rssi(remote_addr); }
 
	static float get_distance(const int8_t rssi)
	{ return pow(10, (reference_power - rssi)/(10*distance_factor)); }
 
private:
    void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) override {
		connection_id = param->connect.conn_id;
    	memcpy(&remote_addr, param->connect.remote_bda, sizeof(remote_addr));

		// Install the RSSI callback
    	BLEDevice::setCustomGapHandler(&Monitor::rssi_event);

    	Serial.printf("Connection #: %i, remote: ", connection_id);
		show_address(param->connect.remote_bda);
		Serial.printf(" [Callback installed]\n");
    }
 
    void onDisconnect(BLEServer* pServer) {
		Serial.printf("Connection #%i closed\n", connection_id);
		BLEDevice::setCustomGapHandler(nullptr);
		connection_id = -1;
    }
 
    static void rssi_event(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
 
	static esp_bd_addr_t remote_addr;
	uint8_t value = 0;
};

int16_t Monitor::connection_id = -1;
esp_bd_addr_t Monitor::remote_addr = {};
 
void Monitor::rssi_event(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    static int16_t rssi_average = 0;
 
	show_address(remote_addr);

    // Adjust damping_factor to lower values to have a more reactive response
    if (event == ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT) {
        const float damping_factor = 0.8;
        rssi_average = rssi_average * damping_factor + 
        	param->read_rssi_cmpl.rssi * (1 - damping_factor);

		Serial.printf(
            ", rssi=%hi, distance~=%g\n",
			param->read_rssi_cmpl.rssi, get_distance(rssi_average)
		);
    }
}