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
#include "tm_stm32_gps.h"

/* Internal variables */
static char GPS_Term[15];
static uint8_t GPS_Term_Number;
static uint8_t GPS_Term_Pos;
static uint8_t TM_GPS_CRC, TM_GPS_CRC_Received;
static uint8_t TM_GPS_Star;
static uint8_t TM_GPS_Statement = GPS_ERR;
static uint32_t GPS_Flags = 0, GPS_Flags_OK;
static TM_GPS_Data_t TM_GPS_INT_Data;
static uint8_t TM_GPS_FirstTime;
static char GPS_Statement_Name[7];

#ifndef GPS_DISABLE_GPGSV
uint8_t GPGSV_StatementsCount = 0;
uint8_t GPSGV_StatementNumber = 0;
uint8_t GPGSV_Term_Number = 0;
uint8_t GPGSV_Term_Mod = 0;
#endif

/* Private */
TM_GPS_Result_t TM_GPS_INT_Do(TM_GPS_t* GPS_Data, char c);
void TM_GPS_INT_CheckTerm(TM_GPS_t* GPS_Data);
TM_GPS_Result_t TM_GPS_INT_Return(TM_GPS_t* GPS_Data);
uint8_t TM_GPS_INT_StringStartsWith(char* string, const char* str);
uint8_t TM_GPS_INT_Atoi(char* str, uint32_t* val);
uint32_t TM_GPS_INT_Pow(uint8_t x, uint8_t y);
uint8_t TM_GPS_INT_Hex2Dec(char c);
uint8_t TM_GPS_INT_FlagsOk(TM_GPS_t* GPS_Data);
void TM_GPS_INT_ClearFlags(TM_GPS_t* GPS_Data);
void TM_GPS_INT_CheckEmpty(TM_GPS_t* GPS_Data);

#define TM_GPS_INT_Add2CRC(c)                            (TM_GPS_CRC ^= c)
#define TM_GPS_INT_ReturnWithStatus(GPS_Data, status)    (GPS_Data)->Status = status; return status;
#define TM_GPS_INT_SetFlag(flag)                         (GPS_Flags |= (flag))

/* Public */
void TM_GPS_Init(TM_GPS_t* GPS_Data, uint32_t baudrate) {
	/* Initialize USART */
	GPS_USART_INIT(baudrate);
	/* Set first-time variable */
	TM_GPS_FirstTime = 1;
	
	/* Reset everything */
	GPS_Data->CustomStatementsCount = 0;
	
	/* Clear all flags */
	TM_GPS_INT_ClearFlags(GPS_Data);
	
	/* Set flags used */
#ifndef GPS_DISABLE_GPGGA
	GPS_Flags_OK |= GPS_FLAG_LATITUDE;
	GPS_Flags_OK |= GPS_FLAG_NS;
	GPS_Flags_OK |= GPS_FLAG_LONGITUDE;
	GPS_Flags_OK |= GPS_FLAG_EW;
	GPS_Flags_OK |= GPS_FLAG_SATS;
	GPS_Flags_OK |= GPS_FLAG_FIX;
	GPS_Flags_OK |= GPS_FLAG_ALTITUDE;
	GPS_Flags_OK |= GPS_FLAG_TIME;
#endif
#ifndef GPS_DISABLE_GPRMC
	GPS_Flags_OK |= GPS_FLAG_SPEED;
	GPS_Flags_OK |= GPS_FLAG_DATE;
	GPS_Flags_OK |= GPS_FLAG_VALIDITY;
	GPS_Flags_OK |= GPS_FLAG_DIRECTION;
#endif
#ifndef GPS_DISABLE_GPGSA
	GPS_Flags_OK |= GPS_FLAG_HDOP;
	GPS_Flags_OK |= GPS_FLAG_VDOP;
	GPS_Flags_OK |= GPS_FLAG_PDOP;
	GPS_Flags_OK |= GPS_FLAG_FIXMODE;
	GPS_Flags_OK |= GPS_FLAG_SATS1_12;
#endif
#ifndef GPS_DISABLE_GPGSV	
	GPS_Flags_OK |= GPS_FLAG_SATSINVIEW;
	GPS_Flags_OK |= GPS_FLAG_SATSDESC;
#endif
}

