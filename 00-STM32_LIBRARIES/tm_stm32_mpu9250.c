/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32_mpu9250.h"

/* Magnetometer */
#define WHO_AM_I_AK8963     0x00 // Result = 0x48
#define INFO                0x01
#define AK8963_ST1          0x02
#define AK8963_XOUT_L	    0x03
#define AK8963_XOUT_H	    0x04
#define AK8963_YOUT_L	    0x05
#define AK8963_YOUT_H	    0x06
#define AK8963_ZOUT_L	    0x07
#define AK8963_ZOUT_H	    0x08
#define AK8963_ST2          0x09
#define AK8963_CNTL         0x0A
#define AK8963_ASTC         0x0C
#define AK8963_I2CDIS       0x0F
#define AK8963_ASAX         0x10
#define AK8963_ASAY         0x11
#define AK8963_ASAZ         0x12

/* MPU9250 data */
#define SELF_TEST_X_GYRO    0x00                  
#define SELF_TEST_Y_GYRO    0x01                                                                          
#define SELF_TEST_Z_GYRO    0x02

#define SELF_TEST_X_ACCEL   0x0D
#define SELF_TEST_Y_ACCEL   0x0E    
#define SELF_TEST_Z_ACCEL   0x0F

#define SELF_TEST_A         0x10

#define XG_OFFSET_H         0x13
#define XG_OFFSET_L         0x14
#define YG_OFFSET_H         0x15
#define YG_OFFSET_L         0x16
#define ZG_OFFSET_H         0x17
#define ZG_OFFSET_L         0x18
#define SMPLRT_DIV          0x19
#define CONFIG              0x1A
#define GYRO_CONFIG         0x1B
#define ACCEL_CONFIG        0x1C
#define ACCEL_CONFIG2       0x1D
#define LP_ACCEL_ODR        0x1E   
#define WOM_THR             0x1F   

#define MOT_DUR             0x20
#define ZMOT_THR            0x21
#define ZRMOT_DUR           0x22

