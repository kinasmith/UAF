import processing.serial.*;

PrintWriter output;

short LF = 10;
char HEADER = 'H';    // character to identify the start of a message
short portIndex = 3;
Serial myPort;

String Tc_1_String;
float Tc_1;
float Tc_2;
float Tc_3;
float Tc_4;
int h_status;

int xPos = 0;

void setup() {
  myPort = new Serial(this, Serial.list()[portIndex], 9600);
  size(1200, 800);
  drawBackground();
  myPort.clear();
  output = createWriter("data_"+year()+month()+day()+"-"+ hour()+minute()+".txt");
  //output = createWriter("data.txt");
  
}

void draw() {
  while (myPort.available() > 0) {
    String msg = myPort.readStringUntil(LF);
    if (msg != null) {
      String[] data = msg.split(",");
      if (data[0].charAt(0) == HEADER) {
        for (int i = 1; i < data.length-1; i++) {
          String[] tmp_d0 = split(data[0], 'H');
          Tc_1_String = tmp_d0[1];
          Tc_1 = float(tmp_d0[1]);
          Tc_2 = float(data[1]);
          Tc_3 = float(data[2]);
          h_status = int(data[3]);
        }
        output.print(millis()/1000);
        output.print(",");
        output.print(Tc_1_String);
        output.print(",");
        output.print(data[1]);
        output.print(",");
        output.print(data[2]);
        output.print(",");
        output.print(data[3]); 
        output.flush();
        print(millis()/1000);
        print(",");
        print(Tc_1);
        print(", ");
        print(Tc_2);
        print(", ");
        print(Tc_3);
        //print(", ");
        //print(Tc_4);
        print(", ");
        print(h_status);
        println();
        updateDisplay();
      }
    }
  }
}

void updateDisplay() {
  Tc_1 = map(Tc_1, 20, 25, (height/3), 0);
  Tc_2 = map(Tc_2, 20, 25, (height/3)*2, (height/3));
  Tc_3 = map(Tc_3, 20, 25, (height/3)*3, (height/3)*2);
  //Tc_4 = map(Tc_4, 20, 35, 800, 600);
  stroke(255);
  point(xPos/3, Tc_1);
  point(xPos/3, Tc_2);
  point(xPos/3, Tc_3);
  //point(xPos/4, Tc_4);

  if (xPos >= width) {
    xPos = 0;
    background(0);
  } else {
    xPos++;
  }
}

void exit() {
  output.flush();
  output.close();
}

void drawBackground() {
  background(0);
  stroke(255);
  line(0, height/3, width, height/3);
  line(0, (height/3)*2, width, (height/3)*2);
  line(0, (height/3)*3, width, (height/3)*3);
}