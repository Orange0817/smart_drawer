#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include <vector>

#define SS_PIN 21  
#define RST_PIN 4  
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo myservo;  
MFRC522 rfid(SS_PIN, RST_PIN); 
Preferences preferences;

const int btnPin = 15;
const int ledPin = 12;
const int servoPin = 26;

// 狀態管理
enum State { STATE_LOCKED, STATE_UNLOCKED, STATE_ADD_MODE };
State currentState = STATE_LOCKED;

// 計時變數 (Non-blocking)
unsigned long unlockTime = 0;
unsigned long addModeStartTime = 0;
const unsigned long AUTO_WARN_TIME = 30000; // 開鎖後 30 秒發出警報
const unsigned long ADD_TIMEOUT = 20000;    // 新增卡片逾時 20 秒

// 白名單動態陣列 (上限 5 張)
const int MAX_CARDS = 5;
std::vector<String> allowedCards;

// 序列埠接收緩衝區
String serialBuffer = "";

// 函式宣告
void loadCardsFromPreferences();
void saveCardsToPreferences();
void setLockState(bool unlock);
void handleSerialCommands();
void updateOLED(const String &line1, const String &line2 = "", const String &line3 = "");
String getUIDString(byte *uid, byte size);

void setup() {
    Serial.begin(115200);
    pinMode(btnPin, INPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    Wire.begin(33, 32);
    SPI.begin(18, 19, 23, 5); // 確保 VSPI 腳位正確
    rfid.PCD_Init(); 

    myservo.attach(servoPin);
    myservo.write(180); // 預設上鎖狀態

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 初始化失敗"));
        for (;;);
    }
    
    // 自 Preferences 載入白名單卡片
    loadCardsFromPreferences();

    updateOLED("System Ready", "Drawer Locked");
    Serial.println("SYSTEM_READY");
}

void loop() {
    // 1. 處理 Serial 指令
    handleSerialCommands();

    // 2. 實體按鈕讀取 (按下立刻上鎖)
    if (digitalRead(btnPin) == HIGH) {
        if (currentState != STATE_LOCKED) {
            setLockState(false);
            updateOLED("Manual Locked");
            Serial.println("LOCKED_BY_BUTTON");
            delay(300); // 簡易防彈跳
        }
    }

    // 3. 處理已開鎖但未鎖上的 30 秒警示
    if (currentState == STATE_UNLOCKED) {
        if (millis() - unlockTime >= AUTO_WARN_TIME) {
            digitalWrite(ledPin, HIGH); // 超過 30 秒亮警示燈
        }
    }

    // 4. 新增卡片模式 (逾時 20 秒檢測)
    if (currentState == STATE_ADD_MODE) {
        if (millis() - addModeStartTime >= ADD_TIMEOUT) {
            currentState = STATE_LOCKED;
            updateOLED("Timeout", "Return Normal");
            Serial.println("TIMEOUT");
            delay(1500);
            updateOLED("Drawer Locked");
        }
    }

    // 5. RFID 讀卡處理
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        String scannedUID = getUIDString(rfid.uid.uidByte, rfid.uid.size);
        Serial.print("SCANNED: ");
        Serial.println(scannedUID);

        if (currentState == STATE_ADD_MODE) {
            // 新增模式：回傳給網頁端由使用者存進資料庫
            Serial.print("TAG_SCANNED:");
            Serial.println(scannedUID);

            updateOLED("Card Scanned!", scannedUID);
            currentState = STATE_LOCKED; // 讀完切回待機
            delay(2000);
            updateOLED("Drawer Locked");
        } 
        else {
            // 一般開門檢驗模式
            bool isAuthorized = false;
            for (const String &uid : allowedCards) {
                if (uid.equalsIgnoreCase(scannedUID)) {
                    isAuthorized = true;
                    break;
                }
            }

            if (isAuthorized) {
                setLockState(true);
                updateOLED("Unlocked!", "Welcome");
                Serial.println("UNLOCK_SUCCESS");
            } else {
                setLockState(false);
                updateOLED("Access Denied", "Invalid Card");
                Serial.println("UNLOCK_FAILED");
                delay(1500);
                updateOLED("Drawer Locked");
            }
        }

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
    }
}

// 序列埠指令解析
void handleSerialCommands() {
    while (Serial.available()) {
        char c = (char)Serial.read();
        if (c == '\n' || c == '\r') {
            serialBuffer.trim();
            if (serialBuffer.length() > 0) {
                if (serialBuffer == "add") {
                    currentState = STATE_ADD_MODE;
                    addModeStartTime = millis();
                    updateOLED("Waiting Card...", "Scan within 20s");
                    Serial.println("WAITING_CARD");
                } 
                else if (serialBuffer == "CLEAR_CARDS") {
                    allowedCards.clear();
                    Serial.println("CARDS_CLEARED_TEMP");
                } 
                else if (serialBuffer.startsWith("SET_CARD:")) {
                    if (allowedCards.size() < MAX_CARDS) {
                        String newCard = serialBuffer.substring(9);
                        newCard.trim();
                        allowedCards.push_back(newCard);
                        Serial.print("CARD_ADDED_TEMP:");
                        Serial.println(newCard);
                    }
                } 
                else if (serialBuffer == "SAVE_CARDS") {
                    saveCardsToPreferences();
                    updateOLED("Cards Synced", "Count: " + String(allowedCards.size()));
                    Serial.println("SYNC_OK");
                    delay(1500);
                    updateOLED("Drawer Locked");
                }
            }
            serialBuffer = "";
        } else {
            serialBuffer += c;
        }
    }
}

// 開關鎖執行控制
void setLockState(bool unlock) {
    if (unlock) {
        currentState = STATE_UNLOCKED;
        unlockTime = millis();
        myservo.write(90);    // 解鎖角度
        digitalWrite(ledPin, LOW);
    } else {
        currentState = STATE_LOCKED;
        myservo.write(180);   // 上鎖角度
        digitalWrite(ledPin, LOW);
    }
}

// 格式化 UID 為 "XX XX XX XX" 格式
String getUIDString(byte *uid, byte size) {
    String res = "";
    for (byte i = 0; i < size; i++) {
        if (uid[i] < 0x10) res += "0";
        res += String(uid[i], HEX);
        if (i < size - 1) res += " ";
    }
    res.toUpperCase();
    return res;
}

// OLED 顯示輔助函式
void updateOLED(const String &line1, const String &line2, const String &line3) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(line1);
    if (line2.length() > 0) {
        display.setCursor(0, 20);
        display.println(line2);
    }
    if (line3.length() > 0) {
        display.setCursor(0, 40);
        display.println(line3);
    }
    display.display();
}

// 從 Preferences 載入白名單
void loadCardsFromPreferences() {
    preferences.begin("rfid_lock", true);
    allowedCards.clear();
    int count = preferences.getInt("count", 0);
    for (int i = 0; i < count; i++) {
        String key = "c_" + String(i);
        String card = preferences.getString(key.c_str(), "");
        if (card.length() > 0) {
            allowedCards.push_back(card);
        }
    }
    preferences.end();
    Serial.print("Loaded Cards Count: ");
    Serial.println(allowedCards.size());
}

// 儲存白名單到 Preferences
void saveCardsToPreferences() {
    preferences.begin("rfid_lock", false);
    preferences.clear(); // 清除舊卡片
    preferences.putInt("count", allowedCards.size());
    for (size_t i = 0; i < allowedCards.size(); i++) {
        String key = "c_" + String(i);
        preferences.putString(key.c_str(), allowedCards[i]);
    }
    preferences.end();
}
