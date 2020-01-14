#include <Keypad.h> //The keypad and LCD i2c libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "RTClib.h"

RTC_DS1307 RTC;

//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const char *daysOfTheWeek[] =
    {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char *months[] =
    {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

#define I2C_ADDR 0x3F //defining the LCD pins
LiquidCrystal_I2C lcd(0x3F, 20, 4);

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
bool inService = true;
// public variables for RTC

// ----- internal functions
void trigger_buzzer(int type);
void show_time();
void debugTime();
void scanSwitch();
void welcome_sceen(int showDelay);

int paymentAPI( char data[]);

void setup()
{
// initialize_lcd
    lcd.begin();
    lcd.backlight();
// initialize serial
    Serial.begin(115200);
// initialize serial
    pinMode(A0, OUTPUT);
    digitalWrite(A0, HIGH);
// Initial RTC_DS1307
    RTC.adjust(DateTime(__DATE__, __TIME__));
    Wire.begin();
    RTC.begin();        
    delay(2000); // This delay allows the MCU to read the current date and time.
 
    if (!RTC.isrunning())
    {
        inService = false;
        Serial.println("RTC is NOT running!");
        delay(2000);
        // ------- shows error
        lcd.setCursor(0, 0);
        lcd.print("RTC is NOT running!");
        lcd.setCursor(0, 1);
        lcd.print("Device is Out of Service");
        lcd.setCursor(0, 2);
        lcd.print("Waiitng hardware reset");
        trigger_buzzer(2);
        while(1);
    }
    else
    {
        // ----- debug RTC is woring ----//
        Serial.print("The current date and time is: ");
        debugTime();
        welcome_sceen(2000);
    }
}

char txnAmount[4] = {};
int count = 0;
void loop()
{
    char key = keypad.getKey();
    lcd.setCursor(0, 3);
    lcd.cursor();
    //         01234567890123
    lcd.print("vale = ");
    if(key)  
    {
        trigger_buzzer(1);      // enable buzzer
        if(( key == 'D') && (count != 0) )      
        {
            // move left 
            lcd.scrollDisplayLeft();
        }
        else
        {
            if (key == 'A')
            {
                // sent the value to paument
                Serial.print("sent to payment value : " );
                
                if ( paymentAPI (txnAmount) != 0 )
                {
                    // payment error
                    lcd.setCursor(0, 4);
                             //012345678901234567890
                    lcd.print("==== sent error ====");
                }
                else
                {
                    lcd.setCursor(0, 4);
                             //012345678901234567890
                    lcd.print("==sent sucessfully==");
                }
                delay(800);
                count = 0;
                txnAmount = {}; // clear txnAmount
                lcd.setCursor(0, 3);
                lcd.cursor();
                lcd.print("vale = ");
            }
            else if (count < 5)
            {
                lcd.setCursor(7 + count, 3);
                //Print the detected key
                lcd.print(key);
                txnAmount[count] = key;
                count++;
            }
            else
            {
                /////// default another define here
            }
        }

    }
    showTime();
    delay(200);
}


void trigger_buzzer (int type)
{
    // normal trigger when entry keypressed
    if(type == 1){
        digitalWrite(A0, LOW);
        delay(100);
        digitalWrite(A0, HIGH);
    }
    // error trigger when device inService == false
    else if (type == 2)
    {
        digitalWrite(A0, LOW);
        delay(2000);
        digitalWrite(A0, HIGH);
    }
}


void showTime()
{
        DateTime now = RTC.now();
        lcd.setCursor(0, 0);
        lcd.print("Date : ");
        lcd.setCursor(11, 0);
        lcd.print(now.day(), DEC);
        lcd.print('/');
        lcd.print(now.month(), DEC);
        lcd.print('/');
        lcd.print(now.year(), DEC);
        lcd.print(' ');
        lcd.setCursor(0, 1);
        lcd.print("Time : ");
        lcd.setCursor(7, 1);
        if (now.hour() < 10)
            lcd.print('0');
        lcd.print(now.hour(), DEC);
        lcd.print(':');
        if (now.minute() < 10)
            lcd.print('0');
        lcd.print(now.minute(), DEC);
        lcd.print(':');
        if (now.second() < 10)
            lcd.print('0');
        lcd.print(now.second(), DEC);

        lcd.setCursor(7, 0);
        int dayofweek = now.dayOfWeek();
        switch (dayofweek)
        {
        case 1:
            lcd.print("Mon");
            break;
        case 2:
            lcd.print("Tue");
            break;
        case 3:
            lcd.print("Wed");
            break;
        case 4:
            lcd.print("Thu");
            break;
        case 5:
            lcd.print("Fri");
            break;
        case 6:
            lcd.print("Sat");
            break;
        case 0:
            lcd.print("Sun");
            break;
            //delay(1000);
           // scanSwitch();
        }
}

void scanSwitch()
{
    char key = keypad.getKey();
    if (key)
    {
        //Serial.println(key);
        lcd.setCursor(9, 1);
        //Print the detected key
        lcd.print(key);
    }
    delay(1000);
}


void debugTime()
{
    DateTime now = RTC.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    delay(1000);
    //delay(3000);
}

void welcome_sceen(int showDelay)
{
    lcd.setCursor(0, 0);
    lcd.print("Waiitng ........");
    delay(showDelay);
    lcd.clear();
}

int paymentAPI(char data[]){
    return -1;
}