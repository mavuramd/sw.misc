#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define BME280_ADDRESS 0x76
#define NORMAL_MODE 0x03
#define FORCED_MODE 0x01

#define SAMPLING_HUM 0x01
#define SAMPLING_TMP (0x01 << 5)
#define SAMPLING_PRS (0x01 << 2)

enum {
  BME280_REGISTER_DIG_T1 = 0x88,
  BME280_REGISTER_DIG_T2 = 0x8A,
  BME280_REGISTER_DIG_T3 = 0x8C,

  BME280_REGISTER_DIG_P1 = 0x8E,
  BME280_REGISTER_DIG_P2 = 0x90,
  BME280_REGISTER_DIG_P3 = 0x92,
  BME280_REGISTER_DIG_P4 = 0x94,
  BME280_REGISTER_DIG_P5 = 0x96,
  BME280_REGISTER_DIG_P6 = 0x98,
  BME280_REGISTER_DIG_P7 = 0x9A,
  BME280_REGISTER_DIG_P8 = 0x9C,
  BME280_REGISTER_DIG_P9 = 0x9E,

  BME280_REGISTER_DIG_H1 = 0xA1,
  BME280_REGISTER_DIG_H2 = 0xE1,
  BME280_REGISTER_DIG_H3 = 0xE3,
  BME280_REGISTER_DIG_H4 = 0xE4,
  BME280_REGISTER_DIG_H5 = 0xE5,
  BME280_REGISTER_DIG_H6 = 0xE7,

  BME280_REGISTER_CHIPID = 0xD0,
  BME280_REGISTER_VERSION = 0xD1,
  BME280_REGISTER_SOFTRESET = 0xE0,

  BME280_REGISTER_CAL26 = 0xE1, // R calibration stored in 0xE1-0xF0

  BME280_REGISTER_CONTROLHUMID = 0xF2,
  BME280_REGISTER_STATUS = 0XF3,
  BME280_REGISTER_CONTROL = 0xF4,
  BME280_REGISTER_CONFIG = 0xF5,
  BME280_REGISTER_PRESSUREDATA = 0xF7,
  BME280_REGISTER_TEMPDATA = 0xFA,
  BME280_REGISTER_HUMIDDATA = 0xFD
};


typedef struct {
  unsigned short int dig_T1; ///< temperature compensation value
  short int dig_T2;  ///< temperature compensation value
  short int dig_T3;  ///< temperature compensation value

  unsigned short int dig_P1; ///< pressure compensation value
  short int dig_P2;  ///< pressure compensation value
  short int dig_P3;  ///< pressure compensation value
  short int dig_P4;  ///< pressure compensation value
  short int dig_P5;  ///< pressure compensation value
  short int dig_P6;  ///< pressure compensation value
  short int dig_P7;  ///< pressure compensation value
  short int dig_P8;  ///< pressure compensation value
  short int dig_P9;  ///< pressure compensation value

  unsigned char dig_H1; ///< humidity compensation value
  short int dig_H2; ///< humidity compensation value
  unsigned char dig_H3; ///< humidity compensation value
  short int dig_H4; ///< humidity compensation value
  short int dig_H5; ///< humidity compensation value
  char dig_H6;  ///< humidity compensation value
} bme280_calib_data;

bme280_calib_data _bme280_calib;

int file;
char buf[10];

void write8(char reg,char value){
  buf[0] = reg;
  buf[1] = value;
  if(write(file, buf, 2) != 2){
    exit(1);
  }
}

unsigned short int read16(char reg){
  buf[0] = reg;
  if(write(file, buf, 1) != 1){
    exit(1);
  }

  if (read(file, buf, 2) != 2) {
    printf("Error: %d \n", errno);
    exit(1);
  } else {
    return ( ((unsigned short int)buf[0] << 8) | ((unsigned short int)buf[1]) );
  }
}

char read8(char reg){
  buf[0] = reg;

  if(write(file, buf, 1) != 1){
    exit(1);
  }

  if (read(file, buf, 1) != 1) {
    printf("Error: %d \n", errno);
    exit(1);
  } else {
    return ((char)buf[0]);
  }
}

unsigned int read24(char reg){
  buf[0] = reg;

  if(write(file, buf, 1) != 1){
    exit(1);
  }

  if (read(file, buf, 3) != 3) {
    printf("Error: %d \n", errno);
    exit(1);
  } else {
    return (((unsigned int)buf[0] << 16) | ((unsigned int)buf[1] << 8) | ((unsigned int)buf[2]));
  }
}

unsigned short int read16_LE(char reg) {
  unsigned short int temp = read16(reg);
  return (temp >> 8) | (temp << 8);
}

short int  readS16_LE(char reg) {
  return (short int)read16_LE(reg);
}

