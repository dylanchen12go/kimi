#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Audio.h"
 
//创建蓝牙对象

// Digital I/O used
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

Audio audio;
String a="http://tsn.baidu.com/text2audio?tex=",b="&lan=zh&cuid=tian&ctp=1&tok=25.c4166cb6964292ff4b5ec190e075e51f.315360000.2040987557.282335-115492703";
// 1. Replace with your network credentials
const char* ssid = "fifer";
const char* password = "13301980123";

// 2. Replace with your OpenAI API key
const char* kimi_apiKey = "sk-Uy5NJrb7XD6h3bOAFP7NrBmvSnHth9rVGdsrSSE4qxRQGlta";

// Send request to OpenAI API
String inputText = "你好，kimi！";
String apiUrl = "https://api.moonshot.cn/v1/chat/completions";

String answer;
String getGPTAnswer(String inputText) {
  HTTPClient http;
  http.setTimeout(20000);
  http.begin(apiUrl);
  http.addHeader("Content-Type", "application/json");
  String token_key = String("Bearer ") + kimi_apiKey;
  http.addHeader("Authorization", token_key);
  String payload = 
    "{\"model\":\"moonshot-v1-8k\",\"messages\":["
    "{\"role\":\"system\",\"content\":\"你是陈翰钦的AI助手，你必须用中文回答且字数不超过25个\"},"
    "{\"role\":\"user\",\"content\":\"" + inputText + "\"}"
    "],\"temperature\": 0.3}";

  int httpResponseCode = http.POST(payload);
  if (httpResponseCode == 200) {
    String response = http.getString();
    http.end();
    Serial.println(response);

    // Parse JSON response
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, response);
    String outputText = jsonDoc["choices"][0]["message"]["content"];
    return outputText;
    // Serial.println(outputText);
  } else {
    http.end();
    Serial.printf("Error %i \n", httpResponseCode);
    String response = http.getString();
    Serial.printf("%s \n",response);
    return "<error>";
  }
}

void setup() {
  // Initialize Serial
  Serial.begin(115200);
  // Connect to Wi-Fi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  
  
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(15); // 0...21
  Serial.println(WiFi.localIP());
  answer = getGPTAnswer(inputText);
  Serial.println("Answer: " + answer);
  Serial.println("Enter a prompt:");
  audio.connecttohost((a+answer+b).c_str());
}

void loop() {
  // do nothing
  audio.loop();
  if (Serial.available()) {
    inputText = Serial.readString();
    // inputText.trim();
    Serial.println("\n Input:"+inputText);

    answer = getGPTAnswer(inputText);
    Serial.println("Answer: " + answer);
    Serial.println("Enter a prompt:");
    
    audio.connecttohost((a+answer+b).c_str());
  }
  // delay(2);
}
