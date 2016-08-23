(RS-274 Compatible G-code generated for UCCNC controller and Stepcraft 2 Mill)
(Created by in EagleCAD with the stepcraft.ulp 0.2.)
(File: "LT6656_BREAKOUT.brd" created on: 8/19/16 12:46 PM)
(Use a V Bit)

G90 G94 G49
G21
G00 Z5
G00 X0Y0
S19500 M03
G04 P4000
(curr: 0.000,0.000,5.000)
(start: 3.810,0.635)
(end:  3.810,7.461)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y0.635
G01 Z-0.070 F500
G01 X3.810Y7.461 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.810Y0.635
G01 Z-0.070 F500
G01 X3.810Y7.461 F1500
(curr: 3.810,7.461,-0.070)
(start: 3.810,7.461)
(end:  3.810,8.414)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y8.414 F1500
(curr: 3.810,8.414,-0.070)
(start: 3.810,8.414)
(end:  3.810,9.366)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y9.366 F1500
(curr: 3.810,9.366,-0.070)
(start: 3.810,9.366)
(end:  3.810,10.319)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y10.319 F1500
(curr: 3.810,10.319,-0.070)
(start: 3.810,10.319)
(end:  3.810,16.669)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y16.669 F1500
(curr: 3.810,16.669,-0.070)
(start: 3.810,16.669)
(end:  6.350,16.669)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.350Y16.669 F1500
(curr: 6.350,16.669,-0.070)
(start: 6.350,16.669)
(end:  6.350,9.366)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.350Y9.366 F1500
(curr: 6.350,9.366,-0.070)
(start: 6.350,9.366)
(end:  3.810,9.366)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y9.366 F1500
(curr: 3.810,9.366,-0.070)
(start: 6.350,9.366)
(end:  6.350,8.414)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X6.350Y9.366
G01 Z-0.070 F500
G01 X6.350Y8.414 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X6.350Y9.366
G01 Z-0.070 F500
G01 X6.350Y8.414 F1500
(curr: 6.350,8.414,-0.070)
(start: 6.350,8.414)
(end:  3.810,8.414)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y8.414 F1500
(curr: 3.810,8.414,-0.070)
(start: 6.350,8.414)
(end:  6.350,0.635)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X6.350Y8.414
G01 Z-0.070 F500
G01 X6.350Y0.635 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X6.350Y8.414
G01 Z-0.070 F500
G01 X6.350Y0.635 F1500
(curr: 6.350,0.635,-0.070)
(start: 6.350,0.635)
(end:  3.810,0.635)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y0.635 F1500
(curr: 3.810,0.635,-0.070)
(start: 3.810,8.414)
(end:  1.746,8.414)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y8.414
G01 Z-0.070 F500
G01 X1.746Y8.414 F1500
(curr: 1.746,8.414,-0.070)
(start: 1.746,8.414)
(end:  1.746,7.461)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X1.746Y7.461 F1500
(curr: 1.746,7.461,-0.070)
(start: 1.746,7.461)
(end:  3.810,7.461)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y7.461 F1500
(curr: 3.810,7.461,-0.070)
(start: 3.810,9.366)
(end:  1.429,9.366)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y9.366
G01 Z-0.070 F500
G01 X1.429Y9.366 F1500
(curr: 1.429,9.366,-0.070)
(start: 3.810,10.319)
(end:  1.429,10.319)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y10.319
G01 Z-0.070 F500
G01 X1.429Y10.319 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.810Y10.319
G01 Z-0.070 F500
G01 X1.429Y10.319 F1500
(curr: 1.429,10.319,-0.070)
(start: 3.810,10.319)
(end:  5.397,10.319)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y10.319
G01 Z-0.070 F500
G01 X5.397Y10.319 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.810Y10.319
G01 Z-0.070 F500
G01 X5.397Y10.319 F1500
(curr: 5.397,10.319,-0.070)
(start: 3.810,7.461)
(end:  5.556,7.461)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y7.461
G01 Z-0.070 F500
G01 X5.556Y7.461 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.810Y7.461
G01 Z-0.070 F500
G01 X5.556Y7.461 F1500
(curr: 5.556,7.461,-0.070)
(start: 6.350,16.669)
(end:  8.890,16.669)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X6.350Y16.669
G01 Z-0.070 F500
G01 X8.890Y16.669 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X6.350Y16.669
G01 Z-0.070 F500
G01 X8.890Y16.669 F1500
(curr: 8.890,16.669,-0.070)
(start: 8.890,16.669)
(end:  8.890,0.635)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.890Y0.635 F1500
(curr: 8.890,0.635,-0.070)
(start: 8.890,0.635)
(end:  6.350,0.635)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.350Y0.635 F1500
(curr: 6.350,0.635,-0.070)
(start: 3.810,0.635)
(end:  3.810,7.461)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y0.635
G01 Z-0.090 F500
G01 X3.810Y7.461 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.810Y0.635
G01 Z-0.090 F500
G01 X3.810Y7.461 F1500
(curr: 3.810,7.461,-0.090)
(start: 3.810,7.461)
(end:  3.810,8.414)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y8.414 F1500
(curr: 3.810,8.414,-0.090)
(start: 3.810,8.414)
(end:  3.810,9.366)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y9.366 F1500
(curr: 3.810,9.366,-0.090)
(start: 3.810,9.366)
(end:  3.810,10.319)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y10.319 F1500
(curr: 3.810,10.319,-0.090)
(start: 3.810,10.319)
(end:  3.810,16.669)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y16.669 F1500
(curr: 3.810,16.669,-0.090)
(start: 3.810,16.669)
(end:  6.350,16.669)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.350Y16.669 F1500
(curr: 6.350,16.669,-0.090)
(start: 6.350,16.669)
(end:  6.350,9.366)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.350Y9.366 F1500
(curr: 6.350,9.366,-0.090)
(start: 6.350,9.366)
(end:  3.810,9.366)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y9.366 F1500
(curr: 3.810,9.366,-0.090)
(start: 6.350,9.366)
(end:  6.350,8.414)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X6.350Y9.366
G01 Z-0.090 F500
G01 X6.350Y8.414 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X6.350Y9.366
G01 Z-0.090 F500
G01 X6.350Y8.414 F1500
(curr: 6.350,8.414,-0.090)
(start: 6.350,8.414)
(end:  3.810,8.414)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y8.414 F1500
(curr: 3.810,8.414,-0.090)
(start: 6.350,8.414)
(end:  6.350,0.635)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X6.350Y8.414
G01 Z-0.090 F500
G01 X6.350Y0.635 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X6.350Y8.414
G01 Z-0.090 F500
G01 X6.350Y0.635 F1500
(curr: 6.350,0.635,-0.090)
(start: 6.350,0.635)
(end:  3.810,0.635)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y0.635 F1500
(curr: 3.810,0.635,-0.090)
(start: 3.810,8.414)
(end:  1.746,8.414)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y8.414
G01 Z-0.090 F500
G01 X1.746Y8.414 F1500
(curr: 1.746,8.414,-0.090)
(start: 1.746,8.414)
(end:  1.746,7.461)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X1.746Y7.461 F1500
(curr: 1.746,7.461,-0.090)
(start: 1.746,7.461)
(end:  3.810,7.461)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y7.461 F1500
(curr: 3.810,7.461,-0.090)
(start: 3.810,9.366)
(end:  1.429,9.366)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y9.366
G01 Z-0.090 F500
G01 X1.429Y9.366 F1500
(curr: 1.429,9.366,-0.090)
(start: 3.810,10.319)
(end:  1.429,10.319)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y10.319
G01 Z-0.090 F500
G01 X1.429Y10.319 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.810Y10.319
G01 Z-0.090 F500
G01 X1.429Y10.319 F1500
(curr: 1.429,10.319,-0.090)
(start: 3.810,10.319)
(end:  5.397,10.319)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y10.319
G01 Z-0.090 F500
G01 X5.397Y10.319 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.810Y10.319
G01 Z-0.090 F500
G01 X5.397Y10.319 F1500
(curr: 5.397,10.319,-0.090)
(start: 3.810,7.461)
(end:  5.556,7.461)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.810Y7.461
G01 Z-0.090 F500
G01 X5.556Y7.461 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.810Y7.461
G01 Z-0.090 F500
G01 X5.556Y7.461 F1500
(curr: 5.556,7.461,-0.090)
(start: 6.350,16.669)
(end:  8.890,16.669)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X6.350Y16.669
G01 Z-0.090 F500
G01 X8.890Y16.669 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X6.350Y16.669
G01 Z-0.090 F500
G01 X8.890Y16.669 F1500
(curr: 8.890,16.669,-0.090)
(start: 8.890,16.669)
(end:  8.890,0.635)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.890Y0.635 F1500
(curr: 8.890,0.635,-0.090)
(start: 8.890,0.635)
(end:  6.350,0.635)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.350Y0.635 F1500
G00 Z1.000
M05
G04 P2000
G00 X0Y0
