import tkinter as tk
from tkinter import ttk
import paho.mqtt.client as mqtt
import json

# MQTT settings
MQTT_BROKER = "10.0.0.254"
MQTT_TOPIC = "debug/"

# Define the callback for when a message is received
def on_message(client, userdata, msg):
    try:
        # Parse the JSON message
        data = json.loads(msg.payload)
        
        # Update the table with new values
        bar_state_var.set(data.get("bar_state", "N/A"))
        imu_state_var.set(data.get("imu_state", "N/A"))
        rov_armed_var.set(data.get("rov_armed", "N/A"))
        
        # Update controller states
        controller_state = data.get("controller_state", {})
        for key, var in controller_state_vars.items():
            var.set(controller_state.get(key, "N/A"))
        
        # Update motor thrust
        motor_thrust = data.get("motor_thrust", {})
        for key, var in motor_thrust_vars.items():
            var.set(motor_thrust.get(key, "N/A"))
        
        # Update PWM values
        pwm_values = data.get("pwm", {})
        for key, var in pwm_vars.items():
            var.set(pwm_values.get(key, "N/A"))
        
        # Update other values
        depth_var.set(data.get("depth", "N/A"))
        Zspeed_var.set(data.get("Zspeed", "N/A"))
        pitch_var.set(data.get("pitch", "N/A"))
        roll_var.set(data.get("roll", "N/A"))
        yaw_var.set(data.get("yaw", "N/A"))
        force_pitch_var.set(data.get("force_pitch", "N/A"))
        force_roll_var.set(data.get("force_roll", "N/A"))
        force_z_var.set(data.get("force_z", "N/A"))
        motor_thrust_max_xy_var.set(data.get("motor_thrust_max_xy", "N/A"))
        motor_thrust_max_z_var.set(data.get("motor_thrust_max_z", "N/A"))
        reference_pitch_var.set(data.get("reference_pitch", "N/A"))
        reference_roll_var.set(data.get("reference_roll", "N/A"))
        reference_z_var.set(data.get("reference_z", "N/A"))

    except json.JSONDecodeError:
        print("Error decoding JSON message")

# Set up the MQTT client
client = mqtt.Client()
client.on_message = on_message
client.connect(MQTT_BROKER, 1883, 60)
client.subscribe(MQTT_TOPIC, qos=0)

# GUI Setup
root = tk.Tk()
root.title("MQTT Data Viewer")

# Create table labels and variables for each parameter
bar_state_var = tk.StringVar()
imu_state_var = tk.StringVar()
rov_armed_var = tk.StringVar()
controller_state_vars = {
    "DEPTH": tk.StringVar(),
    "PITCH": tk.StringVar(),
    "ROLL": tk.StringVar()
}
motor_thrust_vars = {
    "FDX": tk.StringVar(),
    "FSX": tk.StringVar(),
    "RDX": tk.StringVar(),
    "RSX": tk.StringVar(),
    "UPFDX": tk.StringVar(),
    "UPFSX": tk.StringVar(),
    "UPRDX": tk.StringVar(),
    "UPRSX": tk.StringVar()
}
pwm_vars = {
    "FDX": tk.StringVar(),
    "FSX": tk.StringVar(),
    "RDX": tk.StringVar(),
    "RSX": tk.StringVar(),
    "UPFDX": tk.StringVar(),
    "UPFSX": tk.StringVar(),
    "UPRDX": tk.StringVar(),
    "UPRSX": tk.StringVar()
}
depth_var = tk.StringVar()
Zspeed_var = tk.StringVar()
pitch_var = tk.StringVar()
roll_var = tk.StringVar()
yaw_var = tk.StringVar()
force_pitch_var = tk.StringVar()
force_roll_var = tk.StringVar()
force_z_var = tk.StringVar()
motor_thrust_max_xy_var = tk.StringVar()
motor_thrust_max_z_var = tk.StringVar()
reference_pitch_var = tk.StringVar()
reference_roll_var = tk.StringVar()
reference_z_var = tk.StringVar()

# Create the table with three columns for data
labels = [
    ("Bar State", bar_state_var),
    ("IMU State", imu_state_var),
    ("ROV Armed", rov_armed_var),
    ("Depth", depth_var),
    ("Zspeed", Zspeed_var),
    ("Pitch", pitch_var),
    ("Roll", roll_var),
    ("Yaw", yaw_var),
    ("Force Pitch", force_pitch_var),
    ("Force Roll", force_roll_var),
    ("Force Z", force_z_var),
    ("Motor Thrust Max XY", motor_thrust_max_xy_var),
    ("Motor Thrust Max Z", motor_thrust_max_z_var),
    ("Reference Pitch", reference_pitch_var),
    ("Reference Roll", reference_roll_var),
    ("Reference Z", reference_z_var)
] + [(key, var) for key, var in controller_state_vars.items()]

# For motor thrust and PWM, we'll group them together into the motor section
motor_labels = [
    ("Motor Thrust (FDX)", motor_thrust_vars["FDX"]),
    ("Motor Thrust (FSX)", motor_thrust_vars["FSX"]),
    ("Motor Thrust (RDX)", motor_thrust_vars["RDX"]),
    ("Motor Thrust (RSX)", motor_thrust_vars["RSX"]),
    ("Motor Thrust (UPFDX)", motor_thrust_vars["UPFDX"]),
    ("Motor Thrust (UPFSX)", motor_thrust_vars["UPFSX"]),
    ("Motor Thrust (UPRDX)", motor_thrust_vars["UPRDX"]),
    ("Motor Thrust (UPRSX)", motor_thrust_vars["UPRSX"]),
    ("PWM (FDX)", pwm_vars["FDX"]),
    ("PWM (FSX)", pwm_vars["FSX"]),
    ("PWM (RDX)", pwm_vars["RDX"]),
    ("PWM (RSX)", pwm_vars["RSX"]),
    ("PWM (UPFDX)", pwm_vars["UPFDX"]),
    ("PWM (UPFSX)", pwm_vars["UPFSX"]),
    ("PWM (UPRDX)", pwm_vars["UPRDX"]),
    ("PWM (UPRSX)", pwm_vars["UPRSX"])
]

# Arrange the labels in a grid format with 3 columns
for i, (label, var) in enumerate(labels):
    tk.Label(root, text=label).grid(row=i, column=0, padx=10, pady=5)
    tk.Label(root, textvariable=var).grid(row=i, column=1, padx=10, pady=5)

# Place the motor-related information in the third column
for i, (label, var) in enumerate(motor_labels):
    tk.Label(root, text=label).grid(row=i, column=2, padx=10, pady=5)
    tk.Label(root, textvariable=var).grid(row=i, column=3, padx=10, pady=5)

# Start MQTT loop in a separate thread
import threading
def mqtt_loop():
    client.loop_forever()

mqtt_thread = threading.Thread(target=mqtt_loop, daemon=True)
mqtt_thread.start()

# Start the GUI
root.mainloop()
