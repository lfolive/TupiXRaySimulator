dl=0.1;  //cm
dc=0.01; //cm
translate([dl/2,0,0]) color("gray",alpha=0.3)
    cube([dl,3,4],true);
dz1=0.01; //cm
for(i=[1:30])
    // 100 p.l./cm == 1 p.l./0.01cm
    translate([-dc/2,0.75,i*dz1]) color("DimGray",1.0)
        cube([dc,1,dz1/2],true);

dz2=0.02; //cm
for(i=[1:18])
    // 50 p.l./cm == 1 p.l./0.02cm
    translate([-dc/2,0.75,0.35+i*dz2]) color("DimGray",1.0)
        cube([dc,1,dz2/2],true);

dz3=0.04; //cm
for(i=[1:12])
    // 25 p.l./cm == 1 p.l./0.04cm
    translate([-dc/2,0.75,0.75+i*dz3]) color("DimGray",1.0)
        cube([dc,1,dz3/2],true);

dz4=0.08; //cm
for(i=[1:7])
    // 12.5 p.l./cm == 1 p.l./0.08cm
    translate([-dc/2,0.75,1.25+i*dz4]) color("DimGray",1.0)
        cube([dc,1,dz4/2],true);