#define FIFO_EN             0x23
#define I2C_MST_CTRL        0x24   
#define I2C_SLV0_ADDR       0x25
#define I2C_SLV0_REG        0x26
#define I2C_SLV0_CTRL       0x27
#define I2C_SLV1_ADDR       0x28
#define I2C_SLV1_REG        0x29
#define I2C_SLV1_CTRL       0x2A
#define I2C_SLV2_ADDR       0x2B
#define I2C_SLV2_REG        0x2C
#define I2C_SLV2_CTRL       0x2D
#define I2C_SLV3_ADDR       0x2E
#define I2C_SLV3_REG        0x2F
#define I2C_SLV3_CTRL       0x30
#define I2C_SLV4_ADDR       0x31
#define I2C_SLV4_REG        0x32
#define I2C_SLV4_DO         0x33
#define I2C_SLV4_CTRL       0x34
#define I2C_SLV4_DI         0x35
#define I2C_MST_STATUS      0x36
#define INT_PIN_CFG         0x37
#define INT_ENABLE          0x38
#define DMP_INT_STATUS      0x39
#define INT_STATUS          0x3A
#define ACCEL_XOUT_H        0x3B
#define ACCEL_XOUT_L        0x3C
#define ACCEL_YOUT_H        0x3D
#define ACCEL_YOUT_L        0x3E
#define ACCEL_ZOUT_H        0x3F
#define ACCEL_ZOUT_L        0x40
#define TEMP_OUT_H          0x41
#define TEMP_OUT_L          0x42
#define GYRO_XOUT_H         0x43
#define GYRO_XOUT_L         0x44
#define GYRO_YOUT_H         0x45
#define GYRO_YOUT_L         0x46
#define GYRO_ZOUT_H         0x47
#define GYRO_ZOUT_L         0x48
#define EXT_SENS_DATA_00    0x49
#define EXT_SENS_DATA_01    0x4A
#define EXT_SENS_DATA_02    0x4B
#define EXT_SENS_DATA_03    0x4C
#define EXT_SENS_DATA_04    0x4D
#define EXT_SENS_DATA_05    0x4E
#define EXT_SENS_DATA_06    0x4F
#define EXT_SENS_DATA_07    0x50
#define EXT_SENS_DATA_08    0x51
#define EXT_SENS_DATA_09    0x52
#define EXT_SENS_DATA_10    0x53
#define EXT_SENS_DATA_11    0x54
#define EXT_SENS_DATA_12    0x55
#define EXT_SENS_DATA_13    0x56
#define EXT_SENS_DATA_14    0x57
#define EXT_SENS_DATA_15    0x58
#define EXT_SENS_DATA_16    0x59
#define EXT_SENS_DATA_17    0x5A
#define EXT_SENS_DATA_18    0x5B
#define EXT_SENS_DATA_19    0x5C
#define EXT_SENS_DATA_20    0x5D
#define EXT_SENS_DATA_21    0x5E
#define EXT_SENS_DATA_22    0x5F
#define EXT_SENS_DATA_23    0x60
#define MOT_DETECT_STATUS   0x61
#define I2C_SLV0_DO         0x63
#define I2C_SLV1_DO         0x64
#define I2C_SLV2_DO         0x65
#define I2C_SLV3_DO         0x66
#define I2C_MST_DELAY_CTRL  0x67
#define SIGNAL_PATH_RESET   0x68
#define MOT_DETECT_CTRL     0x69
#define USER_CTRL           0x6A  // Bit 7 enable DMP, bit 3 reset DMP
#define PWR_MGMT_1          0x6B // Device defaults to the SLEEP mode
#define PWR_MGMT_2          0x6C
#define DMP_BANK            0x6D  // Activates a specific bank in the DMP
#define DMP_RW_PNT          0x6E  // Set read/write pointer to a specific start address in specified DMP bank
#define DMP_REG             0x6F  // Register in DMP from which to read or to which to write
#define DMP_REG_1           0x70
#define DMP_REG_2           0x71 
#define FIFO_COUNTH         0x72
#define FIFO_COUNTL         0x73
#define FIFO_R_W            0x74
#define WHO_AM_I_MPU9250    0x75 // Should return 0x71
#define XA_OFFSET_H         0x77
#define XA_OFFSET_L         0x78
#define YA_OFFSET_H         0x7A
#define YA_OFFSET_L         0x7B
#define ZA_OFFSET_H         0x7D
#define ZA_OFFSET_L         0x7E

