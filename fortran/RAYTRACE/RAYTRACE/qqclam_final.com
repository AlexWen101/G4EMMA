$! This is file ERICH::PRV5:[STAN.SASP]QQCLAM_FINAL.COM
$! SAME AS FILE QQCLAM118.COM
$! MIT RAYTRACE FILE FOR FINAL DESIGN OF TRIUMF SASP SPECTROMETER
$! 90 DEGREE,Q-Q-CLAMSHELL DESIGN FOR SECOND ARM SPECTROMETER
$SET DEFAULT [STAN.SASP]
$ASSIGN SYS$INPUT FOR005
$ASSIGN QQCLAM118.OUT FOR$PRINT
$ASSIGN QQCLAM118.ABC FOR007
$RUN [STAN]MITDIFV8
$DECK
 QQCLAM118  90 DEGREE P=660 MEV/C
   25  500    0    0    0   25    0
594., 33., 5., 0., 1.
POLES
1.5, 1.5, 1.5
70., 16., 30., 10.
.846635, .031286, -.055724, -.0075445, 0.
30., -10., -10., 30.
.1122, 6.2671, -1.4982, 3.5882, -2.1209, 1.723
.1122, 6.2671, -1.4982, 3.5882, -2.1209, 1.723
POLES
1.5, 1.5, 1.5
16., 120.531, 38., 10.
-.850683, -.012548, .021891, -.00041891, .016523
30., -10., -10., 30.
.1122, 6.2671, -1.4982, 3.5882, -2.1209, 1.723
.1122, 6.2671, -1.4982, 3.5882, -2.1209, 1.723
SHRT
0., 0., 0., 0., -45., 0.
DIPOLE
3., 6., 3., 3.0, 4.
0., 0., 10.00, 220000., 1.6073
.0298, -75., 0.  			! CARD 4
1300., 0., 0., 0.
40., -15., 0., 0.
.2401, 1.8639, -.5572, .3904, 0., 0.
.2401, 1.8639, -.5572, .3904, 0., 0.
0., 0., 0., 0., 0., 0., 0.  		! CARD 9
0., 0., 0., 0.
-684.58, -.52751E6, -.20709E10, -.21053E14, 0., 0., 0.
0., 0., 0., 0., 0., 0., 0.
0., 0., 0., 1., 1., 1.   		! CARD 13
0., 0., 0., 0., 0., 0., 0., 0.   	! CARD 14
DIPOLE
3., 6., 3., 3.0, 4.
0., 0., 10., 220000., 1.6073
.0298, 0., -75.  			! CARD 4
1300., 0., 0., 0.
0., 0., -15., 40.
.2401, 1.8639, -.5572, .3904, 0., 0.
.2401, 1.8639, -.5572, .3904, 0., 0.
0., 0., 0., 0., 0., 0., 0.  		! CARD 9
0., 0., 0., 0.
0., 0., 0., 0., 0., 0., 0.
-654.38, -.49721E6, .17044E10, -.86203E13, .168105E17, 0., 0.
0., 0., 0., 1., 1., 1.  		! CARD 13
0., 0., 0., 0., 0., 0., 0., 0.  	! CARD 14
SHRT
0., 0., 0., 0., -45., 0.
DRIFT
157.236
SENTINEL
9999.999    .20772    ! FP TILT, DISP.MATCH
5., 1., 1., 85., 0.2, 43., 0.,  0.
$EOD