TM_GPS_Result_t TM_GPS_Update(TM_GPS_t* GPS_Data) {
	/* Check for data in USART */
	if (!GPS_USART_BUFFER_EMPTY) {
		while (!GPS_USART_BUFFER_EMPTY) {
			TM_GPS_INT_Do(GPS_Data, (char)GPS_USART_BUFFER_GET_CHAR);
			if (GPS_Data->Status == TM_GPS_Result_NewData) {
				return GPS_Data->Status;
			}
		}
	}
	
	if (TM_GPS_FirstTime) {
		/* No any valid data, return First Data Waiting */
		/* Returning only after power up and calling when no all data is received */
		TM_GPS_INT_ReturnWithStatus(GPS_Data, TM_GPS_Result_FirstDataWaiting);
	}
	
	/* We have old data */
	TM_GPS_INT_ReturnWithStatus(GPS_Data, TM_GPS_Result_OldData);
}

TM_GPS_Custom_t * TM_GPS_AddCustom(TM_GPS_t* GPS_Data, char* GPG_Statement, uint8_t TermNumber) {
	TM_GPS_Custom_t * temp;
	
	/* Check if available */
	if (GPS_Data->CustomStatementsCount >= GPS_CUSTOM_NUMBER) {
		/* Error */
		return NULL;
	}
	
	/* Allocate memory */
	temp = (TM_GPS_Custom_t *) malloc(sizeof(TM_GPS_Custom_t));
	if (temp == NULL) {
		return NULL;
	}
	
	/* Fill settings */
	strcpy(temp->Statement, GPG_Statement);
	temp->TermNumber = TermNumber;
	GPS_Data->CustomStatements[GPS_Data->CustomStatementsCount] = temp; /* Add to array */
	GPS_Data->CustomStatementsCount++;					/* Increase memory count */
	
	return temp;										/* Return pointer */
}

float TM_GPS_ConvertSpeed(float SpeedInKnots, TM_GPS_Speed_t toSpeed) {
	switch ((uint8_t)toSpeed) {
		/* Metric */
		case TM_GPS_Speed_KilometerPerSecond:	return SpeedInKnots * 0.000514f;
		case TM_GPS_Speed_MeterPerSecond: 		return SpeedInKnots * 0.5144f;
		case TM_GPS_Speed_KilometerPerHour:	 	return SpeedInKnots * 1.852f;
		case TM_GPS_Speed_MeterPerMinute: 		return SpeedInKnots * 30.87f;
		
		/* Imperial */
		case TM_GPS_Speed_MilePerSecond: 		return SpeedInKnots * 0.0003197f;
		case TM_GPS_Speed_MilePerHour: 			return SpeedInKnots * 1.151f;
		case TM_GPS_Speed_FootPerSecond: 		return SpeedInKnots * 1.688f;
		case TM_GPS_Speed_FootPerMinute: 		return SpeedInKnots * 101.3f;
		
		/* For Runners and Joggers */
		case TM_GPS_Speed_MinutePerKilometer: 	return SpeedInKnots * 32.4f;
		case TM_GPS_Speed_SecondPerKilometer: 	return SpeedInKnots * 1944.0f;
		case TM_GPS_Speed_SecondPer100Meters: 	return SpeedInKnots * 194.4f;
		case TM_GPS_Speed_MinutePerMile: 		return SpeedInKnots * 52.14f;
		case TM_GPS_Speed_SecondPerMile: 		return SpeedInKnots * 3128.0f;
		case TM_GPS_Speed_SecondPer100Yards: 	return SpeedInKnots * 177.7f;
		
		/* Nautical */
		case TM_GPS_Speed_SeaMilePerHour: 		return SpeedInKnots * 1.0f;
		default:
			return 0;
	}
}

void TM_GPS_ConvertFloat(float num, TM_GPS_Float_t* Float_Data, uint8_t decimals) {
	if (decimals > 9) {
		decimals = 9;
	}
	
	/* Get integer part */
	Float_Data->Integer = (int32_t)num;
	
	/* Get decimal part */
	if (num < 0) {
		Float_Data->Decimal = (int32_t)((float)(Float_Data->Integer - num) * TM_GPS_INT_Pow(10, decimals));
	} else {
		Float_Data->Decimal = (int32_t)((float)(num - Float_Data->Integer) * TM_GPS_INT_Pow(10, decimals));
	}
}

