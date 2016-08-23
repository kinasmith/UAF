(RS-274 Compatible G-code generated for UCCNC controller and Stepcraft 2 Mill)
(Created by in EagleCAD with the stepcraft.ulp 0.2.)
(File: "AAT1217_BREAKOUT.brd" created on: 8/19/16 3:06 PM)
(Use a V Bit)

G90 G94 G49
G21
G00 Z5
G00 X0Y0
S19500 M03
G04 P4000
(curr: 0.000,0.000,5.000)
(start: 8.096,5.397)
(end:  7.144,5.397)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X8.096Y5.397
G01 Z-0.070 F500
G01 X7.144Y5.397 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X8.096Y5.397
G01 Z-0.070 F500
G01 X7.144Y5.397 F1500
(curr: 7.144,5.397,-0.070)
(start: 7.144,5.397)
(end:  5.874,5.397)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X5.874Y5.397 F1500
(curr: 5.874,5.397,-0.070)
(start: 5.874,5.397)
(end:  5.874,5.080)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X5.874Y5.080 F1500
(curr: 5.874,5.080,-0.070)
(start: 5.874,5.080)
(end:  4.286,5.080)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y5.080 F1500
(curr: 4.286,5.080,-0.070)
(start: 4.286,5.080)
(end:  4.286,6.668)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y6.668 F1500
(curr: 4.286,6.668,-0.070)
(start: 4.286,6.668)
(end:  6.033,6.668)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.033Y6.668 F1500
(curr: 6.033,6.668,-0.070)
(start: 6.033,6.668)
(end:  6.033,6.350)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.033Y6.350 F1500
(curr: 6.033,6.350,-0.070)
(start: 6.033,6.350)
(end:  6.191,6.350)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.191Y6.350 F1500
(curr: 6.191,6.350,-0.070)
(start: 6.191,6.350)
(end:  7.144,6.350)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X7.144Y6.350 F1500
(curr: 7.144,6.350,-0.070)
(start: 7.144,6.350)
(end:  8.096,6.350)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.096Y6.350 F1500
(curr: 8.096,6.350,-0.070)
(start: 8.096,6.350)
(end:  8.731,6.350)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.731Y6.350 F1500
(curr: 8.731,6.350,-0.070)
(start: 8.731,6.350)
(end:  9.207,5.874)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X9.207Y5.874 F1500
(curr: 9.207,5.874,-0.070)
(start: 9.207,5.874)
(end:  9.207,5.397)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X9.207Y5.397 F1500
(curr: 9.207,5.397,-0.070)
(start: 9.207,5.397)
(end:  10.795,5.397)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X10.795Y5.397 F1500
(curr: 10.795,5.397,-0.070)
(start: 10.795,5.397)
(end:  11.271,5.874)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.271Y5.874 F1500
(curr: 11.271,5.874,-0.070)
(start: 11.271,5.874)
(end:  11.271,9.207)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.271Y9.207 F1500
(curr: 11.271,9.207,-0.070)
(start: 11.271,9.207)
(end:  9.049,9.207)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X9.049Y9.207 F1500
(curr: 9.049,9.207,-0.070)
(start: 9.049,9.207)
(end:  8.890,9.207)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.890Y9.207 F1500
(curr: 8.890,9.207,-0.070)
(start: 8.890,9.207)
(end:  8.096,8.414)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.096Y8.414 F1500
(curr: 8.096,8.414,-0.070)
(start: 8.096,8.414)
(end:  8.096,6.350)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.096Y6.350 F1500
(curr: 8.096,6.350,-0.070)
(start: 8.096,5.397)
(end:  8.096,3.175)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X8.096Y5.397
G01 Z-0.070 F500
G01 X8.096Y3.175 F1500
(curr: 8.096,3.175,-0.070)
(start: 8.096,3.175)
(end:  12.065,3.175)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X12.065Y3.175 F1500
(curr: 12.065,3.175,-0.070)
(start: 12.065,3.175)
(end:  12.065,4.921)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X12.065Y4.921 F1500
(curr: 12.065,4.921,-0.070)
(start: 12.065,4.921)
(end:  12.541,5.397)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X12.541Y5.397 F1500
(curr: 12.541,5.397,-0.070)
(start: 12.541,5.397)
(end:  13.176,5.397)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.176Y5.397 F1500
(curr: 13.176,5.397,-0.070)
(start: 13.176,5.397)
(end:  13.176,8.890)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.176Y8.890 F1500
(curr: 13.176,8.890,-0.070)
(start: 13.176,8.890)
(end:  14.288,8.890)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.288Y8.890 F1500
(curr: 14.288,8.890,-0.070)
(start: 14.288,8.890)
(end:  14.288,13.176)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.288Y13.176 F1500
(curr: 14.288,13.176,-0.070)
(start: 14.288,13.176)
(end:  9.049,13.176)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X9.049Y13.176 F1500
(curr: 9.049,13.176,-0.070)
(start: 9.049,13.176)
(end:  9.049,9.207)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X9.049Y9.207 F1500
(curr: 9.049,9.207,-0.070)
(start: 7.144,6.350)
(end:  7.144,8.255)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X7.144Y6.350
G01 Z-0.070 F500
G01 X7.144Y8.255 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X7.144Y6.350
G01 Z-0.070 F500
G01 X7.144Y8.255 F1500
(curr: 7.144,8.255,-0.070)
(start: 7.144,8.255)
(end:  6.191,8.255)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.191Y8.255 F1500
(curr: 6.191,8.255,-0.070)
(start: 6.191,8.255)
(end:  6.191,6.350)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.191Y6.350 F1500
(curr: 6.191,6.350,-0.070)
(start: 7.144,5.397)
(end:  7.144,3.651)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X7.144Y5.397
G01 Z-0.070 F500
G01 X7.144Y3.651 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X7.144Y5.397
G01 Z-0.070 F500
G01 X7.144Y3.651 F1500
(curr: 7.144,3.651,-0.070)
(start: 7.144,3.651)
(end:  4.286,3.651)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y3.651 F1500
(curr: 4.286,3.651,-0.070)
(start: 4.286,3.651)
(end:  4.286,4.604)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y4.604 F1500
(curr: 4.286,4.604,-0.070)
(start: 4.286,4.604)
(end:  4.286,5.080)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y5.080 F1500
(curr: 4.286,5.080,-0.070)
(start: 8.096,3.175)
(end:  7.620,2.699)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X8.096Y3.175
G01 Z-0.070 F500
G01 X7.620Y2.699 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X8.096Y3.175
G01 Z-0.070 F500
G01 X7.620Y2.699 F1500
(curr: 7.620,2.699,-0.070)
(start: 7.620,2.699)
(end:  6.033,2.699)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.033Y2.699 F1500
(curr: 6.033,2.699,-0.070)
(start: 6.033,2.699)
(end:  6.033,1.111)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.033Y1.111 F1500
(curr: 6.033,1.111,-0.070)
(start: 6.033,1.111)
(end:  2.699,1.111)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X2.699Y1.111 F1500
(curr: 2.699,1.111,-0.070)
(start: 2.699,1.111)
(end:  1.270,2.540)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X1.270Y2.540 F1500
(curr: 1.270,2.540,-0.070)
(start: 1.270,2.540)
(end:  1.270,4.604)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X1.270Y4.604 F1500
(curr: 1.270,4.604,-0.070)
(start: 1.270,4.604)
(end:  4.286,4.604)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y4.604 F1500
(curr: 4.286,4.604,-0.070)
(start: 1.270,4.604)
(end:  1.270,7.144)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X1.270Y4.604
G01 Z-0.070 F500
G01 X1.270Y7.144 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X1.270Y4.604
G01 Z-0.070 F500
G01 X1.270Y7.144 F1500
(curr: 1.270,7.144,-0.070)
(start: 1.270,7.144)
(end:  4.286,7.144)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y7.144 F1500
(curr: 4.286,7.144,-0.070)
(start: 4.286,7.144)
(end:  4.286,6.668)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y6.668 F1500
(curr: 4.286,6.668,-0.070)
(start: 8.096,5.397)
(end:  7.144,5.397)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X8.096Y5.397
G01 Z-0.090 F500
G01 X7.144Y5.397 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X8.096Y5.397
G01 Z-0.090 F500
G01 X7.144Y5.397 F1500
(curr: 7.144,5.397,-0.090)
(start: 7.144,5.397)
(end:  5.874,5.397)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X5.874Y5.397 F1500
(curr: 5.874,5.397,-0.090)
(start: 5.874,5.397)
(end:  5.874,5.080)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X5.874Y5.080 F1500
(curr: 5.874,5.080,-0.090)
(start: 5.874,5.080)
(end:  4.286,5.080)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y5.080 F1500
(curr: 4.286,5.080,-0.090)
(start: 4.286,5.080)
(end:  4.286,6.668)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y6.668 F1500
(curr: 4.286,6.668,-0.090)
(start: 4.286,6.668)
(end:  6.033,6.668)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.033Y6.668 F1500
(curr: 6.033,6.668,-0.090)
(start: 6.033,6.668)
(end:  6.033,6.350)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.033Y6.350 F1500
(curr: 6.033,6.350,-0.090)
(start: 6.033,6.350)
(end:  6.191,6.350)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.191Y6.350 F1500
(curr: 6.191,6.350,-0.090)
(start: 6.191,6.350)
(end:  7.144,6.350)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X7.144Y6.350 F1500
(curr: 7.144,6.350,-0.090)
(start: 7.144,6.350)
(end:  8.096,6.350)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.096Y6.350 F1500
(curr: 8.096,6.350,-0.090)
(start: 8.096,6.350)
(end:  8.731,6.350)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.731Y6.350 F1500
(curr: 8.731,6.350,-0.090)
(start: 8.731,6.350)
(end:  9.207,5.874)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X9.207Y5.874 F1500
(curr: 9.207,5.874,-0.090)
(start: 9.207,5.874)
(end:  9.207,5.397)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X9.207Y5.397 F1500
(curr: 9.207,5.397,-0.090)
(start: 9.207,5.397)
(end:  10.795,5.397)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X10.795Y5.397 F1500
(curr: 10.795,5.397,-0.090)
(start: 10.795,5.397)
(end:  11.271,5.874)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.271Y5.874 F1500
(curr: 11.271,5.874,-0.090)
(start: 11.271,5.874)
(end:  11.271,9.207)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.271Y9.207 F1500
(curr: 11.271,9.207,-0.090)
(start: 11.271,9.207)
(end:  9.049,9.207)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X9.049Y9.207 F1500
(curr: 9.049,9.207,-0.090)
(start: 9.049,9.207)
(end:  8.890,9.207)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.890Y9.207 F1500
(curr: 8.890,9.207,-0.090)
(start: 8.890,9.207)
(end:  8.096,8.414)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.096Y8.414 F1500
(curr: 8.096,8.414,-0.090)
(start: 8.096,8.414)
(end:  8.096,6.350)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X8.096Y6.350 F1500
(curr: 8.096,6.350,-0.090)
(start: 8.096,5.397)
(end:  8.096,3.175)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X8.096Y5.397
G01 Z-0.090 F500
G01 X8.096Y3.175 F1500
(curr: 8.096,3.175,-0.090)
(start: 8.096,3.175)
(end:  12.065,3.175)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X12.065Y3.175 F1500
(curr: 12.065,3.175,-0.090)
(start: 12.065,3.175)
(end:  12.065,4.921)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X12.065Y4.921 F1500
(curr: 12.065,4.921,-0.090)
(start: 12.065,4.921)
(end:  12.541,5.397)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X12.541Y5.397 F1500
(curr: 12.541,5.397,-0.090)
(start: 12.541,5.397)
(end:  13.176,5.397)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.176Y5.397 F1500
(curr: 13.176,5.397,-0.090)
(start: 13.176,5.397)
(end:  13.176,8.890)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.176Y8.890 F1500
(curr: 13.176,8.890,-0.090)
(start: 13.176,8.890)
(end:  14.288,8.890)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.288Y8.890 F1500
(curr: 14.288,8.890,-0.090)
(start: 14.288,8.890)
(end:  14.288,13.176)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.288Y13.176 F1500
(curr: 14.288,13.176,-0.090)
(start: 14.288,13.176)
(end:  9.049,13.176)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X9.049Y13.176 F1500
(curr: 9.049,13.176,-0.090)
(start: 9.049,13.176)
(end:  9.049,9.207)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X9.049Y9.207 F1500
(curr: 9.049,9.207,-0.090)
(start: 7.144,6.350)
(end:  7.144,8.255)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X7.144Y6.350
G01 Z-0.090 F500
G01 X7.144Y8.255 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X7.144Y6.350
G01 Z-0.090 F500
G01 X7.144Y8.255 F1500
(curr: 7.144,8.255,-0.090)
(start: 7.144,8.255)
(end:  6.191,8.255)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.191Y8.255 F1500
(curr: 6.191,8.255,-0.090)
(start: 6.191,8.255)
(end:  6.191,6.350)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.191Y6.350 F1500
(curr: 6.191,6.350,-0.090)
(start: 7.144,5.397)
(end:  7.144,3.651)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X7.144Y5.397
G01 Z-0.090 F500
G01 X7.144Y3.651 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X7.144Y5.397
G01 Z-0.090 F500
G01 X7.144Y3.651 F1500
(curr: 7.144,3.651,-0.090)
(start: 7.144,3.651)
(end:  4.286,3.651)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y3.651 F1500
(curr: 4.286,3.651,-0.090)
(start: 4.286,3.651)
(end:  4.286,4.604)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y4.604 F1500
(curr: 4.286,4.604,-0.090)
(start: 4.286,4.604)
(end:  4.286,5.080)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y5.080 F1500
(curr: 4.286,5.080,-0.090)
(start: 8.096,3.175)
(end:  7.620,2.699)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X8.096Y3.175
G01 Z-0.090 F500
G01 X7.620Y2.699 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X8.096Y3.175
G01 Z-0.090 F500
G01 X7.620Y2.699 F1500
(curr: 7.620,2.699,-0.090)
(start: 7.620,2.699)
(end:  6.033,2.699)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.033Y2.699 F1500
(curr: 6.033,2.699,-0.090)
(start: 6.033,2.699)
(end:  6.033,1.111)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X6.033Y1.111 F1500
(curr: 6.033,1.111,-0.090)
(start: 6.033,1.111)
(end:  2.699,1.111)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X2.699Y1.111 F1500
(curr: 2.699,1.111,-0.090)
(start: 2.699,1.111)
(end:  1.270,2.540)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X1.270Y2.540 F1500
(curr: 1.270,2.540,-0.090)
(start: 1.270,2.540)
(end:  1.270,4.604)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X1.270Y4.604 F1500
(curr: 1.270,4.604,-0.090)
(start: 1.270,4.604)
(end:  4.286,4.604)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y4.604 F1500
(curr: 4.286,4.604,-0.090)
(start: 1.270,4.604)
(end:  1.270,7.144)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X1.270Y4.604
G01 Z-0.090 F500
G01 X1.270Y7.144 F1500
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X1.270Y4.604
G01 Z-0.090 F500
G01 X1.270Y7.144 F1500
(curr: 1.270,7.144,-0.090)
(start: 1.270,7.144)
(end:  4.286,7.144)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y7.144 F1500
(curr: 4.286,7.144,-0.090)
(start: 4.286,7.144)
(end:  4.286,6.668)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X4.286Y6.668 F1500
G00 Z1.000
M05
G04 P2000
G00 X0Y0
