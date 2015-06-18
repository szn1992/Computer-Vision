#!/usr/bin/bash
./kmeans 18 face01.ppm k1.txt > l1.pgm
./autocolor l1.pgm > o1.pgm
./kmeans 14 face02.ppm k2.txt > l2.pgm
./autocolor l2.pgm > o2.pgm
./kmeans 12 face03.ppm k3.txt > l3.pgm
./autocolor l3.pgm > o3.pgm
./kmeans 9 face04.ppm k4.txt > l4.pgm
./autocolor l4.pgm > o4.pgm
./kmeans 5 face05.ppm k5.txt > l5.pgm
./autocolor l5.pgm > o5.pgm
echo "A"
./kmeans 8 face06.ppm k6.txt > l6.pgm
./autocolor l6.pgm > o6.pgm
./kmeans 7 face07.ppm k7.txt > l7.pgm
./autocolor l7.pgm > o7.pgm
./kmeans 7 face08.ppm k8.txt > l8.pgm
./autocolor l8.pgm > o8.pgm
./kmeans 11 face09.ppm k9.txt > l9.pgm
./autocolor l9.pgm > o9.pgm
./kmeans 13 face10.ppm k10.txt > l10.pgm
./autocolor l10.pgm > o10.pgm
echo "B"
./kmeans 11 face11.ppm k11.txt > l11.pgm
./autocolor l11.pgm > o11.pgm
./kmeans 8 face12.ppm k12.txt > l12.pgm
./autocolor l12.pgm > o12.pgm
./kmeans 10 face13.ppm k13.txt > l13.pgm
./autocolor l13.pgm > o13.pgm
./kmeans 12 face14.ppm k14.txt > l14.pgm
./autocolor l14.pgm > o14.pgm
./kmeans 10 face15.ppm k15.txt > l15.pgm
./autocolor l15.pgm > o15.pgm
echo "C"
./kmeans 18 face16.ppm k16.txt > l16.pgm
./autocolor l16.pgm > o16.pgm
./kmeans 13 face17.ppm k17.txt > l17.pgm
./autocolor l17.pgm > o17.pgm
./kmeans 9 face18.ppm k18.txt > l18.pgm
./autocolor l18.pgm > o18.pgm
./kmeans 13 face19.ppm k19.txt > l19.pgm
./autocolor l19.pgm > o19.pgm
./kmeans 9 face20.ppm k20.txt > l20.pgm
./autocolor l20.pgm > o20.pgm
echo "D"
./kmeans 12 face21.ppm k21.txt > l21.pgm
./autocolor l21.pgm > o21.pgm
./kmeans 11 face22.ppm k22.txt > l22.pgm
./autocolor l22.pgm > o22.pgm
./kmeans 7 face23.ppm k23.txt > l23.pgm
./autocolor l23.pgm > o23.pgm
./kmeans 8 face24.ppm k24.txt > l24.pgm
./autocolor l24.pgm > o24.pgm
./kmeans 9 face25.ppm k25.txt > l25.pgm
./autocolor l25.pgm > o25.pgm
echo "E"
./kmeans 7 face26.ppm k26.txt > l26.pgm
./autocolor l26.pgm > o26.pgm
./kmeans 7 face27.ppm k27.txt > l27.pgm
./autocolor l27.pgm > o27.pgm
./kmeans 13 face28.ppm k28.txt > l28.pgm
./autocolor l28.pgm > o28.pgm
./kmeans 13 face29.ppm k29.txt > l29.pgm
./autocolor l29.pgm > o29.pgm
./kmeans 10 face30.ppm k30.txt > l30.pgm
./autocolor l30.pgm > o30.pgm
echo "F"
