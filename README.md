# CIS441-OpenAPS

## Overall Instruction

OpenAPS Logic (on Arduino): Implement the EventualBG algorithm to predict the eventual stable BG level once all active insulin has finished working. Optionally, you can implement COB-based Predictions (COBpredBGs), which predict BG changes based on the announced Carbohydrates on Board (COB).

Virtual Component (on Codio or Local Machine): Implement a virtual CGM and a virtual insulin pump, which will simulate the real-world components. This will communicate with OpenAPS and a virtual patient through MQTT, serving as a buffer between two components.

Virtual Patient (on Codio or Local Machine): We provide a Python script that simulates the patient and their BG response. You will need to run this to test the OpenAPS system.

Thingsboard: We provide a virtual patient dashboard, which displays the current glucose levels and insulin status of the virtual patient. You can configure the patient's parameters (meal intake, bolus insulin, etc)  and simulation interval settings.

You must run the components in the following order: first, the Arduino OpenAPS, then the virtual component (CGM and insulin pump), and finally the virtual patient. Additionally, ensure that the Thingsboard dashboard configuration is updated before each simulation. Do not modify the dashboard during the simulation, as the settings are shared between OpenAPS and the virtual patient.

## OpenAPS

Implement the OpenAPS logic on Arduino Nano 33 IoT to predict blood glucose levels and calculate insulin dosages.

### Setup
1. Install PlatformIO on your VSCode with Arduino Nano 33 IoT board selected.
2. Install libraries: ArduinoMqttCLient, WiFiNINA, FreeRTOS_SAMD21.
3. Add `arduino_secrets.` with WiFi credentials.

### How to compile and execute program
1. Open `\openAPS\src\main.cpp` in PlatformIO.
2. Compile and upload to your Arduino board.
3. Check your output in the serial monitor.

## Virtual Component

We have provided a C++ file with a template to help you get started on this assignment. The template includes comments with detailed instructions to guide you in creating the virtual insulin pump and virtual CGM, which will relay MQTT messages between the virtual patient and OpenAPS, and vice versa. However, if you prefer, you are free to design and implement your own program from scratch, as long as it meets the project requirements.

### How to compile and execute program
1. Complete the main.cpp file with your implementation.

2. Compile with Makefile.

```
make
```

3. Execute the program.
```
./main
```

## Virtual Patient

### How to configure environment and execute program
1. Install the required Python packages:

```
pip install -r requirements.txt
```

2. Copy .env_template file to .env and fill in the values

3. Run main.py to simulate the virtual patient.
```
python main.py
```


