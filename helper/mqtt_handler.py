# mqtt_handler.py
import paho.mqtt.client as mqtt
import json
import threading

# Global MQTT client
mqtt_client = None
mqtt_callbacks = []

# MQTT settings with default values
MQTT_BROKER = ["10.0.0.254", "127.0.0.1"]
MQTT_TOPIC_CONFIG = "config/"
MQTT_TOPIC_COMMANDS = "state_commands/"
MQTT_TOPIC_AXES = "axes/"
MQTT_TOPIC_STATUS = "status/"
MQTT_TOPIC_ARM = "arm_commands/"
MQTT_TOPIC_LOG = "log/"

def initialize_mqtt(broker, topic_config, topic_commands, topic_axes, topic_status, topic_arm):
    global mqtt_client, MQTT_BROKER, MQTT_TOPIC_CONFIG, MQTT_TOPIC_COMMANDS, MQTT_TOPIC_AXES, MQTT_TOPIC_STATUS, MQTT_TOPIC_ARM
    MQTT_BROKER = broker
    MQTT_TOPIC_CONFIG = topic_config
    MQTT_TOPIC_COMMANDS = topic_commands
    MQTT_TOPIC_AXES = topic_axes
    MQTT_TOPIC_STATUS = topic_status
    MQTT_TOPIC_ARM = topic_arm

    if mqtt_client is not None:
        mqtt_client.disconnect()

    mqtt_client = mqtt.Client()
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message
    try:
        result = mqtt_client.connect(broker[0], 1883, 60)
    except:
        print(f"Failed to connect to {broker[0]}, trying {broker[1]}")
        result = mqtt_client.connect(broker[1], 1883, 60)
        result += 10

    mqtt_thread = threading.Thread(target=mqtt_client.loop_forever)
    mqtt_thread.daemon = True
    mqtt_thread.start()

    mqtt_client.subscribe(topic_config)
    # mqtt_client.subscribe(topic_commands)
    mqtt_client.subscribe(MQTT_TOPIC_LOG)
    mqtt_client.subscribe(topic_status)
    return result

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected successfully to MQTT broker!")
    else:
        print(f"Failed to connect, return code {rc}")

def on_message(client, userdata, msg):
    try:
        message = json.loads(msg.payload.decode('utf-8'))
        for callback in mqtt_callbacks:
            callback(message, msg.topic)
    except json.JSONDecodeError:
        if msg.topic == MQTT_TOPIC_LOG:
            message = msg.payload.decode('utf-8')
            for callback in mqtt_callbacks:
                callback(message, msg.topic)
        else:
            print("Failed to decode JSON message")

def register_callback(callback):
    mqtt_callbacks.append(callback)

def unregister_callback(callback):
    mqtt_callbacks.remove(callback)
    
def mqtt_send_message(topic, payload):
    if mqtt_client is not None:
        mqtt_client.publish(topic, json.dumps(payload))
        print(f"Sent to {topic}: {payload}")
    else:
        print("MQTT client is not initialized")