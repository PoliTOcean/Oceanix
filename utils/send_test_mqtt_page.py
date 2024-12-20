# send_test_mqtt_page.py
import tkinter as tk
from mqtt_handler import mqtt_send_message, MQTT_TOPIC_COMMANDS, MQTT_TOPIC_AXES

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