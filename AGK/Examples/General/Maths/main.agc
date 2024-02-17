
// This example demonstrates the usage of several
// mathematical commands that are included in AppGameKit Studio

// nice fonts
UseNewDefaultFonts( 1 )

// set the random seed
SetRandomSeed ( 100 )

// get a random value
a = Random ( )

// get a random value between 1 and 10
b = Random ( 1, 10 )

// round 1.8 to the nearest number
value = Round ( 1.8 )

// round 1.8 down
a1 = Floor ( 1.8 )

// round 1.2 up
b1 = Ceil ( 1.2 )

// get the square root of 25
value1 = sqrt ( 25 )

// main loop
do
    // print values out
    Print ( a )
    Print ( b )
    Print ( value )
    Print ( a1 )
    Print ( b1 )
    Print ( value1 )

    // update the screen
    Sync ( )
loop
