import tkinter as tk
import paho.mqtt.client as mqtt
import json

# MQTT settings
MQTT_BROKER = "10.0.0.254"
MQTT_PORT = 1883

# MQTT topics
TOPIC_STATE_COMMANDS = "state_commands/"
TOPIC_AXES = "axes/"

# Define the MQTT client
client = mqtt.Client()
client.connect(MQTT_BROKER, MQTT_PORT, 60)

# Function to send messages
def send_message(topic, payload):
    client.publish(topic, json.dumps(payload))
    print(f"Sent to {topic}: {payload}")

# Define button actions
def send_arm_rov():
    send_message(TOPIC_STATE_COMMANDS, {"ARM_ROV": 1})

def send_axes_zero():
    send_message(TOPIC_AXES, {"X": 0, "Y": 0, "Z": 0, "YAW": 0})

def send_axes_x():
    send_message(TOPIC_AXES, {"X": 10000, "Y": 0, "Z": 0, "YAW": 0})

def send_axes_z():
    send_message(TOPIC_AXES, {"X": 0, "Y": 0, "Z": 10000, "YAW": 0})

def send_request_config():
    send_message(TOPIC_STATE_COMMANDS, {"REQUEST_CONFIG": 0})

# GUI setup
root = tk.Tk()
root.title("MQTT Command Sender")

# Buttons
tk.Button(root, text="ARM ROV", command=send_arm_rov).grid(row=0, column=0, padx=10, pady=10)
tk.Button(root, text="Set Axes to (0, 0, 0, 0)", command=send_axes_zero).grid(row=1, column=0, padx=10, pady=10)
tk.Button(root, text="Set Axes to (10000, 0, 0, 0)", command=send_axes_x).grid(row=2, column=0, padx=10, pady=10)
tk.Button(root, text="Set Axes to (0, 0, 10000, 0)", command=send_axes_z).grid(row=3, column=0, padx=10, pady=10)
tk.Button(root, text="Request config", command=send_request_config).grid(row=4, column=0, padx=10, pady=10)

# Start the GUI loop
root.mainloop()

# Close the MQTT client on exit
client.disconnect()
