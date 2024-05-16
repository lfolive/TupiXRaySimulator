l=2;
x=2;
g=2.5;
t=0.7;
h=0.1;
s=0.1;
ok = true;

if (!ok) {
  color("gold",alpha=0.8)
  translate([0,0.001,0])
  difference() {
    translate([-t/2,-1.2,-h/2]) cube([t,1,h]);
    difference() {
        cube([g,g,0.21],center=true);
        cylinder(h=0.22,r=1,center=true,$fn=70);
    }
  }
}  
if (ok) {
  translate([0,-0.001,0]) scale([0.999,0.999,0.999])
  difference() {
    color("red",alpha=0.6)
    translate([0,-x/4,0]) cube([l/2,x/2,0.20],center=true);
    color("red",alpha=0.6)
    difference() {
        cube([g,g,0.21],center=true);
        cylinder(h=0.22,r=1,center=true,$fn=70);
    }
    translate([-t/2,-1.2,-h/2]) cube([t,1,h]);
  }
}
if (!ok) {
    color("gold",alpha=0.8) difference() 
    {
      rotate([0,0,-18]) difference() {
          cylinder(h=0.2,r=1,center=true,$fn=70);
          translate([0,0.5,0])
              cylinder(h=1,r=0.3,center=true,$fn=50);
          translate([-0.75,0,0])
              cylinder(h=1,r=0.1,center=true,$fn=30);
      }
      translate([0,-x/4,0])
        cube([l/2,x/2,0.25],center=true);
    }
}

