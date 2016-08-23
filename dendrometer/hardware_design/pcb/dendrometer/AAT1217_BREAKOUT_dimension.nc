(RS-274 Compatible G-code generated for UCCNC controller and Stepcraft 2 Mill)
(Created by in EagleCAD with the stepcraft.ulp 0.2.)
(File: "AAT1217_BREAKOUT.brd" created on: 8/19/16 3:06 PM)
(Use a 1/32inch Bit)

G90 G94 G49
G21
G00 Z5
G00 X0Y0
S19500 M03
G04 P4000
(curr: 2.651,3.334,-1.650)
(start: 0.000,0.000)
(end:  14.275,0.000)
(xy != p_xy && z <= 0: Retract, Rapid to Start, Plunge, Cut to End)
G00 Z1.000
G00 X0.000Y0.000
G01 Z-0.170 F30
G01 X14.275Y0.000 F360
(xy != p_xy && z > 0:Rapid to Start, Plunge, Cut to End)
G00 X0.000Y0.000
G01 Z-0.170 F30
G01 X14.275Y0.000 F360
(curr: 14.275,0.000,-0.170)
(start: 14.275,0.000)
(end:  14.275,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.275Y13.166 F360
(curr: 14.275,13.166,-0.170)
(start: 14.275,13.166)
(end:  0.000,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y13.166 F360
(curr: 0.000,13.166,-0.170)
(start: 0.000,13.166)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.170)
(start: 0.000,0.000)
(end:  14.275,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-0.340 F30
G01 X14.275Y0.000 F360
(curr: 14.275,0.000,-0.340)
(start: 14.275,0.000)
(end:  14.275,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.275Y13.166 F360
(curr: 14.275,13.166,-0.340)
(start: 14.275,13.166)
(end:  0.000,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y13.166 F360
(curr: 0.000,13.166,-0.340)
(start: 0.000,13.166)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.340)
(start: 0.000,0.000)
(end:  14.275,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-0.510 F30
G01 X14.275Y0.000 F360
(curr: 14.275,0.000,-0.510)
(start: 14.275,0.000)
(end:  14.275,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.275Y13.166 F360
(curr: 14.275,13.166,-0.510)
(start: 14.275,13.166)
(end:  0.000,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y13.166 F360
(curr: 0.000,13.166,-0.510)
(start: 0.000,13.166)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.510)
(start: 0.000,0.000)
(end:  14.275,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-0.680 F30
G01 X14.275Y0.000 F360
(curr: 14.275,0.000,-0.680)
(start: 14.275,0.000)
(end:  14.275,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.275Y13.166 F360
(curr: 14.275,13.166,-0.680)
(start: 14.275,13.166)
(end:  0.000,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y13.166 F360
(curr: 0.000,13.166,-0.680)
(start: 0.000,13.166)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.680)
(start: 0.000,0.000)
(end:  14.275,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-0.850 F30
G01 X14.275Y0.000 F360
(curr: 14.275,0.000,-0.850)
(start: 14.275,0.000)
(end:  14.275,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.275Y13.166 F360
(curr: 14.275,13.166,-0.850)
(start: 14.275,13.166)
(end:  0.000,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y13.166 F360
(curr: 0.000,13.166,-0.850)
(start: 0.000,13.166)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-0.850)
(start: 0.000,0.000)
(end:  14.275,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.020 F30
G01 X14.275Y0.000 F360
(curr: 14.275,0.000,-1.020)
(start: 14.275,0.000)
(end:  14.275,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.275Y13.166 F360
(curr: 14.275,13.166,-1.020)
(start: 14.275,13.166)
(end:  0.000,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y13.166 F360
(curr: 0.000,13.166,-1.020)
(start: 0.000,13.166)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-1.020)
(start: 0.000,0.000)
(end:  14.275,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.190 F30
G01 X14.275Y0.000 F360
(curr: 14.275,0.000,-1.190)
(start: 14.275,0.000)
(end:  14.275,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.275Y13.166 F360
(curr: 14.275,13.166,-1.190)
(start: 14.275,13.166)
(end:  0.000,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y13.166 F360
(curr: 0.000,13.166,-1.190)
(start: 0.000,13.166)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-1.190)
(start: 0.000,0.000)
(end:  14.275,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.360 F30
G01 X14.275Y0.000 F360
(curr: 14.275,0.000,-1.360)
(start: 14.275,0.000)
(end:  14.275,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.275Y13.166 F360
(curr: 14.275,13.166,-1.360)
(start: 14.275,13.166)
(end:  0.000,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y13.166 F360
(curr: 0.000,13.166,-1.360)
(start: 0.000,13.166)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-1.360)
(start: 0.000,0.000)
(end:  14.275,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.530 F30
G01 X14.275Y0.000 F360
(curr: 14.275,0.000,-1.530)
(start: 14.275,0.000)
(end:  14.275,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.275Y13.166 F360
(curr: 14.275,13.166,-1.530)
(start: 14.275,13.166)
(end:  0.000,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y13.166 F360
(curr: 0.000,13.166,-1.530)
(start: 0.000,13.166)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
(curr: 0.000,0.000,-1.530)
(start: 0.000,0.000)
(end:  14.275,0.000)
(xy == p_xy && z != cutHeight: Plunge, Cut)
G01 Z-1.650 F30
G01 X14.275Y0.000 F360
(curr: 14.275,0.000,-1.650)
(start: 14.275,0.000)
(end:  14.275,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X14.275Y13.166 F360
(curr: 14.275,13.166,-1.650)
(start: 14.275,13.166)
(end:  0.000,13.166)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y13.166 F360
(curr: 0.000,13.166,-1.650)
(start: 0.000,13.166)
(end:  0.000,0.000)
(xy == p_xy && z == cutHeight:Cutting Line)
G01 X0.000Y0.000 F360
G00 Z1.000
M05
G04 P4000
G00 X0Y0
