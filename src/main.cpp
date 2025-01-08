#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include <SPI.h>
#include <AudioGeneratorWAV.h>
#include <AudioOutputI2S.h>
#include <AudioFileSourceSD.h>

/* flags related to the commands given to the car */
#define STOP_WHEELS 0
#define MOVE_FORWARD 1
#define MOVE_LEFT 2
#define MOVE_RIGHT 3
#define MOVE_BACKWARDS 4
#define ACTIVATE_HORN 1
#define TOGGLE_OBSTACLE_AVOIDANCE 1
#define TOGGLE_HEADLIGHTS 2

/* motors identifiers */
#define LEFT_MOTORS 0
#define RIGHT_MOTORS 1

/* pins used by the motors */
#define LEFT_MOTORS_EN 32
#define LEFT_MOTORS_IN1 13
#define LEFT_MOTORS_IN2 27
#define RIGHT_MOTORS_IN3 14
#define RIGHT_MOTORS_IN4 12
#define RIGHT_MOTORS_EN 33

/* pin used by the infrared distance sensor */
#define IR_SENSOR 35

/* the time period for the infrared sensor to read data */
#define IR_SENSOR_READ_INTERVAL 25

/* the duration of car reversing when avoiding an obstacle */
#define REVERSING_TIME 250

/* the distance at which an obstacle should be avoided */
#define OBSTACLE_DISTANCE_THRESHOLD 15

/* pins assigned to the LEDs for headlights and taillights */
#define HEADLIGHTS 17
#define TAILLIGHTS 16

/* pins used by the SD Card Module using the SPI protocol */
#define SD_CS    5
#define SD_SCLK  18
#define SD_MOSI  23
#define SD_MISO  19

/* file paths for the audio files */
#define ACCELERATION_SOUND_PATH "/acceleration.wav"
#define HORN_SOUND_PATH "/horn.wav"
#define REVERSING_SOUND_PATH "/reverse.wav"

/* credentials of the Wi-Fi AP */
const char* SSID = "Wi-Fi_RC_Car";
const char* password = "qwerty123";

/* indicates whether the car is accelerating */
bool accelerating = false;

/* indicates whether the car is honking */
bool honking = false;

/* indicates whether the car is reversing */
bool reversing = false;

/* indicates whether any sound playback has stopped */
bool soundStopped = true;

/* indicates the current state of the obstacle avoidance feature */
bool avoidObstacles = false;

/* indicates whether any obstacle has been successfully avoided */
bool obstacleAvoided = false;

/* timestamp of the last sensor reading */
unsigned long lastSensorReadTime = 0;

/* timestamp of the last obstacle avoidance event */
unsigned long lastObstacleAvoidedTime = 0;

/* audio objects */
AudioGeneratorWAV *wav;
AudioFileSourceSD *file;
AudioOutputI2S *out;

/* create AsyncWebServer object on port 80 */
AsyncWebServer server(80);

/* create an AsyncWebSocket object to handle connections on the /ws path */
AsyncWebSocket ws("/ws");

