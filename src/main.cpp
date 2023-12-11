#include <Arduino.h>
#include "BluetoothSerial.h" //引入蓝牙函数库
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif // 判断蓝牙是否启用
int led_pwm = 0;
const int onoff = 2;
const int PWM_Pin = 18;

const int channel_PWM = 1;
const int freq_PWM = 2000;
const int resolution_PWM = 10;

BluetoothSerial SerialBT; // 定义一个自拟的蓝牙串口通信名称
static double ii = 0.01;
static int iii = 0;
int mode = 0;        //用于初始化进入随机光效模式
int mode_select = 4;    //光效模式选择变量
// int breathe(int kk);

/**********************灯效数学描述函数****************************/

int light_2(double a){        //灯效2数学函数
  double res = a*a*a+3*a*a+2*a+1;
  double max = 10*10*10+3*10*10+2*10+1;
  res = res/max;
  res = 1023*res;
  return res;
}

int light_3(double a){        //灯效3数学函数
  double aa = 5*3.14*a/10;
  double r = (sin(aa)+1)/2;
  double rr = (-(a-5)*(a-5)+25)*r;
  double rrr = rr/25;
  rrr = 1023*rrr;
  return rrr;

}


/*************************灯效函数**************************************/

/******************************呼吸灯breathe模式*************************************/
void breath_mode(int channel_PWM, double &ii, int &iii){
      /*光效1 前后周期判定*/
      if (ii > 3.14)      //灯效1阈值为3.14        2.3为10
      {
        ii = 3.1;        //半周期阈值
        iii++;             //半周期计数，每过一个半周期自加，指示当前是前半周期还是后半周期。
      }
      else if (ii <= 0.5)
      {
        ii = 0.6;
        iii++;
        delay(10);
        SerialBT.print(" from 1 mode changed");
        mode_select == (int)random(2,5);
        SerialBT.print(mode_select);
      }
      if (iii % 2 == 0)
      {
        double kk = sin(ii);
        int mm = (int)(kk*1023);
        ledcWrite(channel_PWM, mm);
        delay(25);
        ii += 0.01;
        SerialBT.print("breathing now");
        SerialBT.println(ii);
      }
      else if (iii % 2 == 1)
      {
        double kk = sin(ii);
        int mm = (int)(kk*1023);
        ledcWrite(channel_PWM, mm);
        delay(25);
        ii -= 0.01;
        SerialBT.print("breathing now");
        SerialBT.println(ii);
      }


}

/******************************多项式灯效模式*************************************/

void polynomial_mode(int channel_PWM, double &ii, int &iii){
      if (ii > 10)      //其他光效的前、后半周期的判定语句   由于使用大循环来实现光效，故需额外判定当前位置。
      {
        ii = 10;
        iii++;
      }
      else if (ii <= 0)
      {
        ii = 0;
        iii++;
        delay(10);
              SerialBT.print("mode changed");
        mode_select = (int)random(1,5);
      }
      if (iii%2==0){
        ledcWrite(channel_PWM, light_2(ii));
        delay(10);
        ii += 0.1;
        SerialBT.print("polynomial now");
        SerialBT.println(ii);


      }else if(iii%2==1){
        ledcWrite(channel_PWM, light_2(ii));
        delay(10);
        ii -= 0.1;
        SerialBT.print("polynomial now");
        SerialBT.println(ii);
      }

}
/******************************信号两暗（相对）夹一亮模式*************************************/

void signal_mode(int channel_PWM, double &ii, int &iii){
      if (ii > 10)      //其他光效的前、后半周期的判定语句   由于使用大循环来实现光效，故需额外判定当前位置。
      {
        ii = 10;
        iii++;
      }
      else if (ii <= 0)
      {
        ii = 0;
        iii++;
        delay(10);
              SerialBT.print("mode changed");
        mode_select = (int)random(1,5);
      }
        if (iii%2==0){
          ledcWrite(channel_PWM, light_3(ii));
          delay(10);
          ii += 0.015;
          SerialBT.print("signal now");
          SerialBT.println(ii);


        }else if(iii%2==1){
          ledcWrite(channel_PWM, light_3(ii));
          delay(10);
          ii -= 0.015;
          SerialBT.print("signal now");
          SerialBT.println(ii);
        }

}

/******************************rock摇滚两闪亮加两闪暗（相对）模式*************************************/

