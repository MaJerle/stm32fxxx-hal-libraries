/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen MAJERLE
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
#include "tm_stm32_ahrs_imu.h"

/* Calculate 1/sqrt(x) with magic number support */
static
float oneOverSqrt(float x) {
    return 1.0f / (float) sqrt(x);
}

static
void calculateAngles(TM_AHRSIMU_t* AHRSIMU) {
    AHRSIMU->Roll = (float) atan2(AHRSIMU->_q0 * AHRSIMU->_q1 + AHRSIMU->_q2 * AHRSIMU->_q3, 0.5f - AHRSIMU->_q1 * AHRSIMU->_q1 - AHRSIMU->_q2 * AHRSIMU->_q2);
    AHRSIMU->Pitch = (float) asin(-2.0f * (AHRSIMU->_q1 * AHRSIMU->_q3 - AHRSIMU->_q0 * AHRSIMU->_q2));
    AHRSIMU->Yaw = (float) atan2(AHRSIMU->_q1 * AHRSIMU->_q2 + AHRSIMU->_q0 * AHRSIMU->_q3, 0.5f - AHRSIMU->_q2 * AHRSIMU->_q2 - AHRSIMU->_q3 * AHRSIMU->_q3);

    /* Calculate degrees and remove inclination */
    AHRSIMU->Roll *= AHRSIMU_RAD2DEG(1);
    AHRSIMU->Pitch *= AHRSIMU_RAD2DEG(1);
    AHRSIMU->Yaw = AHRSIMU->Yaw * AHRSIMU_RAD2DEG(1) - AHRSIMU->Inclination;
    
    /* Check values because of inclination */
    if (AHRSIMU->Yaw < -180) {
        AHRSIMU->Yaw = 180.0f - (-180.0f - AHRSIMU->Yaw);
    } else if (AHRSIMU->Yaw > 180) {
        AHRSIMU->Yaw = -180.0f - (180.0f - AHRSIMU->Yaw);
    }
}

void TM_AHRSIMU_Init(TM_AHRSIMU_t* AHRSIMU, float beta, float sampleRate, float inclination) {
    AHRSIMU->_beta = beta;
    AHRSIMU->_sampleRate = 1 / sampleRate;
    AHRSIMU->Inclination = inclination;
    
    AHRSIMU->_q0 = 1.0f;
    AHRSIMU->_q1 = 0.0f;
    AHRSIMU->_q2 = 0.0f;
    AHRSIMU->_q3 = 0.0f;
}

