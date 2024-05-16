dl=0.1;  //cm
dc=0.01; //cm espessura da camada de chumbo

// base de acrílico
//translate([dl/2,0,0]) color("gray",alpha=0.3) cube([dl,3,4],true);

// pares de linhas paralelas
dz1=0.01; //cm
for(i=[1:30])
    // 100 p.l./cm == 1 p.l./0.01cm
    translate([-dc/2,0.75,i*dz1-1]) color("DimGray",1.0)
        cube([dc,1,dz1/2],true);

dz2=0.02; //cm
for(i=[1:18])
    // 50 p.l./cm == 1 p.l./0.02cm
    translate([-dc/2,0.75,0.4+i*dz2-1]) color("DimGray",1.0)
        cube([dc,1,dz2/2],true);

dz3=0.04; //cm
for(i=[1:12])
    // 25 p.l./cm == 1 p.l./0.04cm
    translate([-dc/2,0.75,0.85+i*dz3-1]) color("DimGray",1.0)
        cube([dc,1,dz3/2],true);

dz4=0.08; //cm
for(i=[1:7])
    // 12.5 p.l./cm == 1 p.l./0.08cm
    translate([-dc/2,0.75,1.4+i*dz4-1]) color("DimGray",1.0)
        cube([dc,1,dz4/2],true);

z=1.5;
for(i=[-6.5:2:6.5]) {
    vertice = [
    [0,-i*0.1,z],      // 0
    [0,-(i+1)*0.1,z],
    [0,0.1*(-(i+1)*0.1),-z],
    [0,0.1*(-i*0.1),-z],      // 3
    [-dc,0.1*(-i*0.1),-z],    // 4
    [-dc,0.1*(-(i+1)*0.1),-z],
    [-dc,(-(i+1)*0.1),z],
    [-dc,(-i*0.1),z]     // 7
    ];

    face = [
    [0,1,2,3],
    [4,5,6,7],
    [5,4,3,2],
    [7,6,1,0],
    [6,5,2,1],
    [0,3,4,7]
    ];

    translate([0,-0.75,0]) color("DimGray",1.0) polyhedron(vertice,face);
}
font = "Liberation Sans";
{
    text1 = "5.0 LP/mm";
    translate([0,-0.4,1.6])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str(text1), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}
{
    text2 = "50.0 LP/mm";
    translate([0,-0.35,-1.7])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str(text2), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}
 