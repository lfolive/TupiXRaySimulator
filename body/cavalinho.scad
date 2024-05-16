translate([0,0.4,0]) rotate([0,0,90]) union() {
    translate([-0.9,0,0.9]) {
        translate([-0.4,0,0.6]) rotate([0,60,0])
            cylinder(h=1,r=0.3,center=true,$fn=30);
        rotate([0,-10,0])
            cylinder(h=1,r=0.1,center=true,$fn=30);
    }
    rotate([0,90,0])
        cylinder(h=2,r=0.5,center=true,$fn=30);
    translate([-0.9,-0.3,-1])
        rotate([-10,10,0])
            cylinder(h=1.3,r=0.1,center=true,$fn=30);
    translate([-0.9,0.3,-1])
        rotate([10,10,0])
            cylinder(h=1.3,r=0.1,center=true,$fn=30);
    translate([0.9,-0.3,-1])
        rotate([-10,-10,0])
            cylinder(h=1.3,r=0.1,center=true,$fn=30);
    translate([0.9,0.3,-1])
        rotate([10,-10,0])
            cylinder(h=1.3,r=0.1,center=true,$fn=30);
}
