# CIS441-OpenAPS

## Screen capture video of the real-time plot on Codio dashboard
[OPEN APS project demo](https://www.loom.com/share/af499ebd1d734c1cacd3a1d0072272a0?sid=0633ee18-18eb-41f1-be5c-0420056a6a32)

Please refer to Codio for milestone 1 part 1

## Overall Instruction

**OpenAPS Logic (on Arduino)**: Implement the EventualBG algorithm to predict the eventual stable BG level once all active insulin has finished working. Optionally, you can implement COB-based Predictions (COBpredBGs), which predict BG changes based on the announced Carbohydrates on Board (COB).

**Virtual Component (on Codio or Local Machine)**: Implement a virtual CGM and a virtual insulin pump, which will simulate the real-world components. This will communicate with OpenAPS and a virtual patient through MQTT, serving as a buffer between two components.

**Virtual Patient with Patient Profile (on Codio or Local Machine)**: We provide a Python script that simulates the patient and their BG response. You will need to run this to test the OpenAPS system. You can also configure the patient's parameters (meal intake, bolus insulin, etc) and simulation interval settings.

**Dashboard (on Codio)**: We provide a virtual patient dashboard, which displays the current glucose levels and insulin status of the virtual patient.

You must run the components in the following order: first, the virtual component (CGM and insulin pump), then the virtual patient, and finally the Arduino OpenAPS.

## OpenAPS

Implement the OpenAPS logic on Arduino Nano 33 IoT to predict blood glucose levels and calculate insulin dosages. If the AirPennNet device is not working properly, please use another WiFi or personal hotspot. 

We have two OpenAPS VSCode project folders for each milestones. Please start from the proper milestone folders, `openAPS-milestone1` or `openAPS-milestone2`.

### Setup
1. Install PlatformIO on your VSCode with Arduino Nano 33 IoT board selected.
2. Install libraries: ArduinoMqttClient, WiFiNINA, FreeRTOS_SAMD21.
3. Add `\openAPS-milestone1\include\arduino_secrets` with WiFi credentials.

### How to compile and execute program
1. Open `\openAPS-milestone1\src\main.cpp` in PlatformIO.
2. Compile and upload to your Arduino board.
3. Check your output in the serial monitor.

## Virtual Component

We have provided a C++ file with a template to help you get started on this assignment. The template includes comments with detailed instructions to guide you in creating the virtual insulin pump and virtual CGM, which will relay MQTT messages between the virtual patient and OpenAPS, and vice versa. However, if you prefer, you are free to design and implement your own program from scratch, as long as it meets the project requirements.

If you're experiencing issues running the Virtual Component in your local environment, please use the "OpenAPS Virtual Component" in Codio that we've set up.
Before running the virtual component, make sure to install the necessary MQTT packages
For convenience, compile your code using the Makefile.
In addition, we've set up Virtual Patient in Codio as well. Please remember to install pip packages and fill the .env file before running the virtual patient. 


### How to compile and execute program
1. Complete the main.cpp file with your implementation.

2. Compile with Makefile.
make

3. export env. and check this is stored in program
export TEAM_NAME=Steady_State
echo $TEAM_NAME
# should print: Steady_State

3. Execute the program.
```
./main
```

### Validator for validating Virtual Component (Miletone 1, Part 1 only)

Virtual Component in the Codio Box contains a validator app. Please execute it with `TEAM_NAME` as a parameter to test whether your Virtual Component was implemented correctly.

```
./validator <TEAM_NAME>
```

## Virtual Patient

### How to configure environment and execute program
1. Install the required Python packages:

```
pip install -r requirements.txt
```

2. Copy .env_template file to .env and fill in the values

3. Run main.py to simulate the virtual patient with a mode parameter (0: without OpenAPS, 1: with OpenAPS).
```
python3 main.py 1
```


