# debug_mqtt_viewer_page.py
import tkinter as tk
from mqtt_handler import register_callback, unregister_callback, MQTT_TOPIC_STATUS

class DebugMQTTViewerPage(tk.Frame):
    def __init__(self, parent, controller):
        super().__init__(parent)
        self.controller = controller

        # Variables to store data
        self.bar_state_var = tk.StringVar()
        self.imu_state_var = tk.StringVar()
        self.rov_armed_var = tk.StringVar()
        self.controller_state_vars = {key: tk.StringVar() for key in ["DEPTH", "PITCH", "ROLL"]}
        self.motor_thrust_vars = {key: tk.DoubleVar() for key in ["FDX", "FSX", "RDX", "RSX", "UPFDX", "UPFSX", "UPRDX", "UPRSX"]}
        self.pwm_vars = {key: tk.StringVar() for key in ["FDX", "FSX", "RDX", "RSX", "UPFDX", "UPFSX", "UPRDX", "UPRSX"]}
        self.depth_var = tk.StringVar()
        self.Zspeed_var = tk.StringVar()
        self.pitch_var = tk.StringVar()
        self.roll_var = tk.StringVar()
        self.yaw_var = tk.StringVar()
        self.force_pitch_var = tk.StringVar()
        self.force_roll_var = tk.StringVar()
        self.force_z_var = tk.StringVar()
        self.motor_thrust_max_xy_var = tk.StringVar()
        self.motor_thrust_max_z_var = tk.StringVar()
        self.reference_pitch_var = tk.StringVar()
        self.reference_roll_var = tk.StringVar()
        self.reference_z_var = tk.StringVar()

        # Left column for other info
        left_frame = tk.Frame(self)
        left_frame.grid(row=0, column=0, sticky="ns")

        labels = [
            ("Bar State", self.bar_state_var),
            ("IMU State", self.imu_state_var),
            ("ROV Armed", self.rov_armed_var),
            ("Depth", self.depth_var),
            ("Zspeed", self.Zspeed_var),
            ("Pitch", self.pitch_var),
            ("Roll", self.roll_var),
            ("Yaw", self.yaw_var),
            ("Force Pitch", self.force_pitch_var),
            ("Force Roll", self.force_roll_var),
            ("Force Z", self.force_z_var),
            ("Motor Thrust Max XY", self.motor_thrust_max_xy_var),
            ("Motor Thrust Max Z", self.motor_thrust_max_z_var),
            ("Reference Pitch", self.reference_pitch_var),
            ("Reference Roll", self.reference_roll_var),
            ("Reference Z", self.reference_z_var)
        ] + [(key, var) for key, var in self.controller_state_vars.items()]

        for i, (label, var) in enumerate(labels):
            tk.Label(left_frame, text=label, width=15, anchor="w").grid(row=i, column=0, padx=10, pady=5, sticky="w")
            tk.Label(left_frame, textvariable=var, width=6, 
                anchor="e", font='TkFixedFont').grid(row=i, column=1, padx=10, pady=5)

        # Right column for motor thrust bars
        right_frame = tk.Frame(self)
        right_frame.grid(row=0, column=1, sticky="ns")

        self.canvas = tk.Canvas(right_frame, width=800, height=400)
        self.canvas.pack()

        # Create labels for motor thrust and PWM values
        self.motor_labels = {}
        self.pwm_labels = {}
        self.thrust_text_labels = {}

        motor_keys = ["FDX", "FSX", "RDX", "RSX", "UPFDX", "UPFSX", "UPRDX", "UPRSX"]
        for i, key in enumerate(motor_keys):
            x = i * 70 + 45  # Adjusted to make the bars thicker and closer
            self.motor_labels[key] = self.canvas.create_rectangle(x-15, 200, x+15, 200, fill="blue")
            self.pwm_labels[key] = self.canvas.create_text(x, 350, text="PWM: N/A", font=("Arial", 10))
            self.canvas.create_text(x, 370, text=key, font=("Arial", 10))
            self.thrust_text_labels[key] = self.canvas.create_text(x, 390, text="Thrust: N/A", font=("Arial", 10))

        register_callback(self.update_data)

    def update_data(self, message, topic):
        if topic != MQTT_TOPIC_STATUS:
            return

        # Update the variables with the received message data
        self.bar_state_var.set(message.get("bar_state", "N/A"))
        self.imu_state_var.set(message.get("imu_state", "N/A"))
        self.rov_armed_var.set(message.get("rov_armed", "N/A"))

        controller_state = message.get("controller_state", {})
        for key, var in self.controller_state_vars.items():
            var.set(controller_state.get(key, "N/A"))

        motor_thrust = message.get("motor_thrust", {})
        for key, var in self.motor_thrust_vars.items():
            thrust = motor_thrust.get(key, "0.0")
            thrust = float(thrust)  # Ensure thrust is a float
            var.set(thrust)
            self.update_motor_thrust_bar(key, thrust)

        pwm_values = message.get("pwm", {})
        for key, var in self.pwm_vars.items():
            pwm = pwm_values.get(key, "N/A")
            var.set(pwm)
            self.canvas.itemconfig(self.pwm_labels[key], text=f"PWM: {pwm}")

        self.depth_var.set(message.get("depth", "N/A"))
        self.Zspeed_var.set(message.get("Zspeed", "N/A"))
        self.pitch_var.set(message.get("pitch", "N/A"))
        self.roll_var.set(message.get("roll", "N/A"))
        self.yaw_var.set(message.get("yaw", "N/A"))
        self.force_pitch_var.set(message.get("force_pitch", "N/A"))
        self.force_roll_var.set(message.get("force_roll", "N/A"))
        self.force_z_var.set(message.get("force_z", "N/A"))
        self.motor_thrust_max_xy_var.set(message.get("motor_thrust_max_xy", "N/A"))
        self.motor_thrust_max_z_var.set(message.get("motor_thrust_max_z", "N/A"))
        self.reference_pitch_var.set(message.get("reference_pitch", "N/A"))
        self.reference_roll_var.set(message.get("reference_roll", "N/A"))
        self.reference_z_var.set(message.get("reference_z", "N/A"))

    def update_motor_thrust_bar(self, motor, thrust):
        # Convert thrust value to a height for the bar (range from -3 to 3)
        height = 200 - (thrust * 50)  # 50 pixels per unit thrust
        x = self.canvas.coords(self.motor_labels[motor])[0] + 15
        self.canvas.coords(self.motor_labels[motor], x-15, height, x+15, 200)
        self.canvas.itemconfig(self.thrust_text_labels[motor], text=f"Thrust: {thrust:.2f}")

    def __del__(self):
        unregister_callback(self.update_data)