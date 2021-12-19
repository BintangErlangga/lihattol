#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h> //6.17.3
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

Servo myservo;

// replace the MAC address below by the MAC address printed on a sticker on the Arduino Shield 2
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EthernetClient client;

int    HTTP_PORT   = 80;
String HTTP_METHOD = "GET";
char   HOST_NAME[] = "192.168.1.57";// change to your PC's IP address
//char   HOST_NAME[] = "bilii.000webhostapp.com"; //kalo pake hosting
//String PATH_NAME   = "/data-api.php"; //kalo make ip tambahain folderny kek /rfidui/
String PATH_NAME = "/rfidui/data-api.php";
String getData;

#define SS_PIN 9
#define RST_PIN 8
#define buzzer 7

int servopin = 6;
int ir = A1;
int r = 2;
int y = 3;
int g = 4;
int x = 15;

//char pesan1[] = "1";
//char pesan2[] = "2";
//char pesan3[] = "3";

//untuk led
bool green = false;
bool red = false;

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void setup() {
  myservo.attach(6);
  Serial.begin(115200);
  pinMode (buzzer,OUTPUT);
  pinMode (r,OUTPUT);
  pinMode (y,OUTPUT);
  pinMode (g,OUTPUT);
  pinMode (ir, INPUT);
  while(!Serial);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
 // mfrc522.PDC_DumpVersionToSerial();
  
  //delay(3000);
  //START IP DHCP
  Serial.println("Konfigurasi DHCP, Silahkan Tunggu!");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("DHCP Gagal!");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet Tidak tereteksi :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Hubungkan kabel Ethernet!");
    }
    while (true) {delay(1);}
  }  
  //End DHCP 
  delay(5000); 
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());  
  client.connect(HOST_NAME, HTTP_PORT);
  Serial.println("Selamat Datang!");
  Serial.println("Tap Kartu Anda");
  
  digitalWrite(r, HIGH);
  digitalWrite(y, LOW);
  digitalWrite(g, LOW);
  myservo.write(0);

}

