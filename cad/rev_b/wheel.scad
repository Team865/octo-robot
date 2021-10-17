global_fn = 160;         // how smooth should curves be       

wheel_h = 24.6;
wheel_h = 10;
reinforce=1;

d_coupler_shaft = 6;         // encoder shafe daimater.                  measured
r_coupler_shaft = d_coupler_shaft/2;
m_coupler_shaft_extra = .375;     // extra space for shaft hole.              estimated
m_coupler_side_extra_gap = 1.1;
to_flat_bit = 4.45;     // distance from edge to flat bit on shaft  measured
coupler_outer_wall = 2; // amount of meaterial around the shaft. 
epsilon=0.001;

d_motor_shaft = 5.3;
d_motor_shaft_extra = .375;
d_motor_coupler_inner_wall = d_motor_shaft + d_motor_shaft_extra;
r_motor_coupler_inner_wall = d_motor_coupler_inner_wall / 2;
z_motor_coupler_height = wheel_h + 6.5 + reinforce;
z_motor_coupler_height = wheel_h + 10 + reinforce;
x_motor_coupler_flat_edge = 2.5;
x_motor_coupler_flat_edge = 2.25;  // samples
x_motor_coupler_flat_edge = 2.15;  // viable
x_motor_coupler_flat_edge = 2.05;  // viable
x_motor_coupler_flat_edge = 1.95;  // viable
x_motor_coupler_flat_edge = 1.85;  // viable

z_shaft_height = 13;    // measured

d_coupler_inner_wall = d_coupler_shaft + m_coupler_shaft_extra;
r_coupler_inner_wall = d_coupler_inner_wall / 2;
x_coupler_flat_edge = -r_coupler_shaft + to_flat_bit + m_coupler_side_extra_gap;



module ring( depth, inner_diam, wall ) {
    rotate_extrude($fn=global_fn) polygon( points=[
        [inner_diam/2, 0 ],
        [inner_diam/2, depth],
        [inner_diam/2+wall, depth ],
        [inner_diam/2+wall, 0 ]]);
}

module encoder_coupler()
{
    ring( z_shaft_height, d_coupler_inner_wall, coupler_outer_wall );
    linear_extrude( height = z_shaft_height ) polygon( points=[
        [ x_coupler_flat_edge, -r_coupler_inner_wall ],
        [ x_coupler_flat_edge,  r_coupler_inner_wall ],
        [ r_coupler_inner_wall,                 r_coupler_inner_wall ],
        [ r_coupler_inner_wall,                -r_coupler_inner_wall ]    
    ]);
}

module motor_coupler()
{
    ring( z_motor_coupler_height, d_motor_coupler_inner_wall, coupler_outer_wall );    

    linear_extrude( height = z_motor_coupler_height ) polygon( points=[
        [ x_motor_coupler_flat_edge, -r_motor_coupler_inner_wall ],
        [ x_motor_coupler_flat_edge,  r_motor_coupler_inner_wall ],
        [ r_motor_coupler_inner_wall,                 r_coupler_inner_wall ],
        [ r_motor_coupler_inner_wall,                -r_coupler_inner_wall ]    
    ]);    
    linear_extrude( height = z_motor_coupler_height ) polygon( points=[
        [ -x_motor_coupler_flat_edge, -r_motor_coupler_inner_wall ],
        [ -x_motor_coupler_flat_edge,  r_motor_coupler_inner_wall ],
        [ -r_motor_coupler_inner_wall,                 r_coupler_inner_wall ],
        [ -r_motor_coupler_inner_wall,                -r_coupler_inner_wall ]    
    ]);    
    
}

wheel_diam = 47.3;
wheel_diam = 48;
wheel_diam = 49.25;
wheel_diam = 49.5;
wheel_diam = 55 + 9.35*2;
wheel_diam = 55 + 9.35;
wheel_diam = 55 + 10.5;


wheel_rim_h = 1.35;
wheel_rim_w = 2;

wheel_wall=2;

module plastic_wheel()
{
ring( wheel_h + reinforce, wheel_diam - wheel_wall*2, wheel_wall );
ring( wheel_rim_h, wheel_diam, wheel_rim_w );
for( zup = [ 0 : .2 : 1 ] ) {
translate([0,0,wheel_h-wheel_rim_h + zup])
ring( .2, wheel_diam, wheel_rim_w * (zup + .2) / 1.2 );
}
translate([0,0,wheel_h-wheel_rim_h+1])
ring( wheel_rim_h-1+reinforce, wheel_diam, wheel_rim_w );
motor_coupler();

for( angle = [ 0 : 60 : 360 ] )
{
    rotate( angle, [0, 0, 1 ] ) {
      linear_extrude( height = wheel_h ) polygon( points=[
        [ d_motor_coupler_inner_wall/2+1, -1 ],
        [ wheel_diam/2-1, -1 ],
        [ wheel_diam/2-1, 1 ],
        [ d_motor_coupler_inner_wall/2+1, 1 ] ]
        );          
    }
}
}

module rubber_wheel_pos()
{
    zbot = wheel_rim_h;
    ztop = wheel_h-wheel_rim_h+1-.2;
    zdiff = ztop - zbot;
    
    for ( z = [ zbot : .2 : ztop ] ) {
        range = ( z - zbot ) / zdiff;
        extra = sin( range * 180 ) * 2;
        translate( [0, 0, z ] )
        ring( .2, wheel_diam + extra*2, 2  );         
    }
}

module rubber_wheel()
{
    difference() {
        rubber_wheel_pos();
        plastic_wheel();
    }
}

//intersection() {
plastic_wheel();
// translate([0,0,-1])
// cylinder( r = 10, h = 50 );   
//}
//color([.5,.5,.5])
//rubber_wheel();
