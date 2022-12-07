#include <Arduino.h>

// include the library code:
#include <LiquidCrystal.h>

// dfrobot sheild pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


// i2c library
#include <Wire.h>
#define wire Wire



// define the button pin

#define BUTTONPIN A0




int8_t menucurrentchoice = 0;

uint8_t menumaxchoices = 6;

String menutext[6] = { "I2C Scan" ,
                       "Dump EEPROMS" ,
                       "format eeproms" ,
                       "show 0x51" ,
                       "Send bank off" ,
                       "send bank 0x51"
                       };

// String menutext[6] = { "I2C Scan" ,
//                        "Dump EEPROMS" ,
//                        "dump FRAM" ,
//                        "Send Bank 0x00" ,
//                        "Send bank 0x01" ,
//                        "send bank off"
//                        };


void menucommand_00();

void menucommand_01();

void menucommand_02();

void menucommand_03();

void menucommand_04();

void menucommand_05();





void domenuaction() {

  lcd.setCursor( 0 , 1 );
  lcd.print( "Executing" );
  delay( 750 );

  switch (menucurrentchoice)
  {
  case 0:
    menucommand_00();
    break;
  
  case 1:
    menucommand_01();
    break;

  case 2:
    menucommand_02();
    break;
  
  case 3:
    menucommand_03();
    break;
  
  case 4:
    menucommand_04();
    break;
  case 5:
    menucommand_05();
    break;
  
  default:
    break;
  }

  return;

}




void displaymenu() {

  lcd.clear();
  lcd.setCursor( 0 , 0 );

  lcd.print( menutext[ menucurrentchoice ] );

}




// button scores
// none   1023
// right  0
// up     131
// down   308
// left   481
// select 720



void checkbutton() {

  int buttonraw = analogRead( BUTTONPIN );

  if ( buttonraw > 1000 ) { /* no button is pressed */ return; }

  if ( buttonraw < 100 ) { /* right button is pressed */ }

  if ( ( buttonraw > 100 ) && ( buttonraw < 200 ) ) { /* up button is pressed */ menucurrentchoice++; }
  if ( ( buttonraw > 250 ) && ( buttonraw < 350 ) ) { /* down button is pressed */ menucurrentchoice--; }
  if ( ( buttonraw > 450 ) && ( buttonraw < 500 ) ) { /* left button is pressed */ }
  if ( ( buttonraw > 700 ) && ( buttonraw < 750 ) ) { /* select button is pressed */ domenuaction(); }
  


  if ( menucurrentchoice == menumaxchoices ) { menucurrentchoice = ( menumaxchoices - 1 ); }
  if ( menucurrentchoice < 0 ) { menucurrentchoice = 0; }

  displaymenu();

}












void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");

  // set the buttonpin to input
  pinMode( BUTTONPIN , INPUT );

  Serial.begin( 9600 );

  // show the menu screen
  displaymenu();

}






void loop() {

  checkbutton();

  //lcd.clear();
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print( analogRead( BUTTONPIN ) );

  delay( 750 );

}












// utility commands

#define PAGESIZE 32

/// @brief formats an eeprom
/// @param eepromid the i2c address of the eeprom to format
void format_eeprom( uint8_t eepromid ) {

  // format the eeprom in 1024 32 byte pages for a total of 32768 bytes.

  for ( uint16_t currentpage = PAGESIZE ; currentpage < 32768 ; currentpage += PAGESIZE ) {

    //Serial.print( "Formatting page " );
    //Serial.println( currentpage );

    // say hello to the chip
    wire.beginTransmission( eepromid );
    // send the page address
    wire.write( (uint8_t)( ( currentpage >> 8 ) && 0xFF ) );
    wire.write( (uint8_t)( currentpage && 0xFF ) );

    // now send 32 bytes of data
    for ( uint8_t i = 0 ; i < PAGESIZE ; i++ ) {
      wire.write( i );
    }

    // now say goodbye to the chip
    wire.endTransmission();

    // now delay for 21ms!!!  to account for the 20ms write time for the eeproms...
    delay( 21 );

    // all done, cycle around to the next page

  }

}




