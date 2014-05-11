%{
	FILE: orbit.m
	PURPOSE: calculations for UBC Orbit calculations
	AUTHOR: Geoffrey Card
	DATE: 2014-05-04 - 

	Write's up a link budget
	Draws the physical orbit
	Checks that everything works
%}

clear
%close('all')
clc

% save file
ofp = fopen('savefile.txt', 'w');
% date and time
fprintf(ofp, '%s\n\n', datestr(now));

% back-end MATLAB/GNU Octave stuff
if exist('OCTAVE_VERSION')
	graphics_toolkit('gnuplot')
	%graphics_toolkit('fltk')
end
% true = save as png, close windows
% false = keep plots open
printing = false;

% all units metres, hertz, degrees, dB, dBm, K, kg, s, bits, and KMS, unless otherwise stated

% frequency
f = 437.5E6;
% speed of light
c = 3E8;
% wavelength
lambda = c/f;
% bandwidth
BW = 12.5E3;

% data rate
rate = 2400; % this doesn't do anything yet
% data to transmit per day
%daily_data = 0.173*2^20*8; % 0.173 MB -> b % (original stated)
daily_data = 529920; % bits/day % (based on Ricky's email)
% data / ( data + protocol envelopes )
protocol_overhead = 0.25;

% slant calc
% minimum viewing angle
delta = 30; % > 15
% radius of Earth
r_e = 6378E3;
% maximum altitude of satellite
alt = 800E3;
% radius to satellite
r_s = r_e + alt;
% central angle between sat and gnd
alpha = 180 - asind(sind(90+delta).*r_e./r_s) - (90+delta);
% slant, longest link distance
slant = r_s*sind(alpha)./sind(90+delta);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% LINK BUDGET BEGIN              %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%{
	The calculations below lack detail, but they're all either correct or best estimates.
	I'll outline here what I do and don't know.

	Data:
	14 + 6*4 + 8 = 14+24+8 = 46 bytes/minute
	24*60*46 = 66240 bytes/day = 529920 bits/day
	
	Orbit:
	LEO (low Earth orbit)
	semi-major axis: 7078+-100km (600km to 800km altitude)
	eccentricity < 0.01
	let's say altutude 800km (worst-case)

	Frequency:
	amateur space band: 435 - 438 MHz
	let's call it 437.5 MHz

	Satellite:
	Transmitter:
	Power: 1.59W (32dBm)
	Receiver:
	LNA gain: 0 dB (none, if you think it'll help, we can add one)
	I don't know enough to give temperature values for noise
	cable type: SMA of some kind
	cable length: < 0.3 m (depends on position in sat)
	cable insertion loss: I don't know, about 2.1dB?
	total cable losses: insertion loss dominates, too short for attentuation, plus antenna mismatch
	
	Ground Station:
	Transmitter:
	Power: I suggest at least 15W, but it depends on the PA
	Receivers:
	LNA gain: depends on what you find
	I don't know enough to give temperature values for noise
	cable type: I don't know
	cable length: expect 10 - 25 m
	cable insertion loss: I don't know, a few dB?
	total cable losses: insertion loss for each section, plus attentuation, plus antenna mismatch

	Antenna gain:
	Ground station:
	Yagi-Uda, RHCP/LHCP
	pointing error: 5 degrees (according to Michelson)
	Spacecraft:
	Monopole, Linear
	pointing error: 20 degrees (according to Nik (attitude))

	Antenna polarization loss:
	we have linear to circular, therefore we lose half each way
	polarization loss: -3dB

	Atmosphereic and Ionoshperic Losses:
	calculated using spreadsheet:
	https://www.google.ca/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&ved=0CC0QFjAA&url=http%3A%2F%2Fwww.amsat.org%2Famsat%2Fftp%2Fsoftware%2Fspreadsheet%2FAMSAT-IARU_Link_Budget_Rev1.xls&ei=NKppU4PBL4T9oATe5YLwAg&usg=AFQjCNE31zd48Nzao70GBnJLjjHKZMW0eQ&sig2=IObWr8fExm36Z913ivDwEw
	
	minimum elevation angle: 15 degrees (according to Michelson) (due to interference from other sources)

	Modulation/Demodulation Method:
	FSK, but all of that is accounted for in the CC1120

	Transceiver: TI CC1120
	output power: 16dBm
	receiver bandwidth: 12.5kHz
	sensitivity: ~-110 dBm
	required S/N: difficult, I only found this:
	TI: F = 433MHz, 
		Data Rate = 1200, 
		Modulation = FSK
		In this case the sensitivity is -123 dBm (@SNR 5dB)

	Power Amplifier: RFMD RF5110G
	output power: 32dBm (so gain 16dB)
	efficiency: 53%
		
	Miscellaneous:
	we also have to account for doppler shift
%}

% Boltzmann constant
k_B = 1.38E-23;
% temperature
T = 270;

% maximum path loss
L_path = 20*log10(lambda./(4*pi*slant));

% power of transmitters
% sat Tx
P_sat_Tx = +16;
% gnd Tx
P_gnd_Tx = +16;

% sensitivity of receivers
sens_sat = -120;
sens_gnd = -120;

% gain of amplifiers
% sat
G_sat_PA = +16;
G_sat_LNA = +0;
% gnd
G_gnd_PA = +30;
G_gnd_LNA = +10;

% losses from cable
L_sat_cable = -2.5;
L_gnd_cable = -3;

% gain of antenna
G_sat_ant = +2.2;
% pointing error (degrees)
%err_point = 20; % use in calculations (not implemented)
% losses from pointing
L_sat_point = -10;

% gain of antenna
G_gnd_ant = +15;
% pointing error (degrees)
%err_point = 5; % use in calculations (not implemented)
% losses from pointing
L_gnd_point = -0.5;

% polarization losses
L_polar_up = -3; % right-handed circular to linear
L_polar_dn = -3; % linear to right-handed circular

% losses from atmosphere
L_atm = -1.1;
% losses from ionophere
L_ion = -0.4;

% UPLINK
P_sat_Rx = P_gnd_Tx + G_gnd_PA + L_gnd_cable + G_gnd_ant + L_gnd_point + L_path + L_atm + L_ion + L_polar_up + L_sat_point + G_sat_ant + L_gnd_cable + G_sat_LNA;
margin_up = P_sat_Rx - sens_sat;
% DOWNLINK
P_gnd_Rx = P_sat_Tx + G_sat_PA + L_sat_cable + G_sat_ant + L_sat_point + L_path + L_atm + L_ion + L_polar_dn + L_gnd_point + G_gnd_ant + L_gnd_cable + G_gnd_LNA;
margin_dn = P_gnd_Rx - sens_gnd;

% noise
P_sat_Rx_signal = P_gnd_Tx + G_gnd_PA + L_gnd_cable + G_gnd_ant + L_gnd_point + L_path + L_atm + L_ion;
P_sat_Rx_noise = 10*log10(k_B*T*BW) + 30; % +30 for dBW to dBm conversion
P_gnd_Rx_signal = P_sat_Tx + G_sat_PA + L_sat_cable + G_sat_ant + L_sat_point + L_path + L_atm + L_ion;
P_gnd_Rx_noise = 10*log10(k_B*T*BW) + 30; % +30 for dBW to dBm conversion

% SNR, S/N, C/N
SNR_up = P_sat_Rx_signal - P_sat_Rx_noise;
SNR_dn = P_gnd_Rx_signal - P_gnd_Rx_noise;

fprintf(ofp, 'link budget\n');
fprintf(ofp, ...
		['\ndownlink (sat -> gnd)    \n' ...
		 'P_sat_Tx        %+15.2f dBm\n' ...
		 'G_sat_PA        %+15.2f dB \n' ...
		 'L_sat_cable     %+15.2f dB \n' ...
		 'G_sat_ant       %+15.2f dBi\n' ...
		 'L_sat_point     %+15.2f dB \n' ...
		 'L_path          %+15.2f dB \n' ...
		 'L_atm           %+15.2f dB \n' ...
		 'L_ion           %+15.2f dB \n' ...
		 'L_polar_up      %+15.2f dB \n' ...
		 'L_gnd_point     %+15.2f dB \n' ...
		 'G_gnd_ant       %+15.2f dBi\n' ...
		 'L_gnd_cable     %+15.2f dB \n' ...
		 'G_gnd_LNA       %+15.2f dB \n' ...
		 '-----------------------------------\n' ...
		 'P_gnd_Tx        %+15.2f dBm\n' ...
		 'sens_gnd        %+15.2f dBm\n' ...
		 '-----------------------------------\n' ...
		 'margin_dn       %+15.2f dB \n' ...
		 ], ...
		P_sat_Tx,    ...
		G_sat_PA,    ...
		L_sat_cable, ...
		G_sat_ant,   ...
		L_sat_point, ...
		L_path,      ...
		L_atm,       ...
		L_ion,       ...
		L_polar_up,  ...
		L_gnd_point, ...
		G_gnd_ant,   ...
		L_gnd_cable, ...
		G_gnd_LNA,   ...
		P_gnd_Rx,    ...
		sens_gnd,    ...
		margin_dn    ...
		);
fprintf(ofp, ...
		['\nuplink (gnd -> sat)      \n' ...
		 'P_gnd_Tx        %+15.2f dBm\n' ...
		 'G_gnd_PA        %+15.2f dB \n' ...
		 'L_gnd_cable     %+15.2f dB \n' ...
		 'G_gnd_ant       %+15.2f dBi\n' ...
		 'L_gnd_point     %+15.2f dB \n' ...
		 'L_path          %+15.2f dB \n' ...
		 'L_atm           %+15.2f dB \n' ...
		 'L_ion           %+15.2f dB \n' ...
		 'L_polar_dn      %+15.2f dB \n' ...
		 'L_sat_point     %+15.2f dB \n' ...
		 'G_sat_ant       %+15.2f dBi\n' ...
		 'L_sat_cable     %+15.2f dB \n' ...
		 'G_sat_LNA       %+15.2f dB \n' ...
		 '-----------------------------------\n' ...
		 'P_sat_Tx        %+15.2f dBm\n' ...
		 'sens_gnd        %+15.2f dBm\n' ...
		 '-----------------------------------\n' ...
		 'margin_up       %+15.2f dB \n' ...
		 ], ...
		P_gnd_Tx,    ...
		G_gnd_PA,    ...
		L_gnd_cable, ...
		G_gnd_ant,   ...
		L_gnd_point, ...
		L_path,      ...
		L_atm,       ...
		L_ion,       ...
		L_polar_dn,  ...
		L_sat_point, ...
		G_sat_ant,   ...
		L_sat_cable, ...
		G_sat_LNA,   ...
		P_sat_Rx,    ...
		sens_sat,    ...
		margin_up    ...
		);
fprintf(ofp, ...
		['\n' ...
		 'P_sat_Rx_signal = %+15.2f dBm\n' ...
		 'P_sat_Rx_noise  = %+15.2f dBm\n' ...
		 'SNR_up          = %+15.2f dB \n' ...
		 '\n'                              ...
		 'P_gnd_Rx_signal = %+15.2f dBm\n' ...
		 'P_gnd_Rx_noise  = %+15.2f dBm\n' ...
		 'SNR_dn          = %+15.2f dB \n' ...
		 ], ...
		P_sat_Rx_signal, ...
		P_sat_Rx_noise,  ...
		SNR_up,          ...
		P_gnd_Rx_signal, ...
		P_gnd_Rx_noise,  ...
		SNR_dn           ...
		);
fprintf(ofp, '\n\n');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% LINK BUDGET END                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% ORBIT BEGIN                    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%{
	assumes circular polar orbit
%}

% orbital velocity calc
M_e = 5.97219E24; % mass of Earth
G = 6.67384E-11; % gravitational constant
v = sqrt(G*M_e/r_s); % orbital veloctiy
period = 2*pi*r_s/v; % time it takes to make one orbit
% anglar velocity in degrees/second
dlatdt = v*360/(2*pi*r_s); % change in latitude angle per time
dlongdt = 360/(24*3600); % change in longitude angle per time

% orbit
days = 7; % more days, more accurate, more computations
len = days*24*60; % precise to the minute
t = linspace(0,days*24*3600,len);
% latitude / phi
theta = linspace(0,dlatdt*days*24*3600,len); % lat
lat = asind(sind(theta));
% longitude / theta
phi = linspace(0,dlongdt*days*24*3600,len); % long
long = atan2(sind(phi).*cosd(theta),cosd(phi).*cosd(theta))*180/pi;
% 3D orbit
x_sat = r_s*cosd(long).*cosd(lat);
y_sat = r_s*sind(long).*cosd(lat);
z_sat = r_s*sind(lat);

% Vancouver
lat_van = 49.261731;
long_van = -123.249541;
% 3D Vancouver
x_van = r_e*cosd(long_van).*cosd(lat_van);
y_van = r_e*sind(long_van).*cosd(lat_van);
z_van = r_e*sind(lat_van);

% Earth
load('world0.dat')
%world0(:,2) % latitude
%world0(:,1) % longitude
% 3D Earth
x_earth = r_e*cosd(world0(:,1)).*cosd(world0(:,2));
y_earth = r_e*sind(world0(:,1)).*cosd(world0(:,2));
z_earth = r_e*sind(world0(:,2));

% determine in/out of range
in_range = 1*( abs(acosd( cosd(90-lat).*cosd(90-lat_van) + sind(90-lat).*sind(90-lat_van).*cosd(long-long_van))) <= mean(alpha) );
out_range = 1-in_range;
% NaNs are not drawn
% nan_in_range draws only stuff in range
nan_in_range = ones(1,length(in_range));
% nan_in_range draws only stuff out of range
nan_out_range = ones(1,length(out_range));
for ii = 1:len
	if in_range(ii) == 0
		nan_in_range(ii) = NaN;
	else
		nan_out_range(ii) = NaN;
	end
end

% fixes 2D plot
% NaN on date line crossings
long_fix = ones(size(long));
for ii=2:length(long)
	if long(ii-1) > long(ii)
		long_fix(ii) = NaN;
	end
end

% longest communications gap
max_gap = 0;
gap = 0;
for ii = 1:length(out_range)
	if out_range(ii) == 0
		gap = 0;
	else
		gap = gap + 1;
		if gap > max_gap
			max_gap = gap;
		end
	end
end

% doppler shift
% link unit vector
% from sat to gnd
u_x_link = x_van-x_sat;
u_y_link = y_van-y_sat;
u_z_link = z_van-z_sat;
u_link = [u_x_link' u_y_link' u_z_link'];
u_link_sq = sqrt(u_x_link.^2 + u_y_link.^2 + u_z_link.^2);
u_link = u_link ./ (u_link_sq'*[1 1 1]);
% 3D orbit derivative
% [r_s*cosd(phi+5).*cosd(theta), r_s*sind(phi+5).*cosd(theta), r_s*sind(theta)]
dxdt_sat = r_s*(-dlongdt*pi/180*sind(phi).*cosd(theta) - dlatdt*pi/180*cosd(phi).*sind(theta));
dydt_sat = r_s*( dlongdt*pi/180*cosd(phi).*cosd(theta) - dlatdt*pi/180*sind(phi).*sind(theta));
dzdt_sat = r_s*( dlatdt *pi/180*cosd(theta));
dsatdt = [dxdt_sat' dydt_sat' dzdt_sat'];
u_dsatdt = dsatdt/norm(dsatdt(1,:));
% dot product, doppler shift
dlinkdt = norm(dsatdt(1,:))*sum((u_dsatdt.*u_link)')';
f_doppler = c./(c+dlinkdt)*f - f;

% elevation and azimuth
% unit vectors from Vancouver
u_van_up = [cosd(long_van)*cosd(lat_van), sind(long_van)*cosd(lat_van), sind(lat_van)];
u_van_north = [-cosd(long_van)*sind(lat_van), -sind(long_van)*sind(lat_van), cosd(lat_van)];
u_van_east = [-sind(long_van)*cosd(lat_van), cosd(long_van)*cosd(lat_van), 0];
% calculate
el = zeros(size(x_sat));
az = zeros(size(x_sat));
for ii = 1:length(x_sat)
	u_sat = [x_sat(ii)-x_van y_sat(ii)-y_van z_sat(ii)-z_van];
	u_sat = u_sat/norm(u_sat);
	el(ii) = asind( dot(u_van_up, u_sat) );
	az(ii) = atan2( dot(u_van_east, u_sat), dot(u_van_north, u_sat) )*180/pi;
end


% 2D plot, height is doppler shift
fig1 = figure(1);
clf
hold on
plot3(world0(:,1), world0(:,2), zeros(size(world0(:,1))), 'k')
plot3(long.*long_fix, lat, f_doppler'.*nan_in_range, 'r')
plot3(long.*long_fix, lat, zeros(size(long)).*nan_out_range,'g')
hold off
box off
axis([-180 180 -90 90 -max(abs(f_doppler)) max(abs(f_doppler))])
if exist('OCTAVE_VERSION') == 0
	axis vis3d
end
axis on
%set(gcf, 'units','normalized','outerposition',[0 0 1 1])
%set(gcf, 'units','normalized','position',[0.1 0.1 0.9 0.9])
set(gca,'DataAspectRatio',[1 1 range(f_doppler)/range(lat)])
view(0,90)
if printing == true
	print(fig1, '-dpng', '2D_orbit.png')
	close(fig1)
end


% 3D plot
fig2 = figure(2);
clf
hold on
plot3(x_earth,y_earth,z_earth, 'color', [0 0 0])
plot3(x_van,y_van,z_van, '*r')
plot3(x_sat,y_sat,z_sat.*nan_out_range, 'g')
plot3(x_sat,y_sat,z_sat.*nan_in_range, 'r')
hold off
box off
axis([-r_s r_s -r_s r_s -r_s r_s])
if exist('OCTAVE_VERSION') == 0
	axis vis3d
end
axis off
%set(gcf, 'units','normalized','outerposition',[0 0 1 1])
%set(gcf, 'units','normalized','position',[0.1 0.1 0.9 0.9])
set(gca,'DataAspectRatio',[1 1 1])
view(long_van+90, lat_van)
if printing == true
	print(fig2, '-dpng', '3D_orbit.png')
	close(fig2)
end

% save all to file
fprintf(ofp, '\norbital results:\n\n');
fprintf(ofp, 'altitude: %.0f km\n', alt/1E3);
fprintf(ofp, 'delta: %.2f degrees\n', delta);
fprintf(ofp, 'slant: %.0f km\n', slant/1E3);
fprintf(ofp, 'alpha: %.2f degrees\n', alpha);
fprintf(ofp, 'period: %.2f minutes\n', period/60);
fprintf(ofp, 'longest uptime: %.2f minutes\n', 2*alpha/dlatdt/60);
fprintf(ofp, 'longest downtime: %.2f hours\n', max_gap/60);
fprintf(ofp, 'orbits per day: %.2f\n', 24*3600/period);
fprintf(ofp, 'number of passes per day: %.2f\n', 2*alpha/360 * 24*3600/period);
fprintf(ofp, 'daily uptime: %.2f minutes/day\n', sum(in_range)/days);
fprintf(ofp, 'minimum data rate: %6i b/s\n', ceil(daily_data / (sum(in_range)*60/days ) * (1+protocol_overhead)/1) );
fprintf(ofp, 'maximum doppler shift: %.0f kHz\n', max(abs(f_doppler.*in_range')));
fprintf(ofp, '\n\n\ndata:\n');
for ii = 1:length(x_sat)
	if in_range(ii) == 1
		thingy = '*';
	else
		thingy = ' ';
	end
	jj = ii-1;
	fprintf(ofp, '%c %4i days %2i hours %2i minutes     lat: %4.0f    long: %4.0f     az: %4.0f    el: %4.0f      doppler: %6.2f kHz\n', thingy, floor(jj/24/60), mod(floor(jj/60),24), mod(jj,60), lat(ii), long(ii), az(ii), el(ii), f_doppler(ii)/1E3);
end
fprintf(ofp, '\n\n');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% ORBIT END                      %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

fclose(ofp);
% and save all the variables in case someone want to do more maths
save('savefile.mat')