void TM_GPS_DistanceBetween(TM_GPS_Distance_t* Distance_Data) {
	float f1, f2, l1, l2, df, dfi, a;
	
	/* Calculate distance between 2 pointes */
	f1 = GPS_DEGREES2RADIANS(Distance_Data->Latitude1);
	f2 = GPS_DEGREES2RADIANS(Distance_Data->Latitude2);
	l1 = GPS_DEGREES2RADIANS(Distance_Data->Longitude1);
	l2 = GPS_DEGREES2RADIANS(Distance_Data->Longitude2);
	df = GPS_DEGREES2RADIANS(Distance_Data->Latitude2 - Distance_Data->Latitude1);
	dfi = GPS_DEGREES2RADIANS(Distance_Data->Longitude2 - Distance_Data->Longitude1);

	a = sin(df * 0.5f) * sin(df * 0.5f) + cos(f1) * cos(f2) * sin(dfi * 0.5f) * sin(dfi * 0.5f);
	Distance_Data->Distance = GPS_EARTH_RADIUS * 2 * atan2(sqrt(a), sqrt(1 - a)) * 1000;	/* Get distance in meters */
	
	/* Calculate bearing between two points from point1 to point2 */
	df = sin(l2 - l1) * cos(f2);
	dfi = cos(f1) * sin(f2) - sin(f1) * cos(f2) * cos(l2 - l1);
	Distance_Data->Bearing = (GPS_RADIANS2DEGREES(atan2(df, dfi)));
	
	/* Make bearing always positive from 0 - 360 degrees instead of -180 to 180 */
	if (Distance_Data->Bearing < 0) {
		Distance_Data->Bearing += 360;
	}
}

/* Private */
TM_GPS_Result_t TM_GPS_INT_Do(TM_GPS_t* GPS_Data, char c) {
	if (TM_GPS_INT_FlagsOk(GPS_Data)) {
		TM_GPS_INT_ClearFlags(GPS_Data);				/* Data were valid before, new data are coming, not new anymore */
		GPS_Data->Status = TM_GPS_Result_OldData;		/* Data were "new" on last call, now are only "Old data", no NEW data */
	}
	if (c == '$') {										/* Start of string detected */
		TM_GPS_Star = 0;								/* Star detection reset */
		TM_GPS_CRC = 0;									/* Reset CRC */
		GPS_Term_Number = 0;							/* First term in new statement */
		GPS_Term_Pos = 0;								/* At position 0 of a first term */
		GPS_Term[GPS_Term_Pos++] = c;					/* Add character to first term */
	} else if (c == ',') {
		TM_GPS_INT_Add2CRC(c);							/* Add to parity */
		GPS_Term[GPS_Term_Pos++] = 0;					/* End of term */
		TM_GPS_INT_CheckEmpty(GPS_Data);				/* Check if term is empty */
		TM_GPS_INT_CheckTerm(GPS_Data);					/* Check term */
		GPS_Term_Number++;								/* Increase term number */
		GPS_Term_Pos = 0;								/* At position 0 of a first term */
	} else if (c == '\n') {
		GPS_Term_Number = 0;							/* Reset term number */
#ifndef GPS_DISABLE_GPGSV
		/* Check for GPGSV statement */
		if (TM_GPS_Statement == GPS_GPGSV && GPGSV_StatementsCount == GPSGV_StatementNumber) {
			TM_GPS_INT_SetFlag(GPS_FLAG_SATSDESC);		/* Set flag */
		}
#endif
	} else if (c == '\r') {
		GPS_Term[GPS_Term_Pos++] = 0;					/* End of character string */
		TM_GPS_CRC_Received = TM_GPS_INT_Hex2Dec(GPS_Term[0]) * 16 + TM_GPS_INT_Hex2Dec(GPS_Term[1]);	/* Between * and \r are 2 characters of Checksum */
		if (TM_GPS_CRC_Received != TM_GPS_CRC) {		/* CRC is not OK, data failed somewhere */			
			TM_GPS_INT_ClearFlags(GPS_Data);			/* Clear all flags */
		}
		GPS_Term_Number = 0;							/* Reset term number */
	} else if (c == '*') {
		TM_GPS_Star = 1;								/* Star detected */
		GPS_Term[GPS_Term_Pos++] = 0;					/* Add 0 at the end */
		TM_GPS_INT_CheckEmpty(GPS_Data);				/* Check empty */
		TM_GPS_INT_CheckTerm(GPS_Data);					/* Check term */		
		GPS_Term_Number++;								/* Increase term number */
		GPS_Term_Pos = 0;								/* At position 0 of a first term */
	} else {
		/* Other characters detected */
		if (!TM_GPS_Star) {								/* If star is not detected yet */
			TM_GPS_INT_Add2CRC(c);						/* Add to parity */
		}
		GPS_Term[GPS_Term_Pos++] = c;					/* Add to term */
	}
	return TM_GPS_INT_Return(GPS_Data);					/* Return current GPS status */
}

