# send_test_mqtt_page.py
import tkinter as tk
from mqtt_handler import mqtt_send_message, MQTT_TOPIC_COMMANDS, MQTT_TOPIC_AXES, MQTT_TOPIC_ARM

class SendTestMQTTPage(tk.Frame):
    def __init__(self, parent, controller):
        super().__init__(parent)
        self.controller = controller

        # Buttons for predefined commands
        tk.Button(self, text="ARM ROV", command=self.send_arm_rov).grid(row=0, column=0, padx=10, pady=10)
        tk.Button(self, text="Set Axes to (0, 0, 0, 0)", command=self.send_axes_zero).grid(row=1, column=0, padx=10, pady=10)
        tk.Button(self, text="Set Axes to (10000, 0, 0, 0)", command=self.send_axes_x).grid(row=2, column=0, padx=10, pady=10)
        tk.Button(self, text="Set Axes to (0, 0, 10000, 0)", command=self.send_axes_z).grid(row=3, column=0, padx=10, pady=10)
        tk.Button(self, text="Change controller status", command=self.change_controller_status).grid(row=4, column=0, padx=10, pady=10)
        tk.Button(self, text="Send Custom Axes", command=self.send_custom_axes).grid(row=5, column=0, padx=10, pady=10)
        

        # Sliders for custom values
        self.slider_x = self.create_slider("X", 6)
        self.slider_y = self.create_slider("Y", 7)
        self.slider_z = self.create_slider("Z", 8)
        self.slider_yaw = self.create_slider("YAW", 9)
        
        # New buttons for arm commands
        tk.Button(self, text="Rotate Wrist CCW", command=self.rotate_wrist_ccw).grid(row=0, column=2, padx=10, pady=10)
        tk.Button(self, text="Rotate Wrist CW", command=self.rotate_wrist_cw).grid(row=1, column=2, padx=10, pady=10)
        tk.Button(self, text="Stop Wrist", command=self.stop_wrist).grid(row=2, column=2, padx=10, pady=10)
        tk.Button(self, text="Open Nipper", command=self.open_nipper).grid(row=3, column=2, padx=10, pady=10)
        tk.Button(self, text="Close Nipper", command=self.close_nipper).grid(row=4, column=2, padx=10, pady=10)
        tk.Button(self, text="Stop Nipper", command=self.stop_nipper).grid(row=5, column=2, padx=10, pady=10)
        tk.Button(self, text="Torque Wrist On", command=self.torque_wrist_on).grid(row=6, column=2, padx=10, pady=10)
        tk.Button(self, text="Torque Wrist Off", command=self.torque_wrist_off).grid(row=7, column=2, padx=10, pady=10)


    def create_slider(self, label, row):
        tk.Label(self, text=label).grid(row=row, column=0, padx=10, pady=5)
        slider = tk.Scale(self, from_=-32000, to=32000, orient=tk.HORIZONTAL)
        slider.grid(row=row, column=1, padx=10, pady=5)
        return slider

    def send_message(self, topic, payload):
        mqtt_send_message(topic, payload)

    def send_arm_rov(self):
        self.send_message(MQTT_TOPIC_COMMANDS, {"ARM_ROV": 1})

    def send_axes_zero(self):
        self.send_message(MQTT_TOPIC_AXES, {"X": 0, "Y": 0, "Z": 0, "YAW": 0})

    def send_axes_x(self):
        self.send_message(MQTT_TOPIC_AXES, {"X": 10000, "Y": 0, "Z": 0, "YAW": 0})

    def send_axes_z(self):
        self.send_message(MQTT_TOPIC_AXES, {"X": 0, "Y": 0, "Z": 10000, "YAW": 0})

    def change_controller_status(self):
        self.send_message(MQTT_TOPIC_COMMANDS, {"CHANGE_CONTROLLER_STATE": 0})

    def send_custom_axes(self):
        x_value = self.slider_x.get()
        y_value = self.slider_y.get()
        z_value = self.slider_z.get()
        yaw_value = self.slider_yaw.get()
        self.send_message(MQTT_TOPIC_AXES, {"X": x_value, "Y": y_value, "Z": z_value, "YAW": yaw_value})
        
    def rotate_wrist_ccw(self):
        self.send_message(MQTT_TOPIC_ARM, {"command": "ROTATE_WRIST_CCW"})

    def rotate_wrist_cw(self):
        self.send_message(MQTT_TOPIC_ARM, {"command": "ROTATE_WRIST_CW"})

    def stop_wrist(self):
        self.send_message(MQTT_TOPIC_ARM, {"command": "STOP_WRIST"})

    def open_nipper(self):
        self.send_message(MQTT_TOPIC_ARM, {"command": "OPEN_NIPPER"})

    def close_nipper(self):
        self.send_message(MQTT_TOPIC_ARM, {"command": "CLOSE_NIPPER"})

    def stop_nipper(self):
        self.send_message(MQTT_TOPIC_ARM, {"command": "STOP_NIPPER"})

    def torque_wrist_on(self):
        self.send_message(MQTT_TOPIC_ARM, {"command": "TORQUE_WRIST_ON"})

    def torque_wrist_off(self):
        self.send_message(MQTT_TOPIC_ARM, {"command": "TORQUE_WRIST_OFF"})