void TM_AHRSIMU_UpdateAHRS(TM_AHRSIMU_t* AHRSIMU, float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) {
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float hx, hy;
    float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

    /* Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation) */
    if ((mx != 0.0f) || (my != 0.0f) || (mz != 0.0f)) {        
        /* Rate of change of quaternion from gyroscope */
        qDot1 = 0.5f * (-AHRSIMU->_q1 * gx - AHRSIMU->_q2 * gy - AHRSIMU->_q3 * gz);
        qDot2 = 0.5f * (AHRSIMU->_q0 * gx + AHRSIMU->_q2 * gz - AHRSIMU->_q3 * gy);
        qDot3 = 0.5f * (AHRSIMU->_q0 * gy - AHRSIMU->_q1 * gz + AHRSIMU->_q3 * gx);
        qDot4 = 0.5f * (AHRSIMU->_q0 * gz + AHRSIMU->_q1 * gy - AHRSIMU->_q2 * gx);

        /* Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation) */
        if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

            /* Normalise accelerometer measurement */
            recipNorm = oneOverSqrt(ax * ax + ay * ay + az * az);
            ax *= recipNorm;
            ay *= recipNorm;
            az *= recipNorm;

            /* Normalise magnetometer measurement */
            recipNorm = oneOverSqrt(mx * mx + my * my + mz * mz);
            mx *= recipNorm;
            my *= recipNorm;
            mz *= recipNorm;

            /* Auxiliary variables to avoid repeated arithmetic */
            _2q0mx = 2.0f * AHRSIMU->_q0 * mx;
            _2q0my = 2.0f * AHRSIMU->_q0 * my;
            _2q0mz = 2.0f * AHRSIMU->_q0 * mz;
            _2q1mx = 2.0f * AHRSIMU->_q1 * mx;
            _2q0 = 2.0f * AHRSIMU->_q0;
            _2q1 = 2.0f * AHRSIMU->_q1;
            _2q2 = 2.0f * AHRSIMU->_q2;
            _2q3 = 2.0f * AHRSIMU->_q3;
            _2q0q2 = 2.0f * AHRSIMU->_q0 * AHRSIMU->_q2;
            _2q2q3 = 2.0f * AHRSIMU->_q2 * AHRSIMU->_q3;
            q0q0 = AHRSIMU->_q0 * AHRSIMU->_q0;
            q0q1 = AHRSIMU->_q0 * AHRSIMU->_q1;
            q0q2 = AHRSIMU->_q0 * AHRSIMU->_q2;
            q0q3 = AHRSIMU->_q0 * AHRSIMU->_q3;
            q1q1 = AHRSIMU->_q1 * AHRSIMU->_q1;
            q1q2 = AHRSIMU->_q1 * AHRSIMU->_q2;
            q1q3 = AHRSIMU->_q1 * AHRSIMU->_q3;
            q2q2 = AHRSIMU->_q2 * AHRSIMU->_q2;
            q2q3 = AHRSIMU->_q2 * AHRSIMU->_q3;
            q3q3 = AHRSIMU->_q3 * AHRSIMU->_q3;

            /* Reference direction of Earth's magnetic field */
            hx = mx * q0q0 - _2q0my * AHRSIMU->_q3 + _2q0mz * AHRSIMU->_q2 + mx * q1q1 + _2q1 * my * AHRSIMU->_q2 + _2q1 * mz * AHRSIMU->_q3 - mx * q2q2 - mx * q3q3;
            hy = _2q0mx * AHRSIMU->_q3 + my * q0q0 - _2q0mz * AHRSIMU->_q1 + _2q1mx * AHRSIMU->_q2 - my * q1q1 + my * q2q2 + _2q2 * mz * AHRSIMU->_q3 - my * q3q3;
            _2bx = (float) sqrt(hx * hx + hy * hy);
            _2bz = -_2q0mx * AHRSIMU->_q2 + _2q0my * AHRSIMU->_q1 + mz * q0q0 + _2q1mx * AHRSIMU->_q3 - mz * q1q1 + _2q2 * my * AHRSIMU->_q3 - mz * q2q2 + mz * q3q3;
            // #####  Correction START #####
            _2bx *= 2.0f;
            _2bz *= 2.0f;
            // ##### Correction END #####
            _4bx = 2.0f * _2bx;
            _4bz = 2.0f * _2bz;

            /* Gradient decent algorithm corrective step */
            s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * AHRSIMU->_q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * AHRSIMU->_q3 + _2bz * AHRSIMU->_q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * AHRSIMU->_q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
            s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * AHRSIMU->_q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * AHRSIMU->_q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * AHRSIMU->_q2 + _2bz * AHRSIMU->_q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * AHRSIMU->_q3 - _4bz * AHRSIMU->_q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
            s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * AHRSIMU->_q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * AHRSIMU->_q2 - _2bz * AHRSIMU->_q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * AHRSIMU->_q1 + _2bz * AHRSIMU->_q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * AHRSIMU->_q0 - _4bz * AHRSIMU->_q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
            s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * AHRSIMU->_q3 + _2bz * AHRSIMU->_q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * AHRSIMU->_q0 + _2bz * AHRSIMU->_q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * AHRSIMU->_q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
            
            /* normalise step magnitude */
            recipNorm = oneOverSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);
            s0 *= recipNorm;
            s1 *= recipNorm;
            s2 *= recipNorm;
            s3 *= recipNorm;

            /* Apply feedback step */
            qDot1 -= AHRSIMU->_beta * s0;
            qDot2 -= AHRSIMU->_beta * s1;
            qDot3 -= AHRSIMU->_beta * s2;
            qDot4 -= AHRSIMU->_beta * s3;
        }

        /* Integrate rate of change of quaternion to yield quaternion */
        AHRSIMU->_q0 += qDot1 * AHRSIMU->_sampleRate;
        AHRSIMU->_q1 += qDot2 * AHRSIMU->_sampleRate;
        AHRSIMU->_q2 += qDot3 * AHRSIMU->_sampleRate;
        AHRSIMU->_q3 += qDot4 * AHRSIMU->_sampleRate;

        /* Normalise quaternion */
        recipNorm = oneOverSqrt(AHRSIMU->_q0 * AHRSIMU->_q0 + AHRSIMU->_q1 * AHRSIMU->_q1 + AHRSIMU->_q2 * AHRSIMU->_q2 + AHRSIMU->_q3 * AHRSIMU->_q3);
        AHRSIMU->_q0 *= recipNorm;
        AHRSIMU->_q1 *= recipNorm;
        AHRSIMU->_q2 *= recipNorm;
        AHRSIMU->_q3 *= recipNorm;
    } else {
        /* Update IMU algorithm */
        TM_AHRSIMU_UpdateIMU(AHRSIMU, gx, gy, gz, ax, ay, az);
    }
    
    /* Calculate new angles */
    calculateAngles(AHRSIMU);
}