void TM_GPS_INT_CheckTerm(TM_GPS_t* GPS_Data) {
	uint32_t temp;
#ifndef GPS_DISABLE_GPGSA
	static uint8_t ids_count = 0;
#endif
	uint8_t count, i;
	if (GPS_Term_Number == 0) {
		/* Statement indicator */
		if (TM_GPS_INT_StringStartsWith(GPS_Term, "$GPGGA")) {
			TM_GPS_Statement = GPS_GPGGA;
		} else if (TM_GPS_INT_StringStartsWith(GPS_Term, "$GPRMC")) {
			TM_GPS_Statement = GPS_GPRMC;
		} else if (TM_GPS_INT_StringStartsWith(GPS_Term, "$GPGSA")) {
			TM_GPS_Statement = GPS_GPGSA;
		} else if (TM_GPS_INT_StringStartsWith(GPS_Term, "$GPGSV")) {
			TM_GPS_Statement = GPS_GPGSV;
		} else {
			TM_GPS_Statement = GPS_ERR;
		}
		
		strcpy(GPS_Statement_Name, GPS_Term);			/* Copy term to variable */
		return;											/* Do nothing */
	}
	
	/* Check custom terms one by one */
	for (i = 0; i < GPS_Data->CustomStatementsCount; i++) {
		/* Term is inside current statement */
		if (TM_GPS_INT_StringStartsWith(GPS_Statement_Name, GPS_Data->CustomStatements[i]->Statement)) {
			/* Term number is correct */
			if (GPS_Term_Number == GPS_Data->CustomStatements[i]->TermNumber) {
				/* Copy string value */
				strcpy(GPS_Data->CustomStatements[i]->Value, GPS_Term);
				
				/* Set updated flag */
				GPS_Data->CustomStatements[i]->Updated = 1;
			}
		}
	}
	
	switch (GPS_CONCAT(TM_GPS_Statement, GPS_Term_Number)) {
#ifndef GPS_DISABLE_GPGGA
		case GPS_POS_LATITUDE:	/* GPGGA */
			/* Convert latitude */
			count = TM_GPS_INT_Atoi(GPS_Term, &temp);
			TM_GPS_INT_Data.Latitude = temp / 100;
			TM_GPS_INT_Data.Latitude += (float)(temp % 100) / (float)60;
		
			count = TM_GPS_INT_Atoi(&GPS_Term[++count], &temp);
			TM_GPS_INT_Data.Latitude += temp / (TM_GPS_INT_Pow(10, count) * 60.0);
			TM_GPS_INT_SetFlag(GPS_FLAG_LATITUDE);
			break;
		case GPS_POS_NS: /* GPGGA */
			if (GPS_Term[0] == 'S') {
				TM_GPS_INT_Data.Latitude = -TM_GPS_INT_Data.Latitude;	/* South has negative coordinate */
			}
			TM_GPS_INT_SetFlag(GPS_FLAG_NS);
			break;
		case GPS_POS_LONGITUDE: /* GPGGA */
			/* Convert longitude */
			count = TM_GPS_INT_Atoi(GPS_Term, &temp);
			TM_GPS_INT_Data.Longitude = temp / 100; /* Degrees */
			TM_GPS_INT_Data.Longitude += (float)(temp % 100) / (float)60;
		
			count = TM_GPS_INT_Atoi(&GPS_Term[++count], &temp);
			TM_GPS_INT_Data.Longitude += temp / (TM_GPS_INT_Pow(10, count) * 60.0);
			TM_GPS_INT_SetFlag(GPS_FLAG_LONGITUDE);
			break;
		case GPS_POS_EW: /* GPGGA */
			if (GPS_Term[0] == 'W') {
				TM_GPS_INT_Data.Longitude = -TM_GPS_INT_Data.Longitude;	/* West has negative coordinate */
			}
			TM_GPS_INT_SetFlag(GPS_FLAG_EW);
			break;
		case GPS_POS_SATS: /* GPGGA */
			/* Satellites in use */
			TM_GPS_INT_Atoi(GPS_Term, &temp);
			TM_GPS_INT_Data.Satellites = temp;
			TM_GPS_INT_SetFlag(GPS_FLAG_SATS);
			break;
		case GPS_POS_FIX: /* GPGGA */
			/* GPS Fix */
			TM_GPS_INT_Atoi(GPS_Term, &temp);
			TM_GPS_INT_Data.Fix = temp;
			TM_GPS_INT_SetFlag(GPS_FLAG_FIX);
			break;
		case GPS_POS_ALTITUDE: /* GPGGA */
			/* Convert altitude above sea */
			if (GPS_Term[0] == '-') {
				count = TM_GPS_INT_Atoi(&GPS_Term[1], &temp);
				TM_GPS_INT_Data.Altitude = temp;
				
				count++;
				count = TM_GPS_INT_Atoi(&GPS_Term[++count], &temp);
				TM_GPS_INT_Data.Altitude += temp / (TM_GPS_INT_Pow(10, count) * 1.0);
				
				TM_GPS_INT_Data.Altitude = -TM_GPS_INT_Data.Altitude;
			} else {
				count = TM_GPS_INT_Atoi(GPS_Term, &temp);
				TM_GPS_INT_Data.Altitude = temp;
		
				count = TM_GPS_INT_Atoi(&GPS_Term[++count], &temp);
				TM_GPS_INT_Data.Altitude += temp / (TM_GPS_INT_Pow(10, count) * 1.0);
			}
			TM_GPS_INT_SetFlag(GPS_FLAG_ALTITUDE);
			break;
		case GPS_POS_TIME: /* GPGGA */
			/* Set time */
			count = TM_GPS_INT_Atoi(GPS_Term, &temp);
			TM_GPS_INT_Data.Time.Seconds = temp % 100;
			TM_GPS_INT_Data.Time.Minutes = (int)(temp * (float) 0.01) % 100;
			TM_GPS_INT_Data.Time.Hours = (int)(temp * (float)0.0001) % 100;
			/* Hundredths */
			TM_GPS_INT_Atoi(&GPS_Term[++count], &temp);
			TM_GPS_INT_Data.Time.Hundredths = temp;
			TM_GPS_INT_SetFlag(GPS_FLAG_TIME);
			break;
#endif
#ifndef GPS_DISABLE_GPRMC
		case GPS_POS_SPEED:	/* GPRMC */	
			/* Convert speed */
			count = TM_GPS_INT_Atoi(GPS_Term, &temp);
			TM_GPS_INT_Data.Speed = (float)temp;
		
			count = TM_GPS_INT_Atoi(&GPS_Term[++count], &temp);
			TM_GPS_INT_Data.Speed += (float)((float)temp / (TM_GPS_INT_Pow(10, count) * 1.0));
		
			/* Set flag */
			TM_GPS_INT_SetFlag(GPS_FLAG_SPEED);
			break;
		case GPS_POS_DATE: /* GPRMC */	
			/* Set date */
			TM_GPS_INT_Atoi(GPS_Term, &temp);
			TM_GPS_INT_Data.Date.Year = temp % 100;
			TM_GPS_INT_Data.Date.Month = (int)(temp * 0.01f) % 100;
			TM_GPS_INT_Data.Date.Date = (int)(temp * 0.0001f) % 100;
			TM_GPS_INT_SetFlag(GPS_FLAG_DATE);
			break;
		case GPS_POS_VALIDITY: /* GPRMC */	
			/* GPS valid status */
			TM_GPS_INT_Data.Validity = GPS_Term[0] == 'A';
			TM_GPS_INT_SetFlag(GPS_FLAG_VALIDITY);
			break;
		case GPS_POS_DIRECTION: /* GPRMC */
			count = TM_GPS_INT_Atoi(GPS_Term, &temp);
			TM_GPS_INT_Data.Direction = (float)temp;
		
			count = TM_GPS_INT_Atoi(&GPS_Term[++count], &temp);
			TM_GPS_INT_Data.Direction += (float)((float)temp / (TM_GPS_INT_Pow(10, count) * 1.0f));
			TM_GPS_INT_SetFlag(GPS_FLAG_DIRECTION);
			break;
#endif
#ifndef GPS_DISABLE_GPGSA
		case GPS_POS_HDOP: /* GPGSA */
			count = TM_GPS_INT_Atoi(GPS_Term, &temp);
			TM_GPS_INT_Data.HDOP = (float)temp;
		
			count = TM_GPS_INT_Atoi(&GPS_Term[++count], &temp);
			TM_GPS_INT_Data.HDOP += (float)((float)temp / (TM_GPS_INT_Pow(10, count) * 1.0f));
			TM_GPS_INT_SetFlag(GPS_FLAG_HDOP);
			break;
		case GPS_POS_PDOP: /* GPGSA */
			count = TM_GPS_INT_Atoi(GPS_Term, &temp);
			TM_GPS_INT_Data.PDOP = (float)temp;
		
			count = TM_GPS_INT_Atoi(&GPS_Term[++count], &temp);
			TM_GPS_INT_Data.PDOP += (float)((float)temp / (TM_GPS_INT_Pow(10, count) * 1.0f));
			TM_GPS_INT_SetFlag(GPS_FLAG_PDOP);
			break;
		case GPS_POS_VDOP: /* GPGSA */
			count = TM_GPS_INT_Atoi(GPS_Term, &temp);
			TM_GPS_INT_Data.VDOP = (float)temp;
		
			count = TM_GPS_INT_Atoi(&GPS_Term[++count], &temp);
			TM_GPS_INT_Data.VDOP += (float)((float)temp / (TM_GPS_INT_Pow(10, count) * 1.0f));
			TM_GPS_INT_SetFlag(GPS_FLAG_VDOP);
			break;
		case GPS_POS_FIXMODE: /* GPGSA */
			/* Satellites in view */
			TM_GPS_INT_Atoi(GPS_Term, &temp);
			TM_GPS_INT_Data.FixMode = temp;
			TM_GPS_INT_SetFlag(GPS_FLAG_FIXMODE);
			break;
		case GPS_POS_SAT1:
		case GPS_POS_SAT2:
		case GPS_POS_SAT3:
		case GPS_POS_SAT4:
		case GPS_POS_SAT5:
		case GPS_POS_SAT6:
		case GPS_POS_SAT7:
		case GPS_POS_SAT8:
		case GPS_POS_SAT9:
		case GPS_POS_SAT10:
		case GPS_POS_SAT11:
		case GPS_POS_SAT12:
			/* Satellite numbers */
			TM_GPS_INT_Atoi(GPS_Term, &temp);
			TM_GPS_INT_Data.SatelliteIDs[GPS_Term_Number - 3] = temp;
			ids_count++;							/* Increase number of satellites found */
		
			if (ids_count == TM_GPS_INT_Data.Satellites) {
				ids_count = 0;						/* Reset count, we have all statements */
				TM_GPS_INT_SetFlag(GPS_FLAG_SATS1_12);
			}
			break;
#endif
#ifndef GPS_DISABLE_GPGSV
		case GPS_POS_SATSINVIEW: /* GPGSV */
			/* Satellites in view */
			TM_GPS_INT_Atoi(GPS_Term, &temp);
			TM_GPS_INT_Data.SatellitesInView = temp;
			TM_GPS_INT_SetFlag(GPS_FLAG_SATSINVIEW);
			break;
#endif
		default: 
			break;
	}
	
#ifndef GPS_DISABLE_GPGSV
	/* Check for GPGSV statement separatelly */
	if (TM_GPS_Statement == GPS_GPGSV) {	
		if (GPS_Term_Number == 1) {
			TM_GPS_INT_Atoi(GPS_Term, &temp);			/* Save number of GPGSV statements */
			GPGSV_StatementsCount = temp;
		}
		if (GPS_Term_Number == 2) {
			TM_GPS_INT_Atoi(GPS_Term, &temp);			/* Save current of GPGSV statement number */
			GPSGV_StatementNumber = temp;
		}
		
		/* Data */
		if (GPS_Term_Number >= 4) {
			TM_GPS_INT_Atoi(GPS_Term, &temp);			/* Convert to number */
			
			GPGSV_Term_Number = GPS_Term_Number - 4;	/* Get proper value */
			GPGSV_Term_Mod = GPGSV_Term_Number % 4;
			GPGSV_Term_Number = (GPSGV_StatementNumber - 1) * 4 + (GPGSV_Term_Number / 4);
			
			if (GPGSV_Term_Number < GPS_MAX_SATS_IN_VIEW) {	/* If still memory available */
				/* Check offset from 4 */
				if (GPGSV_Term_Mod == 0) {
					TM_GPS_INT_Data.SatDesc[GPGSV_Term_Number].ID = temp;
				} else if (GPGSV_Term_Mod == 1) {
					TM_GPS_INT_Data.SatDesc[GPGSV_Term_Number].Elevation = temp;
				} else if (GPGSV_Term_Mod == 2) {
					TM_GPS_INT_Data.SatDesc[GPGSV_Term_Number].Azimuth = temp;
				} else if (GPGSV_Term_Mod == 3) {
					TM_GPS_INT_Data.SatDesc[GPGSV_Term_Number].SNR = temp;
				}
			}
		}
	}
#endif
}