TM_MPU9250_Result_t TM_MPU9250_Init(TM_MPU9250_t* MPU9250, TM_MPU9250_Device_t dev) {
    uint8_t data;
    
    /* Set values */
    MPU9250->I2C_Addr = MPU9250_I2C_ADDR | (uint8_t)dev;
    MPU9250->I2C_Addr_Mag = MPU9250_I2C_ADDR_MAG;
    
    /* Init delay */
    TM_DELAY_Init();
    
    /* Init I2C */
    TM_I2C_Init(MPU9250_I2C, MPU9250_I2C_PP, MPU9250_I2C_CLOCK);
    
    /* Check if device connected */
    if (TM_I2C_IsDeviceConnected(MPU9250_I2C, MPU9250->I2C_Addr) != TM_I2C_Result_Ok) {
        return TM_MPU9250_Result_DeviceNotConnected;
    }
    
    /* Check who I am */
    TM_I2C_Read(MPU9250_I2C, MPU9250->I2C_Addr, WHO_AM_I_MPU9250, &data);
    if (data != 0x71) {
        return TM_MPU9250_Result_DeviceNotConnected;
    }
    
    // wake up device
    TM_I2C_Write(MPU9250_I2C, MPU9250->I2C_Addr, PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors 
    Delayms(100); // Wait for all registers to reset 

    // get stable time source
    TM_I2C_Write(MPU9250_I2C, MPU9250->I2C_Addr, PWR_MGMT_1, 0x01);  // Auto select clock source to be PLL gyroscope reference if ready else
    Delayms(200); 

    // Configure Gyro and Thermometer
    // Disable FSYNC and set thermometer and gyro bandwidth to 41 and 42 Hz, respectively; 
    // minimum delay time for this setting is 5.9 ms, which means sensor fusion update rates cannot
    // be higher than 1 / 0.0059 = 170 Hz
    // DLPF_CFG = bits 2:0 = 011; this limits the sample rate to 1000 Hz for both
    // With the MPU9250, it is possible to get gyro sample rates of 32 kHz (!), 8 kHz, or 1 kHz
    TM_I2C_Write(MPU9250_I2C, MPU9250->I2C_Addr, CONFIG, 0x03);  

    // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
    TM_I2C_Write(MPU9250_I2C, MPU9250->I2C_Addr, SMPLRT_DIV, 0x04);  // Use a 200 Hz rate; a rate consistent with the filter update rate 
                            // determined inset in CONFIG above

    // Set gyroscope full scale range
    // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
    TM_I2C_Read(MPU9250_I2C, MPU9250->I2C_Addr, GYRO_CONFIG, &data); // get current GYRO_CONFIG register value
    // c = c & ~0xE0; // Clear self-test bits [7:5] 
    data &= ~0x02; // Clear Fchoice bits [1:0] 
    data &= ~0x18; // Clear AFS bits [4:3]
    data |= 0x00 << 3; // Set full scale range for the gyro
    // c =| 0x00; // Set Fchoice for the gyro to 11 by writing its inverse to bits 1:0 of GYRO_CONFIG
    TM_I2C_Write(MPU9250_I2C, MPU9250->I2C_Addr, GYRO_CONFIG, data); // Write new GYRO_CONFIG value to register

    // Set accelerometer full-scale range configuration
    TM_I2C_Read(MPU9250_I2C, MPU9250->I2C_Addr, ACCEL_CONFIG, &data); // get current ACCEL_CONFIG register value
    // c = c & ~0xE0; // Clear self-test bits [7:5] 
    data &= ~0x18;  // Clear AFS bits [4:3]
    data |= 0x00 << 3; // Set full scale range for the accelerometer 
    TM_I2C_Write(MPU9250_I2C, MPU9250->I2C_Addr, ACCEL_CONFIG, data); // Write new ACCEL_CONFIG register value

    // Set accelerometer sample rate configuration
    // It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
    // accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
    TM_I2C_Read(MPU9250_I2C, MPU9250->I2C_Addr, ACCEL_CONFIG2, &data); // get current ACCEL_CONFIG2 register value
    data &= ~0x0F; // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])  
    data |= 0x03;  // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
    TM_I2C_Write(MPU9250_I2C, MPU9250->I2C_Addr, ACCEL_CONFIG2, data); // Write new ACCEL_CONFIG2 register value
    // The accelerometer, gyro, and thermometer are set to 1 kHz sample rates, 
    // but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

    // Configure Interrupts and Bypass Enable
    // Set interrupt pin active high, push-pull, hold interrupt pin level HIGH until interrupt cleared,
    // clear on read of INT_STATUS, and enable I2C_BYPASS_EN so additional chips 
    // can join the I2C bus and all can be controlled by the Arduino as master
    TM_I2C_Write(MPU9250_I2C, MPU9250->I2C_Addr, INT_PIN_CFG, 0x22);
    TM_I2C_Write(MPU9250_I2C, MPU9250->I2C_Addr, INT_ENABLE, 0x01);
    
    /* Check if device connected */
    if (TM_I2C_IsDeviceConnected(MPU9250_I2C, MPU9250->I2C_Addr_Mag) != TM_I2C_Result_Ok) {
        return TM_MPU9250_Result_DeviceNotConnected;
    }
    
    TM_I2C_Write(MPU9250_I2C, MPU9250->I2C_Addr_Mag, AK8963_CNTL, 0x00); // Power down magnetometer  
    Delayms(10);
    TM_I2C_Write(MPU9250_I2C, MPU9250->I2C_Addr_Mag, AK8963_CNTL, 0x0F); // Enter Fuse ROM access mode
    Delayms(10);
    TM_I2C_Write(MPU9250_I2C, MPU9250->I2C_Addr_Mag, AK8963_CNTL, 0x00); // Power down magnetometer  
    Delayms(10);
    // Configure the magnetometer for continuous read and highest resolution
    // set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
    // and enable continuous mode data acquisition Mmode (bits [3:0]), 0010 for 8 Hz and 0110 for 100 Hz sample rates
    TM_I2C_Write(MPU9250_I2C, MPU9250->I2C_Addr_Mag, AK8963_CNTL, 1 << 4 | 2); // Set magnetometer data resolution and sample ODR
    Delayms(10);
    
    /* Calculate multiplicators */
    MPU9250->AMult = 2.0f / 32768.0f;
    MPU9250->GMult = 250.0f / 32768.0f;
    MPU9250->MMult = 10.0f * 4912.0f / 32768.0f;
    
    return TM_MPU9250_Result_Ok;
}

TM_MPU9250_Result_t TM_MPU9250_ReadAcce(TM_MPU9250_t* MPU9250) {
    uint8_t data[6];
    
    /* Read accelerometer data */
    TM_I2C_ReadMulti(MPU9250_I2C, MPU9250->I2C_Addr, ACCEL_XOUT_H, data, 6);
    
    MPU9250->Ax_Raw = ((int16_t)data[0] << 8) | data[1];
    MPU9250->Ay_Raw = ((int16_t)data[2] << 8) | data[3];  
    MPU9250->Az_Raw = ((int16_t)data[4] << 8) | data[5];
    
    MPU9250->Ax = (float)MPU9250->Ax_Raw * MPU9250->AMult;
    MPU9250->Ay = (float)MPU9250->Ay_Raw * MPU9250->AMult;
    MPU9250->Az = (float)MPU9250->Az_Raw * MPU9250->AMult;
}

TM_MPU9250_Result_t TM_MPU9250_ReadGyro(TM_MPU9250_t* MPU9250) {
    uint8_t data[6];
    TM_I2C_ReadMulti(MPU9250_I2C, MPU9250->I2C_Addr, GYRO_XOUT_H, data, 6);
    
    MPU9250->Gx_Raw = ((int16_t)data[0] << 8) | data[1];
    MPU9250->Gy_Raw = ((int16_t)data[2] << 8) | data[3];  
    MPU9250->Gz_Raw = ((int16_t)data[4] << 8) | data[5];
    
    MPU9250->Gx = (float)MPU9250->Gx_Raw * MPU9250->GMult;
    MPU9250->Gy = (float)MPU9250->Gy_Raw * MPU9250->GMult;
    MPU9250->Gz = (float)MPU9250->Gz_Raw * MPU9250->GMult;
}

TM_MPU9250_Result_t TM_MPU9250_ReadMag(TM_MPU9250_t* MPU9250) {
    uint8_t data[7];
    uint8_t check;
    
    /* Check status */
    TM_I2C_Read(MPU9250_I2C, MPU9250->I2C_Addr_Mag, AK8963_ST1, &check);
    
    if (check & 0x01) {
        TM_I2C_ReadMulti(MPU9250_I2C, MPU9250->I2C_Addr_Mag, AK8963_XOUT_L, data, 7);
        if (!(data[6] & 0x08)) {
            MPU9250->Mx_Raw = ((int16_t)data[1] << 8) | data[0];
            MPU9250->My_Raw = ((int16_t)data[3] << 8) | data[2];
            MPU9250->Mz_Raw = ((int16_t)data[5] << 8) | data[4]; 
        }
    }
}

TM_MPU9250_Result_t TM_MPU9250_DataReady(TM_MPU9250_t* MPU9250) {
    uint8_t data;
    TM_I2C_Read(MPU9250_I2C, MPU9250->I2C_Addr, INT_STATUS, &data);
    
    if (data & 0x01) {
        return TM_MPU9250_Result_Ok;
    }
    return TM_MPU9250_Result_Error;
}
