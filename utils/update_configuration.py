import tkinter as tk
from tkinter import ttk
import paho.mqtt.client as mqtt
import json
import threading

# ============ MQTT CONFIGURATION ============
MQTT_BROKER = '10.0.0.254'
MQTT_PORT = 1883
MQTT_TOPIC_RECEIVE = 'config/'
MQTT_TOPIC_SEND = 'config/'
MQTT_TOPIC_REQUEST = 'state_commands/'

# Store incoming configuration
current_config = {}

# Keys to be edited in the GUI
editable_keys = [
    "minErrorIntZ", "maxErrorIntZ",
    "minErrorIntPitch", "maxErrorIntPitch",
    "minErrorIntRoll", "maxErrorIntRoll",
    "Kx0_Z", "Kx1_Z", "Ki_Z",
    "Kx0_Pitch", "Kx1_Pitch", "Ki_Pitch",
    "Kx0_Roll", "Kx1_Roll", "Ki_Roll",
    "pwm_slew_rate_max", "thrust_max_xy", "thrust_max_z", "Zspeed_beta"
]

# ============ MQTT CALLBACKS ============
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected successfully to MQTT broker!")
        client.subscribe(MQTT_TOPIC_RECEIVE)
        print(f"Subscribed to topic: {MQTT_TOPIC_RECEIVE}")
    else:
        print(f"Failed to connect, return code {rc}")

def on_message(client, userdata, msg):
    """Handles incoming MQTT configuration messages."""
    global current_config
    try:
        message = json.loads(msg.payload.decode('utf-8'))
        print(f"Received configuration: {message}")
        current_config = message
        load_config_into_gui(message)
    except json.JSONDecodeError:
        print("Failed to decode JSON message")

# ============ MQTT CLIENT SETUP ============
mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)

def run_mqtt():
    mqtt_client.loop_forever()

mqtt_thread = threading.Thread(target=run_mqtt)
mqtt_thread.daemon = True
mqtt_thread.start()

# ============ GUI FUNCTIONS ============
def load_config_into_gui(config):
    """Load configuration values into the input fields in the GUI."""
    for key, value in config.get("Controller", {}).items():
        if key in entry_widgets:
            entry_widgets[key].delete(0, tk.END)
            entry_widgets[key].insert(0, str(value))

    for key, value in config.get("Motors", {}).items():
        if key in entry_widgets:
            entry_widgets[key].delete(0, tk.END)
            entry_widgets[key].insert(0, str(value))
            
    for key, value in config.get("General", {}).items():
        if key in entry_widgets:
            entry_widgets[key].delete(0, tk.END)
            entry_widgets[key].insert(0, str(value))

def send_updated_configuration():
    """Send the updated configuration as a JSON message to the MQTT topic."""
    global current_config
    updated_config = current_config.copy()

    for key, entry in entry_widgets.items():
        try:
            value = entry.get()
            if value.lower() in ['true', 'false']:
                updated_value = value.lower() == 'true'
            elif value.isdigit():
                updated_value = int(value)
            else:
                updated_value = float(value)
        except ValueError:
            updated_value = value
        
        if key in updated_config["Controller"]:
            updated_config["Controller"][key] = updated_value
        elif key in updated_config["Motors"]:
            updated_config["Motors"][key] = updated_value
        elif key in updated_config["General"]:
            updated_config["General"][key] = updated_value

    print(f"Sending configuration: {updated_config}")
    mqtt_client.publish(MQTT_TOPIC_SEND, json.dumps(updated_config))

def request_configuration():
    """Request configuration from the device by sending a specific message."""
    request_message = {"REQUEST_CONFIG": 0}
    print(f"Requesting configuration: {request_message}")
    mqtt_client.publish(MQTT_TOPIC_REQUEST, json.dumps(request_message))

# ============ GUI SETUP ============
root = tk.Tk()
root.title("MQTT Configuration GUI")

# Create a scrollable frame
canvas = tk.Canvas(root)
scrollbar = ttk.Scrollbar(root, orient="vertical", command=canvas.yview)
scrollable_frame = tk.Frame(canvas)

scrollable_frame.bind(
    "<Configure>",
    lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
)

canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
canvas.configure(yscrollcommand=scrollbar.set)

canvas.pack(side="left", fill="both", expand=True)
scrollbar.pack(side="right", fill="y")

# Create input fields for each editable key
entry_widgets = {}

for i, param in enumerate(editable_keys):
    label = tk.Label(scrollable_frame, text=param)
    label.grid(row=i, column=0, sticky='w')

    entry = tk.Entry(scrollable_frame)
    entry.grid(row=i, column=1, padx=5, pady=2)
    
    entry_widgets[param] = entry

# Control buttons
button_frame = tk.Frame(root)
button_frame.pack(pady=10)

load_button = tk.Button(button_frame, text="Load Config", command=request_configuration)
load_button.pack(side='left', padx=5)

send_button = tk.Button(button_frame, text="Send Configuration", command=send_updated_configuration)
send_button.pack(side='left', padx=5)

# ============ RUN THE GUI ============
root.mainloop()
