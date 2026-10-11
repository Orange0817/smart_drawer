#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>   
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SS_PIN 21  
#define RST_PIN 4  
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int btn = 15;
int val, last=LOW;
   
Servo myservo;  
MFRC522 rfid(SS_PIN, RST_PIN); 
  
struct RFIDTag {  
    uint8_t uid[4];
    String name;
};
   
struct RFIDTag tags[1] = {  
    {{187, 189, 75, 99}, "卡片"}, 
};
   
byte totalTags = sizeof(tags) / sizeof(RFIDTag);  
bool foundTag=false; 
   
void setup() { 
    Serial.begin(115200);
    pinMode(btn, INPUT);
    Wire.begin(33, 32);
    SPI.begin();
    rfid.PCD_Init(); 
    myservo.attach(26);
    myservo.write(180);
    delay(1000);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); 
    }
  
    display.clearDisplay();
    display.setTextSize(1);      
    display.setTextColor(SSD1306_WHITE); 
    display.setCursor(0, 0);     
    display.display(); 
}

void loop() {
    // 1. 每輪都要讀取按鈕狀態，沒有讀卡也能偵測！
    val = digitalRead(btn);
    
    // 這裡可以放不管有沒有讀卡，都要持續執行的 print 或動作
    // Serial.println("不論有沒有刷卡，我都會一直執行");

    // 2. 把 RFID 讀卡邏輯包在 if 裡面，只有「有新卡且可讀取」時才進去處理
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
        
        if (piccType == MFRC522::PICC_TYPE_MIFARE_MINI ||  
            piccType == MFRC522::PICC_TYPE_MIFARE_1K ||
            piccType == MFRC522::PICC_TYPE_MIFARE_4K) {
            
            Serial.print("十進制 UID：");
            printDec(rfid.uid.uidByte, rfid.uid.size);
            Serial.println();
            
            byte *id = rfid.uid.uidByte; 
            byte idSize = rfid.uid.size; 
            
            foundTag = false; // 每次讀卡前先重置旗標
            for (byte i = 0; i < totalTags; i++) {
                if (memcmp(tags[i].uid, id, idSize) == 0) {  
                    Serial.println(tags[i].name); 
                    foundTag = true;
                    break;
                }
            }
            
            if(foundTag == true){
                Serial.println("unlock");
                myservo.write(90);    
                display.clearDisplay();
                display.setCursor(0, 0); // 記得每次 clear 後重設游標位置
                display.println(F("unlock"));
                display.println(F("hello"));
                display.display(); 
            }
            else {
                Serial.println("lock");
                myservo.write(180);
                display.clearDisplay();
                display.setCursor(0, 0);
                display.println(F("Verification failed"));
                display.display();  
            }
        } else {
            Serial.println("不支援此卡類型");
        }
        
        // 使卡片進入休眠
        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
    }

    // 3. 獨立的按鈕處理邏輯（修正了原本 btn == HIGH 的錯誤，改用 val）
    if(val == HIGH){
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println(F("lock"));
        display.display();
        Serial.println("lock by button");
        myservo.write(180);
        foundTag = false;
    }

    // 後段的偵錯資訊，現在不管有沒有讀卡都會一直印了
    Serial.print("Button state: ");
    Serial.print(val);
    Serial.print(" Last: ");
    Serial.println(last);
    
    last = val;
    delay(100);
}

void printDec(byte *idnum, byte idnumSize) {
    for (byte i = 0; i < idnumSize; i++){
        Serial.print(idnum[i]);
        Serial.print(" ");
    }
}
