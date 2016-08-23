(RS-274 Compatible G-code generated for UCCNC controller and Stepcraft 2 Mill)
(Created by in EagleCAD with the stepcraft.ulp 0.2.)
(File: "MAX31855T_BREAKOUT.brd" created on: 8/22/16 1:04 PM)
(Use a 1/32inch Bit)

G90 G94 G49
G21
G00 Z5
G00 X0Y0
S19500 M03
G04 P4000
(curr: 0.000,0.000,5.000)
(start: 7.620,16.351)
(end:  7.620,12.700)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X7.620Y16.351
G01 Z-0.090 F30
G01 X7.620Y12.700 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X7.620Y16.351
G01 Z-0.090 F30
G01 X7.620Y12.700 F360
(curr: 7.620,12.700,-0.090)
(start: 7.620,12.700)
(end:  8.890,12.700)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.890Y12.700 F360
(curr: 8.890,12.700,-0.090)
(start: 8.890,12.700)
(end:  10.160,12.700)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X10.160Y12.700 F360
(curr: 10.160,12.700,-0.090)
(start: 10.160,12.700)
(end:  12.065,12.700)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X12.065Y12.700 F360
(curr: 12.065,12.700,-0.090)
(start: 12.065,12.700)
(end:  12.541,12.224)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X12.541Y12.224 F360
(curr: 12.541,12.224,-0.090)
(start: 12.541,12.224)
(end:  14.605,12.224)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.605Y12.224 F360
(curr: 14.605,12.224,-0.090)
(start: 14.605,12.224)
(end:  15.399,11.430)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X15.399Y11.430 F360
(curr: 15.399,11.430,-0.090)
(start: 15.399,11.430)
(end:  15.399,9.366)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X15.399Y9.366 F360
(curr: 15.399,9.366,-0.090)
(start: 15.399,9.366)
(end:  13.176,9.366)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.176Y9.366 F360
(curr: 13.176,9.366,-0.090)
(start: 13.176,9.366)
(end:  13.176,6.033)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.176Y6.033 F360
(curr: 13.176,6.033,-0.090)
(start: 13.176,6.033)
(end:  11.430,4.286)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.430Y4.286 F360
(curr: 11.430,4.286,-0.090)
(start: 11.430,4.286)
(end:  11.430,-0.476)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.430Y-0.476 F360
(curr: 11.430,-0.476,-0.090)
(start: 8.890,-0.318)
(end:  8.890,4.604)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X8.890Y-0.318
G01 Z-0.090 F30
G01 X8.890Y4.604 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X8.890Y-0.318
G01 Z-0.090 F30
G01 X8.890Y4.604 F360
(curr: 8.890,4.604,-0.090)
(start: 11.589,7.461)
(end:  11.589,10.795)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X11.589Y7.461
G01 Z-0.090 F30
G01 X11.589Y10.795 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X11.589Y7.461
G01 Z-0.090 F30
G01 X11.589Y10.795 F360
(curr: 11.589,10.795,-0.090)
(start: 11.589,10.795)
(end:  11.271,11.113)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.271Y11.113 F360
(curr: 11.271,11.113,-0.090)
(start: 11.271,11.113)
(end:  10.160,11.113)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X10.160Y11.113 F360
(curr: 10.160,11.113,-0.090)
(start: 10.160,11.113)
(end:  8.890,11.113)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.890Y11.113 F360
(curr: 8.890,11.113,-0.090)
(start: 8.890,11.113)
(end:  7.303,11.113)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X7.303Y11.113 F360
(curr: 7.303,11.113,-0.090)
(start: 7.303,11.113)
(end:  6.033,12.383)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.033Y12.383 F360
(curr: 6.033,12.383,-0.090)
(start: 6.033,12.383)
(end:  6.033,13.335)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.033Y13.335 F360
(curr: 6.033,13.335,-0.090)
(start: 6.033,13.335)
(end:  3.651,13.335)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.651Y13.335 F360
(curr: 3.651,13.335,-0.090)
(start: 3.651,13.335)
(end:  3.651,16.192)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.651Y16.192 F360
(curr: 3.651,16.192,-0.090)
(start: 3.651,16.192)
(end:  4.286,16.828)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y16.828 F360
(curr: 4.286,16.828,-0.090)
(start: 4.286,16.828)
(end:  5.080,16.828)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X5.080Y16.828 F360
(curr: 5.080,16.828,-0.090)
(start: 5.080,16.828)
(end:  7.144,16.828)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X7.144Y16.828 F360
(curr: 7.144,16.828,-0.090)
(start: 7.144,16.828)
(end:  7.620,16.351)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X7.620Y16.351 F360
(curr: 7.620,16.351,-0.090)
(start: 7.620,10.001)
(end:  6.191,10.001)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X7.620Y10.001
G01 Z-0.090 F30
G01 X6.191Y10.001 F360
(curr: 6.191,10.001,-0.090)
(start: 6.191,10.001)
(end:  6.191,8.255)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.191Y8.255 F360
(curr: 6.191,8.255,-0.090)
(start: 6.191,8.255)
(end:  3.810,5.874)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y5.874 F360
(curr: 3.810,5.874,-0.090)
(start: 3.810,5.874)
(end:  3.810,-0.159)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X3.810Y-0.159 F360
(curr: 3.810,-0.159,-0.090)
(start: 6.350,-0.318)
(end:  6.350,5.874)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X6.350Y-0.318
G01 Z-0.090 F30
G01 X6.350Y5.874 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X6.350Y-0.318
G01 Z-0.090 F30
G01 X6.350Y5.874 F360
(curr: 6.350,5.874,-0.090)
(start: 6.350,5.874)
(end:  7.620,7.144)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X7.620Y7.144 F360
(curr: 7.620,7.144,-0.090)
(start: 7.620,7.144)
(end:  7.620,7.303)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X7.620Y7.303 F360
(curr: 7.620,7.303,-0.090)
(start: 7.620,7.303)
(end:  7.620,10.001)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X7.620Y10.001 F360
(curr: 7.620,10.001,-0.090)
(start: 7.620,7.303)
(end:  8.890,7.303)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X7.620Y7.303
G01 Z-0.090 F30
G01 X8.890Y7.303 F360
(curr: 8.890,7.303,-0.090)
(start: 8.890,7.303)
(end:  8.890,11.113)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.890Y11.113 F360
(curr: 8.890,11.113,-0.090)
(start: 8.890,12.700)
(end:  8.890,17.145)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X8.890Y12.700
G01 Z-0.090 F30
G01 X8.890Y17.145 F360
(curr: 8.890,17.145,-0.090)
(start: 8.890,17.145)
(end:  9.207,17.463)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X9.207Y17.463 F360
(curr: 9.207,17.463,-0.090)
(start: 9.207,17.463)
(end:  9.207,23.336)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X9.207Y23.336 F360
(curr: 9.207,23.336,-0.090)
(start: 5.080,23.336)
(end:  5.080,16.828)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X5.080Y23.336
G01 Z-0.090 F30
G01 X5.080Y16.828 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X5.080Y23.336
G01 Z-0.090 F30
G01 X5.080Y16.828 F360
(curr: 5.080,16.828,-0.090)
(start: 12.700,23.336)
(end:  12.700,17.463)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X12.700Y23.336
G01 Z-0.090 F30
G01 X12.700Y17.463 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X12.700Y23.336
G01 Z-0.090 F30
G01 X12.700Y17.463 F360
(curr: 12.700,17.463,-0.090)
(start: 12.700,17.463)
(end:  12.224,16.986)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X12.224Y16.986 F360
(curr: 12.224,16.986,-0.090)
(start: 12.224,16.986)
(end:  10.795,16.986)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X10.795Y16.986 F360
(curr: 10.795,16.986,-0.090)
(start: 10.795,16.986)
(end:  10.160,16.351)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X10.160Y16.351 F360
(curr: 10.160,16.351,-0.090)
(start: 10.160,16.351)
(end:  10.160,12.700)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X10.160Y12.700 F360
(curr: 10.160,12.700,-0.090)
(start: 3.651,13.335)
(end:  3.651,11.906)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X3.651Y13.335
G01 Z-0.090 F30
G01 X3.651Y11.906 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X3.651Y13.335
G01 Z-0.090 F30
G01 X3.651Y11.906 F360
(curr: 3.651,11.906,-0.090)
(start: 3.651,11.906)
(end:  1.270,9.525)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X1.270Y9.525 F360
(curr: 1.270,9.525,-0.090)
(start: 1.270,9.525)
(end:  0.000,9.525)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y9.525 F360
(curr: 0.000,9.525,-0.090)
(start: 10.160,11.113)
(end:  10.160,7.303)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X10.160Y11.113
G01 Z-0.090 F30
G01 X10.160Y7.303 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X10.160Y11.113
G01 Z-0.090 F30
G01 X10.160Y7.303 F360
(curr: 10.160,7.303,-0.090)
(start: 11.589,7.461)
(end:  11.271,7.144)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X11.589Y7.461
G01 Z-0.090 F30
G01 X11.271Y7.144 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X11.589Y7.461
G01 Z-0.090 F30
G01 X11.271Y7.144 F360
(curr: 11.271,7.144,-0.090)
(start: 11.271,7.144)
(end:  10.160,7.144)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X10.160Y7.144 F360
(curr: 10.160,7.144,-0.090)
(start: 10.160,7.144)
(end:  10.160,5.874)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X10.160Y5.874 F360
(curr: 10.160,5.874,-0.090)
(start: 10.160,5.874)
(end:  8.890,4.604)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.890Y4.604 F360
G00 Z1.000
M05
G04 P2000
G00 X0Y0
