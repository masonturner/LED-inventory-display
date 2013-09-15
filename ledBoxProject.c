#include <Ethernet.h>
#include <SPI.h>


////////////////////////////////////////////////////////////////////////
//Configuration Settings 
////////////////////////////////////////////////////////////////////////

// Ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address of device
byte server[] = { 192, 168, 1, 245 }; // server that is called for database services

const int num_LEDs = 8;
const int number_of_74hc595s = 1;
const int numOfRegisterPins = number_of_74hc595s * 8;

////////////////////////////////////////////////////////////////////////
//Initializations
////////////////////////////////////////////////////////////////////////

// Ethernet
EthernetClient client;


////////////////////////////////////////////////////////////////////////
//Variables
////////////////////////////////////////////////////////////////////////

// Shift Register
int SER_Pin = 5;   //pin 14 on the 75HC595, Yellow Wire
int RCLK_Pin = 6;  //pin 12 on the 75HC595, Green Wire
int SRCLK_Pin = 7; //pin 11 on the 75HC595, Blue Wire

boolean registers[numOfRegisterPins];
char ledMatrix[num_LEDs + 1];


void setup() {
// Serial

	Serial.begin(9600);
    

	if(Ethernet.begin(mac) == 0) {
		Serial.println("Failed to configure Ethernet using DHCP");
	}
	else Serial.println(Ethernet.localIP());

	pinMode(SER_Pin, OUTPUT);
  	pinMode(RCLK_Pin, OUTPUT);
  	pinMode(SRCLK_Pin, OUTPUT);


  	//reset all register pins
  	clearRegisters();
  	writeRegisters();
		

}



void loop() {
	Serial.println("Getting the array...");
	get_led_char_array(ledMatrix);
	Serial.println("Done");
	for(int i = 0; i < num_LEDs; i++) {
		if(ledMatrix[i] == '0') setRegisterPin(i, LOW);
		else if(ledMatrix[i] == '1') setRegisterPin(i, HIGH);
		else setRegisterPin(i, LOW);
	}
	writeRegisters();
	delay(3000);	
}


////////////////////////////////////////////////////////////////////////
//Function Definition
////////////////////////////////////////////////////////////////////////
//set all register pins to LOW
void clearRegisters(){
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
     registers[i] = LOW;
  }
} 

//Set and display registers
//Only call AFTER all values are set how you would like (slow otherwise)
void writeRegisters(){

  digitalWrite(RCLK_Pin, LOW);

  for(int i = numOfRegisterPins - 1; i >=  0; i--){
    digitalWrite(SRCLK_Pin, LOW);

    int val = registers[i];

    digitalWrite(SER_Pin, val);
    digitalWrite(SRCLK_Pin, HIGH);

  }
  digitalWrite(RCLK_Pin, HIGH);

}

//set an individual pin HIGH or LOW
void setRegisterPin(int index, int value){
  registers[index] = value;
}

void get_led_char_array(char *ledArray) {
	boolean get_ledArray_bool = true;
	Serial.println("Connecting...");
	if(client.connect(server, 80)) {
		Serial.println("Connected");
		client.println("GET /ledTest.php HTTP/1.0\nHost:192.168.1.245");
		client.println();
	}
	else {
		Serial.println("connection failed.");
		Serial.println("disconnecting.");
		client.stop();
	}
	Serial.println("Begin Get led Array");
	while(get_ledArray_bool == true) {
		if(client.available()) { 
			char c = client.read();
			Serial.print(c);
			if(c == '#') {					
				for(int i = 0; i < num_LEDs; i++) {
					if(!client.available()) break;
					ledArray[i] = client.read();
					Serial.print(ledArray[i]);  
				}
			}
		}

		if(!client.connected()) {
			Serial.println();
			Serial.println("disconnecting.");
			client.stop();
			get_ledArray_bool = false;
		}
	}
	ledArray[num_LEDs + 1] = '\0';
}