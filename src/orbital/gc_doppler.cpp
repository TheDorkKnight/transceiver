/*
 * FILE:    gc_doppler.cpp
 * PURPOSE: calculate doppler shift of polar orbiting satellite
 * AUTHOR:  Geoffrey Card
 * DATE:    2014-05-07
 * NOTES:   Assumes polar orbit, requires input of:
 *          	carrier frequency, 
 *          	orbital altitude,
 *          	latitude and longitude of ground station
 *          You may want to rewrite these into a more compact form, 
 *          they do the same math multiple times.
 */

#include <cmath>
// radians to degrees
#define RAD_TO_DEG 180/3.14159 // deg
#define DEG_TO_RAD 3.14159/180 // rad

using namespace std;

// frequency
#define F 437.5E6 // Hz
// orbital altitude
#define HEIGHT 800E3 // m

// constants
#define C 3E8 // m/s
#define GRAVITATIONAL_CONSTANT 6.67384E-11 // m^3.kg^-1.s^-2
#define M_EARTH 5.97219E24 // kg
#define R_EARTH 6378E3 // m

// semi-major axis of orbit
#define R_SAT (R_EARTH + HEIGHT) // m

// velocity components
#define DLATDT sqrt(GRAVITATIONAL_CONSTANT*M_EARTH/(R_SAT*R_SAT*R_SAT)) // rad/s
#define DLONGDT 2*3.14159/(24*3600)                         // rad/s

// ground station coordinates
#define LAT_GND    49.261731 // deg
#define LONG_GND -123.249541 // deg
// position
#define X_GND R_EARTH*cos(LONG_GND*DEG_TO_RAD)*cos(LAT_GND*DEG_TO_RAD) // m
#define Y_GND R_EARTH*sin(LONG_GND*DEG_TO_RAD)*cos(LAT_GND*DEG_TO_RAD) // m
#define Z_GND R_EARTH*sin(LAT_GND*DEG_TO_RAD)                          // m
// unit up vector
#define U_U_X_GND cos(LONG_GND*DEG_TO_RAD)*cos(LAT_GND*DEG_TO_RAD)
#define U_U_Y_GND sin(LONG_GND*DEG_TO_RAD)*cos(LAT_GND*DEG_TO_RAD)
#define U_U_Z_GND sin(LAT_GND*DEG_TO_RAD)               
// unit north vector
#define U_N_X_GND -cos(LONG_GND*DEG_TO_RAD)*sin(LAT_GND*DEG_TO_RAD)
#define U_N_Y_GND -sin(LONG_GND*DEG_TO_RAD)*sin(LAT_GND*DEG_TO_RAD)
#define U_N_Z_GND  cos(LAT_GND*DEG_TO_RAD)               
// unit east vector
#define U_E_X_GND -sin(LONG_GND*DEG_TO_RAD)*cos(LAT_GND*DEG_TO_RAD)
#define U_E_Y_GND  cos(LONG_GND*DEG_TO_RAD)*cos(LAT_GND*DEG_TO_RAD)
#define U_E_Z_GND  0 // I'll save some processing by ignoring this in calculation              

/*
	PARAMETERS:
		lat_sat: satellite latitude in degrees
		long_sat: satellite longitude in degrees
	RETURNS:
		doppler shift in hertz
		
	NOTE: Using traditional latitude and longitude coordinates 
		will always return the shift of a northward bound satellite.
		To converty to southward bound:
			long_sat = long_sat + 180;
			lat_sat = 180 - lat_sat;
		or use polar coordinates instead of latitude and longgitude (0 to 360 for lat and long),
		or use calc_doppler_sgn;
		your choice.
*/
float calc_doppler(float lat_sat, float long_sat)
{
	// relative displacement
	float xrel = X_GND - R_SAT*cos(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD);
	float yrel = Y_GND - R_SAT*sin(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD);
	float zrel = Z_GND - R_SAT*sin(lat_sat*DEG_TO_RAD);
	float drel = sqrt(xrel*xrel + yrel*yrel + zrel*zrel);
	
	// almost the velocity vector
	float udxdts = -DLONGDT*sin(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD) - DLATDT*cos(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD);
	float udydts =  DLONGDT*cos(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD) - DLATDT*cos(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD);
	float udzdts =  DLATDT*cos(lat_sat*DEG_TO_RAD);
	
	// relative velocity
	float dlinkdt = R_SAT/drel*(udxdts*xrel + udydts*yrel + udzdts*zrel); // m/s
	
	// doppler shift
	return C/(C-dlinkdt)*F-F;
}