/* store the HTML, CSS and JavaScript for the web page */
const char indexHtml[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
        <title>Wi-Fi RC Car</title>
        <style>
            body {
                display: flex;
                flex-direction: column;
                justify-content: center;
                align-items: center;
                height: 100vh;
                margin: 0;
                font-family: Arial, sans-serif;
                background-color: #1c1c1c;
                color: #ffffff;
                overflow: hidden;
            }
            .title {
                font-size: 2em;
                font-weight: bold;
                margin-bottom: 20px;
                color: #ffffff;
                text-align: center;
            }
            .controller {
                display: grid;
                grid-template-rows: repeat(6, 1fr);
                grid-template-columns: repeat(3, 1fr);
                gap: 10px;
                width: 100%;
                max-width: 320px;
                height: 100%;
                max-height: 500px;
            }
            .button {
                display: flex;
                justify-content: center;
                align-items: center;
                background-color: #007bff;
                color: white;
                font-size: 5vw;
                font-weight: bold;
                border: none;
                border-radius: 10px;
                cursor: pointer;
                outline: none;
                -webkit-tap-highlight-color: rgba(0, 0, 0, 0);
                box-shadow: 0 4px 6px rgba(0, 0, 0, 0.3);
                transition: background-color 0.2s, transform 0.2s;
            }
            .button:active {
                background-color: #0056b3;
                transform: translateY(2px);
            }
            .empty {
                pointer-events: none;
                background: none;
                box-shadow: none;
            }
            .noselect {
                -webkit-touch-callout: none;
                -webkit-user-select: none;
                -khtml-user-select: none;
                -moz-user-select: none;
                -ms-user-select: none;
                user-select: none;
            }
            .slider-container {
                grid-column: 1 / span 3;
                display: flex;
                flex-direction: column;
                justify-content: center;
                align-items: center;
                background-color: #333;
                padding: 10px;
                border-radius: 10px;
                box-shadow: 0 4px 6px rgba(0, 0, 0, 0.3);
            }
            .slider-title {
                font-size: 1.2em;
                margin-bottom: 10px;
                color: #ffffff;
            }
            .slider {
                width: 80%;
                accent-color: #007bff;
            }
            :root {
              touch-action: pan-x pan-y;
              height: 100% 
            }
        </style>
    </head>
    <body class="noselect">
        <div class="title">Wi-Fi RC Car</div>
        <div class="controller">
            <button class="button empty"></button>
            <button class="button" ontouchstart="sendCommand('move', '1')" ontouchend="sendCommand('move', '0')">
                &#9650;
            </button>
            <button class="button empty"></button>
            <button class="button" ontouchstart="sendCommand('move', '2')" ontouchend="sendCommand('move', '0')">
                &#9664;
            </button>
            <button class="button empty"></button>
            <button class="button" ontouchstart="sendCommand('move', '3')" ontouchend="sendCommand('move', '0')">
                &#9654;
            </button>
            <button class="button empty"></button>
            <button class="button" ontouchstart="sendCommand('move', '4')" ontouchend="sendCommand('move', '0')">
                &#9660;
            </button>
            <button class="button empty"></button>
            <button class="button" ontouchstart="sendCommand('activate', '1')" ontouchend="sendCommand('activate', '0')">       	
                Horn <br>&#128226;
            </button>
            <button class="button" ontouchstart="sendCommand('toggle', '1')">
                Avoid <br> &#128679;
            </button>
            <button class="button" ontouchstart="sendCommand('toggle', '2')">
                Lights <br>&#128161;
            </button>
            <button class="button empty"></button>
            <div class="slider-container">
                <div class="slider-title">Speed</div>
                <input type="range" min="127" max="255" value="255" class="slider" id="speedSlider" oninput="sendCommand('speed', this.value)">
            </div>
        </div>
        <script>
            var gateway = `ws://${window.location.hostname}/ws`;
            var websocket;

            window.addEventListener("load", onLoad);

            function initWebSocket() {
                console.log("Trying to open a WebSocket connection...");
                websocket = new WebSocket(gateway);
                websocket.onopen = onOpen;
                websocket.onclose = onClose;
                websocket.onmessage = onMessage;
            }
            function onOpen(event) {
                console.log("Connection opened");
            }
            function onClose(event) {
                console.log("Connection closed");
                setTimeout(initWebSocket, 2000);
            }
            function onMessage(event) {
            }
            function onLoad(event) {
                initWebSocket();
            }

            function sendCommand(command, value) {
                console.log(command, value);
                var data = command + "" + value;
                websocket.send(data);
            }
        </script>
    </body>
</html>
)rawliteral";

/*
 * Function that sets the direction of a specified pair of motors
 *
 * @param motors - the pair of motors to control
 * @param direction - the desired direction for the motors to spin
*/
void setMotorsDirection(uint8_t motors, uint8_t direction) {
  if (motors == LEFT_MOTORS) {
    switch (direction) {
      case MOVE_FORWARD: // move left motors forward
        digitalWrite(LEFT_MOTORS_IN1, LOW);
        digitalWrite(LEFT_MOTORS_IN2, HIGH); 
        break;
      case MOVE_BACKWARDS: // move left motors backwards
        digitalWrite(LEFT_MOTORS_IN1, HIGH);
        digitalWrite(LEFT_MOTORS_IN2, LOW);
        break;
      default: // stop left motors
        digitalWrite(LEFT_MOTORS_IN1, LOW);
        digitalWrite(LEFT_MOTORS_IN2, LOW);
        break;
    }
  } else if (motors == RIGHT_MOTORS) {
    switch (direction) {
      case MOVE_FORWARD: // move right motors forward
        digitalWrite(RIGHT_MOTORS_IN3, LOW);
        digitalWrite(RIGHT_MOTORS_IN4, HIGH); 
        break;
      case MOVE_BACKWARDS: // move right motors backwards
        digitalWrite(RIGHT_MOTORS_IN3, HIGH);
        digitalWrite(RIGHT_MOTORS_IN4, LOW);
        break;
      default: // stop right motors
        digitalWrite(RIGHT_MOTORS_IN3, LOW);
        digitalWrite(RIGHT_MOTORS_IN4, LOW);
        break;
    }
  }
}

