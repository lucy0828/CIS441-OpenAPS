#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <mqtt/async_client.h>

using namespace std;
using namespace std::chrono;

const string ADDRESS { "" };
const string USERNAME { "" };
const string PASSWORD { "" };

const int QOS = 1;

// communication between Virtual Compenent and Virtual Patient
const string INSULIN_TOPIC { "" };
const string CGM_TOPIC { "" };

// communication between Virtual Compenent and OpenAPS
const string OA_INSULIN_TOPIC { "" };
const string OA_CGM_TOPIC { "" };

// Separate callback class inheriting from mqtt::callback
class MessageRelayCallback : public virtual mqtt::callback {
    mqtt::async_client& client_;

public:
    MessageRelayCallback(mqtt::async_client& client)
        : client_(client) {}

    // subscribe mqtt topics
    void connected(const string& cause) override {
    }

    // message handler
    void message_arrived(mqtt::const_message_ptr msg) override {
    }

    // handle cgm message
    void on_message_cgm(const string& payload) {
    }

    // handle insulin message
    void on_message_insulin(const string& payload) {
    }

    
};


// The main MQTTClientHandler class to manage the connection and the callback
class MQTTClientHandler {
    mqtt::async_client client_;
    MessageRelayCallback callback_;

public:
    MQTTClientHandler(const string& host, const string& username, const string& password)
        : client_(host, ""), callback_(client_) {

        // connect to mqtt

        // set callback functions
    }

    // This keeps the program running indefinitely, which ensures that the MQTT client remains active and ready to receive and send messages.
    void inject_loop() {
        cout << "Press Ctrl+C to exit the mqtt handler." << endl;
        while (true) {
            this_thread::sleep_for(seconds(1));
        }
    }
};

int main() {

    MQTTClientHandler mqtt_handler(ADDRESS, USERNAME, PASSWORD);
    mqtt_handler.inject_loop();

    return 0;
}