void dump_eeprom_serial( uint8_t eepromid ) {

  // read 32 k of data from the chip in pagesize chunks

  for ( uint16_t currentpage = 0 ; currentpage < 32768 ; currentpage += PAGESIZE ) {

    // say hello to the chip
    wire.beginTransmission( eepromid );
    // send the page address
    wire.write( (uint8_t)( ( currentpage >> 8 ) && 0xFF ) );
    wire.write( (uint8_t)( currentpage && 0xFF ) );
    // say goodbye to the chip
    wire.endTransmission();

    // now request PAGESIZE bytes
    wire.requestFrom( eepromid , (uint8_t)(PAGESIZE) );

    uint8_t tempbyte = 0;
    for ( int i = 0 ; i < PAGESIZE ; i++ ) {
        tempbyte = wire.read();
        if ( tempbyte < 0x10 ) { Serial.print( "0" ); }
        Serial.print( tempbyte , HEX );
    }


    // all done, cycle around to the next page
    Serial.println();

  }




}












// String menutext[6] = { "I2C Scan" ,
//                        "Dump EEPROMS" ,
//                        "dump FRAM" ,
//                        "Send Bank 0x00" ,
//                        "Send bank 0x01" ,
//                        "send bank off"
//                        };





void menucommand_00() {

  lcd.clear();

  // turn on i2c as a master
  wire.begin();

  for ( int i = 0 ; i < 127 ; i++ ) {

    // check for a connection
    wire.beginTransmission( i );

    if ( wire.endTransmission() == 0 ) {
      // device is present at this address!
      if ( i < 16 ) { lcd.print("0" ); }
      lcd.print( i , HEX );
    }
    
  }

  // done so hold here
  while (1);
  
}





// String menutext[6] = { "I2C Scan" ,
//                        "Dump EEPROMS" ,
//                        "dump FRAM" ,
//                        "Send Bank 0x00" ,
//                        "Send bank 0x01" ,
//                        "send bank off"
//                        };



// dump eeproms
void menucommand_01() {
  
  // bring up i2c as a master
  wire.begin();

  lcd.clear();
  lcd.setCursor( 0 , 0 );
  lcd.print( "Dumping EEPROMS" );
  
  // for each bank
  for ( int i = 0 ; i < 8 ; i++ ) {

    // read out 32k
    
    Serial.print( "Bank: " );
    Serial.print( i );
    Serial.println( " : " );

    dump_eeprom_serial( 0x50 + i );

    Serial.println();

  }
  


}


// format all
void menucommand_02() {

  wire.begin();
  
  lcd.clear();
  lcd.setCursor( 0 , 0 );
  lcd.print( "Formatting" );

  // format eeproms
  for ( int i = 0 ; i < 8 ; i++ ) {

    lcd.setCursor( 0 , 1 );
    lcd.print( i );

    format_eeprom( 0x50 + i );

  }

  //while(1);

}

// show 0x51
void menucommand_03() {
  
  wire.begin();
  
  lcd.clear();
  lcd.setCursor( 0 , 0 );

  uint8_t data[16] = {0};

  wire.beginTransmission( 0x51 );
  wire.write( 0x00 );
  wire.write( 0x00 );
  wire.endTransmission();

  // receive 16 bytes
  wire.requestFrom( 0x51 , 8 );
  for ( int i = 0 ; i < 8 ; i++ ) {
    data[i] = (uint8_t)( wire.read() );
  }
  wire.requestFrom( 0x51 , 8 );
  for ( int i = 8 ; i < 16 ; i++ ) {
    data[i] = (uint8_t)( wire.read() );
  }


  // now display it
  lcd.setCursor( 0 , 0 );
  for ( int i = 0 ; i < 8 ; i++ ) {
    if ( data[i] < 16 ) { lcd.print( "0" ); }
    lcd.print( data[i] , HEX );
  }

  lcd.setCursor( 0 , 1 );
  for ( int i = 8 ; i < 16 ; i++ ) {
    if ( data[i] < 16 ) { lcd.print( "0" ); }
    lcd.print( data[i] , HEX );
  }

  while(1);

}


// bank off
void menucommand_04() {

  wire.begin();
  
  lcd.clear();
  lcd.setCursor( 0 , 0 );

  wire.beginTransmission( 0x70 );
  wire.write( 0b00000000 );
  wire.endTransmission();

  lcd.print( "Done!" );
  delay( 2000 );

}


// bank 00 on
void menucommand_05() {

  wire.begin();
  
  lcd.clear();
  lcd.setCursor( 0 , 0 );

  wire.beginTransmission( 0x70 );
  wire.write( 0b00000010 );
  wire.endTransmission();

  lcd.print( "Done!" );
  delay( 2000 );

}

















//