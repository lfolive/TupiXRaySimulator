//linear_extrude(height = 5, center = true, convexity = 10, slices = 20, scale = 1.0, $fn = 16);

//Parafuso
color("lightgray", 1.0)
linear_extrude(height = 4, center = true, twist = -700, $fn = 30)
rotate([0,55,0]) 
translate([1, 0, 0]) 
circle(r = 0.5);

//Detetor
color("darkgray", 1.0)
translate([1.5,0,0]) {
    cube([0.1,5.0,5.0],true);
}

////Feixe de raios X
color("LightYellow", 0.25)
translate([1.5,0,0])
rotate([0,-90,0])
cylinder(h=31.5, r2 = 0.05, r1 = 3.7, $fn = 30);

//Divergência (panqueca)
translate([-30.0, 0, 0]) 
rotate([0,90,0])
cylinder(h=0.001, r = 0.05, $fn = 30);

//Fonte
color("cyan", 1.0)
translate([-30.25,0,0]) {
    cube([0.5,0.5,0.5],true);
}
