import { defineStore } from 'pinia'
import { ref } from 'vue'

export const useMqttStore = defineStore('mqtt', () => {

    /*  
    The composition API way of defining a Pinia store
    ref()s become state properties
    computed()s become getters
    function()s become actions 
    MQTT Paho Documentation:
        1. https://eclipse.dev/paho/index.php?page=clients/js/index.php 
        2. https://eclipse.dev/paho/files/jsdoc/Paho.MQTT.Client.html
    */ 

    // STATES 
    const mqtt              = ref(null);
    const host              = ref("broker.emqx.io");  // Host Name or IP address
    const port              = ref(8083);  // Port number
    const payload           = ref({"id": 620162321, "timestamp": 1702566538, "number": 0, "ledA": 0, "ledB": 0}); // Set initial values for payload
    const payloadTopic      = ref("");
    const subTopics         = ref({});

    // ACTIONS
    const onSuccess = () => {
        // Called when the connect acknowledgement has been received from the server.
    }

    const onConnected = (reconnect, URI) => {
        // Called when a connection is successfully made to the server after a connect() method.
        console.log(`Connected to: ${URI}, Reconnect: ${reconnect}`);      
        if (reconnect) {
            const topics = Object.keys(subTopics.value);
            if (topics.length > 0) {
                topics.forEach((topic) => {
                    subscribe(topic);
                });
            }
        }
    }

    const onConnectionLost = (response) => {
        // Called when a connection has been lost after a successful connect().
        if (response.errorCode !== 0) {
            console.log(`MQTT: Connection lost - ${response.errorMessage}`);
        }
    }

    const onFailure = (response) => {
        // Called when the connect request has failed or timed out.
        console.log(`MQTT: Connection to ${host.value} failed. \nError message: ${response.errorMessage}`);                  
    }

    const onMessageArrived = (response) => {
        // Called when a message has arrived in this Paho.MQTT.client.
        try {
            payload.value = JSON.parse(response.payloadString); 
            payloadTopic.value = response.destinationName;
            console.log(`Topic: ${payloadTopic.value} \nPayload: ${response.payloadString}`);  
        } catch (error) {
            console.log(`onMessageArrived Error: ${error}`);
        }
    }

    const makeid = (length) => {
        let result = '';
        const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
        for (let i = 0; i < length; i++) {
            result += characters.charAt(Math.floor(Math.random() * characters.length));
        }
        return "IOT_F_" + result;
    };

    // SUBSCRIBE UTIL FUNCTIONS
    const sub_onSuccess = (response) => {   
        const topic = response.invocationContext.topic;  
        console.log(`MQTT: Subscribed to - ${topic}`);  
        subTopics.value[topic] = "subscribed"; 
    }

    const sub_onFailure = (response) => {       
        const topic = response.invocationContext.topic;  
        console.log(`MQTT: Failed to subscribe to - ${topic} \nError message: ${response.errorMessage}`);  
    }

    const subscribe = (topic) => {
        try {
            const subscribeOptions = { onSuccess: sub_onSuccess, onFailure: sub_onFailure, invocationContext: { "topic": topic } };
            mqtt.value.subscribe(topic, subscribeOptions);   
        } catch (error) {
            console.log(`MQTT: Unable to Subscribe ${error}`);
        }
    }

    // UNSUBSCRIBE UTIL FUNCTIONS
    const unSub_onSuccess = (response) => {    
        const topic = response.invocationContext.topic;  
        console.log(`MQTT: Unsubscribed from - ${topic}`);          
        delete subTopics.value[topic];
    }

    const unSub_onFailure = (response) => {   
        const topic = response.invocationContext.topic;  
        console.log(`MQTT: Failed to unsubscribe from - ${topic} \nError message: ${response.errorMessage}`);  
    }

    const unsubscribe = (topic) => {     
        try {
            const unsubscribeOptions = { onSuccess: unSub_onSuccess, onFailure: unSub_onFailure, invocationContext: { "topic": topic } };
            mqtt.value.unsubscribe(topic, unsubscribeOptions);
        } catch (error) {
            console.log(`MQTT: Unable to Unsubscribe ${error}`);
        }
    }

    const unsubscribeAll = () => {   
        const topics = Object.keys(subTopics.value);
        if (topics.length > 0) {
            topics.forEach((topic) => {
                try {
                    const unsubscribeOptions = { onSuccess: unSub_onSuccess, onFailure: unSub_onFailure, invocationContext: { "topic": topic } };
                    mqtt.value.unsubscribe(topic, unsubscribeOptions);
                } catch (error) {
                    console.log(`MQTT: Unable to Unsubscribe ${error}`);
                }
            });
        }
        disconnect();
    }

    // PUBLISH UTIL FUNCTION
    const publish = (topic, payload) => { 
        const message = new Paho.MQTT.Message(payload);
        message.destinationName = topic;
        mqtt.value.publish(message);                     
    }

    // DISCONNECT UTIL FUNCTION
    const disconnect = () => {  
        mqtt.value.disconnect();                     
    }

    const connect = () => {
        const IDstring = makeid(12);
        console.log(`MQTT: Connecting to Server: ${host.value} Port: ${port.value}`);

        mqtt.value = new Paho.MQTT.Client(host.value, port.value, "/mqtt", IDstring);   

        const options = { 
            timeout: 3, 
            onSuccess: onSuccess, 
            onFailure: onFailure, 
            invocationContext: { "host": host.value, "port": port.value }, 
            useSSL: false, 
            reconnect: true, 
            uris: [`ws://${host.value}:${port.value}/mqtt`] 
        }; 

        mqtt.value.onConnectionLost = onConnectionLost;
        mqtt.value.onMessageArrived = onMessageArrived;
        mqtt.value.onConnected = onConnected;
        mqtt.value.connect(options);    
    };

    return {  
        payload,
        payloadTopic,
        subscribe,
        unsubscribe,
        unsubscribeAll,
        publish,
        connect,
        disconnect,
    }
}, { persist: true });


 