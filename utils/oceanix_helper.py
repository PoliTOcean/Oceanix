# main_app.py
import tkinter as tk
from tkinter import ttk

# Import pages
from mqtt_config_page import MQTTConfigPage
from debug_mqtt_viewer_page import DebugMQTTViewerPage
from send_test_mqtt_page import SendTestMQTTPage
from update_configuration_page import UpdateConfigurationPage
from logger_page import LoggerPage

class MainApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Modular GUI with MQTT")
        self.geometry("800x900")

        # Create a navigation bar
        nav_bar = tk.Frame(self, bg="lightgrey")
        nav_bar.pack(side="top", fill="x")

        tk.Button(nav_bar, text="MQTT Config", command=lambda: self.show_frame("MQTTConfigPage")).pack(side="left", padx=5, pady=5)
        tk.Button(nav_bar, text="Status Viewer", command=lambda: self.show_frame("DebugMQTTViewerPage")).pack(side="left", padx=5, pady=5)
        tk.Button(nav_bar, text="Send Test MQTT", command=lambda: self.show_frame("SendTestMQTTPage")).pack(side="left", padx=5, pady=5)
        tk.Button(nav_bar, text="Update Config", command=lambda: self.show_frame("UpdateConfigurationPage")).pack(side="left", padx=5, pady=5)
        tk.Button(nav_bar, text="Logger", command=lambda: self.show_frame("LoggerPage")).pack(side="left", padx=5, pady=5) 

        # Create a container for the frames
        container = ttk.Frame(self)
        container.pack(side="top", fill="both", expand=True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        self.frames = {}
        for F in (MQTTConfigPage, DebugMQTTViewerPage, SendTestMQTTPage, UpdateConfigurationPage, LoggerPage): 
            page_name = F.__name__
            frame = F(parent=container, controller=self)
            self.frames[page_name] = frame
            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame("MQTTConfigPage")

    def show_frame(self, page_name):
        frame = self.frames[page_name]
        frame.tkraise()


if __name__ == "__main__":
    app = MainApp()
    app.mainloop()
