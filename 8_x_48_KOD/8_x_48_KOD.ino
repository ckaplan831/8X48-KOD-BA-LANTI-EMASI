

char msg[] =" CUMA KAPLAN ";// EKRANDA YAZMASINI ISTEDİĞİNİZ YAZIYI YAZIN "CUMA KAPLAN";
int scrollspeed=10;//YAZININ KAYIŞ HIZINI AYARLAYIN 0'A YAKLAŞTIKÇA DAHA HIZLI KAYACAKTIR YAZI

int x;
int y;


int clockPin1 = 2; //74HC595 İN 11. PİNİNİ ARDUİNO ÜZERİNDE 2 İNCİ PİNE BAĞLADIK 
int latchPin1 = 3; //74HC595 İN 12. PİNİNİ ARDUİNO ÜZERİNDE 3 İNCİ PİNE BAĞLADIK // SUTUNLARIN OLDUGU 595 
int dataPin1 = 4;  //74HC595 İN 14. PİNİNİ ARDUİNO ÜZERİNDE 4 İNCİ PİNE BAĞLADIK 


int clockPin2 = 6; //74HC595 İN 11. PİNİNİ ARDUİNO ÜZERİNDE 6 İNCİ PİNE BAĞLADIK 
int latchPin2 = 5; //74HC595 İN 12. PİNİNİ ARDUİNO ÜZERİNDE 5 İNCİ PİNE BAĞLADIK  // SATIRLARIN OLDUGU 595 ENTEGRESI YANI TRANSİSTÖRLERİN BAĞLI OLDUGU YER
int dataPin2 = 7;  //74HC595 İN 14. PİNİNİ ARDUİNO ÜZERİNDE 7 İNCİ PİNE BAĞLADIK 

// BITMAP
// Bu dizideki bitler matristeki bir LED'i temsil eder
// 8 satır sayısıdır, 6 LED matris sayısıdır
byte bitmap[8][7]; 

int numZones = sizeof(bitmap) / 8; // Bir Bölge bir adet 8 x 8 Matriksi ifade eder (8 sütunlu grup)
int maxZoneIndex = numZones-1;
int numCols = numZones * 8;

//HARFLERİ TANIMLIYALIM 
byte alphabets[][8] = {
 {0,0,0,0,0},//@ as SPACE
 //{8,28,54,99,65},//<<
 {31, 36, 68, 36, 31},//A
 {127, 73, 73, 73, 54},//B
 {62, 65, 65, 65, 34},//C
 {127, 65, 65, 34, 28},//D
 {127, 73, 73, 65, 65},//E
 {127, 72, 72, 72, 64},//F
 {62, 65, 65, 69, 38},//G
 {127, 8, 8, 8, 127},//H
 {0, 65, 127, 65, 0},//I
 {2, 1, 1, 1, 126},//J
 {127, 8, 20, 34, 65},//K
 {127, 1, 1, 1, 1},//L
 {127, 32, 16, 32, 127},//M
 {127, 32, 16, 8, 127},//N
 {62, 65, 65, 65, 62},//O
 {127, 72, 72, 72, 48},//P
 {62, 65, 69, 66, 61},//Q
 {127, 72, 76, 74, 49},//R
 {50, 73, 73, 73, 38},//S
 {64, 64, 127, 64, 64},//T
 {126, 1, 1, 1, 126},//U
 {124, 2, 1, 2, 124},//V
 {126, 1, 6, 1, 126},//W
 {99, 20, 8, 20, 99},//X
 {96, 16, 15, 16, 96},//Y
 {67, 69, 73, 81, 97},//Z
};

void setup() {
 pinMode(latchPin1, OUTPUT);
 pinMode(clockPin1, OUTPUT);
 pinMode(dataPin1, OUTPUT);

 pinMode(latchPin2, OUTPUT);
 pinMode(clockPin2, OUTPUT);
 pinMode(dataPin2, OUTPUT);
 
 //Clear bitmap
 for (int row = 0; row < 8; row++) {
   for (int zone = 0; zone <= maxZoneIndex; zone++) {
     bitmap[row][zone] = 0; 
   }
 }
}

// FONKSİYONLARI
// Matristeki bitmap dizisini görüntüler
void RefreshDisplay()
{
 for (int row = 0; row < 8; row++) {
   int rowbit = 1 << row;
   digitalWrite(latchPin2, LOW);// Veri iletimi için latchPin LOW tuşunu basılı tutun
   shiftOut(dataPin2, clockPin2, MSBFIRST, rowbit);   //Veri ilet

  // Sütun baytlarını göndermeye başla
   digitalWrite(latchPin1, LOW);// Veri iletimi için latchPin LOW tuşunu basılı tutun

   // Her matrise geç
   for (int zone = maxZoneIndex; zone >= 0; zone--) 
   {
     shiftOut(dataPin1, clockPin1, MSBFIRST, bitmap[row][zone]);
   }

   // titremeyi önlemek için iki mandalı aynı anda çevirin
   digitalWrite(latchPin1, HIGH);// Mandal pimini 1 yüksek sinyal çipine döndürün
   digitalWrite(latchPin2, HIGH);// Mandal pimini 2 yüksek sinyal çipine döndürün
   //Wait
   delayMicroseconds(300);
 }
}

// Satırı ve sütunu bitmap bitine dönüştürür ve kapatır / açar
void Plot(int col, int row, bool isOn)
{
 int zone = col / 8;
 int colBitIndex = x % 8;
 byte colBit = 1 << colBitIndex;
 if (isOn)
   bitmap[row][zone] =  bitmap[y][zone] | colBit;
 else
   bitmap[row][zone] =  bitmap[y][zone] & (~colBit);
}
// Mesajın her karakterini birer birer sütun halinde işaretleyin, ekranı güncelleyin, bitmap sola kaydırın.
void XProcess()
{
 for (int charIndex=0; charIndex < (sizeof(msg)-1); charIndex++)
 {
   int alphabetIndex = msg[charIndex] - '@';
   if (alphabetIndex < 0) alphabetIndex=0;
   
   // Mesajın bir karakterini çiz
   // Her karakter 5 sütun genişliğindedir, karakterler arasında 2 piksel boşluk oluşturmak için iki kez daha döngü yapın
   for (int col = 0; col < 7; col++)
   {
     for (int row = 0; row < 8; row++)
     {
       // 0'dan 4'e kadar olan sütunlar için alfabenin söylediği şeye ayarlayın, ancak her zaman 5 ve 6 numaralı sütunları boş bırakın.
       bool isOn = 0; 
       if (col<5) isOn = bitRead( alphabets[alphabetIndex][col], 7-row ) == 1;
       Plot( numCols-1, row, isOn); // En sağdaki sütunda çizim yapın, aşağıdaki kaydırma döngüsü sola doğru kaydırılır.
     }
     for (int refreshCount=0; refreshCount < scrollspeed; refreshCount++)
       RefreshDisplay();
     // Bitmap'i bir sütunu sola kaydır
     for (int row=0; row<8; row++)
     {
       for (int zone=0; zone < numZones; zone++)
       {
         // Bu sağ kaydırma ekranda sol kaydırma olarak görünecektir, çünkü en soldaki sütun baytın en az anlamlı bitiyle temsil edilir.
         bitmap[row][zone] = bitmap[row][zone] >> 1;
         // Bir sonraki bölgedeki en düşük bitin üzerinden bu bölgenin en yüksek biti olarak kaydırılması.
         if (zone < maxZoneIndex) bitWrite(bitmap[row][zone], 7, bitRead(bitmap[row][zone+1],0));
       }
     }
   }
 }
}
void loop() {
 XProcess();
}
