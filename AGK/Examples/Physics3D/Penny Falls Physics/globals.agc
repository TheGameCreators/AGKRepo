// Setup a few global variables, load some media and create a few sprites
global gameover_sprite as integer
global highscores_sprite as integer
global score_sprite as integer

// Used for the hi-score table
global hs_text as integer[10]
global hs_text_shadow as integer[10]

// create an array
global hiscorearray as integer[10]

// for our exit button sprite ID
global exit_button as integer

// a couple of global variables to store our text background IDs
global tbg1 as integer
global tbg2 as integer
global tbg3 as integer

// set some global variables that we will use for our virtual resolution display properties
global dw# as float

global friction# as float
friction# = 0.8
