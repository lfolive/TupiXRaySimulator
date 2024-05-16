font = "Liberation Sans";

dl=0.1;  //cm
dc=0.01; //cm espessura da camada de chumbo

dz=0.1;  // largura da linha
z=1.5;

module Base() {
    // base de acrílico
    translate([dl/2,0,0]) color("gray",alpha=0.15) cube([dl,3,4],true);
}

// pares de linhas paralelas
module LP1_25() {
    dz4=0.08; //cm
    for(i=[1:7]) {
        // 12.5 p.l./cm == 1 p.l./0.08cm
        translate([-dc/2,0.975,1.9+i*dz4-1]) color("DimGray",1.0)
            cube([dc,0.55,dz4/2],true);
    }
    for(i=[1:6]) {
        translate([-dc/2,0.71-i*dz4,1.22]) color("DimGray",1.0)
            cube([dc,dz4/2,0.53],true);
    }
}

module LP1_25t() {
    text2d = "1.25 LP/mm";
    translate([0,1.08,0.8])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str(text2d), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}

module LP2_5() {
    dz3=0.04; //cm
    for(i=[1:12]) {
        // 25 p.l./cm == 1 p.l./0.04cm
        translate([-dc/2,1.0,1.0+i*dz3-1]) color("DimGray",1.0)
            cube([dc,0.5,dz3/2],true);
        translate([-dc/2,0.75-i*dz3,0.26]) color("DimGray",1.0)
            cube([dc,dz3/2,0.47],true);
    }
}

module LP2_5t() {
    text2c = "2.5 LP/mm";
    translate([0,1.05,-0.15])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str(text2c), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}

module LP5() {
    dz2=0.02; //cm
    for(i=[1:18]) {
        // 50 p.l./cm == 1 p.l./0.02cm
        translate([-dc/2,1.0,0.2+i*dz2-1]) color("DimGray",1.0)
            cube([dc,0.5,dz2/2],true);
    }
    for(i=[1:23]) {
        translate([-dc/2,1.75-i*dz2-1,-0.61]) color("DimGray",1.0)
            cube([dc,dz2/2,0.355],true);
    }
}

module LP5t() {
    text2b = "5 LP/mm";
    translate([0,1.0,-0.95])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str(text2b), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}

module LP10() {
    dz1=0.01; //cm
    for(i=[1:30]) {
    // 100 p.l./cm == 10 p.l./0.1cm
        translate([-dc/2,1.00,-0.5+i*dz1-1]) color("DimGray",1.0)
            cube([dc,0.5,dz1/2],true);
    }
    for(i=[1:45]) {
        translate([-dc/2,1.00-i*dz1-0.25,-1.345]) color("DimGray",1.0)
            cube([dc,dz1/2,0.3],true);
    }
}

module LP10t() {
    text2a = "10 LP/mm";
    translate([0,1.0,-1.65])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str(text2a), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}

module Radial() { // z = 1.5
    for(i=[-0.4:0.2:0.4]) 
    {
        vertice = [
        [0,i-dz/2,z],      // 0
        [0,i+dz/2,z],
        [0,0.1*(i+dz/2),-z],
        [0,0.1*(i-dz/2),-z],      // 3
        [-dc,0.1*(i-dz/2),-z],    // 4
        [-dc,0.1*(i+dz/2),-z],
        [-dc,i+dz/2,z],
        [-dc,i-dz/2,z]     // 7
        ];

        face = [
        [0,1,2,3],
        [4,5,6,7],
        [5,4,3,2],
        [7,6,1,0],
        [6,5,2,1],
        [0,3,4,7]
        ];

        translate([0,-0.5,z-(5.5-1)*(2*z/9)]) 
        color("DimGray",1.0) polyhedron(vertice,face);
    }
}
// 10mm/(h+a)  =   1mm/a
// a=1; 5LP/1mm   5.0
// a=2; 5LP/2mm   2.5
// a=3; 5LP/3mm   1.667*
// a=4; 5LP/4mm   1.25
// a=5; 5LP/5mm   1
// a=6; 5LP/6mm   0.833*
// a=7; 5LP/7mm   0.714*
// a=8; 5LP/8mm   0.625
// a=9; 5LP/9mm   0.556*
// a=10; 5LP/10mm 0.5

module LB0_5() {
    text1 = "0.5 LP/mm";
    translate([0,-0.16,1.6])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str(text1), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}

module LB0_55() {
    text1c = "0.55"; x = 0.55; echo((5/x-1)/3);
    translate([0,-0.93,-z-0.037+(5/x-1)/3])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str("-- ",text1c), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}

module LB0_65() {
    text1c = "0.65"; x = 0.65; echo((5/x-1)/3);
    translate([0,-0.86,-z-0.037+(5/x-1)/3])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str("-- ",text1c), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}

module LB0_8() {
    text1c = "0.8"; x = 0.8; echo((5/x-1)/3);
    translate([0,-0.79,-z-0.037+(5/x-1)/3])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str("-- ",text1c), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}

module LB1() {
    text1b = "1.0"; x = 1.0; echo((5/x-1)/3);
    translate([0,-0.74,-z-0.037+(5/x-1)/3])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str("-- ",text1b), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}
module LB1_3() {
    text1a = "1.3"; x = 1.3; echo((5/x-1)/3);
    translate([0,-0.69,-z-0.037+(5/x-1)/3])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str("-- ",text1a), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}

module LB2() {
    text1d = "2.0"; x = 2.0; echo((5/x-1)/3);
    translate([0,-0.63,-z-0.037+(5/x-1)/3])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str("-- ",text1d), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}

module LB3() {
    text1d = "3.0"; x = 3.0; echo((5/x-1)/3);
    translate([0,-0.59,-z-0.037+(5/x-1)/3])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str("-- ",text1d), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}

module LB5() {
    text2 = "5.0 LP/mm";
    translate([0,-0.17,-1.7])color("DimGray",1.0)
    rotate([90, 0, -90]) {
        linear_extrude(height = dc) {
            text(text = str(text2), font = font, size = 0.1, 
                                         valign = "baseline");
        }
    }
}

Base();
LP1_25();
LP2_5();
LP5();
LP10();
LP1_25t();
LP2_5t();
LP5t();
LP10t();
LB0_5();
LB0_55();
LB0_65();
LB0_8();
LB1();
LB1_3();
LB2();
LB3();
LB5();
Radial();