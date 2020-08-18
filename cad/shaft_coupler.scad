global_fn = 80;         // how smooth should curves be       

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
z_motor_coupler_height = 6.5;
x_motor_coupler_flat_edge = 2.5;

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



encoder_coupler();

translate([0,0,z_shaft_height-epsilon])
motor_coupler();

