(RS-274 Compatible G-code generated for UCCNC controller and Stepcraft 2 Mill)
(Created by in EagleCAD with the stepcraft.ulp 0.2.)
(File: "LTC3092_BREAKOUT.brd" created on: 8/22/16 11:46 AM)
(Use a 1/32inch Bit)

G90 G94 G49
G21
G00 Z5
G00 X0Y0
S19500 M03
G04 P4000
(curr: 0.000,0.000,5.000)
(start: 3.810,-0.318)
(end:  3.810,5.397)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y-0.318
G01 Z-0.090 F30
G01 X3.810Y5.397 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.810Y-0.318
G01 Z-0.090 F30
G01 X3.810Y5.397 F360
(curr: 3.810,5.397,-0.090)
(start: 3.810,5.397)
(end:  3.810,15.240)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y15.240 F360
(curr: 3.810,15.240,-0.090)
(start: 3.810,15.240)
(end:  3.810,23.019)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y23.019 F360
(curr: 3.810,23.019,-0.090)
(start: 3.810,15.240)
(end:  6.033,15.240)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y15.240
G01 Z-0.090 F30
G01 X6.033Y15.240 F360
(curr: 6.033,15.240,-0.090)
(start: 6.033,15.240)
(end:  7.620,15.240)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X7.620Y15.240 F360
(curr: 7.620,15.240,-0.090)
(start: 7.620,15.240)
(end:  7.620,23.336)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X7.620Y23.336 F360
(curr: 7.620,23.336,-0.090)
(start: 3.810,5.397)
(end:  7.779,5.397)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y5.397
G01 Z-0.090 F30
G01 X7.779Y5.397 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.810Y5.397
G01 Z-0.090 F30
G01 X7.779Y5.397 F360
(curr: 7.779,5.397,-0.090)
(start: 7.779,5.397)
(end:  7.779,10.319)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X7.779Y10.319 F360
(curr: 7.779,10.319,-0.090)
(start: 7.779,10.319)
(end:  6.033,10.319)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.033Y10.319 F360
(curr: 6.033,10.319,-0.090)
(start: 6.033,10.319)
(end:  6.033,15.240)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.033Y15.240 F360
(curr: 6.033,15.240,-0.090)
(start: 7.779,5.397)
(end:  10.160,5.397)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X7.779Y5.397
G01 Z-0.090 F30
G01 X10.160Y5.397 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X7.779Y5.397
G01 Z-0.090 F30
G01 X10.160Y5.397 F360
(curr: 10.160,5.397,-0.090)
(start: 10.160,5.397)
(end:  10.160,10.319)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X10.160Y10.319 F360
(curr: 10.160,10.319,-0.090)
(start: 10.160,10.319)
(end:  13.970,10.319)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.970Y10.319 F360
G00 Z1.000
M05
G04 P2000
G00 X0Y0