void rock_mode(int channel_PWM, double &ii, int &iii){
      if (ii > 10)      //其他光效的前、后半周期的判定语句   由于使用大循环来实现光效，故需额外判定当前位置。
      {
        ii = 10;
        iii++;
      }
      else if (ii <= 0)
      {
        ii = 0;
        iii++;
        delay(10);
              SerialBT.print("mode changed");
        mode_select = (int)random(1,5);
      }
      if(iii%2==0){
        if(2<ii && ii<4){
          ledcWrite(channel_PWM, 1023);
        }else if (6<ii&&ii<9){
          ledcWrite(channel_PWM, 1023);
        }else{
          ledcWrite(channel_PWM, 0);
        }
        ii+=1;
        delay(100);
        SerialBT.print("rock now");
        SerialBT.println(ii);

      }else if(iii%2==1){
        if(2<ii && ii<4){
          ledcWrite(channel_PWM, 511);
        }else if (6<ii&&ii<9){
          ledcWrite(channel_PWM, 511);
        }else{
          ledcWrite(channel_PWM, 0);
        }
        ii-=1;
        delay(100);
        SerialBT.print("rock now");
        SerialBT.println(ii);
      }

}





/************************************************************************/
void setup()
{
  Serial.begin(115200);

  pinMode(onoff, OUTPUT);
  pinMode(23, OUTPUT);

  digitalWrite(onoff, LOW);
  SerialBT.begin("ESP蓝牙UV"); // 启动蓝牙串口并设置蓝牙的名称
  Serial.println("jjjjjjjjj");

  ledcSetup(channel_PWM, freq_PWM, resolution_PWM); // 设置舵机通道
  ledcAttachPin(PWM_Pin, channel_PWM);              // 将 LEDC 通道绑定到指定 IO 口上以实现输出
}

void loop()
{
  static int func_b = 0;
  // int rand_select = (int)random(1,3);


  if (SerialBT.available()) // 判断蓝牙串口是否收到数据
  {
    char res = SerialBT.read(); // 将收到的数据读取出来，下面分别处理
    if (res == 'A')
    {
      // digitalWrite(onoff, LOW);
      // SerialBT.write(onoff);
      ledcWrite(channel_PWM, 0);
      SerialBT.print("已打开  LOW");
      Serial.println("Get OFF");
    }
    else if (res == 'B')
    {
      // digitalWrite(onoff, HIGH);
      ledcWrite(channel_PWM, 10);
      SerialBT.print("已关闭  HIGH");
      Serial.println("Get ON");
    }
    else if (res == 'G')
    {
      if (led_pwm < 1023 && led_pwm >= 0)
      {
        led_pwm = led_pwm + 50;
        ledcWrite(channel_PWM, map(led_pwm, 0, 1023, 0, 255));
        SerialBT.write(led_pwm);
      }
      else
      {
        led_pwm = 0;
      }
    }
    else if (res == 'K')
    {
      if (led_pwm < 1023 && led_pwm >= 0)
      {
        led_pwm = led_pwm - 50;
        ledcWrite(channel_PWM, map(led_pwm, 0, 1023, 0, 255));
        SerialBT.write(led_pwm);
      }
      else
      {
        led_pwm = 0;
      }
    }
    else if (res == 'E')
    {
      func_b = 1;      //赋值为1 进入随机光效模式
      // ledcWrite(channel_PWM,1023);
      SerialBT.print("mode start now");
    }
    else if (res == 'F')
    {
      func_b = 0;
    }
  }

  if (func_b)   //进入模式后执行if内语句
  {
      if(mode == 0){          //第一次随机选择，初始化
      mode_select == (int)random(1,5);
      mode = 1;
      SerialBT.print("已选择");
      SerialBT.println(mode_select);
      delay(10);
    }

    if(mode_select == 1){ 
      // 灯效1     sin breath mode  传入参数: channel_PWM, ii, iii
      breath_mode(channel_PWM, ii, iii);

    }
    else if(mode_select == 2){
    //灯效2       polynomial  mode   传入参数:    channel_PWM, ii, iii
      polynomial_mode(channel_PWM, ii, iii);

    }
    else if(mode_select == 3){
    // 灯效3    signal mode
      signal_mode(channel_PWM, ii, iii);
    }
    else if(mode_select == 4){
    // 灯效4     rock mode
      rock_mode(channel_PWM, ii, iii);
    }


  }
  delay(2);
}




