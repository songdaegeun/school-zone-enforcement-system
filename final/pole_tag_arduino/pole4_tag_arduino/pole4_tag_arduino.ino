#include <ros.h>
#include <std_msgs/Byte.h>

ros::NodeHandle nh;
std_msgs::Byte msg;

void buzzer_cb(const std_msgs::Byte& msg);
void traffic_light_cb(const std_msgs::Byte& msg);

ros::Publisher pole4_tag_pub("pole4_sensor",&msg);
ros::Subscriber<std_msgs::Byte> buzzer_sub("/buzzer_tag", &buzzer_cb);
ros::Subscriber<std_msgs::Byte> traffic_light_sub("traffic_light_tag", &traffic_light_cb);

const int trackingPin = 9;
const int red_pin =10;
const int yellow_pin =11;
const int green_pin =12;
const int buzzer_pin=13;
int val, tag_value=0;
int buzzer_on_flag=0;
int led_flag=0;

unsigned long old_time,tag_old_time=0;
unsigned long react_sec=750;  //ms
unsigned long maintain_sec=5000;
unsigned long ir_old_time=4251;  //@ can change

void setup() {
  Serial.begin(9600); // open the serial port at 9600 bps:
  nh.initNode();
 
  nh.advertise(pole4_tag_pub);
  nh.subscribe(buzzer_sub);
  nh.subscribe(traffic_light_sub);

  pinMode(trackingPin, INPUT); // set trackingPin as INPUT
  pinMode(red_pin, OUTPUT);
  pinMode(yellow_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
}

void loop()
{
  val = digitalRead(trackingPin); // read the value of tracking module

  if(val==1 && (millis()-tag_old_time)>maintain_sec)
  //태그가 되지 않고 태그가 된지 maintain_sec이후 인경우
  {
    nh.logwarn("ir not detect");
    tag_value=0;
    ir_old_time=millis();
  }
  else if(val==0) 
  //태그가 될 경우
  {
    nh.logwarn("ir detect");
    tag_old_time=millis(); 
  }
  if(millis()>ir_old_time+react_sec)
  //태그가 된 react_sec후에 반응
  {
    tag_value=1;
  }

  if(millis()-old_time>=1)
  //1초 동안 publish
  {
    msg.data=tag_value;
    pole4_tag_pub.publish(&msg);
    old_time=millis();
  }
  
  if(buzzer_on_flag==1)
  {
    //nh.logwarn("buzzer on");
    led_flag=4;
    digitalWrite(buzzer_pin, LOW); 
    digitalWrite(red_pin, LOW);
    digitalWrite(yellow_pin, HIGH);
    digitalWrite(green_pin, HIGH);
  }
  else
  {
    //nh.logwarn("buzzer off");
    led_flag=4;
    digitalWrite(buzzer_pin, HIGH); 
    digitalWrite(red_pin, HIGH);
    digitalWrite(yellow_pin, HIGH);
    digitalWrite(green_pin, HIGH);
  }
  
  switch (led_flag) {
  case 1:
    //nh.logwarn("red on");
    digitalWrite(red_pin, LOW);
    digitalWrite(yellow_pin, HIGH);
    digitalWrite(green_pin, HIGH);
    break;
  case 2:
    //nh.logwarn("yellow on");
    digitalWrite(red_pin, HIGH);
    digitalWrite(yellow_pin, LOW);
    digitalWrite(green_pin, HIGH);
    break;
  case 3:
    //nh.logwarn("green on");
    digitalWrite(red_pin, HIGH);
    digitalWrite(yellow_pin, HIGH);
    digitalWrite(green_pin, LOW);
    break;
  case 4:
    break;
  default:
    //nh.logwarn("led off");
    digitalWrite(red_pin, HIGH);
    digitalWrite(yellow_pin, HIGH);
    digitalWrite(green_pin, HIGH);
    break;
  }
  delay(25);
  nh.spinOnce();
}

void buzzer_cb(const std_msgs::Byte& msg)
{
    if(msg.data==1)
    {
      buzzer_on_flag=1;
    }
    else
    {
      buzzer_on_flag=0;
    }
}
void traffic_light_cb(const std_msgs::Byte& msg)
{
    if(msg.data==1)  //red
    {
      led_flag=1;
    }
    else if(msg.data==2)  //yellow
    {
      led_flag=2;
    }
    else if(msg.data==3)  //green
    {
      led_flag=3;
    }
}
