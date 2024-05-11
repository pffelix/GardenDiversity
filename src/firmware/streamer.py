# Publich to a ThingSpeak Channel Using MQTT
#
# This is an example of publishing to multiple fields simultaneously.
# Connections over standard TCP, websocket or SSL are possible by setting
# the parameters below.
#
# CPU and RAM usage is collected every 20 seconds and published to a
# ThingSpeak channel using an MQTT Publish
#
# This example requires the Paho MQTT client package which
# is available at: http://eclipse.org/paho/clients/python
import paho.mqtt.publish as publish
import json


class Streamer:

    def __init__(self):
        # The ThingSpeak Channel ID.
        # Replace <YOUR-CHANNEL-ID> with your channel ID.
        self.channel_ID = "1933237"

        # The hostname of the ThingSpeak MQTT broker.
        self.mqtt_host = "mqtt3.thingspeak.com"

        # Your MQTT credentials for the device
        self.mqtt_client_ID = "FDcHDRAtHTAOOxMHFBE8Owc"
        self.mqtt_username = "FDcHDRAtHTAOOxMHFBE8Owc"
        self.mqtt_password = "PwIVaBT71/0AojgUaIJX9nJY"

        self.t_transport = "websockets"
        self.t_port = 80

        # Create the topic string.
        self.topic = "channels/" + self.channel_ID + "/publish"

    def mqtt_publish(self, payload_object):

        # build the publish string.
        for entry in payload_object:
            payload_object[entry] = [(item, float(prediction)) for item, prediction in payload_object[entry]]
            payload_object[entry] = payload_object[entry][:4]
            payload = "field1=" + json.dumps(payload_object[entry])
            # attempt to publish this data to the topic.
            try:
                publish.single(self.topic, payload, hostname=self.mqtt_host, transport=self.t_transport, port=self.t_port, client_id=self.mqtt_client_ID, auth={'username':self.mqtt_username,'password':self.mqtt_password})
                print("MQTT published")
            except Exception as e:
                print(e)
