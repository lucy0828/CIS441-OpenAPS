// written by PIN-YI (Jimmy), Yu and Shraavasti 25/10/11

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <mqtt/async_client.h>

using namespace std;
using namespace std::chrono;

const string ADDRESS  { "tcp://mqtt-dev.precise.seas.upenn.edu:1883" };
const string USERNAME { "cis441-541_2025" };
const string PASSWORD { "cukwy2-geNwit-puqced" };
const int QOS = 1;

// MQTT topics for Steady_State team
const string INSULIN_TOPIC { "cis441-541/Steady_State/insulin-pump" };                 // to Virtual Patient
const string CGM_TOPIC { "cis441-541/Steady_State/cgm" };                              // from Virtual Patient
const string OA_INSULIN_TOPIC { "cis441-541/Steady_State/insulin-pump-openaps" };      // from OpenAPS
const string OA_CGM_TOPIC { "cis441-541/Steady_State/cgm-openaps" };                   // to OpenAPS

// ---------------- MessageRelayCallback Class ----------------
class MessageRelayCallback : public virtual mqtt::callback {
    mqtt::async_client& client_;

public:
    explicit MessageRelayCallback(mqtt::async_client& client)
        : client_(client) {}

    // Triggered when connection is established (including reconnect)
    void connected(const string& cause) override {
        try {
            cout << "[MQTT] Connected to broker. Subscribing..." << endl;
            client_.subscribe(CGM_TOPIC, QOS)->wait();
            client_.subscribe(OA_INSULIN_TOPIC, QOS)->wait();
            cout << "[MQTT] Subscribed to: " << CGM_TOPIC
                 << " and " << OA_INSULIN_TOPIC << endl;
        } catch (const mqtt::exception& ex) {
            cerr << "[ERROR] Subscription failed: " << ex.what() << endl;
        }
    }

    // Triggered when a subscribed message arrives
    void message_arrived(mqtt::const_message_ptr msg) override {
        const string topic   = msg->get_topic();
        const string payload = msg->to_string();

        cout << "[MQTT] Message arrived: [" << topic << "] " << payload << endl;

        if (topic == CGM_TOPIC) {
            on_message_cgm(payload);        // handle CGM (VP → OA)
        }
        else if (topic == OA_INSULIN_TOPIC) {
            on_message_insulin(payload);    // handle INSULIN (OA → VP)
        }
        else {
            cout << "[MQTT] (unhandled topic) " << topic << endl;
        }
    }

    // Handle CGM message from Virtual Patient → forward to OpenAPS
    void on_message_cgm(const string& payload) {
        try {
            auto pubmsg = mqtt::make_message(OA_CGM_TOPIC, payload);
            pubmsg->set_qos(QOS);
            client_.publish(pubmsg)->wait();
            cout << "[Relay] VP/CGM → OpenAPS on " << OA_CGM_TOPIC
                 << " | payload=" << payload << endl;
        } catch (const mqtt::exception& e) {
            cerr << "[Relay] CGM relay failed: " << e.what() << endl;
        }
    }

    // Handle insulin message from OpenAPS → forward to Virtual Patient
    void on_message_insulin(const string& payload) {
        try {
            auto pubmsg = mqtt::make_message(INSULIN_TOPIC, payload);
            pubmsg->set_qos(QOS);
            client_.publish(pubmsg)->wait();
            cout << "[Relay] OpenAPS → Virtual Patient on " << INSULIN_TOPIC
                 << " | payload=" << payload << endl;
        } catch (const mqtt::exception& e) {
            cerr << "[Relay] Insulin relay failed: " << e.what() << endl;
        }
    }
};

// ---------------- MQTTClientHandler Class ----------------
class MQTTClientHandler {
    mqtt::async_client client_;
    MessageRelayCallback callback_;

public:
    MQTTClientHandler(const string& host, const string& username, const string& password)
        : client_(host, "Relay_Steady_State")    // fixed client ID
        , callback_(client_) {

        // set callback before connecting (so connected() can trigger subscribe)
        client_.set_callback(callback_);

        // prepare connection options
        auto connOpts = mqtt::connect_options_builder()
            .clean_session()
            .automatic_reconnect(true)
            .keep_alive_interval(seconds(20))
            .user_name(username)
            .password(password)
            .finalize();

        // connect to broker
        try {
            cout << "[MQTT] Connecting to " << host << " ..." << endl;
            client_.connect(connOpts)->wait();
            cout << "[MQTT] Connected successfully." << endl;
        } catch (const mqtt::exception& e) {
            cerr << "[MQTT] Connection failed: " << e.what() << endl;
        }
    }

    // Keeps client alive (similar to an infinite loop in your instruction)
    void inject_loop() {
        cout << "Press Ctrl+C to exit the MQTT relay." << endl;
        while (true) {
            this_thread::sleep_for(seconds(1));
        }
    }
};

// ---------------- main() ----------------
int main() {
    MQTTClientHandler mqtt_handler(ADDRESS, USERNAME, PASSWORD);
    mqtt_handler.inject_loop();   // keep alive
    return 0;
}