void TM_AHRSIMU_UpdateIMU(TM_AHRSIMU_t* AHRSIMU, float gx, float gy, float gz, float ax, float ay, float az) {
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

    /* Rate of change of quaternion from gyroscope */
    qDot1 = 0.5f * (-AHRSIMU->_q1 * gx - AHRSIMU->_q2 * gy - AHRSIMU->_q3 * gz);
    qDot2 = 0.5f * (AHRSIMU->_q0 * gx + AHRSIMU->_q2 * gz - AHRSIMU->_q3 * gy);
    qDot3 = 0.5f * (AHRSIMU->_q0 * gy - AHRSIMU->_q1 * gz + AHRSIMU->_q3 * gx);
    qDot4 = 0.5f * (AHRSIMU->_q0 * gz + AHRSIMU->_q1 * gy - AHRSIMU->_q2 * gx);

    /* Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation) */
    if (!(ax == 0.0f && ay == 0.0f && az == 0.0f)) {

        /* Normalise accelerometer measurement */
        recipNorm = oneOverSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        /* Auxiliary variables to avoid repeated arithmetic */
        _2q0 = 2.0f * AHRSIMU->_q0;
        _2q1 = 2.0f * AHRSIMU->_q1;
        _2q2 = 2.0f * AHRSIMU->_q2;
        _2q3 = 2.0f * AHRSIMU->_q3;
        _4q0 = 4.0f * AHRSIMU->_q0;
        _4q1 = 4.0f * AHRSIMU->_q1;
        _4q2 = 4.0f * AHRSIMU->_q2;
        _8q1 = 8.0f * AHRSIMU->_q1;
        _8q2 = 8.0f * AHRSIMU->_q2;
        q0q0 = AHRSIMU->_q0 * AHRSIMU->_q0;
        q1q1 = AHRSIMU->_q1 * AHRSIMU->_q1;
        q2q2 = AHRSIMU->_q2 * AHRSIMU->_q2;
        q3q3 = AHRSIMU->_q3 * AHRSIMU->_q3;

        /* Gradient decent algorithm corrective step */
        s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
        s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * AHRSIMU->_q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
        s2 = 4.0f * q0q0 * AHRSIMU->_q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
        s3 = 4.0f * q1q1 * AHRSIMU->_q3 - _2q1 * ax + 4.0f * q2q2 * AHRSIMU->_q3 - _2q2 * ay;
        
        /* Normalise step magnitude */
        recipNorm = oneOverSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); 
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        /* Apply feedback step */
        qDot1 -= AHRSIMU->_beta * s0;
        qDot2 -= AHRSIMU->_beta * s1;
        qDot3 -= AHRSIMU->_beta * s2;
        qDot4 -= AHRSIMU->_beta * s3;
    }

    /* Integrate rate of change of quaternion to yield quaternion */
    AHRSIMU->_q0 += qDot1 * AHRSIMU->_sampleRate;
    AHRSIMU->_q1 += qDot2 * AHRSIMU->_sampleRate;
    AHRSIMU->_q2 += qDot3 * AHRSIMU->_sampleRate;
    AHRSIMU->_q3 += qDot4 * AHRSIMU->_sampleRate;

    /* Normalise quaternion */
    recipNorm = oneOverSqrt(AHRSIMU->_q0 * AHRSIMU->_q0 + AHRSIMU->_q1 * AHRSIMU->_q1 + AHRSIMU->_q2 * AHRSIMU->_q2 + AHRSIMU->_q3 * AHRSIMU->_q3);
    AHRSIMU->_q0 *= recipNorm;
    AHRSIMU->_q1 *= recipNorm;
    AHRSIMU->_q2 *= recipNorm;
    AHRSIMU->_q3 *= recipNorm;
    
    /* Calculate new angles */
    calculateAngles(AHRSIMU);
}
