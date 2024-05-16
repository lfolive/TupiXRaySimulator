dx=0.1; //cm
 scale([0.25,1,1]) union() {
translate([-dx/2,0,0])
    cube([dx,2.0,4.0],center=true);
translate([-3*dx/2,0,0])
    cube([dx,2.0,3.0],center=true);
translate([-5*dx/2,0,0])
    cube([dx,2.0,2.0],center=true);
translate([-7*dx/2,0,0])
    cube([dx,2.0,1.0],center=true);
}