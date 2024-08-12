#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP_Mail_Client.h>
#include <WiFi.h>

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial GPS(RXPin, TXPin);

// ESP32 Wi-Fi credentials
const char* ssid = "fruitu";
const char* password = "12345678";

// Email credentials
#define emailSenderAccount    "cameramodule95@gmail.com"
#define emailSenderPassword   "**** **** **** ****"
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort        465
#define emailSubject          "ESP32-CAM Photo Captured"
#define emailRecipient        "chinturejeti123@gmail.com"

// Pins connected to ESP32
const int pinA0 = A0;
const int pinA1 = 8;
const int pin13 = 13;
const int pin14 = 14;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  Serial.begin(115200);
  Serial.println();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  // Print ESP32 Local IP Address
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());

  // Initialize GPS
  GPS.begin(GPSBaud);

  // Set pin modes for Arduino
  pinMode(pinA0, INPUT);
  pinMode(pinA1, INPUT);
  pinMode(pin13, OUTPUT);
  pinMode(pin14, OUTPUT);
}

void loop() {
  // Read the values of A0 and A1 pins
  int valueA0 = digitalRead(pinA0);
  int valueA1 = digitalRead(pinA1);

  // Check if either A0 or A1 has a value of 1
  if (valueA0 == 1 || valueA1 == 1) {
    // Set the values of pins 13 and 14 to HIGH
    digitalWrite(pin13, HIGH);
    digitalWrite(pin14, HIGH);

    // Additional actions can be performed here based on the condition
    sendEmail(); // Send email
    sendGPSDetails(); // Send GPS details
  } else {
    // Reset pins 13 and 14 to LOW if both A0 and A1 are 0
    digitalWrite(pin13, LOW);
    digitalWrite(pin14, LOW);
  }

  // Add other functionalities as needed

  delay(1000); // Adjust delay as needed
}

void sendEmail() {
  // SMTP session config
  SMTPSession smtp;
  smtp.debug(1);
  smtp.callback(smtpCallback);

  Session_Config config;
  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = 0;
  config.time.day_light_offset = 1;

  config.server.host_name = smtpServer;
  config.server.port = smtpServerPort;
  config.login.email = emailSenderAccount;
  config.login.password = emailSenderPassword;
  config.login.user_domain = "";

  // Message
  SMTP_Message message;
  message.sender.name = "ESP32-CAM";
  message.sender.email = emailSenderAccount;
  message.subject = emailSubject;
  message.addRecipient("Recipient", emailRecipient);

  String htmlMsg = "<h2>Photo captured with ESP32-CAM and attached in this email.</h2>";
  message.html.content = htmlMsg.c_str();
  message.html.charSet = "utf-8";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_qp;

  SMTP_Attachment att;
  att.descr.filename = "photo.jpg";
  att.descr.mime = "image/png";
  att.file.path = "/photo.jpg";
  att.file.storage_type = esp_mail_file_storage_type_flash;
  att.descr.transfer_encoding = Content_Transfer_Encoding::enc_base64;
  message.addAttachment(att);

  // Connect to server and send email
  if (!smtp.connect(&config))
    return;
  if (!MailClient.sendMail(&smtp, &message, true))
    Serial.println("Error sending Email, " + smtp.errorReason());
}

void sendGPSDetails() {
  while (GPS.available() > 0) {
    gps.encode(GPS.read());
    if (gps.location.isUpdated()) {
      Serial.print("Latitude= "); 
      Serial.print(gps.location.lat(), 6);
      Serial.print(" Longitude= "); 
      Serial.println(gps.location.lng(), 6);
    }
  }
}

void smtpCallback(SMTP_Status status) {
  Serial.println(status.info());

  if (status.success()) {
    Serial.println("----------------");
    Serial.printf("Message sent success: %d\n", status.completedCount());
    Serial.printf("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");
    smtp.sendingResult.clear();
  }
}