TM_GPS_Result_t TM_GPS_INT_Return(TM_GPS_t* GPS_Data) {
	uint8_t i;
	if (TM_GPS_INT_FlagsOk(GPS_Data)) {
		TM_GPS_FirstTime = 0;							/* Clear first time */
		
		/* Set data */
#ifndef GPS_DISABLE_GPGGA
		GPS_Data->Latitude = TM_GPS_INT_Data.Latitude;
		GPS_Data->Longitude = TM_GPS_INT_Data.Longitude;
		GPS_Data->Satellites = TM_GPS_INT_Data.Satellites;
		GPS_Data->Fix = TM_GPS_INT_Data.Fix;
		GPS_Data->Altitude = TM_GPS_INT_Data.Altitude;
		GPS_Data->Time = TM_GPS_INT_Data.Time;
#endif
#ifndef GPS_DISABLE_GPRMC
		GPS_Data->Speed = TM_GPS_INT_Data.Speed;
		GPS_Data->Date = TM_GPS_INT_Data.Date;
		GPS_Data->Validity = TM_GPS_INT_Data.Validity;
		GPS_Data->Direction = TM_GPS_INT_Data.Direction;
#endif
#ifndef GPS_DISABLE_GPGSA
		GPS_Data->HDOP = TM_GPS_INT_Data.HDOP;
		GPS_Data->VDOP = TM_GPS_INT_Data.VDOP;
		GPS_Data->PDOP = TM_GPS_INT_Data.PDOP;
		GPS_Data->FixMode = TM_GPS_INT_Data.FixMode;
		for (i = 0; i < 12; i++) {
			GPS_Data->SatelliteIDs[i] = TM_GPS_INT_Data.SatelliteIDs[i];
		}
#endif
#ifndef GPS_DISABLE_GPGSV
		GPS_Data->SatellitesInView = TM_GPS_INT_Data.SatellitesInView;
		for (i = 0; i < GPS_MAX_SATS_IN_VIEW; i++) {
			GPS_Data->SatDesc[i] = TM_GPS_INT_Data.SatDesc[i];
		}
#endif
		TM_GPS_INT_ReturnWithStatus(GPS_Data, TM_GPS_Result_NewData);	/* Return new data */
	}
	
	if (TM_GPS_FirstTime) {
		TM_GPS_INT_ReturnWithStatus(GPS_Data, TM_GPS_Result_FirstDataWaiting);	/* We are first time */
	}
	TM_GPS_INT_ReturnWithStatus(GPS_Data, TM_GPS_Result_OldData);	/* Return old data */
}

