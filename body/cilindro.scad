scale(0.5) difference() {
    cylinder(h=0.2,r=1,center=true,$fn=70);
    translate([0,0.5,0])
        cylinder(h=1,r=0.4,center=true,$fn=50);
    translate([-0.75,0,0])
        cylinder(h=1,r=0.1,center=true,$fn=30);
}


