# update_configuration_page.py
import tkinter as tk
from tkinter import ttk
from mqtt_handler import register_callback, mqtt_send_message, MQTT_TOPIC_COMMANDS, MQTT_TOPIC_CONFIG

class UpdateConfigurationPage(tk.Frame):
    def __init__(self, parent, controller):
        super().__init__(parent)
        self.controller = controller
        self.current_config = {}
        self.entry_widgets = {}
        self.labels = {}
        self.key_sections = {}  # Tracks which section each parameter belongs to

        # Create a canvas and scrollbar for scrollable interface
        self.canvas = tk.Canvas(self, borderwidth=0)
        self.scrollbar = ttk.Scrollbar(self, orient="vertical", command=self.canvas.yview)
        self.scrollable_frame = tk.Frame(self.canvas)

        # Configure the canvas
        self.scrollable_frame.bind(
            "<Configure>",
            lambda e: self.canvas.configure(
                scrollregion=self.canvas.bbox("all")
            )
        )
        self.canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")
        self.canvas.configure(yscrollcommand=self.scrollbar.set)

        # Layout the canvas and scrollbar
        self.canvas.grid(row=0, column=0, sticky="nsew")
        self.scrollbar.grid(row=0, column=1, sticky="ns")

        # Configure grid weights to make the canvas expand
        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(0, weight=1)

        # Create button frame inside the scrollable frame
        self.button_frame = tk.Frame(self.scrollable_frame)
        self.button_frame.grid(row=0, column=0, columnspan=2, pady=10, sticky="ew")

        load_button = tk.Button(self.button_frame, text="Load Config", command=self.request_configuration)
        load_button.pack(side='left', padx=5)

        send_button = tk.Button(self.button_frame, text="Send Configuration", command=self.send_updated_configuration)
        send_button.pack(side='left', padx=5)

        # Register the callback to handle incoming MQTT messages
        register_callback(self.load_config_into_gui)

    def load_config_into_gui(self, config, topic):
        if topic != MQTT_TOPIC_CONFIG:
            return

        self.current_config = config.copy()
        self.key_sections.clear()

        # Destroy existing widgets
        for label in self.labels.values():
            label.destroy()
        for entry in self.entry_widgets.values():
            entry.destroy()
        self.labels.clear()
        self.entry_widgets.clear()

        # Collect all parameters from all sections
        row_index = 1  # Start from row 1 to leave space for the button frame
        for section in config:
            if isinstance(config[section], dict):
                # Add a section label
                section_label = tk.Label(self.scrollable_frame, text=f"--- {section.upper()} ---", font=("Arial", 10, "bold"))
                section_label.grid(row=row_index, column=0, columnspan=2, pady=(10, 5), sticky="w")
                row_index += 1

                for param in config[section]:
                    # Track parameter's section
                    self.key_sections[param] = section

                    # Create label
                    self.labels[param] = tk.Label(self.scrollable_frame, text=param)
                    self.labels[param].grid(row=row_index, column=0, sticky='w', padx=10)

                    # Create entry
                    entry = tk.Entry(self.scrollable_frame)
                    entry.grid(row=row_index, column=1, padx=5, pady=2, sticky='ew')
                    self.entry_widgets[param] = entry

                    # Update entry value
                    entry.delete(0, tk.END)
                    entry.insert(0, str(config[section][param]))

                    row_index += 1

        # Update the scroll region
        self.scrollable_frame.update_idletasks()
        self.canvas.configure(scrollregion=self.canvas.bbox("all"))

    def send_updated_configuration(self):
        updated_config = self.current_config.copy()

        for param, entry in self.entry_widgets.items():
            value = entry.get()
            section = self.key_sections.get(param, "General")

            # Convert value to appropriate type
            try:
                if value.lower() in ['true', 'false']:
                    converted_value = value.lower() == 'true'
                elif '.' in value:
                    converted_value = float(value)
                else:
                    converted_value = int(value)
            except ValueError:
                converted_value = value  # Keep as string if conversion fails

            # Update the configuration
            if section not in updated_config:
                updated_config[section] = {}
            updated_config[section][param] = converted_value

        print(f"Sending updated configuration: {updated_config}")
        mqtt_send_message(MQTT_TOPIC_CONFIG, updated_config)

    def request_configuration(self):
        request_message = {"REQUEST_CONFIG": 0}
        print(f"Requesting configuration: {request_message}")
        mqtt_send_message(MQTT_TOPIC_COMMANDS, request_message)