uint8_t TM_GPS_INT_StringStartsWith(char* string, const char* str) {
	while (*str) {
		if (*str++ != *string++) {
			return 0;
		}
	}
	return 1;
}

uint8_t TM_GPS_INT_Atoi(char* str, uint32_t* val) {
	uint8_t count = 0;
	*val = 0;
	while (GPS_IS_DIGIT(*str)) {
		*val = *val * 10 + GPS_C2N(*str++);
		count++;
	}
	return count;
}

uint32_t TM_GPS_INT_Pow(uint8_t x, uint8_t y) {
	uint32_t ret = 1;
	while (y--) {
		ret *= x;
	}
	return ret;
}

uint8_t TM_GPS_INT_Hex2Dec(char c) {
	if (c >= '0' && c <= '9') {
		return c - '0';									/* 0 - 9 */
	} else if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10; 							/* 10 - 15 */
	} else if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10; 							/* 10 - 15 */
	}
	return 0;
}

uint8_t TM_GPS_INT_FlagsOk(TM_GPS_t* GPS_Data) {
	if (GPS_Flags == GPS_Flags_OK) {					/* Check main flags */
		uint8_t i;
		for (i = 0; i < GPS_Data->CustomStatementsCount; i++) {	/* Check custom terms */
			if (GPS_Data->CustomStatements[i]->Updated == 0) {	/* If not flag set */
				return 0;								/* Return, flags not OK */
			}
		}
		return 1;										/* Flags valid */
	}
	return 0;											/* Not valid */
}

