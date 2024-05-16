difference() {
  translate([-0.21131,0,0]) 
    color("gold",alpha=0.9) cube([1,1,1],center=true);
  rotate([0,0,-30]) translate([-1.5000,0,0]) 
    color("gold",alpha=0.9) 
    cube([3,3,3],center=true);
}

//translate([30,0,0]) { color("red")
//    cube([0.1,5.0,0.1],true);
//}

//translate([-10,0,0]) { color("blue")
//    cube([0.1,0.1,0.1],true);
//}

color("gray",alpha=0.3) 
difference() {
  translate([10,0,0]) rotate([0,-90,0])
    cylinder(h=40,r1=2.5,r2=0,center=true,$fn=50);
  translate([10,0,-2.525]) {
      cube([50,6,5],true);
  }
  translate([10,0,+2.525]) {
      cube([50,6,5],true);
  }
}