/*
 * Function that handles the move command given to the car by controlling the motor directions
 * and updating the car's state (accelerating, reversing, etc.)
 *
 * @param direction - the desired direction for the car to move
 */
void moveWheels(uint8_t direction) {
  /* check if an obstacle was recently avoided, which might have caused the car to reverse */
  if (obstacleAvoided) {
    /* stop the motors before changing direction to prevent both input pins of a motor from being HIGH simultaneously */
    setMotorsDirection(LEFT_MOTORS, STOP_WHEELS);
    setMotorsDirection(RIGHT_MOTORS, STOP_WHEELS);

    /* reset the obstacle avoidance state */
    obstacleAvoided = false;
  }

  /* handle the desired movement direction */
  switch (direction) {
    case MOVE_FORWARD: // move the car forward
      /* set both pairs of motors to move forward */
      setMotorsDirection(LEFT_MOTORS, MOVE_FORWARD);
      setMotorsDirection(RIGHT_MOTORS, MOVE_FORWARD);

      /* turn off the taillights */
      digitalWrite(TAILLIGHTS, LOW);

      /* update the car's state to indicate it is accelerating, allowing the corresponding sound to be played */
      accelerating = true;
      reversing = false;
      break;

    case MOVE_BACKWARDS: // move the car backwards
      /* set both pairs of motors to move backwards */
      setMotorsDirection(LEFT_MOTORS, MOVE_BACKWARDS);
      setMotorsDirection(RIGHT_MOTORS, MOVE_BACKWARDS);

      /* turn off the taillights */
      digitalWrite(TAILLIGHTS, LOW);

      /* update the car's state to indicate it is reversing, allowing the corresponding sound to be played */
      accelerating = false;
      reversing = true;
      break;

    case MOVE_LEFT: // turn the car left
      /* set left motors to move backward */
      setMotorsDirection(LEFT_MOTORS, MOVE_BACKWARDS);

      /* set right motors to move forward */
      setMotorsDirection(RIGHT_MOTORS, MOVE_FORWARD);

      /* turn off the taillights */
      digitalWrite(TAILLIGHTS, LOW);
      break;

    case MOVE_RIGHT: // turn the car right
      /* set left motors to move forward */
      setMotorsDirection(LEFT_MOTORS, MOVE_FORWARD);

      /* set right motors to move backward */
      setMotorsDirection(RIGHT_MOTORS, MOVE_BACKWARDS);

      /* turn off the taillights */
      digitalWrite(TAILLIGHTS, LOW);
      break;

    default: // stop the car
      /* stop both pairs of wheels */
      setMotorsDirection(LEFT_MOTORS, STOP_WHEELS);
      setMotorsDirection(RIGHT_MOTORS, STOP_WHEELS);

      /* turn on the taillights to indicate braking */
      digitalWrite(TAILLIGHTS, HIGH);

      /* update the car's state to indicate it is neither accelerating nor reversing */
      accelerating = false;
      reversing = false;
      break;
  }
}

/*
 * Function that sets the motors speed using PWM
 *
 * @param speedValue - the desired speed value (127-255)
 */
void setMotorsSpeed(uint8_t speedValue) {
  analogWrite(LEFT_MOTORS_EN, speedValue);  // set speed for left motors
  analogWrite(RIGHT_MOTORS_EN, speedValue); // set speed for right motors
}

/*
 * Function that activates a specific feature of the car
 *
 * @param feature - the feature to activate
 */
void activateFeature(uint8_t feature) {
  switch (feature) {
    case ACTIVATE_HORN:
      honking = true; // enable the horn
      break;
    default:
      honking = false; // disable the horn by default
      break;
  }
}

