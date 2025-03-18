# logger_page.py
import tkinter as tk
from tkinter import ttk, scrolledtext
import time
import re
from mqtt_handler import register_callback, unregister_callback, MQTT_TOPIC_LOG

class LoggerPage(tk.Frame):
    def __init__(self, parent, controller):
        super().__init__(parent)
        self.controller = controller
        
        # Configure the layout
        self.columnconfigure(0, weight=1)
        self.rowconfigure(1, weight=1)
        
        # Header frame
        header_frame = tk.Frame(self)
        header_frame.grid(row=0, column=0, sticky="ew", padx=10, pady=5)
        
        # Title and control buttons
        tk.Label(header_frame, text="MQTT Logger - Topic: log/", font=('Arial', 14)).pack(side="left", padx=5)
        
        # Auto-scroll checkbox
        self.auto_scroll = tk.BooleanVar(value=True)
        tk.Checkbutton(header_frame, text="Auto-scroll", variable=self.auto_scroll).pack(side="right", padx=5)
        
        # Show timestamp checkbox
        self.show_timestamp = tk.BooleanVar(value=True)
        tk.Checkbutton(header_frame, text="Show Timestamp", variable=self.show_timestamp).pack(side="right", padx=5)
        
        # Clear button
        tk.Button(header_frame, text="Clear Log", command=self.clear_log).pack(side="right", padx=5)
        
        # Text area for log messages
        self.log_area = scrolledtext.ScrolledText(self, wrap=tk.WORD, width=80, height=30)
        self.log_area.grid(row=1, column=0, sticky="nsew", padx=10, pady=10)
        self.log_area.config(state=tk.DISABLED)
        
        # Configure text tags for coloring
        self.log_area.tag_configure("INFO", foreground="green")
        self.log_area.tag_configure("WARN", foreground="orange")
        self.log_area.tag_configure("ERROR", foreground="red")
        
        # Register to receive MQTT messages
        register_callback(self.on_mqtt_message)
        
    def on_mqtt_message(self, message, topic):
        # Only process messages from the log/ topic
        if topic == MQTT_TOPIC_LOG:
            # Format timestamp if enabled
            timestamp_prefix = ""
            if self.show_timestamp.get():
                timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
                timestamp_prefix = f"[{timestamp}] "
            
            # Determine message color based on content
            tag = None
            if "[INFO ]" in message:
                tag = "INFO"
            elif "[WARN ]" in message:
                tag = "WARN"
            elif "[ERROR]" in message:
                tag = "ERROR"
            
            # Format and display the message
            self.log_area.config(state=tk.NORMAL)
            self.log_area.insert(tk.END, timestamp_prefix, "")
            
            # Insert the actual message with appropriate color tag
            if tag:
                self.log_area.insert(tk.END, message, tag)
            else:
                self.log_area.insert(tk.END, message)
                
            self.log_area.config(state=tk.DISABLED)
            
            # Auto-scroll to the end if enabled
            if self.auto_scroll.get():
                self.log_area.see(tk.END)
    
    def clear_log(self):
        """Clear all content from the log area."""
        self.log_area.config(state=tk.NORMAL)
        self.log_area.delete(1.0, tk.END)
        self.log_area.config(state=tk.DISABLED)
        
    def __del__(self):
        """Clean up by unregistering the callback when the page is destroyed."""
        try:
            unregister_callback(self.on_mqtt_message)
        except:
            pass