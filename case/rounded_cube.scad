module rounded_cube(a,b,c,r){
    hull(){
        translate([r,r,0]) cylinder(r=r, h=c, $fn=64);
        translate([a-r,r,0]) cylinder(r=r, h=c, $fn=64);
        translate([a-r,b-r,0]) cylinder(r=r, h=c, $fn=64);
        translate([r,b-r,0]) cylinder(r=r, h=c, $fn=64);
    }
}
