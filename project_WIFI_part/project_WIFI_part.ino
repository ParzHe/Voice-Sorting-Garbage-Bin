#include <Wire.h>
#include <ESP8266WiFi.h>

int foodState = LOW;       // Food waste bin state
int hazardousState = LOW;  // Food waste bin state
int recycleState = LOW;    // Food waste bin state
int residualState = LOW;   // Food waste bin state

const char* ssid = "IoT";
const char* password = "12345678";

WiFiServer server(80);

void setup() {
  Serial.begin(9600); /* begin serial for debug */
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */

  Serial.print("Connecting to Internet ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  server.begin();
  for (int i = 0; i < 5; i++) {
    Serial.println("Server started");
    Serial.print("IP Address of network: ");
    Serial.println(WiFi.localIP());
    Serial.print("Copy and paste the following URL: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
  }
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  Serial.println("Waiting for new client");
  while (!client.available()) {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Process the request and update the bin status
  updateBins(request);

  // Generate and send the HTML page
  sendHtmlPage(client);

  Serial.println("Client disconnected");
  Serial.println("");
}

void updateBins(String request) {
  if (request.indexOf("/Food=OPEN") != -1) {
    sendI2CMessage("F");
    receiveI2CMessage();
  }
  if (request.indexOf("/Food=CLOSE") != -1) {
    sendI2CMessage("f");
    receiveI2CMessage();
  }

  // if (request.indexOf("/HAZARDOUS=OPEN") != -1) {
  //   sendI2CMessage(5);
  // }
  // if (request.indexOf("/HAZARDOUS=CLOSE") != -1) {
  //   sendI2CMessage(5);
  // }

  if (request.indexOf("/Recyclable=OPEN") != -1) {
    sendI2CMessage("C");
    receiveI2CMessage();
  }
  if (request.indexOf("/Recyclable=CLOSE") != -1) {
    sendI2CMessage("c");
    receiveI2CMessage();
  }

  if (request.indexOf("/Residual=OPEN") != -1) {
    sendI2CMessage("R");
    receiveI2CMessage();
  }
  if (request.indexOf("/Residual=CLOSE") != -1) {
    sendI2CMessage("r");
    receiveI2CMessage();
  }
}

// Sent HTML to client from internet, like a web browser.
void sendHtmlPage(WiFiClient& client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<title>Smart Voice Sorting Garbage Bin</title>");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
  client.println("</head>");

  client.println("<body style=\"height:100%;margin:0;background:#f6f6f6\">");
  client.println("<div style=\"height:10%;text-align:center;padding:3px;background:#4caf50;color:white;\">");
  client.println("<h1>Smart Voice Sorting Garbage Bin</h1>");
  client.println("</div>");
  client.println("<br/>");

  // Generate control modules for each bin
  generateBinControl(client, "Food", foodState);
  // generateBinControl(client, "Hazardous", hazardousState);
  generateBinControl(client, "Recyclable", recycleState);
  generateBinControl(client, "Residual", residualState);

  client.println("</html>");
}

// Generate bin control module
void generateBinControl(WiFiClient& client, const String& binName, int binStatus) {
  client.println("<div style=\"text-align:center;padding:10px;\">");
  client.print("<h2>" + binName + " Bin</h2>");
  client.print("Status: ");
  client.print(binStatus == HIGH ? "OPEN" : "CLOSE");
  client.println("<br><br>");

  client.println("<a href=\"/" + binName + "=OPEN\"><button>OPEN " + binName + "</button></a>");
  client.println("<a href=\"/" + binName + "=CLOSE\"><button>CLOSE " + binName + "</button></a>");
  client.println("<br/><br/>");
  client.println("</div>");
}

// sent instruction to slave - maker uno
void sendI2CMessage(String message) {
  char* temp = new char[message.length() +1];
  strcpy(temp, message.c_str());
  Wire.beginTransmission(8);  // Start communication with device at address 8
  Wire.write(temp);        // Send the message
  Wire.endTransmission();     // End transmission
  delete [] temp;
}

// Receive garbage bin state from maker uno
void receiveI2CMessage() {
  Wire.requestFrom(8, 1); /* request & read data of size 13 from slave - maker uno*/
  while (Wire.available()) {
    char c = Wire.read();
    Serial.print("Receive from Arduino: ");
    Serial.println(c);
    delay(300);
    updateBinState(foodState, c, 'F', 'f');
    updateBinState(hazardousState, c, 'H', 'h');
    updateBinState(recycleState, c, 'C', 'c');
    updateBinState(residualState, c, 'R', 'r');
  }
  delay(150);
}

// update status of the certain bin
void updateBinState(int& bin, char s, char h, char l) {
  /*
  args: 
  bin stands for the state of the specific bin
  s stands for state
  h stands for chractar standing for HIGH
  l stands for character standing for LOW
  */
  if (s == h) {
    bin = HIGH;
  }
  if (s == l) {
    bin = LOW;
  }
}