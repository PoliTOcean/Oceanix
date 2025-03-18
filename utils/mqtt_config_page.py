# mqtt_config_page.py
import tkinter as tk
from mqtt_handler import initialize_mqtt, MQTT_BROKER, MQTT_TOPIC_CONFIG, MQTT_TOPIC_COMMANDS, MQTT_TOPIC_AXES, MQTT_TOPIC_STATUS, MQTT_TOPIC_ARM

class MQTTConfigPage(tk.Frame):
    def __init__(self, parent, controller):
        super().__init__(parent)
        self.controller = controller

        # Default values
        default_broker = MQTT_BROKER
        default_topic_config = MQTT_TOPIC_CONFIG
        default_topic_commands = MQTT_TOPIC_COMMANDS
        default_topic_axes = MQTT_TOPIC_AXES
        default_topic_status = MQTT_TOPIC_STATUS
        default_topic_arm = MQTT_TOPIC_ARM

        tk.Label(self, text="MQTT Broker:").grid(row=0, column=0, padx=10, pady=10)
        self.broker_entry_1 = tk.Entry(self)
        self.broker_entry_1.insert(0, default_broker[0])  # Set default value
        self.broker_entry_1.grid(row=0, column=1, padx=10, pady=10)
        self.broker_entry_2 = tk.Entry(self)
        self.broker_entry_2.insert(0, default_broker[1])  # Set default value
        self.broker_entry_2.grid(row=0, column=2, padx=10, pady=10)

        tk.Label(self, text="MQTT Topic Config:").grid(row=1, column=0, padx=10, pady=10)
        self.topic_config_entry = tk.Entry(self)
        self.topic_config_entry.insert(0, default_topic_config)  # Set default value
        self.topic_config_entry.grid(row=1, column=1, padx=10, pady=10)

        tk.Label(self, text="MQTT Topic Commands:").grid(row=2, column=0, padx=10, pady=10)
        self.topic_commands_entry = tk.Entry(self)
        self.topic_commands_entry.insert(0, default_topic_commands)  # Set default value
        self.topic_commands_entry.grid(row=2, column=1, padx=10, pady=10)
        
        tk.Label(self, text="MQTT Topic Arm:").grid(row=3, column=0, padx=10, pady=10)
        self.topic_arm_entry = tk.Entry(self)
        self.topic_arm_entry.insert(0, default_topic_arm)  # Set default value
        self.topic_arm_entry.grid(row=3, column=1, padx=10, pady=10)

        tk.Label(self, text="MQTT Topic Axes:").grid(row=4, column=0, padx=10, pady=10)
        self.topic_axes_entry = tk.Entry(self)
        self.topic_axes_entry.insert(0, default_topic_axes)  # Set default value
        self.topic_axes_entry.grid(row=4, column=1, padx=10, pady=10)

        tk.Label(self, text="MQTT Topic Status:").grid(row=5, column=0, padx=10, pady=10)
        self.topic_status_entry = tk.Entry(self)
        self.topic_status_entry.insert(0, default_topic_status)  # Set default value
        self.topic_status_entry.grid(row=5, column=1, padx=10, pady=10)

        save_button = tk.Button(self, text="Save and Connect", command=self.save_and_connect)
        save_button.grid(row=8, column=0, columnspan=2, pady=10)
        
        self.connect_output_label = tk.Label(self, text="")
        self.connect_output_label.grid(row=7, column=0, columnspan=2, pady=10)
        #self.save_and_connect()

    def save_and_connect(self):
        broker = list()
        broker.append(self.broker_entry_1.get())
        broker.append(self.broker_entry_2.get())
        topic_config = self.topic_config_entry.get()
        topic_commands = self.topic_commands_entry.get()
        topic_axes = self.topic_axes_entry.get()
        topic_status = self.topic_status_entry.get()
        topic_arm = self.topic_arm_entry.get()

        try:
            res = initialize_mqtt(broker, topic_config, topic_commands, topic_axes, topic_status, topic_arm)
        except (TimeoutError, ConnectionRefusedError):
            self.connect_output_label.config(text="Failed to connect to MQTT broker", fg="red")
            
        if res == 0:
            self.connect_output_label.config(text=f"Connected successfully to MQTT broker {broker[0]}", fg="green")
        elif res == 10:
            self.connect_output_label.config(text=f"Connected successfully to MQTT broker {broker[1]}", fg="green")