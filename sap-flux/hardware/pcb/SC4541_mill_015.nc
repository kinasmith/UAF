(RS-274 Compatible G-code generated for UCCNC controller and Stepcraft 2 Mill)
(Created by in EagleCAD with the stepcraft.ulp 0.2.)
(File: "SC4541.brd" created on: 7/26/16 5:08 PM)
(Use a 1/64inch Bit)

G90 G94 G49
G21
G00 Z5
G00 X0Y0
S19500 M03
G04 P4000
(curr: 0.000,0.000,5.000)
(start: 0.000,7.620)
(end:  2.064,7.620)
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X0.000Y7.620
G01 Z-0.050 F12
G01 X2.064Y7.620 F144
(curr: 2.064,7.620,-0.050)
(start: 2.064,7.620)
(end:  3.016,7.620)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.016Y7.620 F144
(curr: 3.016,7.620,-0.050)
(start: 3.016,7.620)
(end:  12.224,7.620)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X12.224Y7.620 F144
(curr: 12.224,7.620,-0.050)
(start: 2.064,7.620)
(end:  2.064,5.080)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X2.064Y7.620
G01 Z-0.050 F12
G01 X2.064Y5.080 F144
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X2.064Y7.620
G01 Z-0.050 F12
G01 X2.064Y5.080 F144
(curr: 2.064,5.080,-0.050)
(start: 2.064,5.080)
(end:  3.810,3.334)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y3.334 F144
(curr: 3.810,3.334,-0.050)
(start: 3.810,3.334)
(end:  3.810,-0.318)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y-0.318 F144
(curr: 3.810,-0.318,-0.050)
(start: 3.016,7.620)
(end:  3.016,5.715)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.016Y7.620
G01 Z-0.050 F12
G01 X3.016Y5.715 F144
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.016Y7.620
G01 Z-0.050 F12
G01 X3.016Y5.715 F144
(curr: 3.016,5.715,-0.050)
(start: 3.016,5.715)
(end:  3.810,4.921)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y4.921 F144
(curr: 3.810,4.921,-0.050)
(start: 3.810,4.921)
(end:  5.715,4.921)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X5.715Y4.921 F144
(curr: 5.715,4.921,-0.050)
(start: 5.715,4.921)
(end:  6.350,4.286)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.350Y4.286 F144
(curr: 6.350,4.286,-0.050)
(start: 6.350,4.286)
(end:  6.350,-0.318)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.350Y-0.318 F144
(curr: 6.350,-0.318,-0.050)
(start: 3.016,7.620)
(end:  3.016,10.160)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.016Y7.620
G01 Z-0.050 F12
G01 X3.016Y10.160 F144
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.016Y7.620
G01 Z-0.050 F12
G01 X3.016Y10.160 F144
(curr: 3.016,10.160,-0.050)
(start: 3.016,10.160)
(end:  5.715,10.160)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X5.715Y10.160 F144
(curr: 5.715,10.160,-0.050)
(start: 5.715,10.160)
(end:  12.224,10.160)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X12.224Y10.160 F144
(curr: 12.224,10.160,-0.050)
(start: 2.064,7.620)
(end:  2.064,12.700)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X2.064Y7.620
G01 Z-0.050 F12
G01 X2.064Y12.700 F144
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X2.064Y7.620
G01 Z-0.050 F12
G01 X2.064Y12.700 F144
(curr: 2.064,12.700,-0.050)
(start: 2.064,12.700)
(end:  3.810,14.446)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y14.446 F144
(curr: 3.810,14.446,-0.050)
(start: 3.810,14.446)
(end:  3.810,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y17.780 F144
(curr: 3.810,17.780,-0.050)
(start: 5.715,10.160)
(end:  5.715,13.176)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X5.715Y10.160
G01 Z-0.050 F12
G01 X5.715Y13.176 F144
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X5.715Y10.160
G01 Z-0.050 F12
G01 X5.715Y13.176 F144
(curr: 5.715,13.176,-0.050)
(start: 5.715,13.176)
(end:  6.350,13.811)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.350Y13.811 F144
(curr: 6.350,13.811,-0.050)
(start: 6.350,13.811)
(end:  6.350,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.350Y17.780 F144
(curr: 6.350,17.780,-0.050)
(start: 0.000,7.620)
(end:  2.064,7.620)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X0.000Y7.620
G01 Z-0.050 F12
G01 X2.064Y7.620 F144
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X0.000Y7.620
G01 Z-0.050 F12
G01 X2.064Y7.620 F144
(curr: 2.064,7.620,-0.050)
(start: 2.064,7.620)
(end:  3.016,7.620)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.016Y7.620 F144
(curr: 3.016,7.620,-0.050)
(start: 3.016,7.620)
(end:  12.224,7.620)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X12.224Y7.620 F144
(curr: 12.224,7.620,-0.050)
(start: 2.064,7.620)
(end:  2.064,5.080)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X2.064Y7.620
G01 Z-0.050 F12
G01 X2.064Y5.080 F144
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X2.064Y7.620
G01 Z-0.050 F12
G01 X2.064Y5.080 F144
(curr: 2.064,5.080,-0.050)
(start: 2.064,5.080)
(end:  3.810,3.334)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y3.334 F144
(curr: 3.810,3.334,-0.050)
(start: 3.810,3.334)
(end:  3.810,-0.318)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y-0.318 F144
(curr: 3.810,-0.318,-0.050)
(start: 3.016,7.620)
(end:  3.016,5.715)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.016Y7.620
G01 Z-0.050 F12
G01 X3.016Y5.715 F144
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.016Y7.620
G01 Z-0.050 F12
G01 X3.016Y5.715 F144
(curr: 3.016,5.715,-0.050)
(start: 3.016,5.715)
(end:  3.810,4.921)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y4.921 F144
(curr: 3.810,4.921,-0.050)
(start: 3.810,4.921)
(end:  5.715,4.921)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X5.715Y4.921 F144
(curr: 5.715,4.921,-0.050)
(start: 5.715,4.921)
(end:  6.350,4.286)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.350Y4.286 F144
(curr: 6.350,4.286,-0.050)
(start: 6.350,4.286)
(end:  6.350,-0.318)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.350Y-0.318 F144
(curr: 6.350,-0.318,-0.050)
(start: 3.016,7.620)
(end:  3.016,10.160)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.016Y7.620
G01 Z-0.050 F12
G01 X3.016Y10.160 F144
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.016Y7.620
G01 Z-0.050 F12
G01 X3.016Y10.160 F144
(curr: 3.016,10.160,-0.050)
(start: 3.016,10.160)
(end:  5.715,10.160)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X5.715Y10.160 F144
(curr: 5.715,10.160,-0.050)
(start: 5.715,10.160)
(end:  12.224,10.160)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X12.224Y10.160 F144
(curr: 12.224,10.160,-0.050)
(start: 2.064,7.620)
(end:  2.064,12.700)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X2.064Y7.620
G01 Z-0.050 F12
G01 X2.064Y12.700 F144
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X2.064Y7.620
G01 Z-0.050 F12
G01 X2.064Y12.700 F144
(curr: 2.064,12.700,-0.050)
(start: 2.064,12.700)
(end:  3.810,14.446)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y14.446 F144
(curr: 3.810,14.446,-0.050)
(start: 3.810,14.446)
(end:  3.810,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y17.780 F144
(curr: 3.810,17.780,-0.050)
(start: 5.715,10.160)
(end:  5.715,13.176)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X5.715Y10.160
G01 Z-0.050 F12
G01 X5.715Y13.176 F144
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X5.715Y10.160
G01 Z-0.050 F12
G01 X5.715Y13.176 F144
(curr: 5.715,13.176,-0.050)
(start: 5.715,13.176)
(end:  6.350,13.811)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.350Y13.811 F144
(curr: 6.350,13.811,-0.050)
(start: 6.350,13.811)
(end:  6.350,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.350Y17.780 F144
G00 Z1.000
M05
G04 P2000
G00 X0Y0