void loop() {
    digitalWrite(r, HIGH);
    digitalWrite(y, LOW);
    digitalWrite(g, LOW);
 
  //Baca data
   //Program yang akan dijalankan berulang-ulang
  if ( ! mfrc522.PICC_IsNewCardPresent()) {return;}
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {return;}
  
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String uidString;
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     uidString.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "" : ""));
     uidString.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  digitalWrite(r, HIGH);
  digitalWrite(y, LOW);
  digitalWrite(g, LOW);
  Serial.print("Message : ");
  uidString.toUpperCase();
  Serial.println(uidString);
  digitalWrite(buzzer,HIGH);
  delay(100);
  digitalWrite(buzzer,LOW);
    
  String namatol = "condet";

    //POST TO WEB
    client.connect(HOST_NAME, HTTP_PORT);
    client.println(HTTP_METHOD + " " + PATH_NAME + 
                   "?rfid=" + String(uidString) +
                   "&tol=" + namatol + 
                   " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println(); // end HTTP header
  
    while(client.connected()) {
      if(client.available()){
        char endOfHeaders[] = "\r\n\r\n";
        client.find(endOfHeaders);
        getData = client.readString();
        getData.trim();
        
        //AMBIL DATA JSON
        const size_t capacity = JSON_OBJECT_SIZE(10) + 160; //cari dulu nilainya pakai Arduino Json 5 Asisten
        DynamicJsonDocument doc(capacity);
        //StaticJsonDocument<192> doc;
        DeserializationError error = deserializeJson(doc, getData);
      
        const char* id_dibaca       = doc["id"]; 
        const char* rfid_dibaca     = doc["rfid"]; 
        const char* nama_dibaca     = doc["nama"]; 
        const char* alamat_dibaca   = doc["alamat"]; 
        const char* telepon_dibaca  = doc["telepon"];
        const char* saldo_dibaca    = doc["saldo"]; 
        const char* tanggal_dibaca  = doc["tanggal"];
        const char* tol_dibaca      = doc["namatol"];
        const char* ceksaldo        = doc["ceksaldo"];
        const char* harga_dibaca    = doc["harga"];
      
       //LOGIKA
       if( (String(nama_dibaca)!="") && (String(ceksaldo) == "cukup") ){
        green = true;
          
//          //POST TO WEB
//    client.connect(HOST_NAME, HTTP_PORT);
//    client.println(HTTP_METHOD + " " + "api-lcd.php" + 
//                   "?lcd=" + String(pesan2) + 
//                   " HTTP/1.1");
//    client.println("Host: " + String(HOST_NAME));
//    client.println("Connection: close");
//    client.println(); // end HTTP header

        //led nya
        Registered();
        buzzeroke();
        Serial.println("Kartu Terdaftar!");
        Serial.println(getData);
        //POST TO SERIAL
         Serial.print("ID       = ");Serial.println(id_dibaca);
         Serial.print("RFID     = ");Serial.println(rfid_dibaca);
         Serial.print("Nama     = ");Serial.println(nama_dibaca);
         Serial.print("Alamat   = ");Serial.println(alamat_dibaca);
         Serial.print("Telepon  = ");Serial.println(telepon_dibaca);
         Serial.print("Saldo    = ");Serial.println(saldo_dibaca);
         Serial.print("Waktu    = ");Serial.println(tanggal_dibaca);
         Serial.print("Tol    = ");Serial.println(tol_dibaca);
         Serial.print("Harga    = ");Serial.println(harga_dibaca);
         Serial.println();
         myservo.write(90);
//         int pos;
//           for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(x);                       // waits 15ms for the servo to reach the position
//  }
         Serial.println("Gerbang Terbuka");
         
         while(1){
          int vir = digitalRead(ir);
          if(vir == 1){
            continue;
          }else {
//            int pos;
//              for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(x);                       // waits 15ms for the servo to reach the position
//  }
//            //POST TO WEB
//            client.connect(HOST_NAME, HTTP_PORT);
//            client.println(HTTP_METHOD + " " + "api-lcd.php" + 
//                           "?lcd=" + String(pesan3) + 
//                           " HTTP/1.1");
//            client.println("Host: " + String(HOST_NAME));
//            client.println("Connection: close");
//            client.println(); // end HTTP header
            red = true;
            NotRegistered();
            delay(1000);
            myservo.write(0);
            Serial.println("Gerbang Tertutup");
            break;
          }
         }
       }else if( (String(nama_dibaca)!="") and (String(ceksaldo) == "kurang") ){
        digitalWrite(r, HIGH);
        digitalWrite(y, LOW);
        digitalWrite(g, LOW);
         Serial.print("ID       = ");Serial.println(id_dibaca);
         Serial.print("RFID     = ");Serial.println(rfid_dibaca);
         Serial.print("Nama     = ");Serial.println(nama_dibaca);
         Serial.print("Alamat   = ");Serial.println(alamat_dibaca);
         Serial.print("Telepon  = ");Serial.println(telepon_dibaca);
         Serial.print("Saldo    = ");Serial.println(saldo_dibaca);
         Serial.print("Waktu    = ");Serial.println(tanggal_dibaca);
         Serial.print("Tol    = ");Serial.println(tol_dibaca);
         Serial.print("Harga    = ");Serial.println(harga_dibaca);
         Serial.println("Saldo kurang, SILAHKAN TOPUP!!!");
       }
       else if(String(nama_dibaca)== "") {
        red = true;
        NotRegistered();
        buzzergagal();
        Serial.println("Kartu Tidak terdaftar!");
        Serial.print("Tol    = ");Serial.println(tol_dibaca);
        Serial.print("Harga    = ");Serial.println(harga_dibaca);
        Serial.println();
        Serial.println("Gerbang Tertutup");
       }
      } 
    }
    //delay(1000);
}

void buzzeroke(){
  digitalWrite(buzzer,HIGH);
  delay(100);
  digitalWrite(buzzer,LOW);
  delay(100);
  digitalWrite(buzzer,HIGH);
  delay(100);
  digitalWrite(buzzer,LOW);
  delay(100);
}

void buzzergagal(){
  digitalWrite(buzzer,HIGH);
  delay(1000);
  digitalWrite(buzzer,LOW);
  delay(10);
}

void Registered(){
  if(green && red == false){
    digitalWrite(r, LOW);
    digitalWrite(y, LOW);
    digitalWrite(g, HIGH);
  }else if(green && red){
    digitalWrite(r, LOW);
    digitalWrite(y, HIGH);
    delay(250);
    digitalWrite(y, LOW);
    digitalWrite(g, HIGH);
    red = 0;
  }
}

void NotRegistered(){
    if(green == false && red){
    digitalWrite(r, HIGH);
    digitalWrite(y, LOW);
    digitalWrite(g, LOW);
  }else if(green && red){
    digitalWrite(g, LOW);
    digitalWrite(y, HIGH);
    delay(250);
    digitalWrite(y, LOW);
    digitalWrite(r, HIGH);
    green = 0;
  }
}
