# mqtt_config_page.py
import tkinter as tk
from mqtt_handler import initialize_mqtt, MQTT_BROKER, MQTT_TOPIC_CONFIG, MQTT_TOPIC_COMMANDS, MQTT_TOPIC_AXES, MQTT_TOPIC_DEBUG

class MQTTConfigPage(tk.Frame):
    def __init__(self, parent, controller):
        super().__init__(parent)
        self.controller = controller

        # Default values
        default_broker = MQTT_BROKER
        default_topic_config = MQTT_TOPIC_CONFIG
        default_topic_commands = MQTT_TOPIC_COMMANDS
        default_topic_axes = MQTT_TOPIC_AXES
        default_topic_debug = MQTT_TOPIC_DEBUG

        tk.Label(self, text="MQTT Broker:").grid(row=0, column=0, padx=10, pady=10)
        self.broker_entry = tk.Entry(self)
        self.broker_entry.insert(0, default_broker)  # Set default value
        self.broker_entry.grid(row=0, column=1, padx=10, pady=10)

        tk.Label(self, text="MQTT Topic Config:").grid(row=1, column=0, padx=10, pady=10)
        self.topic_config_entry = tk.Entry(self)
        self.topic_config_entry.insert(0, default_topic_config)  # Set default value
        self.topic_config_entry.grid(row=1, column=1, padx=10, pady=10)

        tk.Label(self, text="MQTT Topic Commands:").grid(row=2, column=0, padx=10, pady=10)
        self.topic_commands_entry = tk.Entry(self)
        self.topic_commands_entry.insert(0, default_topic_commands)  # Set default value
        self.topic_commands_entry.grid(row=2, column=1, padx=10, pady=10)

        tk.Label(self, text="MQTT Topic Axes:").grid(row=3, column=0, padx=10, pady=10)
        self.topic_axes_entry = tk.Entry(self)
        self.topic_axes_entry.insert(0, default_topic_axes)  # Set default value
        self.topic_axes_entry.grid(row=3, column=1, padx=10, pady=10)

        tk.Label(self, text="MQTT Topic Debug:").grid(row=4, column=0, padx=10, pady=10)
        self.topic_debug_entry = tk.Entry(self)
        self.topic_debug_entry.insert(0, default_topic_debug)  # Set default value
        self.topic_debug_entry.grid(row=4, column=1, padx=10, pady=10)

        save_button = tk.Button(self, text="Save and Connect", command=self.save_and_connect)
        save_button.grid(row=5, column=0, columnspan=2, pady=10)
        
        self.connect_output_label = tk.Label(self, text="")
        self.connect_output_label.grid(row=6, column=0, columnspan=2, pady=10)
        #self.save_and_connect()

    def save_and_connect(self):
        broker = self.broker_entry.get()
        topic_config = self.topic_config_entry.get()
        topic_commands = self.topic_commands_entry.get()
        topic_axes = self.topic_axes_entry.get()
        topic_debug = self.topic_debug_entry.get()

        try:
            res = initialize_mqtt(broker, topic_config, topic_commands, topic_axes, topic_debug)
        except TimeoutError:
            self.connect_output_label.config(text="Failed to connect to MQTT broker", fg="red")
            
        if res == 0:
            self.connect_output_label.config(text="Connected successfully to MQTT broker", fg="green")