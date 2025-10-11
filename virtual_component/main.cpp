// written by Shraavasti (Shraav) Bhat 10/10/2026
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <mqtt/async_client.h>

using namespace std;
using namespace std::chrono;

const string TEAM = "Steady_State";
const string ADDRESS { "tcp://mqtt-dev.precise.seas.upenn.edu:1883" };
const string USERNAME { "cis441-541_2025" };
const string PASSWORD { "cukwy2-geNwit-puqced" };

const int QOS = 1;

// communication between Virtual Compenent and Virtual Patient
const string CGM_TOPIC { "cis441-541/Steady_State/cgm" };      // subscribe measure glucose level MQTT topic 4
const string INSULIN_TOPIC_VP { "cis441-541/Steady_State/insulin-pump" };  // publish inject basal insulin MQTT topic 2

// communication between Virtual Compenent and OpenAPS
const string OA_INSULIN_TOPIC { "cis441-541/Steady_State/insulin-pump-openaps" };
const string OA_CGM_TOPIC { "cis441-541/Steady_State/cgm-openaps" };

// Separate callback class inheriting from mqtt::callback
class MQTTRelay {
    mqtt::async_client client;
    mqtt::connect_options connOpts;

public:
    MQTTRelay() : client(ADDRESS, "Relay_" + TEAM) {
        // connect to mqtt
        connOpts = mqtt::connect_options_builder()
            .clean_session()
            .automatic_reconnect(true)
            .user_name(USERNAME)
            .password(PASSWORD)
            .keep_alive_interval(std::chrono::seconds(20))
            .finalize();
    }

    void connect_and_subscribe() {
        cout << "\n[Relay] Connecting to MQTT broker..." << endl;
        client.connect(connOpts)->wait();
        cout << "[Relay] Connected" << endl;

        client.start_consuming();

        client.subscribe(CGM_TOPIC, QOS)->wait();
        client.subscribe(OA_INSULIN_TOPIC, QOS)->wait();

        cout << "[Relay] Subscribed to:" << endl;
        cout << "       - " << CGM_TOPIC << "  (from Virtual Patient)" << endl;
        cout << "       - " << OA_INSULIN_TOPIC << "  (from Arduino/OpenAPS)" << endl;
        cout << "[Relay] Ready to relay messages...\n" << endl;

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    void relay_loop() {
        cout << "[Relay] Listening .... (Ctrl + C to exit)" << endl;
        while (true) {
            auto msg = client.consume_message();
            if(!msg) continue;

            string topic = msg->get_topic();
            string payload = msg->to_string();

            cout << "\n[MQTT] Message received: [" << topic << "] " << payload << endl;

            if (topic == CGM_TOPIC) {
                cout << "[Relay] VP -> OpenAPS on" << OA_CGM_TOPIC << endl;
                client.publish(OA_CGM_TOPIC, payload, QOS, false)->wait();
            }
            else if (topic == OA_INSULIN_TOPIC) {
                cout << "[Relay] OpenAPS -> Virtual Patient on " << INSULIN_TOPIC_VP << endl;
                client.publish(INSULIN_TOPIC_VP, payload, QOS, false)->wait();
            }
        }
    }
};

// The main MQTTClientHandler class to manage the connection and the callback
int main() {
    try {
        MQTTRelay relay;
        relay.connect_and_subscribe();
        relay.relay_loop();
    }
    catch (const mqtt::exception &e) {
        cerr << "[ERROR] MQTT exception: " << e.what() << endl;
    }
}