/*
 * Function that toggles a specific feature of the car
 *
 * @param feature - the feature to toggle
 */
void toggleFeature(uint8_t feature) {
  switch (feature) {
    case TOGGLE_HEADLIGHTS:
      /* toggle the headlights on or off */
      digitalWrite(HEADLIGHTS, !digitalRead(HEADLIGHTS));
      break;
    case TOGGLE_OBSTACLE_AVOIDANCE:
      /* toggle the obstacle avoidance feature */
      avoidObstacles = !avoidObstacles;
      break;
    default:
      break; /* do nothing for unrecognized features */
  }
}

/*
 * Function that handles incoming WebSocket messages from clients
 *
 * @param arg - pointer to the WebSocket frame information
 * @param data - the data received from the client
 * @param len - the length of the data
 */
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  
  /* ensure the message is complete, not fragmented, and is a text frame */
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0; // null-terminate the received string
    
    /* check for a "speed" command */
    if (strncmp((char*)data, "speed", 5) == 0) {
      int speedValue = atoi((char*)data + 5); // extract speed value
      setMotorsSpeed(speedValue); // set the motors' speed
      Serial.printf("command: %s, value: %d\n", "speed", speedValue); 
      return;
    }
    
    /* extract the value from the last character of the data */
    int value = data[len - 1] - '0';
    data[len - 1] = 0; // null-terminate the command string
    
    Serial.printf("command: %s, value: %d\n", (char*)data, value); 
    
    /* handle commands based on the received data */
    if (strcmp((char*)data, "move") == 0) {
      moveWheels(value); // handle movement commands
    } else if (strcmp((char*)data, "activate") == 0) {
      activateFeature(value); // handle feature activation
    } else if (strcmp((char*)data, "toggle") == 0) {
      toggleFeature(value); // handle feature toggling
    }
  }
}


/*
 * Function that handles WebSocket events such as client connection, disconnection, and data reception
 *
 * @param server - the WebSocket server
 * @param client - the WebSocket client
 * @param type - the type of WebSocket event
 * @param arg - pointer to additional event information
 * @param data - the data sent by the client (if applicable)
 * @param len - the length of the data (if applicable)
 */
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT: // handle client connection
      Serial.printf("WebSocket client #%u connected from %s\n", 
                    client->id(), 
                    client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT: // handle client disconnection
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA: // handle incoming data from the client
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG: // handle a pong response (heartbeat check)
      // optional: add custom logic if needed
      break;
    case WS_EVT_ERROR: // handle a WebSocket error
      // optional: add error-handling logic if needed
      break;
  }
}

/*
 * Function that initializes the WebSocket protocol
 * Sets up the WebSocket event handler and links it to the server
 */
void initWebSocket() {
  ws.onEvent(onWebSocketEvent); // attach the WebSocket event handler
  server.addHandler(&ws);       // add the WebSocket handler to the server
}

/*
 * Function that handles HTTP GET requests on the root ("/") URL
 * Serves the HTML content stored in the "indexHtml" variable
 */
void handleRootRequests() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", indexHtml); // respond with the HTML page
  });
}

/*
 * Function that initializes the motor control pins
 * Configures the pins for the left and right motors as outputs and sets their initial states
 */
void initMotors() {
  /* configure pins for the left motors */
  pinMode(LEFT_MOTORS_EN, OUTPUT);   // enable pin for left motors
  pinMode(LEFT_MOTORS_IN1, OUTPUT);  // input 1 pin for left motors
  pinMode(LEFT_MOTORS_IN2, OUTPUT);  // input 2 pin for left motors

  /* set the initial state of the left motors to stopped */
  digitalWrite(LEFT_MOTORS_IN1, LOW);
  digitalWrite(LEFT_MOTORS_IN2, LOW);

  /* configure pins for the right motors */
  pinMode(RIGHT_MOTORS_EN, OUTPUT);   // enable pin for right motors
  pinMode(RIGHT_MOTORS_IN3, OUTPUT);  // input 3 pin for right motors
  pinMode(RIGHT_MOTORS_IN4, OUTPUT);  // input 4 pin for right motors

  /* set the initial state of the right motors to stopped */
  digitalWrite(RIGHT_MOTORS_IN3, LOW);
  digitalWrite(RIGHT_MOTORS_IN4, LOW);

  /* set the initial speed of both motors to maximum */
  analogWrite(LEFT_MOTORS_EN, 255);  // full speed for left motors
  analogWrite(RIGHT_MOTORS_EN, 255); // full speed for right motors
}


