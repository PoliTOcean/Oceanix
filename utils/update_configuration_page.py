# update_configuration_page.py
import tkinter as tk
from mqtt_handler import register_callback, mqtt_send_message, MQTT_TOPIC_COMMANDS, MQTT_TOPIC_CONFIG

class UpdateConfigurationPage(tk.Frame):
    def __init__(self, parent, controller):
        super().__init__(parent)
        self.controller = controller
        self.current_config = {}
        self.entry_widgets = {}
        self.editable_keys = [
            "minErrorIntZ", "maxErrorIntZ",
            "minErrorIntPitch", "maxErrorIntPitch",
            "minErrorIntRoll", "maxErrorIntRoll",
            "Kx0_Z", "Kx1_Z", "Ki_Z",
            "Kx0_Pitch", "Kx1_Pitch", "Ki_Pitch",
            "Kx0_Roll", "Kx1_Roll", "Ki_Roll",
            "pwm_slew_rate_max", "thrust_max_xy", "thrust_max_z", "Zspeed_beta"
        ]

        for i, param in enumerate(self.editable_keys):
            label = tk.Label(self, text=param)
            label.grid(row=i, column=0, sticky='w')

            entry = tk.Entry(self)
            entry.grid(row=i, column=1, padx=5, pady=2)
            
            self.entry_widgets[param] = entry

        button_frame = tk.Frame(self)
        button_frame.grid(row=len(self.editable_keys), columnspan=2, pady=10)

        load_button = tk.Button(button_frame, text="Load Config", command=self.request_configuration)
        load_button.pack(side='left', padx=5)

        send_button = tk.Button(button_frame, text="Send Configuration", command=self.send_updated_configuration)
        send_button.pack(side='left', padx=5)
        
        # Register the callback to handle incoming MQTT messages
        register_callback(self.load_config_into_gui)

    def load_config_into_gui(self, config, topic):
        global current_config
        # Assuming the config is received as a JSON object with relevant keys
        if topic != MQTT_TOPIC_CONFIG:
            return
        current_config = config
        print("Loading configuration into GUI:", config)
        for key, value in config.get("Controller", {}).items():
            if key in self.entry_widgets:
                self.entry_widgets[key].delete(0, tk.END)
                self.entry_widgets[key].insert(0, str(value))

        for key, value in config.get("Motors", {}).items():
            if key in self.entry_widgets:
                self.entry_widgets[key].delete(0, tk.END)
                self.entry_widgets[key].insert(0, str(value))
                
        for key, value in config.get("General", {}).items():
            if key in self.entry_widgets:
                self.entry_widgets[key].delete(0, tk.END)
                self.entry_widgets[key].insert(0, str(value))

    def send_updated_configuration(self):
        global current_config
        updated_config = current_config.copy()

        for key, entry in self.entry_widgets.items():
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
        mqtt_send_message(MQTT_TOPIC_CONFIG, updated_config)

    def request_configuration(self):
        request_message = {"REQUEST_CONFIG": 0}
        print(f"Requesting configuration: {request_message}")
        mqtt_send_message(MQTT_TOPIC_COMMANDS, request_message)