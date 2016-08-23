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
(curr: 2.651,20.320,-1.650)
(start: 0.000,0.000)
(end:  13.640,0.000)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X0.000Y0.000
G01 Z-0.170 F30
G01 X13.640Y0.000 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X0.000Y0.000
G01 Z-0.170 F30
G01 X13.640Y0.000 F360
(curr: 13.640,0.000,-0.170)
(start: 13.640,0.000)
(end:  13.640,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.640Y22.850 F360
(curr: 13.640,22.850,-0.170)
(start: 13.640,22.850)
(end:  0.000,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y22.850 F360
(curr: 0.000,22.850,-0.170)
(start: 0.000,22.850)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.170)
(start: 0.000,0.000)
(end:  13.640,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-0.340 F30
G01 X13.640Y0.000 F360
(curr: 13.640,0.000,-0.340)
(start: 13.640,0.000)
(end:  13.640,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.640Y22.850 F360
(curr: 13.640,22.850,-0.340)
(start: 13.640,22.850)
(end:  0.000,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y22.850 F360
(curr: 0.000,22.850,-0.340)
(start: 0.000,22.850)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.340)
(start: 0.000,0.000)
(end:  13.640,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-0.510 F30
G01 X13.640Y0.000 F360
(curr: 13.640,0.000,-0.510)
(start: 13.640,0.000)
(end:  13.640,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.640Y22.850 F360
(curr: 13.640,22.850,-0.510)
(start: 13.640,22.850)
(end:  0.000,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y22.850 F360
(curr: 0.000,22.850,-0.510)
(start: 0.000,22.850)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.510)
(start: 0.000,0.000)
(end:  13.640,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-0.680 F30
G01 X13.640Y0.000 F360
(curr: 13.640,0.000,-0.680)
(start: 13.640,0.000)
(end:  13.640,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.640Y22.850 F360
(curr: 13.640,22.850,-0.680)
(start: 13.640,22.850)
(end:  0.000,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y22.850 F360
(curr: 0.000,22.850,-0.680)
(start: 0.000,22.850)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.680)
(start: 0.000,0.000)
(end:  13.640,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-0.850 F30
G01 X13.640Y0.000 F360
(curr: 13.640,0.000,-0.850)
(start: 13.640,0.000)
(end:  13.640,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.640Y22.850 F360
(curr: 13.640,22.850,-0.850)
(start: 13.640,22.850)
(end:  0.000,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y22.850 F360
(curr: 0.000,22.850,-0.850)
(start: 0.000,22.850)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.850)
(start: 0.000,0.000)
(end:  13.640,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.020 F30
G01 X13.640Y0.000 F360
(curr: 13.640,0.000,-1.020)
(start: 13.640,0.000)
(end:  13.640,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.640Y22.850 F360
(curr: 13.640,22.850,-1.020)
(start: 13.640,22.850)
(end:  0.000,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y22.850 F360
(curr: 0.000,22.850,-1.020)
(start: 0.000,22.850)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-1.020)
(start: 0.000,0.000)
(end:  13.640,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.190 F30
G01 X13.640Y0.000 F360
(curr: 13.640,0.000,-1.190)
(start: 13.640,0.000)
(end:  13.640,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.640Y22.850 F360
(curr: 13.640,22.850,-1.190)
(start: 13.640,22.850)
(end:  0.000,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y22.850 F360
(curr: 0.000,22.850,-1.190)
(start: 0.000,22.850)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-1.190)
(start: 0.000,0.000)
(end:  13.640,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.360 F30
G01 X13.640Y0.000 F360
(curr: 13.640,0.000,-1.360)
(start: 13.640,0.000)
(end:  13.640,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.640Y22.850 F360
(curr: 13.640,22.850,-1.360)
(start: 13.640,22.850)
(end:  0.000,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y22.850 F360
(curr: 0.000,22.850,-1.360)
(start: 0.000,22.850)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-1.360)
(start: 0.000,0.000)
(end:  13.640,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.530 F30
G01 X13.640Y0.000 F360
(curr: 13.640,0.000,-1.530)
(start: 13.640,0.000)
(end:  13.640,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.640Y22.850 F360
(curr: 13.640,22.850,-1.530)
(start: 13.640,22.850)
(end:  0.000,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y22.850 F360
(curr: 0.000,22.850,-1.530)
(start: 0.000,22.850)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-1.530)
(start: 0.000,0.000)
(end:  13.640,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.650 F30
G01 X13.640Y0.000 F360
(curr: 13.640,0.000,-1.650)
(start: 13.640,0.000)
(end:  13.640,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X13.640Y22.850 F360
(curr: 13.640,22.850,-1.650)
(start: 13.640,22.850)
(end:  0.000,22.850)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y22.850 F360
(curr: 0.000,22.850,-1.650)
(start: 0.000,22.850)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
G00 Z1.000
M05
G04 P4000
G00 X0Y0
