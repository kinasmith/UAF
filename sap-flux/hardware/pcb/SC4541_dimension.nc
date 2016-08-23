(RS-274 Compatible G-code generated for UCCNC controller and Stepcraft 2 Mill)
(Created by in EagleCAD with the stepcraft.ulp 0.2.)
(File: "SC4541.brd" created on: 7/26/16 5:08 PM)
(Use a 1/32inch Bit)

G90 G94 G49
G21
G00 Z5
G00 X0Y0
S19500 M03
G04 P4000
(curr: 2.651,15.240,-1.650)
(start: 0.000,0.000)
(end:  11.430,0.000)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X0.000Y0.000
G01 Z-0.170 F30
G01 X11.430Y0.000 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X0.000Y0.000
G01 Z-0.170 F30
G01 X11.430Y0.000 F360
(curr: 11.430,0.000,-0.170)
(start: 11.430,0.000)
(end:  11.430,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.430Y17.780 F360
(curr: 11.430,17.780,-0.170)
(start: 11.430,17.780)
(end:  0.000,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y17.780 F360
(curr: 0.000,17.780,-0.170)
(start: 0.000,17.780)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.170)
(start: 0.000,0.000)
(end:  11.430,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-0.340 F30
G01 X11.430Y0.000 F360
(curr: 11.430,0.000,-0.340)
(start: 11.430,0.000)
(end:  11.430,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.430Y17.780 F360
(curr: 11.430,17.780,-0.340)
(start: 11.430,17.780)
(end:  0.000,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y17.780 F360
(curr: 0.000,17.780,-0.340)
(start: 0.000,17.780)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.340)
(start: 0.000,0.000)
(end:  11.430,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-0.510 F30
G01 X11.430Y0.000 F360
(curr: 11.430,0.000,-0.510)
(start: 11.430,0.000)
(end:  11.430,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.430Y17.780 F360
(curr: 11.430,17.780,-0.510)
(start: 11.430,17.780)
(end:  0.000,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y17.780 F360
(curr: 0.000,17.780,-0.510)
(start: 0.000,17.780)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.510)
(start: 0.000,0.000)
(end:  11.430,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-0.680 F30
G01 X11.430Y0.000 F360
(curr: 11.430,0.000,-0.680)
(start: 11.430,0.000)
(end:  11.430,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.430Y17.780 F360
(curr: 11.430,17.780,-0.680)
(start: 11.430,17.780)
(end:  0.000,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y17.780 F360
(curr: 0.000,17.780,-0.680)
(start: 0.000,17.780)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.680)
(start: 0.000,0.000)
(end:  11.430,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-0.850 F30
G01 X11.430Y0.000 F360
(curr: 11.430,0.000,-0.850)
(start: 11.430,0.000)
(end:  11.430,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.430Y17.780 F360
(curr: 11.430,17.780,-0.850)
(start: 11.430,17.780)
(end:  0.000,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y17.780 F360
(curr: 0.000,17.780,-0.850)
(start: 0.000,17.780)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.850)
(start: 0.000,0.000)
(end:  11.430,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.020 F30
G01 X11.430Y0.000 F360
(curr: 11.430,0.000,-1.020)
(start: 11.430,0.000)
(end:  11.430,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.430Y17.780 F360
(curr: 11.430,17.780,-1.020)
(start: 11.430,17.780)
(end:  0.000,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y17.780 F360
(curr: 0.000,17.780,-1.020)
(start: 0.000,17.780)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-1.020)
(start: 0.000,0.000)
(end:  11.430,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.190 F30
G01 X11.430Y0.000 F360
(curr: 11.430,0.000,-1.190)
(start: 11.430,0.000)
(end:  11.430,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.430Y17.780 F360
(curr: 11.430,17.780,-1.190)
(start: 11.430,17.780)
(end:  0.000,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y17.780 F360
(curr: 0.000,17.780,-1.190)
(start: 0.000,17.780)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-1.190)
(start: 0.000,0.000)
(end:  11.430,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.360 F30
G01 X11.430Y0.000 F360
(curr: 11.430,0.000,-1.360)
(start: 11.430,0.000)
(end:  11.430,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.430Y17.780 F360
(curr: 11.430,17.780,-1.360)
(start: 11.430,17.780)
(end:  0.000,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y17.780 F360
(curr: 0.000,17.780,-1.360)
(start: 0.000,17.780)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-1.360)
(start: 0.000,0.000)
(end:  11.430,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.530 F30
G01 X11.430Y0.000 F360
(curr: 11.430,0.000,-1.530)
(start: 11.430,0.000)
(end:  11.430,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.430Y17.780 F360
(curr: 11.430,17.780,-1.530)
(start: 11.430,17.780)
(end:  0.000,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y17.780 F360
(curr: 0.000,17.780,-1.530)
(start: 0.000,17.780)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-1.530)
(start: 0.000,0.000)
(end:  11.430,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.650 F30
G01 X11.430Y0.000 F360
(curr: 11.430,0.000,-1.650)
(start: 11.430,0.000)
(end:  11.430,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X11.430Y17.780 F360
(curr: 11.430,17.780,-1.650)
(start: 11.430,17.780)
(end:  0.000,17.780)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y17.780 F360
(curr: 0.000,17.780,-1.650)
(start: 0.000,17.780)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
G00 Z1.000
M05
G04 P4000
G00 X0Y0
