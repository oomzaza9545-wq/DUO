#include <WiFi.h>
#include <WebServer.h>

// --- 1. ตั้งค่า WiFi (ใช้ Hotspot มือถือ) ---
const char* ssid = "Galaxy A36 5G 2E2D";       
const char* password = "Papayapogpog888"; 

WebServer server(80);

// --- 2. กำหนด Pin ตามโจทย์ ---
const int outputPin = 2; 

// --- 3. โค้ด HTML (หน้าเว็บมีทั้งปุ่มและช่องพิมพ์ข้อความ) ---
String getHTML(String status) {
  String html = "<!DOCTYPE html><html lang=\"th\"><head>";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"; 
  html += "<meta charset=\"UTF-8\">";
  html += "<title>ESP32 Control & Chat</title>";
  html += "<style>";
  html += "body { font-family: sans-serif; text-align: center; margin-top: 30px; background-color: #f4f4f9; }";
  html += "h1 { color: #333; font-size: 24px; }";
  html += "p { font-size: 18px; color: #666; }";
  html += ".btn { display: inline-block; padding: 15px; font-size: 20px; font-weight: bold; border-radius: 8px; text-decoration: none; color: white; margin: 8px; width: 80%; max-width: 300px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }";
  html += ".btn-on { background-color: #4CAF50; }"; 
  html += ".btn-off { background-color: #F44336; }"; 
  html += ".btn-send { background-color: #3B8ED0; border: none; cursor: pointer; }";
  html += "input[type=text] { padding: 12px; font-size: 16px; width: 60%; max-width: 200px; border-radius: 8px; border: 1px solid #ccc; margin-right: 5px; }";
  html += ".form-box { margin-top: 25px; padding: 20px; background-color: white; border-radius: 12px; display: inline-block; width: 85%; max-width: 350px; box-shadow: 0 4px 8px rgba(0,0,0,0.05); }";
  html += "</style></head><body>";
  
  html += "<h1>ควบคุม & ส่งข้อความ</h1>";
  html += "<p>สถานะล่าสุด: <strong>" + status + "</strong></p>";
  
  // --- ส่วนที่ 1: ปุ่มเปิด/ปิดไฟ ---
  html += "<a href=\"/on\" class=\"btn btn-on\">เปิดไฟ (ON)</a><br>";
  html += "<a href=\"/off\" class=\"btn btn-off\">ปิดไฟ (OFF)</a>";
  
  // --- ส่วนที่ 2: ช่องพิมพ์ข้อความส่งเข้า Serial Monitor ---
  html += "<div class=\"form-box\">";
  html += "<p style=\"margin-top:0; font-weight:bold; color:#333;\">ส่งข้อความเข้าบอร์ด</p>";
  html += "<form action=\"/send\" method=\"GET\">";
  html += "<input type=\"text\" name=\"msg\" placeholder=\"พิมพ์ที่นี่...\" required>";
  html += "<input type=\"submit\" value=\"ส่ง\" class=\"btn btn-send\" style=\"width: auto; padding: 12px 20px; font-size: 16px;\">";
  html += "</form>";
  html += "</div>";
  
  html += "</body></html>";
  return html;
}

// --- 4. ฟังก์ชันจัดการคำสั่งต่างๆ ---
void handleRoot() {
  server.send(200, "text/html", getHTML("พร้อมทำงาน ⏳"));
}

void handleOn() {
  digitalWrite(outputPin, HIGH);
  Serial.println("\n[ระบบ] สั่งเปิดไฟ (ON)");
  server.send(200, "text/html", getHTML("เปิดไฟแล้ว 🟢"));
}

void handleOff() {
  digitalWrite(outputPin, LOW);
  Serial.println("\n[ระบบ] สั่งปิดไฟ (OFF)");
  server.send(200, "text/html", getHTML("ปิดไฟแล้ว 🔴"));
}

// ฟังก์ชันใหม่: รับข้อความจากหน้าเว็บมาโชว์ใน Serial Monitor
void handleSend() {
  if (server.hasArg("msg")) { // เช็คว่ามีข้อความส่งมาไหม
    String msg = server.arg("msg"); // ดึงข้อความมาเก็บในตัวแปร
    
    // ปรินต์ข้อความลงหน้าจอคอมพิวเตอร์
    Serial.print("\n📩 ข้อความจากมือถือ: ");
    Serial.println(msg);
    
    // อัปเดตหน้าเว็บแจ้งว่าส่งสำเร็จ
    server.send(200, "text/html", getHTML("ส่ง '" + msg + "' สำเร็จ ✉️"));
  } else {
    server.send(400, "text/html", getHTML("ส่งข้อความไม่สำเร็จ ❌"));
  }
}

// --- 5. ตั้งค่าเริ่มต้น ---
void setup() {
  Serial.begin(115200);
  delay(1000); 

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected!");
  Serial.print(">>> เข้าเว็บด้วย IP นี้: ");
  Serial.println(WiFi.localIP()); 

  delay(1000); // รอให้ Serial ปรินต์จบก่อนยึดขา Pin 1

  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, LOW); 

  // ผูก URL กับฟังก์ชัน
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/send", handleSend); // เพิ่มการรับข้อความ

  server.begin();
}

// --- 6. ลูปการทำงาน ---
void loop() {
  server.handleClient();
}