void readCoefficients(void) {
  _bme280_calib.dig_T1 = read16_LE(BME280_REGISTER_DIG_T1);
  _bme280_calib.dig_T2 = readS16_LE(BME280_REGISTER_DIG_T2);
  _bme280_calib.dig_T3 = readS16_LE(BME280_REGISTER_DIG_T3);

  _bme280_calib.dig_P1 = read16_LE(BME280_REGISTER_DIG_P1);
  _bme280_calib.dig_P2 = readS16_LE(BME280_REGISTER_DIG_P2);
  _bme280_calib.dig_P3 = readS16_LE(BME280_REGISTER_DIG_P3);
  _bme280_calib.dig_P4 = readS16_LE(BME280_REGISTER_DIG_P4);
  _bme280_calib.dig_P5 = readS16_LE(BME280_REGISTER_DIG_P5);
  _bme280_calib.dig_P6 = readS16_LE(BME280_REGISTER_DIG_P6);
  _bme280_calib.dig_P7 = readS16_LE(BME280_REGISTER_DIG_P7);
  _bme280_calib.dig_P8 = readS16_LE(BME280_REGISTER_DIG_P8);
  _bme280_calib.dig_P9 = readS16_LE(BME280_REGISTER_DIG_P9);

  _bme280_calib.dig_H1 = read8(BME280_REGISTER_DIG_H1);
  _bme280_calib.dig_H2 = readS16_LE(BME280_REGISTER_DIG_H2);
  _bme280_calib.dig_H3 = read8(BME280_REGISTER_DIG_H3);
  _bme280_calib.dig_H4 = ((char)read8(BME280_REGISTER_DIG_H4) << 4) |
      (read8(BME280_REGISTER_DIG_H4 + 1) & 0xF);
  _bme280_calib.dig_H5 = ((char)read8(BME280_REGISTER_DIG_H5 + 1) << 4) |
      (read8(BME280_REGISTER_DIG_H5) >> 4);
  _bme280_calib.dig_H6 = (char)read8(BME280_REGISTER_DIG_H6);
}

int main() {
  int adapter_nr = 1;     /* probably dynamically determined */
  char filename[20];

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  printf("%02d:%02d;", tm.tm_hour, tm.tm_min);

  snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
  file = open(filename, O_RDWR);
  if (file < 0) {
    /* ERROR HANDLING; you can check errno to see what went wrong */
    printf("Error: Device could not be opened\n");
    exit(1);
  } else {
  //printf("File handle: %d \n", file);
  }

  if (ioctl(file, I2C_SLAVE, BME280_ADDRESS) < 0) {
    /* ERROR HANDLING; you can check errno to see what went wrong */
    printf("Error: Failed to write Slave address\n");
    printf("Error: %d \n", errno);
    exit(1);
  }

  // Soft reset
  write8(BME280_REGISTER_SOFTRESET,0xB6);

  //printf("check status reg for update\n");

  unsigned char read8b;

  do {
    read8b = read8(BME280_REGISTER_STATUS);
    //printf("Status reg: %d \n", read8b);
    usleep(1000);
  }while((read8b & 0x01) != 0x00);

  //printf("Chip ID : %d\n", read8(0xD0));

  readCoefficients();

  {
    write8(BME280_REGISTER_CONTROLHUMID,SAMPLING_HUM);
    write8(BME280_REGISTER_CONTROL,(SAMPLING_TMP | SAMPLING_PRS | FORCED_MODE));

    //printf("check status reg for meas\n");

    do {
      read8b = read8(BME280_REGISTER_STATUS);
      //printf("Status reg: %d \n", read8b);
      usleep(1000);
    }while((read8b & 0x08) != 0x00);

    int t_fine;

    {
      unsigned int adc_T = read24(BME280_REGISTER_TEMPDATA);

      //printf("Temperature register value is : %d \n", adc_T);

      if(adc_T == 0x800000){
        printf("Default value. Temperature measurement disabled \n");
      } else {
        adc_T = adc_T >> 4;
        int var1, var2;
        float T;
        var1 = ((((adc_T >> 3) - ((int32_t)_bme280_calib.dig_T1 << 1))) * ((int32_t)_bme280_calib.dig_T2)) >> 11;
        var2 = (((((adc_T >> 4) - ((int32_t)_bme280_calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)_bme280_calib.dig_T1))) >> 12) * ((int32_t)_bme280_calib.dig_T3)) >> 14;
        t_fine = var1 + var2;
        T = (float)((t_fine * 5 + 128) >> 8) / 100;

        printf("%f;", T);
      }
    }

    {
      unsigned int adc_H = read16(BME280_REGISTER_HUMIDDATA);

      //printf("Humidity register value is : %d \n", adc_H);

      if(adc_H == 0x8000){
        printf("Default value. Humidity measurement disabled \n");
      } else {

        int32_t v_x1_u32r;
        v_x1_u32r = (t_fine - ((int32_t)76800));
        v_x1_u32r = (((((adc_H << 14) - (((int32_t)_bme280_calib.dig_H4) << 20) - (((int32_t)_bme280_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)_bme280_calib.dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)_bme280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)_bme280_calib.dig_H2) + 8192) >> 14));
        v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)_bme280_calib.dig_H1)) >> 4));
        v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
        v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
        float h = (float)(v_x1_u32r >> 12)/1024;

        printf("%f;\n", h);
      }
    }

    {
      unsigned int adc_P = read24(BME280_REGISTER_PRESSUREDATA);

      //printf("Pressure register value is : %d \n", adc_P);

      if(adc_P == 0x800000){
        printf("Default value. Pressure measurement disabled \n");
      } else {
        int64_t var1, var2, p;
        adc_P >>= 4;

        var1 = ((int64_t)t_fine) - 128000;
        var2 = var1 * var1 * (int64_t)_bme280_calib.dig_P6;
        var2 = var2 + ((var1 * (int64_t)_bme280_calib.dig_P5) << 17);
        var2 = var2 + (((int64_t)_bme280_calib.dig_P4) << 35);
        var1 = ((var1 * var1 * (int64_t)_bme280_calib.dig_P3) >> 8) + ((var1 * (int64_t)_bme280_calib.dig_P2) << 12);
        var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_bme280_calib.dig_P1) >> 33;
        if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
        }
        p = 1048576 - adc_P;
        p = (((p << 31) - var2) * 3125) / var1;
        var1 = (((int64_t)_bme280_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
        var2 = (((int64_t)_bme280_calib.dig_P8) * p) >> 19;
        p = ((p + var1 + var2) >> 8) + (((int64_t)_bme280_calib.dig_P7) << 4);
        p = (float)p / 256;
        //printf("Pressure is : %f \n", p);
      }
    }
  }

  return 0;
}