void TM_GPS_INT_ClearFlags(TM_GPS_t* GPS_Data) {
	uint8_t i;
	
	GPS_Flags = 0;						/* Reset main flags */
	for (i = 0; i < GPS_Data->CustomStatementsCount; i++) {	/* Clear custom terms */
		GPS_Data->CustomStatements[i]->Updated = 0;		/* If not flag set */
	}
}

void TM_GPS_INT_CheckEmpty(TM_GPS_t* GPS_Data) {
	if (GPS_Term_Pos == 1) {
		switch (GPS_CONCAT(TM_GPS_Statement, GPS_Term_Number)) {
#ifndef GPS_DISABLE_GPGGA
			case GPS_POS_LATITUDE: 	TM_GPS_INT_SetFlag(GPS_FLAG_LATITUDE); break;
			case GPS_POS_NS: 		TM_GPS_INT_SetFlag(GPS_FLAG_NS); break;
			case GPS_POS_LONGITUDE: TM_GPS_INT_SetFlag(GPS_FLAG_LONGITUDE); break;
			case GPS_POS_EW: 		TM_GPS_INT_SetFlag(GPS_FLAG_EW); break;
			case GPS_POS_SATS:		TM_GPS_INT_SetFlag(GPS_FLAG_SATS); break;
			case GPS_POS_FIX: 		TM_GPS_INT_SetFlag(GPS_FLAG_FIX); break;
			case GPS_POS_ALTITUDE: 	TM_GPS_INT_SetFlag(GPS_FLAG_ALTITUDE); break;
			case GPS_POS_TIME: 		TM_GPS_INT_SetFlag(GPS_FLAG_TIME); break;
#endif
#ifndef GPS_DISABLE_GPRMC
			case GPS_POS_SPEED: 	TM_GPS_INT_SetFlag(GPS_FLAG_SPEED); break;
			case GPS_POS_DATE: 		TM_GPS_INT_SetFlag(GPS_FLAG_DATE); break;
			case GPS_POS_VALIDITY: 	TM_GPS_INT_SetFlag(GPS_FLAG_VALIDITY); break;
			case GPS_POS_DIRECTION: TM_GPS_INT_SetFlag(GPS_FLAG_DIRECTION); break;
#endif
#ifndef GPS_DISABLE_GPGSA
			case GPS_POS_HDOP: 		TM_GPS_INT_SetFlag(GPS_FLAG_HDOP); break;
			case GPS_POS_PDOP: 		TM_GPS_INT_SetFlag(GPS_FLAG_PDOP); break;
			case GPS_POS_VDOP: 		TM_GPS_INT_SetFlag(GPS_FLAG_VDOP); break;
			case GPS_POS_FIXMODE: 	TM_GPS_INT_SetFlag(GPS_FLAG_FIXMODE); break;

			case GPS_POS_SAT1:
			case GPS_POS_SAT2:
			case GPS_POS_SAT3:
			case GPS_POS_SAT4:
			case GPS_POS_SAT5:
			case GPS_POS_SAT6:
			case GPS_POS_SAT7:
			case GPS_POS_SAT8:
			case GPS_POS_SAT9:
			case GPS_POS_SAT10:
			case GPS_POS_SAT11:
			case GPS_POS_SAT12:
				TM_GPS_INT_SetFlag(GPS_FLAG_SATS1_12);
				break;
#endif
#ifndef GPS_DISABLE_GPGSV
			case GPS_POS_SATSINVIEW: TM_GPS_INT_SetFlag(GPS_FLAG_SATSINVIEW); break;
#endif
			default: 
				break;
		}
	}
}
