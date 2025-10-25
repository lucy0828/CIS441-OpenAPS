# OpenAPS (Open Artificial Pancreas System) - Milestone 2

## Demo Video

[OpenAPS Project Demo](https://www.loom.com/share/cebe19fa680b4c6697cc93dfe68d5df5)

## Project Structure

```
CIS541OpenAPS_proj_Steady_State-2/
├── openAPS-milestone1/          # Milestone 1 implementation
│   ├── src/
│   │   ├── main.cpp            # Main Arduino code
│   │   ├── OpenAPS.h           # OpenAPS class header
│   │   └── OpenAPS.cpp         # OpenAPS implementation
│   └── platformio.ini          # PlatformIO configuration
├── openAPS-milestone2/          # Milestone 2 implementation (CURRENT)
│   ├── src/
│   │   ├── main.cpp            # Main Arduino code with MQTT
│   │   ├── OpenAPS.h           # OpenAPS class header
│   │   └── OpenAPS.cpp         # OpenAPS implementation
│   └── platformio.ini          # PlatformIO configuration
├── virtual_component/           # Virtual CGM and insulin pump
│   ├── main.cpp                # MQTT relay implementation
│   └── Makefile                # Build configuration
├── virtual_patient/             # Patient simulation
│   ├── main.py                 # Main patient simulation
│   ├── bergman_model.py        # Glucose dynamics model
│   ├── insulin_model.py        # Insulin absorption model
│   ├── meals_model.py          # Meal absorption model
│   ├── mqtt.py                 # MQTT communication
│   └── requirements.txt        # Python dependencies
├── patient_profile.json        # Default patient profile
├── patient_profile_case1.json  # Low BG scenario
├── patient_profile_case2_1.json # Very low naive BG
├── patient_profile_case2_2.json # Safe low BG
├── patient_profile_case3.json  # High BG scenario
└── README.md                   # This file
```

## How to Run the System

### Prerequisites
- **Configure WiFi credentials** in `openAPS-milestone2/include/arduino_secrets.h`:
  ```cpp
  #define SECRET_SSID "your_wifi_name"
  #define SECRET_PASS "your_wifi_password"
  ```

### Running the System

**Start components in this order**:

#### 1. Virtual Component (CGM/Pump simulation)
```bash
cd virtual_component
g++ main.cpp -o main -lpaho-mqttpp3 -lpaho-mqtt3as -lpaho-mqtt3a
make clean
make
./main
```

#### 2. Virtual Patient + Dashboard
```bash
cd virtual_patient
python3 main.py 1
```

#### 3. Arduino OpenAPS (Control algorithm)
- Go to the **PlatformIO tab** in VS Code
- Click **Upload and Monitor**

