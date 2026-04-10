#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int ADDR =0x40;
int i,j,N=5;
long int X0,X1,Y0,Y1,Y2,Y3;
double X,Y,X_out,Y_out1,Y_out2;
double Dati[5][2];
 

void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  // Clear the buffer
  display.clearDisplay();  
  
  Wire.begin();                                   
  delay(100);  
  Wire.beginTransmission(ADDR);
  Wire.endTransmission();  
  i=0;
}

int x = 0;

void loop() {
  
  /**Send command of initiating temperature measurement**/
  Wire.beginTransmission(ADDR);
  Wire.write(0xE3);
  Wire.endTransmission();
 
  Serial.print("Temp");
  Serial.print("\t");
  Serial.println("Humidity");
 
  /**Read data of temperature**/
  Wire.requestFrom(ADDR,2);
 
  if(Wire.available()<=2);
  {
    X0 = Wire.read();
    X1 = Wire.read();
    X0 = X0<<8;
    X_out = X0+X1;
  }
 
  /**Calculate and display temperature**/
  X=(175.72*X_out)/65536;                        
  X=X-46.85;
  Dati[i][1]=X;
  Serial.print(X);
  Serial.print("C");
  Serial.print("\t");
 
  /**Send command of initiating relative humidity measurement**/
  Wire.beginTransmission(ADDR);                     
  Wire.write(0xE5);
  Wire.endTransmission(); 
 
  /**Read data of relative humidity**/
  Wire.requestFrom(ADDR,2);
  if(Wire.available()<=2);
  {
    Y0 = Wire.read();
    Y2=Y0/100; 
    Y0=Y0%100;
    Y1 = Wire.read();
    Y_out1 = Y2*25600;
    Y_out2 = Y0*256+Y1;
  }
 
  /**Calculate and display relative humidity**/
  Y_out1 = (125*Y_out1)/65536;                     
  Y_out2 = (125*Y_out2)/65536;
  Y = Y_out1+Y_out2;
  Y=Y-6;
  Dati[i][2]=Y;
  Serial.print(Y);
  Serial.println("%");                     
  delay(300);

  for(j=0;j<N;j++){
    Serial.print(Dati[j][1]);
    Serial.print("   ");
    Serial.println(Dati[j][2]);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////


  display.clearDisplay();  
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.print(X);
  display.setCursor(0, 16);
  display.cp437(true); 
  display.print(Y);
  display.display();
  
 
  Serial.println(); 
  delay(1200);
  i++;
}
