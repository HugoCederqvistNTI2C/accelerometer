/*

  Fil: accelerometer.ino

  Författare: Hugo Cederqvist

  Datum: 2024-05-15

  Beskrivning: Detta programmet använder sig av en accelerometer som den får information från i form av kraft från olika håll.
  Den tar sedan informationen och beräknar antingen till hur vriden accelerometern är i x- och y axeln med funktionen degree,
  eller så räknar den ut anvståndet man har rört sig från startpositionen med funktionen movement. Dessa uträkningar skrivs
  ut på en oled skärm. Vilken av funktionerna som skrivs ut styrs av en switch button, så varje gång knappen trycks byter
  programmet mellan de två funktionerna.

*/




#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();
float ax_x = 0;
float ax_y = 0;
float ax_z = 0;
float v_y = 0;
float v_x = 0;
float s_x = 0;
float s_y = 0;
int   i = 0;
int button = 7;
int eButton;
int lastButtonState;
int currentButtonState;
int chose = 0;

int y_acc[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int x_acc[8] = {0, 0, 0, 0, 0, 0, 0, 0};



Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



void setup(void) {
  pinMode(button, INPUT);
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }

  Serial.begin(9600);
  if (!accel.begin()) {

    Serial.println("No valid sensor found");
    while (1);


  }
  display.display();
  delay(2000); // Pause for 2 seconds

  display.clearDisplay();

  display.display();

  currentButtonState = digitalRead(button);
  lastButtonState = currentButtonState;  // Sätt föregående knappstatus till aktuell status vid start

}

void loop() {
  int eButton = digitalRead(button); // Läs av knappens status
  static int lastButtonState = LOW;
  static int currentButtonState = LOW;

  if (lastButtonState == HIGH && currentButtonState == LOW) { // Kontrollera om knappen trycktes ned (föregående status var HIGH och aktuell status är LOW)
    chose = (chose == 1) ? 0 : 1; // Toggle mellan 0 och 1
  }

  lastButtonState = currentButtonState; // Spara den aktuella knappens status som föregående status för nästa gång den trycks
  currentButtonState = eButton; // Spara den aktuella knappens status för nästa iteration

  if (chose == 1) {
    degree();
  }
  else {
    movement();
  }
}


/*

  Denna funktionen tar information från accelerometern med hjälp av funktioner från inkluderade bibliotek. Sedan tar den
  informationen i form av krafter från olika håll och beräknar ut hastighet och sedan sträcka. Sedan lägger tar den de
  senaste värdena och lägger dem i en array och tar medelvärdet för att minska buffering. Sedan skriver funktionen
  ut sträckan och hjälp text på oled skärmen. 


*/

void movement() {
  i = 0;
  while (i < 500) {
    sensors_event_t event;
    accel.getEvent(&event);

    for (int j = 7; j > 0; j--) {
      x_acc[j] = x_acc[j - 1];
      y_acc[j] = y_acc[j - 1];
    }
    x_acc[0] = (event.acceleration.x - 0.59);
    y_acc[0] = (event.acceleration.y);

    ax_x = (x_acc[0] + x_acc[1] + x_acc[2] + x_acc[3] + x_acc[4] + x_acc[5] + x_acc[6] + x_acc[7]) / 8;
    ax_y = (y_acc[0] + y_acc[1] + y_acc[2] + y_acc[3] + y_acc[4] + y_acc[5] + y_acc[6] + y_acc[7]) / 8;

    v_x += ax_x * 0.002;
    v_y += ax_y * 0.002;
    s_x += v_x * 0.002;
    s_y += v_y * 0.002;


    i += 1;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.print("Du har rört dig " );
  display.print(s_x);
  display.print("m " );
  display.println("sidleds");
  display.print("Du har rört dig " );
  display.print(s_y);
  display.print("m " );

  display.print("fram");


  display.display();
}


/*

  Funktionen använder inbygda funktioner från biblioteken som är inkluderade i programmet för att få ut krafterna från alla
  hållen. Sedan beräknar den om krafterna till grader i lutning på x och y axeln. Den skriver sedan ut vinklarna och
  förklarande text på oled skärmen.

   Input: Funktionen har inga inputs utan tar all information i från inbyggda funktioner i inkluderade bibliotek.
   Infon kommer från accelerometern.

   Returns: Funktionen skanar returns


*/
void degree() {

  sensors_event_t event;
  accel.getEvent(&event);
  ax_x = (event.acceleration.x - 0.59);
  ax_y = (event.acceleration.y);
  ax_z = (event.acceleration.z);


  v_x = atan(ax_y / ax_z) * 180 / 3, 142;
  v_y = atan(ax_x / ax_z) * 180 / 3, 142;


  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.print("vinkel x axel: " );
  display.println(v_x);
  display.print("vinkel y axel: " );
  display.print(v_y);


  display.display();


  delay(50);
}
