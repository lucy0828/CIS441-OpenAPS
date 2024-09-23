import sys, os, time, json
import numpy as np
from scipy.integrate import solve_ivp
from dotenv import load_dotenv
from datetime import datetime
from copy import deepcopy
import traceback
load_dotenv()

from meals_model import Meals
from insulin_model import Insulin
from bergman_model import Bergman
from mqtt import MQTT
        
        
class VP_MQTT(MQTT):
    def __init__(self, host, port, username, password, topics):
        super().__init__(host, port, username, password)
        self.time_step = 0
        self.topics = topics
        
        self.stated = False

        self.client.message_callback_add(f'{topics["TB_VP_ATTRIBUTE_TOPIC"]}/#', self.on_message_profile)
        self.client.message_callback_add(topics["INSULIN_TOPIC"], self.on_message_insulin)
        self.connect()
        
    def on_connect(self, client, userdata, flags, reason_code, properties):
        print(f">>> Connected with result code {reason_code}!")
        # Subscribe to the topics
        client.subscribe(self.topics['TB_VP_ATTRIBUTE_TOPIC'], qos=1)
        client.subscribe(f'{self.topics["TB_VP_ATTRIBUTE_TOPIC"]}/response/+', qos=1)
        client.subscribe(f'{self.topics["TB_VP_RPC_TOPIC"]}/request/+', qos=1)
        client.subscribe(self.topics["INSULIN_TOPIC"], qos=1)
        # Request the virtual patient profile
        print('>>> Requesting Virtual Patient Profile...')
        client.publish(f'{self.topics["TB_VP_ATTRIBUTE_TOPIC"]}/request/1', '{"sharedKeys":"PatientProfile"}', qos=1)
        
    def on_message_profile(self, client, userdata, message):
        """
        Receive the virtual patient profile and initialize the Bergman model
        """
        # Read the patient profile only once
        if self.stated:
            return
        
        payload = json.loads(message.payload.decode('utf-8'))
        # Update patient type
        diabetic = payload['shared']['PatientProfile']['diabetic'] if 'shared' in payload else payload['PatientProfile']['diabetic']
        self.patient_type = 'diabetic' if diabetic else 'normal'
        # Update the meal schedule
        meals = payload['shared']['PatientProfile']['meals'] if 'shared' in payload else payload['PatientProfile']['meals']
        self.meals = []
        for meal in meals:
            self.meals.append((float(meal['time']), float(meal['carbs']), float(meal['duration'])))
        self.meal_function = Meals(self.meals)
        # Upadta the bolus insulins
        bolus_insulins = payload['shared']['PatientProfile']['bolus_insulins'] if 'shared' in payload else payload['PatientProfile']['bolus_insulins']
        self.bolus_insulins = []
        for insulin in bolus_insulins:
            self.bolus_insulins.append((float(insulin['time']), float(insulin['dose']), float(insulin['duration'])))
        self.insulin_function = Insulin(bolus_insulin=self.bolus_insulins)
        print(f">>> Virtual Patient Profile Recieved: \n{self.patient_type=}, \n{self.meals=}, \n{self.bolus_insulins=}")
        # Initialize the Bergman model
        self.bergman = Bergman(type=self.patient_type, meals=self.meal_function, insulin=self.insulin_function)
        # Update bergman parameters
        params = payload['shared']['PatientProfile']['bergman_param'] if 'shared' in payload else payload['PatientProfile']['bergman_param']
        self.bergman.update_params(params)
        print(f">>> Customized Bergman Parameters: \n{params=}")
        
        # Simulation settings, initialize once, do not update
        if 'shared' in payload:
            self.disp_interval = float(payload['shared']['PatientProfile']['sim_settings']['disp_interval'])
            self.simu_interval = int(payload['shared']['PatientProfile']['sim_settings']['simu_interval'])
            self.simu_length = int(payload['shared']['PatientProfile']['sim_settings']['simu_length'])
            init_state = payload['shared']['PatientProfile']['sim_settings']['init_state']
            self.init_state = [float(init_state['G0']), float(init_state['X0']), float(init_state['I0'])]
            
            # Simulation storage
            self.solution = np.zeros((self.simu_length, 3))
            # Initial conditions
            self.solution[0, :] = self.init_state
            print(f">>> Simulation Settings: \n{self.disp_interval=}, \n{self.simu_interval=}, \n{self.simu_length=}, \n{self.init_state=}")
            
        self.stated = True
        
    def on_message_insulin(self, client, userdata, message):
        # print(f"Received insulin message: {message.payload.decode()}")
        # the basal insulin message handler 
        self.insulin_rate = float(json.loads(message.payload.decode('utf-8'))['insulin_rate'])
        if hasattr(self, 'insulin_function'): 
            self.insulin_function.update_basal_rate(self.insulin_rate)
            print("+", end='', flush=True)
        
    def loop_forever(self):
        try:
            print("Press CTRL+C to exit the simulation loop...")
            self.loop_start()
            while True:
                disp_interval = self.disp_interval if hasattr(self, 'disp_interval') else 1
                time.sleep(disp_interval)
                if not self.stated:
                    continue
                print('.', end='', flush=True)
                
                # simulate one step here
                t = self.time_step * self.simu_interval
                t_next = t + self.simu_interval
                self.time_step += 1
                res = solve_ivp(self.bergman.ode, (t, t_next), self.solution[self.time_step-1, :], args=())
                self.solution[self.time_step, :] = deepcopy(res.y[:, -1])
                
                # virtual CGM sensor, send to openAPS
                self.client.publish(self.topics['CGM_TOPIC'], json.dumps({'Glucose': self.solution[self.time_step][0], 'time': self.time_step * self.simu_interval}), qos=1)
                
                # ThingsBoard, just for display
                ts = int(datetime.now().timestamp() * 1000)
                data = [{
                    'ts': ts,
                    'values': {
                        'Insulin': self.solution[self.time_step][2],
                        'Glucose': self.solution[self.time_step][0],
                    }
                }]
                self.client.publish(self.topics['TB_VP_TELEMETRY_TOPIC'], json.dumps(data), qos=1)
                if self.time_step >= self.simu_length - 1:
                    print("\n>>> Simulation completed.")
                    break
                
        except Exception as e:
            print(f"{repr(e)}")
            traceback.print_exc()
        finally:
            print(">>> Disconnecting from the MQTT broker")
            self.loop_stop()
            self.disconnect()
    
    
def main():        
    MQTT_HOST = os.getenv('MQTT_HOST')
    MQTT_PORT = int(os.getenv('MQTT_PORT'))
    USERNAME = os.getenv('USERNAME')
    PASSWORD = os.getenv('PASSWORD')

    topics = {
        'TOPIC_PREFIX': os.getenv('TOPIC_PREFIX'),
        'TB_VP_ATTRIBUTE_TOPIC': os.getenv('TB_VP_ATTRIBUTE_TOPIC'),
        'TB_VP_TELEMETRY_TOPIC': os.getenv('TB_VP_TELEMETRY_TOPIC'),
        'TB_VP_RPC_TOPIC': os.getenv('TB_VP_RPC_TOPIC'),
        'INSULIN_TOPIC': os.getenv('INSULIN_TOPIC'),
        'CGM_TOPIC': os.getenv('CGM_TOPIC'),
    }
    print(f'>>> Topic settings from environment: \n{topics=}')
    
    vp_mqtt = VP_MQTT(MQTT_HOST, MQTT_PORT, USERNAME, PASSWORD, topics)
    vp_mqtt.loop_forever()
    

if __name__ == "__main__":
    main()