/*
	same as calc_doppler except
	PARAMETER:
		southward: true if satellite is headed South, else false
*/
float calc_doppler_sgn(float lat_sat, float long_sat, bool southward)
{
	// set direction
	if (southward) {
		long_sat = long_sat + 180; // deg
		lat_sat = 180 - lat_sat;   // deg
	}
	
	// relative displacement
	float xrel = X_GND - R_SAT*cos(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD);
	float yrel = Y_GND - R_SAT*sin(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD);
	float zrel = Z_GND - R_SAT*sin(lat_sat*DEG_TO_RAD);
	float drel = sqrt(xrel*xrel + yrel*yrel + zrel*zrel);
	
	// almost the velocity vector (just need to multiply all by R_SAT, saved for next step)
	float udxdts = -DLONGDT*sin(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD) - DLATDT*cos(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD);
	float udydts =  DLONGDT*cos(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD) - DLATDT*cos(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD);
	float udzdts =  DLATDT*cos(lat_sat*DEG_TO_RAD);
	
	// relative velocity
	float dlinkdt = R_SAT/drel*(udxdts*xrel + udydts*yrel + udzdts*zrel); // m/s
	
	// doppler shift
	return C/(C-dlinkdt)*F-F;
}

/*
	PARAMETERS:
		lat_sat: satellite latitude in degrees
		long_sat: satellite longitude in degrees
	RETURNS:
		elevation in degrees
		
	NOTE: Direction is unimportant, feel free to use traditional lat and long.
*/
float calc_elevation(float lat_sat, float long_sat)
{
	// relative displacement
	float xrel = X_GND - R_SAT*cos(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD); // m
	float yrel = Y_GND - R_SAT*sin(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD); // m
	float zrel = Z_GND - R_SAT*sin(lat_sat*DEG_TO_RAD);                          // m
	float drel = sqrt(xrel*xrel + yrel*yrel + zrel*zrel);                        // m
	float uxrel = xrel/drel;
	float uyrel = yrel/drel;
	float uzrel = zrel/drel;
	
	return asin( -U_U_X_GND*uxrel - U_U_Y_GND*uyrel - U_U_Z_GND*uzrel )*RAD_TO_DEG; // deg
}

/*
	PARAMETERS:
		lat_sat: satellite latitude in degrees
		long_sat: satellite longitude in degrees
	RETURNS:
		azimuth in degrees
		
	NOTE: Direction is unimportant, feel free to use traditional lat and long.
*/
float calc_azimuth(float lat_sat, float long_sat)
{
	// relative displacement
	float xrel = X_GND - R_SAT*cos(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD); // m
	float yrel = Y_GND - R_SAT*sin(long_sat*DEG_TO_RAD)*cos(lat_sat*DEG_TO_RAD); // m
	float zrel = Z_GND - R_SAT*sin(lat_sat*DEG_TO_RAD);                          // m
	float drel = sqrt(xrel*xrel + yrel*yrel + zrel*zrel);                        // m
	float uxrel = xrel/drel;
	float uyrel = yrel/drel;
	float uzrel = zrel/drel;
	
	return atan2( -U_E_X_GND*uxrel - U_E_Y_GND*uyrel - U_E_Z_GND*uzrel, 
	              -U_N_X_GND*uxrel - U_N_Y_GND*uyrel                   )*RAD_TO_DEG; // deg
}

#include <cstdio>

int main (void)
{
	FILE * ofp = fopen("savefile.txt", "w"); // notice a pattern in file names?
	
	char thingy = '*';
	float lat_sat = 0, long_sat = 0; // deg
	float lat_sat_tr = 0, long_sat_tr = 0; // deg
	float el = 0, az = 0; // deg
	float f_doppler = 0; // Hz
	
	/*
	for (int i = -90; i < 90; i++) {
		printf("%d %6.2f\n", i, asin(sin(i*DEG_TO_RAD))*RAD_TO_DEG);
	}
	*/
	
	for (int t = 0; t < 10*60; t+=1) {
		lat_sat = (LAT_GND)*DEG_TO_RAD + DLATDT*(t-5*60);   // rad
		long_sat = (LONG_GND)*DEG_TO_RAD + DLONGDT*(t-5*60); // rad
		
		lat_sat_tr = asin(sin(lat_sat)) * RAD_TO_DEG;                  // deg
		long_sat_tr = atan2(sin(long_sat)*cos(lat_sat), 
		                    cos(long_sat)*cos(lat_sat) ) * RAD_TO_DEG; // deg

		lat_sat = lat_sat * RAD_TO_DEG;   // deg
		long_sat = long_sat * RAD_TO_DEG; // deg
		
		el = calc_elevation(lat_sat, long_sat); // deg
		az = calc_azimuth(lat_sat, long_sat);   // deg

		f_doppler = calc_doppler(lat_sat, long_sat); // Hz
		
		if (el >= 45) {
			thingy = '*';
		} else {
			thingy = ' ';
		}
		fprintf(ofp, "%c", thingy);
		fprintf(ofp, " %4d days %2d hours %2d minutes %2d seconds", (int) floor(t/24/3600), (int) floor(t/3600)%24, (int) floor(t/60)%60, (int) t%60);
		fprintf(ofp, "     lat: %4.0f    long: %4.0f     lat: %4.0f    long: %4.0f     az: %4.0f    el: %4.0f", lat_sat, long_sat, lat_sat_tr, long_sat_tr, az, el);
		fprintf(ofp, "      doppler: %6.2f kHz\n", f_doppler/1E3);
		
	}
	
	fclose(ofp);
	return 0;
}