/*
 * Function that initializes the SD card and audio playback system
 */
void initSDAudio() {
  /* initialize SPI for SD card communication */
  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
  }

  /* initialize I2S for audio output (mono channel) */
  out = new AudioOutputI2S(0, 1); // 0 = left channel, 1 = mono
  out->SetGain(0.15); // set a lower gain to prevent distortion

  /* set up audio components */
  wav = new AudioGeneratorWAV();
  file = new AudioFileSourceSD();
}

/*
 * Function to initialize the car's headlights and taillights
 */
void initLights() {
  pinMode(HEADLIGHTS, OUTPUT);
  pinMode(TAILLIGHTS, OUTPUT);

  /* turn off lights initially */
  digitalWrite(HEADLIGHTS, LOW);
  digitalWrite(TAILLIGHTS, LOW);
}

/*
 * Function that checks and plays a sound based on a condition
 *
 * @param filePath - path to the audio file
 * @param condition - condition to determine if the sound should play
 */
void checkAndPlaySound(const char* filePath, bool condition) {
  if (condition && !wav->isRunning()) {
    file->open(filePath);
    wav->begin(file, out);
  } else if (condition && wav->isRunning()) {
    wav->loop(); // continue playing the sound
    soundStopped = false;
  } else if (!soundStopped && !accelerating && !honking && !reversing) {
    wav->stop(); // stop playback when no sound condition is active
    soundStopped = true;
  }
}

/*
 * Function that handles all sound-related functionality
 */
void handleSounds() {
  /* play sounds based on the current car state */
  checkAndPlaySound(ACCELERATION_SOUND_PATH, accelerating);
  checkAndPlaySound(HORN_SOUND_PATH, honking);
  checkAndPlaySound(REVERSING_SOUND_PATH, reversing);
}

/*
 * Function that detects obstacles and automatically avoid them
 */
void detectAndAvoidObstacles() {
  /* check if it's time to read the IR sensor */
  if ((millis() - lastSensorReadTime) >= IR_SENSOR_READ_INTERVAL) {
    uint16_t analogValue = analogRead(IR_SENSOR); // read sensor value
    float volts = (analogValue * 3.3) / 4095.00;  // convert to voltage
    int cmDistance = 29.988 * pow(volts, -1.173); // convert to distance in cm

    /* check if an obstacle is detected within the threshold distance */
    if (cmDistance <= OBSTACLE_DISTANCE_THRESHOLD) {
      moveWheels(STOP_WHEELS); // stop the car
      accelerating = false;   // mark the fact that the car is not accelerating
      moveWheels(MOVE_BACKWARDS); // reverse the car
      reversing = true; // mark the fact that the car is reversing
      lastObstacleAvoidedTime = millis(); // log the reversing start time
      obstacleAvoided = true; // mark the fact that an obstacle was avoided
    }

    lastSensorReadTime = millis(); // log the sensor read time
  }

  /* stop reversing after the defined reversing time has elapsed */
  if ((millis() - lastObstacleAvoidedTime) >= REVERSING_TIME && obstacleAvoided) {
    moveWheels(STOP_WHEELS); // stop the car
    obstacleAvoided = false; // mark the fact that the obstacle avoidance stopped
    reversing = false; // mark the fact that the car stopped reversing
  }
}

void setup() {
  Serial.begin(115200);

  /* start the Wi-Fi AP with the credentials defined */
  WiFi.softAP(SSID, password);

  /* print the IP address of the AP */
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  
  /* initialize the WebSocket protocol */
  initWebSocket();

  /* handle requests on the root ("/") URL */
  handleRootRequests();

  /* start the server */
  server.begin();

  /* initialize the motors */
  initMotors();

  /* initialize the SD Card and audio playback system */
  initSDAudio();

  /* initialize the car's headlights and taillights */
  initLights();
} 

void loop() {
  /* limit the number of clients by closing the oldest client when maximum number of clients has been exceeded */
  ws.cleanupClients();

  /* handle all sound-related functionality */
  handleSounds();

  /* check the state of the obstacle avoidance feature */
  if (avoidObstacles == true) {
    detectAndAvoidObstacles(); // detect and avoid potential collision
  }
}