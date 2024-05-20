#include <DHT.h>
#include <DHT_U.h>

#include <DHT.h> //THƯ VIỆN CẢM BIẾN NHIỆT ĐỘ, ĐỘ ẨM CỦA DHT22
#include <SoftwareSerial.h> //THƯ VIỆN SỬ DỤNG GSM
#include <LiquidCrystal.h> // THƯ VIỆN MÀN HÌNH LCD
SoftwareSerial SIM900(2,3);//RX=3, TX=2
LiquidCrystal lcd(9,8,7,6,5,4); // KHỞI TẠO CHÂN LCD
// KHAI BÁO CHÂN CHO SIMULINO MEGA
const int trigPin = 12;
const int echoPin = 11;
const int Motor_Pin = 22;
const int Buzz_pin = 23;
const int ledlow = 26;
const int led1phan2 = 25;
const int ledfull = 24;
const int ledT= 10;
const int DHT22_pin = 13;
const int PH_pin = A0;
DHT dht(DHT22_pin,DHT22);
long duration;   //THỜI GIAN SÓNG ÂM PHẢN HỒI             // ĐỊNH NGHĨA BIẾN:
int khoang_cach,Level;
int buf[10],alo;////KHAI BÁO MẢNG BUF VÀ BIẾN ALO
float temperature;////KHAI BÁO BIẾN LƯU TRỮ NHIEETTJ DỘ
bool Motor; // HÀM BOOL ĐỂ ĐIỀU KHIỂN MOTOR ON(OFF)
void setup() 
{
  pinMode(trigPin, OUTPUT); // trigPin LÀ OUTPUT CHO CẢM BIẾN
  pinMode(echoPin, INPUT); ///echoPin là INPUT
  pinMode(Motor_Pin, OUTPUT);
  lcd.begin(20,4); //LCD CÓ 20 CỘT VÀ 4 HÀNG
  dht.begin();
  pinMode(ledlow, OUTPUT);///THIẾT LẬP ĐẦU RA BÁO ĐỘNG, CÁC LED BÁO HIỆU
  pinMode(led1phan2, OUTPUT);
  pinMode(ledfull, OUTPUT);
  pinMode(Buzz_pin, OUTPUT);
  pinMode(ledT, OUTPUT);
  SIM900.begin(9600);/////THIẾT LẬP KHỞI TẠO CHO GSM VÀ TERMINAL HIỂN THỊ THÔNG TIN GỬI
  SIM900.println("              Sending...");
  Serial.begin(9600);////THIẾT LẬP KHỞI TẠO CHO TERMINAL HIỂN THỊ THÔNG TIN NHẬN TỪ HỆ THỐNG
  Serial.println("                BELLO");
  Serial.println("     GIAM SAT MUC CHAT LONG TU XA");
  Serial.println("______________________________________");
  Serial.println();
  delay(100);
}
void loop() 
{
// XÓA CHÂN trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
///// ĐỌC CHÂN echoPin, TRẢ VỀ THỜI GIAN SÓNG ÂM PHẢN HỒI:
  duration = pulseIn(echoPin, HIGH);
////// TÍNH LẠI KHOẢNG CÁCH THEO CENTIMET
  khoang_cach = duration * 0.034 / 2;
  Level = map(khoang_cach, 0, 1106, 0, 100);

// IN THÔNG TIN RA LCD
  lcd.setCursor(0, 0);
  lcd.print("Liquid Level: ");
  lcd.print(Level);
  lcd.print("%     ");
  if (Level < 30) //khi mực chất lỏng < 30%
  {
    Motor = true; //motor chay
  }
  if (Level >= 99) //khi mực chất lỏng đến 99%
  {
    Motor = false; //motor ngừng HĐ
  }
  if (Motor) //khi motor HĐ(bool=true)
  {
    digitalWrite(Motor_Pin, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Pump: ON     Auto! ");
  }
  else //khi motor ngừng HĐ
  {
    digitalWrite(Motor_Pin, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Pump: OFF          ");
  }

  
/////BÁO MỰC CHẤT LỎNG TRONG BỂ CHỨA:
  if (Level < 46)//MỰC CHẤT LỎNG THẤP:
  {
    digitalWrite(ledlow, HIGH);
  }
  else
  {
    digitalWrite(ledlow, LOW);
  }
  if (Level > 47 and Level <57)//MỰC TRUNG BÌNH:
  {
    digitalWrite(led1phan2, HIGH);
  }
  else
  {
    digitalWrite(led1phan2, LOW);
  }
  if (Level >= 99)//MỰC CHẤT LỎNG ĐÃ ĐẦY:
  {
    digitalWrite(ledfull, HIGH);
    digitalWrite(Buzz_pin, HIGH);
  }
  else
  {
    digitalWrite(ledfull, LOW);
    digitalWrite(Buzz_pin, LOW);
  }

  
///HIỂN THỊ NHIỆT ĐỘ TRÊN LCD VÀ BÁO ĐÈN LEDT KHI NHIỆT ĐỘ KHÔNG ỔN ĐỊNH:
  temperature=dht.readTemperature();
  lcd.setCursor(0,2);
  lcd.print("Temp:");
  lcd.print(temperature);
  lcd.setCursor(9,2);
  lcd.print(" Do C");
  delay(100);
  if(temperature<=0 or temperature>=45)
  {
    digitalWrite(ledT,HIGH);
  }
  else
  {
    digitalWrite(ledT,LOW);
  }

  
/////ĐO ĐỘ PH CỦA NƯỚC:
  for(int i=0;i<10;i++) //lấy 10 mẫu 
  { 
    buf[i]=analogRead(PH_pin);
    delay(10);
  }
  for(int i=0;i<9;i++) //sắp xếp giá trị Analog tăng
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        alo=buf[i];
        buf[i]=buf[j];
        buf[j]=alo;
      }
    }
  }
  float avgValue=0;
  for(int i=2;i<8;i++) ///lấy giá trị trung bình(avg) của 6 giá trị ở giữa
    avgValue+=buf[i];
  float phValue=avgValue*5.0/1024/6; //chuyển Gtrị Analog sang mV
  phValue=3.5*phValue;  //chuyển mV sang giá trị pH
  lcd.setCursor(0,3);
  lcd.print("pH: ");  
  lcd.print(phValue,2);
  delay(800);
  guiSMS();////GỬI SMS
}
void guiSMS() {
  if (Level >= 99) {
    SIM900.println("Sending to contact: 0123456789");//SĐT NGƯỜI NHẬN
    Serial.println("SMS from: 0987654321");// SĐT HỆ THỐNG GỬI VỀ
    Serial.println("!!!!!MUC NUOC TRONG BE DA DAY!!!!!");
    Serial.print("MUC NUOC TRONG BE LA:");
    Serial.print(Level);
    Serial.print("%     ");
    Serial.println();
  }
  if (Level <= 10) {
    SIM900.println("Sending to contact: 0123456789");//SĐT NGƯỜI NHẬN
    Serial.println("SMS from: 0987654321");// SĐT HỆ THỐNG GỬI VỀ 
    Serial.println("!!!!!MUC NUOC TRONG BE SAP CAN!!!!!");
    Serial.print("MUC NUOC TRONG BE LA:");
    Serial.print(Level);
    Serial.print("%     ");
    Serial.println();
  }
  if (temperature <= 0) 
  {
    SIM900.println("Sending to contact: 0123456789");//SĐT NGƯỜI NHẬN
    Serial.println("SMS from: 0987654321");// SĐT HỆ THỐNG GỬI VỀ
    Serial.println("!!!!!NHIET DO TRONG BE CHUA QUA THAP!!!!!");
    Serial.print("NHIET DO TRONG BE LA:");
    Serial.print(temperature);
    Serial.print("Do C");
    Serial.println();
  }
  if (temperature>=45) 
  {
    SIM900.println("Sending to contact: 0123456789");//SĐT NGƯỜI NHẬN
    Serial.println("SMS from: 0987654321");// SĐT HỆ THỐNG GỬI VỀ
    Serial.println("!!!!!NHIET DO TRONG BE CHUA QUA CAO!!!!!");
    Serial.print("NHIET DO TRONG BE LA:");
    Serial.print(temperature);
    Serial.print("Do C");
    Serial.println();
  }
  delay(10000);
}
