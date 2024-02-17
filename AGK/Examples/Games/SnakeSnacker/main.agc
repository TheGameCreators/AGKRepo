rem ***************************************
rem *         Snake Snacker               *
rem *        Desktop version              *
rem *       Coded by Rick Vanner          *
rem *         Art Peter Jovanovic         *
rem *    Music: www.Shockwave-Sound.com   *
rem ***************************************
UseNewDefaultFonts( 1 )

dim entity[40,20]
global xsize, ysize, scaler, xapple,yapple,applex,appley, dietx, diety, rockstart, rockend, zz, xx, yy, touch

xsize=23 :ysize=15 : scaler=20
rockstart=4 : rockend=8
if GetDeviceWidth()=320 or GetDeviceWidth()=480
    xsize=14 :ysize=9 : scaler=32
    rockend=6
endif

rem Checks to see if device is touch based
if GetMultiTouchExists()=0
 touch=0
else
 touch=1
endif

global dim grid[23,15]
gosub cleargrid

rem Load high score
if getfileexists("snakescore.txt")=1
 opentoread(1,"snakescore.txt")
 highscore=readinteger(1)
 closefile(1)
else
 highscore=0
endif

dim rocks[30]
global dim snake_segs[400,3]

SetOrientationAllowed(0,0,1,1)
SetVirtualResolution(480,320)

rem Loading splash (Samsung Bada blank screen when loading)
CreateSprite ( 98765, LoadImage ( "loading.png" ) )
SetSpriteSize ( 98765, 480, 320 ) : Sync() : Sync() : Sleep(500)

gosub _loadmusic
gosub loadimages
gosub makegametext

startgame:
rem variable setups
apple=0 : applex=1 : appley=1 : gamespeed=18
gapple=0 : gapplex=1 : gappley=1
diet=0 : dietx=1 : diety=1
slowtime=0 : slowx=1 : slowy=1
entity[1,5]=2 : gameover=0 : snakesize=3 : snake_xd=1 : snake_yd=0
score=0


rem Set x,y and angle of snake segs
for i=1 to snakesize step 1
    snake_segs[i,1]=6-i : snake_segs[i,2]=5 : snake_segs[i,3]=0
    gx=6-i
    gy=5
    grid[gx,gy]=1
next i


rem ******************************************************
rem             Main Menu
rem ******************************************************

gosub game_sprites_invisible
gosub show_menu
` Wait for a touch/click
gosub wait4click

gosub show_game
gosub game_sprites_visible

rem ******************************************************
rem          Place the rocks down first!
rem ******************************************************
        for ii=rockstart to rockend
            zz2=0
            repeat
                xx=random(1,xsize-1)
                yy=random(1,ysize-1)
                if yy=5  // stops blocks being set to the same line as the snake at start
                    yy=6
                endif

                zz=grid[xx,yy]

                if zz=0
                    entity[ii,1]=xx
                    entity[ii,2]=yy
                    grid[xx,yy]=2  // 2=a rock in the grid
                    zz2=1
                endif

                if zz=1

                endif
            until zz2=1
        next ii

rem Place an apple,only check against rocks (nothing else exists)
zz=1 : zz2=0
            repeat
                xx=random(1,xsize-1)
                yy=random(1,ysize-2)

                zz=grid[xx,yy]

                if zz=0
                    applex=xx
                    appley=yy
                    grid[xx,yy]=3  // 3=an apple
                    zz2=1
                endif

                if zz=1

                endif
            until zz2=1
            rem Remove loading page sprite
            DeleteSprite(98765)
        gosub mainloop
        end

rem GRID STUFF!
cleargrid:
for x=1 to xsize
    for y=1 to ysize
        grid[x,y]=0
    next y
next x
return

showgrid:
    setprintsize(6)

for y=1 to ysize
    for x=1 to xsize
        l=grid[x,y]
        if l<>0
            line$=str(l)
        else
            line$=" "
        endif
        printc(line$)
    next x
    print("")
next y
return
rem ******************************************************
rem              Main Loop of the Game
rem ******************************************************
mainloop:
rem if GetVirtualJoystickExists(1)=1 then SetVirtualJoystickVisible(1,1)
Repeat

    if score>highscore
        highscore=score
    endif
    rem fade in apple, golden apple and pill
    fadein(2,6)     `gold apple
    fadeout(2,6)

    fadein(3,6)     `pill
    fadeout(3,6)


    fadein(20,6)    `slow
    fadeout(20,6)

    rem update score
    SetTextString (3,str(score))
    SetTextString (5,str(highscore))

    rem draw apple
    SetSpritePosition(500,applex*scaler,appley*scaler)

    rem draw golden apple
    SetSpritePosition(499,gapplex*scaler,gappley*scaler)

    rem draw diet pill
    SetSpritePosition(498,dietx*scaler,diety*scaler)

    rem draw slow down
    SetSpritePosition(slowid,entity[20,1]*scaler,entity[20,2]*scaler)

    rem draw blocks
    count=1
    for i=rockstart to rockend
         SetSpritePosition(entity[i,7],entity[i,1]*scaler,entity[i,2]*scaler)
        inc count
    next i


    rem draw snake
    for i=1 to snakesize
        SetSpriteVisible(i,1)
        if i>1 and i<snakesize
            SetSpriteImage(i,2,1)
            r1=snake_segs[i-1,3]
            r2=snake_segs[i,3]


            if r1=90 and r2<>90
                SetSpriteImage(i,3,1)
            endif
            if r1=180 and r2<>180
                SetSpriteImage(i,3,1)
            endif
            if r1=270 and r2<>270
                SetSpriteImage(i,3,1)
            endif
            if r1=0 and r2<>0
                SetSpriteImage(i,3,1)
            endif
        endif
        rem force the head and tail to be used
        if i = 1 :SetSpriteImage(i,1,1) : endif
        if i=snakesize
            SetSpriteImage(i,4,1)
            snake_segs[i,3]=snake_segs[i-1,3]
        endif
        rem position the segment on screen
        SetSpritePosition(i,snake_segs[i,1]*scaler,snake_segs[i,2]*scaler)
        rem ensure the proper rotation
        z=snake_segs[i,3]
        if i<>1 and i<>snakesize
            if r2=0 and r1=90 :z=270 : endif
            if r2=90 and r1=180 :z=0 : endif
            if r2=180 and r1=270 :z=90 : endif
            if r2=270 and r1=0 :z=z+270 : endif
        endif
        SetSpriteAngle(i,z)
    next i
    rem ********************************************
    rem    MOVE if time is right - SNAKE LOGIC!
    rem ********************************************
    if time=gamespeed
        time=0
        `Clear the snake from the grid
        for i=1 to snakesize
            gx=snake_segs[i,1]
            gy=snake_segs[i,2]
            grid[gx,gy]=0
        next i

        rem Now move the head in the direction chosen by the player
        xtemp=snake_segs[1,1] : ytemp=snake_segs[1,2]

        rem Checks if a snake's head is eating another part of the snake
        gameover=0
        for i=2 to snakesize
            x1=snake_segs[i,1]
            y1=snake_segs[i,2]
            if xtemp=x1 and ytemp=y1
                gameover=1
            endif
        next i


        xtemp=xtemp+snake_xd : ytemp=ytemp+snake_yd : direc_change=0


    rem If head goes off screen, place it at opposite end
    if xtemp<0
        xtemp=xsize
    endif
    if xtemp>xsize
        xtemp=0
    endif
    if ytemp<0
        ytemp=ysize
    endif
    if ytemp>ysize
        ytemp=0
    endif

    rem First copy the position of each segment to it's partner segment
    for i=snakesize to 2 step -1
        snake_segs[i,1]=snake_segs[i-1,1]
        snake_segs[i,2]=snake_segs[i-1,2]
        snake_segs[i,3]=snake_segs[i-1,3]
    next i


    snake_segs[1,1]=xtemp
    snake_segs[1,2]=ytemp

    if snake_xd=1 and snake_yd=0 : snake_segs[1,3]=0 :  endif
    if snake_xd=0 and snake_yd=-1 : snake_segs[1,3]=270 :  endif
    if snake_xd=-1 and snake_yd=0 : snake_segs[1,3]=180 : endif
    if snake_xd=0 and snake_yd=1 : snake_segs[1,3]=90 : endif

endif

` Set grid places for newly positioned snake
for i=1 to snakesize
    gx=snake_segs[i,1]
    gy=snake_segs[i,2]
    grid[gx,gy]=1
next i


time=time+1


    rem *******************************************
    rem If apple eaten make snake longer
    rem *******************************************
    if snake_segs[1,1]=applex
        if snake_segs[1,2]=appley
            snakesize=snakesize+1 : score=score+(snakesize+gamespeed) : gamespeed=gamespeed-1 : eaten=eaten+1
            if gamespeed<5
                gamespeed=5
            endif
            angle=snake_segs[snakesize-1,3]
            if angle=0
                xadd=-1 : yadd=0
            endif
            if angle=90
                xadd=0 : yadd=-1
            endif
            if angle=180
                xadd=1 : yadd=0
            endif
            if angle=270
                xadd=0 : yadd=1
            endif

            snake_segs[snakesize,1]=snake_segs[snakesize-1,1]+xadd
            snake_segs[snakesize,2]=snake_segs[snakesize-1,2]+yadd

            SetParticlesPosition ( 1, applex*scaler+8,appley*scaler+8)
            ResetParticleCount ( 1 )
            SetParticlesFrequency ( 1, 50 )
            SetParticlesLife ( 1, 1.0 )
            SetParticlesSize ( 1, 10 )
            SetParticlesImage ( 1, 7 )
            SetParticlesDirection ( 1, 10, 10 )
            SetParticlesAngle ( 1, 360 )
            SetParticlesVelocityRange ( 1, 0.8, 2.5 )
            SetParticlesMax ( 1, 50 )
            AddParticlesColorKeyFrame ( 1, 0.0, 0, 0, 0, 0 )
            AddParticlesColorKeyFrame ( 1, 0.5, 255, 255, 0, 255 )
            AddParticlesColorKeyFrame ( 1, 2.8, 255, 0, 0, 0 )
            AddParticlesForce ( 1, 2.0, 2.8, 25, -25 )

            rem Check to make sure new apple is placed in a clear area

            zz=1 : zz2=0
            repeat
                xx=random(1,xsize-1)
                yy=random(1,ysize-2)

                zz=grid[xx,yy]

                if zz=0
                    applex=xx
                    appley=yy
                    grid[xx,yy]=3  // 3=an apple
                    zz2=1
                endif

                if zz=1

                endif
            until zz2=1

        playsound(1)
        entity[1,5]=2
        endif
   endif

    rem *******************************************
    rem Has the snake hit a BLOCK?
    rem *******************************************
        for i=rockstart to rockend
            if snake_segs[1,1]=entity[i,1]
            if snake_segs[1,2]=entity[i,2]
                gameover=1
                //playsound(2)
            endif
            endif
        next i


 gosub do_slow
 gosub do_gapple
 gosub do_pill

rem ****************************************************
rem           I N P U T from Player
rem ****************************************************
rem for touch based devices we will use left and right icons to turn the snake

   key=GetRawKeyPressed(38) ` Up arrow
   if key=1 and snake_yd<>1 and direc_change=0
            snake_xd=0
            snake_yd=-1
            direc_change=1
    endif

   key=GetRawKeyPressed(40) ` Down
   if key=1 and snake_yd<>-1 and direc_change=0
            snake_xd=0
            snake_yd=1
            direc_change=1
    endif

   key=GetRawKeyPressed(37) `Left
   if key=1 and snake_xd<>1 and direc_change=0
            snake_xd=-1
            snake_yd=0
            direc_change=1
    endif

    key=GetRawKeyPressed(39) `Right
    if key=1 and snake_xd<>-1 and direc_change=0
            snake_xd=1
            snake_yd=0
            direc_change=1
    endif

if GetPointerPressed()=1 and touched=0
    hitsprite=GetSpriteHit(GetPointerX(),GetPointerY())
endif


if GetPointerPressed()=0
  touched=0 : hitsprite=0
endif

if touch=1

rem Turn snake to the left if icon pressed.

if touched=0
if hitsprite=tleft_s
    if direc_change=0
        if snake_xd=1
            snake_xd=0
            snake_yd=-1
            direc_change=1 : touched=1
        endif
    endif
    if direc_change=0
        if snake_xd=-1
            snake_xd=0
            snake_yd=1
            direc_change=1 : touched=1
        endif
    endif
    if direc_change=0
        if snake_yd=1
            snake_xd=1
            snake_yd=0
            direc_change=1 : touched=1
        endif
    endif
    if direc_change=0
        if snake_yd=-1
            snake_xd=-1
            snake_yd=0
            direc_change=1 : touched=1
        endif
    endif
endif

rem Turn snake to the right if icon pressed.

if hitsprite=tright_s
    if direc_change=0
        if snake_xd=1
            snake_xd=0
            snake_yd=1
            direc_change=1 : touched=1
        endif
    endif
    if direc_change=0
        if snake_xd=-1
            snake_xd=0
            snake_yd=-1
            direc_change=1 : touched=1
        endif
    endif
    if direc_change=0
        if snake_yd=1
            snake_xd=-1
            snake_yd=0
            direc_change=1 : touched=1
        endif
    endif
    if direc_change=0
        if snake_yd=-1
            snake_xd=1
            snake_yd=0
            direc_change=1 : touched=1
        endif
    endif
endif
endif
endif

rem If user presses / clicks Pause then go into pause mode

if touched=0

    if hitsprite=pause_s
    PauseMusicogg(1)
    gosub game_sprites_dimmed
    setspritevisible(pausetext_s,1)

    ` Wait until touch is released
        repeat
        sync()
        until GetPointerPressed()=0
    ` Now check for touch on pause again
        repeat
        hitsprite=GetSpriteHit(GetPointerX(),GetPointerY())
        if GetPointerPressed()=1 and hitsprite<>pause_s
             touched =1
        endif
        sync()
        until touched=1
    ResumeMusicogg(1)
    gosub game_sprites_undimmed
    setspritevisible(pausetext_s,0)

    endif
endif

//Print("")
//Print( "FPS: " + str(ScreenFPS(),1) )
sync()

until gameover=1

rem ****************** G A M E O V E R *********************

stopmusicogg(1)
stopmusicogg(2)
for i=1 to 100
if i=10
playsound(6)
endif
    sync()
next i
playmusicogg(3)

gosub reset_slow
gosub reset_gapple
gosub reset_pill

gosub game_sprites_invisible

for i=1 to 100 rem snakesize
        snake_segs[i,1]=-50
        snake_segs[i,2]=-50
        setspriteposition(i,snake_segs[i,1],snake_segs[i,2])
next i
gosub show_gameover


opentowrite(1,"snakescore.txt")
WriteInteger(1,highscore)
closefile(1)

rem Wait for a touch/click
c=0
a=0
while a=0
click=GetPointerPressed()
if click=1
    a=a+1
endif
gosub textjob
if c=2
sync():c=0
endif
inc c
endwhile

c=0
while a=1
click=GetPointerPressed()
if click=0
    a=a+1
endif
gosub textjob
if c=2
sync() : c=0
endif
inc c
endwhile

gosub show_menu
stopmusicogg(1)
stopmusicogg(2)
stopmusicogg(3)
SetTextVisible(2,0)
SetTextVisible(3,0)
SetTextVisible(4,0)
SetTextVisible(5,0)

goto startgame



do_slow:
rem Has the snake hit a slow down?
if entity[20,4]=1               //check to make sure slow is live
    if snake_segs[1,1]=entity[20,1]
        if snake_segs[1,2]=entity[20,2]
            slowdown=0                  //Set flag so a new SLOW can be created later
            entity[20,4]=0              //Set slow to not alive now
            entity[20,5]=3              //Set to fade to zero vis
            entity[20,6]=0
            SetSpriteColorAlpha(entity[20,7],0)
            playsound(3)

            xx=entity[20,1]             //Clear the grid
            yy=entity[20,2]
            SetSpriteVisible(slowid,0)
            grid[xx,yy]=0
            gamespeed=gamespeed+1       //Slow the snake a little
            if gamespeed>15             //Not too much mind
                gamespeed=15
            endif
        endif
    endif
endif

` should we create a slow down pickup
` slowdown flag 0=create a new slow pickup, 1=pick up is live
` slowtime = time the pick up is alive
if slowdown=0
    r=random(300,500)
    if r=400
        slowdown=1 : slowtime=random(400,700)
        zz=1 : zz2=0
        repeat
            xx=random(1,xsize-1)
            yy=random(1,ysize-2)
            zz=grid[xx,yy]
            if zz=0
                entity[20,1]=xx
                entity[20,2]=yy
                grid[xx,yy]=5  // 5=Slow down
                zz2=1
            endif
        until zz2=1
        entity[20,4]=1  // Entity is Live!
        entity[20,5]=1  // Fade in
        SetSpriteVisible(slowid,1)

    endif
endif
if entity[20,5]=3
    slowdown=0
    xx=entity[20,1]             //clear the grid
    yy=entity[20,2]
    grid[xx,yy]=0

endif

` decrease time of pickup, if zero, set the pick up to fade off
slowtime=slowtime-1
if slowtime<=0
    entity[20,5]=0      //Fade out
endif
return

reset_slow:
entity[20,4]=0              //Set slow to not alive now
entity[20,5]=3              //Set to fade to zero vis
entity[20,6]=0
SetSpriteColorAlpha(entity[20,7],0)
return

do_gapple:
    rem If GOLDEN apple eaten!
    if gapple=1
        if snake_segs[1,1]=gapplex
            if snake_segs[1,2]=gappley
                snakesize=snakesize+1 : score=score+(snakesize+gamespeed*5)

                angle=snake_segs[snakesize-1,3]
                if angle=0
                    xadd=-1 : yadd=0
                endif
                if angle=90
                    xadd=0 : yadd=-1
                endif
                if angle=180
                    xadd=1 : yadd=0
                endif
                if angle=270
                    xadd=0 : yadd=1
                endif

                snake_segs[snakesize,1]=snake_segs[snakesize-1,1]+xadd
                snake_segs[snakesize,2]=snake_segs[snakesize-1,2]+yadd

                SetParticlesPosition ( 1, gapplex*scaler+8,gappley*scaler+8)

                ResetParticleCount ( 1 )
                SetParticlesFrequency ( 1, 50 )
                SetParticlesLife ( 1, 1.0 )
                SetParticlesSize ( 1, 10 )
                SetParticlesImage ( 1, 7 )
                SetParticlesDirection ( 1, 10, 10 )
                SetParticlesAngle ( 1, 360 )
                SetParticlesVelocityRange ( 1, 0.8, 2.5 )
                SetParticlesMax ( 1, 50 )
                AddParticlesColorKeyFrame ( 1, 0.0, 0, 0, 0, 0 )
                AddParticlesColorKeyFrame ( 1, 0.5, 0, 255, 0, 255 )
                AddParticlesColorKeyFrame ( 1, 2.8, 255, 0, 0, 0 )
                AddParticlesForce ( 1, 3.0, 3.8, 25, -25 )
                entity[2,4]=0
                entity[2,5]=3
                entity[2,6]=0
                SetSpriteVisible(499,0)
                playsound(2)
                playsound(5)
            endif
        endif
    endif


rem Should we create a new golden apple?

    if gapple=0
        if eaten>=2     ` Make sure player has eaten at least two red apples before creating a new gold one
            r=random(1,500)
            if r=50

                gapple=1 : gappletime=random(200,300) : eaten=0
                zz=1 :zz2=0
                repeat
                    xx=random(1,xsize-1)
                    yy=random(1,ysize-2)

                    zz=grid[xx,yy]

                    if zz=0
                        gapplex=xx
                        gappley=yy
                        grid[xx,yy]=4  // 4=Golden apple
                        zz2=1
                    endif
                until zz2=1
                entity[2,4]=1
                entity[2,5]=1
                SetSpriteVisible(499,1)
            endif
        endif
    endif

rem Countdown to see if golden apple should be removed
gappletime=gappletime-1


if entity[2,5]=3
    gapple=0
    xx=gapplex
    yy=gappley
    grid[xx,yy]=0
    SetSpriteColorAlpha(entity[2,7],0)
endif

if gappletime<=0
    entity[2,5]=0
endif

return

reset_gapple:
entity[2,4]=0              //Set slow to not alive now
entity[2,5]=3              //Set to fade to zero vis
entity[2,6]=0
SetSpriteColorAlpha(entity[2,7],0)
return

 `           If DIET Pill is eaten!
 do_pill:
    if diet=1
        if snake_segs[1,1]=dietx
            if snake_segs[1,2]=diety
                if snakesize>3
                    s=snakesize
                    snakesize=snakesize-3
                    if snakesize<3
                        snakesize=3
                    endif

                    for i=snakesize to s
                        SetSpriteVisible(i,0)
                    next i

                endif
                diet=0

                angle=snake_segs[snakesize-1,3]
                if angle=0
                    xadd=-1 : yadd=0
                endif
                if angle=90
                    xadd=0 : yadd=-1
                endif
                if angle=180
                    xadd=1 : yadd=0
                endif
                if angle=270
                    xadd=0 : yadd=1
                endif

                snake_segs[snakesize,1]=snake_segs[snakesize-1,1]+xadd
                snake_segs[snakesize,2]=snake_segs[snakesize-1,2]+yadd
                playsound(3)
                playsound(4)
                entity[3,4]=0
                entity[3,5]=3
                entity[3,6]=0
                SetSpriteVisible(498,0)
            endif
        endif
    endif

rem should we create a diet pill (a rare thing indeed!)
if diet=0
        r=random(1,500)
        if r=20
        diet=1 : diettime=random(120,190)

        zz=1 : zz2=0
            repeat
                xx=random(1,xsize-1)
                yy=random(1,ysize-2)
                zz=grid[xx,yy]
                if zz=0
                    dietx=xx
                    diety=yy
                    grid[xx,yy]=6  // 6=Pill
                    zz2=1
                endif
            until zz2=1
            entity[3,4]=1
            entity[3,5]=1
             SetSpriteVisible(498,1)
            PlaySprite ( 498, 60, 1, 1, 15 )
        endif
endif



rem Countdown to see if diet pill should be removed
diettime=diettime-1
if entity[3,5]=3
        diet=0
        xx=dietx
        yy=diety
        grid[xx,yy]=0
        SetSpriteColorAlpha(entity[3,7],0)
endif
if diettime<=0
    entity[3,5]=0
endif

return

reset_pill:
entity[3,4]=0              //Set slow to not alive now
entity[3,5]=3              //Set to fade to zero vis
entity[3,6]=0
SetSpriteColorAlpha(entity[3,7],0)
return


rem ***********************************F U N C T I O N S! ******************************

rem Fade in an entity
rem s = entity number
rem speed = speed of fade in
function fadein(s,speed)
vis=0
if entity[s,4]=1    ` Is this entity alive?
    if entity[s,5]=1 `Is it fading in?
        entity[s,6]=entity[s,6]+speed
        vis=entity[s,6]
        if entity[s,6]>=255
            entity[s,5]=2
            entity[s,6]=0
            vis=255
        endif
    endif
    if entity[s,5]=2
        vis=255
    endif
endif
SetSpriteColorAlpha(entity[s,7],vis)

endfunction

rem Fade out an entity
rem s = entity number
rem speed = speed of fade out
function fadeout(s,speed)
vis=0
if entity[s,4]=1    ` Is this entity alive?
    if entity[s,5]=0  `Is this entity fading out?
        entity[s,6]=entity[s,6]+speed
        vis=255-entity[s,6]
        if entity[s,6]>=255
            entity[s,4]=0   ` Set to dead
            entity[s,5]=3   ` Mark as 0% visible
            entity[s,6]=0   ` Fade counter to zero
            vis=0
        endif
        if entity[s,5]=3
            vis=0
        endif
        SetSpriteColorAlpha(entity[s,7],vis)
    endif
endif
endfunction

wait4click:
Repeat
    click=GetPointerPressed()
    if click=1 and GetPointerX()>480-50 and GetPointerY()<50
     rem pressed the OFF button in the title page
     end
    endif
    sync()
Until click=1
Repeat
    click=GetPointerPressed()
    sync()
Until click=0
return



`
` Check to see if x,y chosen clashes with another object in game
`
` xx and yy are sent in, if the match any of these x,y values
` then false is returned in zz (zz starts off as true before hand)
check_snake:
for ii=1 to snakesize  step 1  //Check snake first
    xx2=snake_segs[ii,1]
    yy2=snake_segs[ii,2]
    if xx=xx2 and yy=yy2
        zz=0
    endif
next ii
return

check_rocks:
for ii=rockstart to rockend step 1//Check rocks
    xx2=entity[ii,1]
    yy2=entity[ii,2]
    if xx=xx2 and yy=yy2
        zz=0
    endif
next ii
return

check_apple:

`if applex>0 and appley>0
        if xx=applex and yy=appley // red apple
        zz=0
    endif
`endif
return

check_gapple:
`if gappplex>0 and gappley>0
    if xx=gapplex and yy=gappley // golden apple
        zz=0
    endif
`endif
return

check_pill:
`if dietx>0 and diety>0
    if xx=dietx and yy=diety //pill down
        zz=0
    endif
`endif
return

check_slow:
`if entity[20,1]>0 and entity[20,2]>0
    if xx=entity[20,1] and yy=entity[20,2] //slow down
    zz=0
    endif
`endif
return



rem ***************************************
rem         Sounds
rem 1 - apple chomp
rem 2 - Golden apple eaten
rem 3 - pill eaten
rem ***************************************
_loadmusic:
loadmusicogg(1,"hello.ogg") 	// Main Game music
loadmusicogg(2,"hello2.ogg")	// Main Menu music
loadmusicogg(3,"gameover.ogg")	// Game Over music
SetMusicVolumeOGG(1,40)
SetMusicVolumeOGG(2,40)
SetMusicVolumeOGG(3,40)

loadsound(1,"apple_chomp1.wav")	//eat apple
loadsound(2,"gapple.wav")	//eat golden apple
loadsound(3,"pill.wav")		//eat the pill
loadsound(4,"ooo.wav")
loadsound(5,"yummy.wav")
loadsound(6,"yuk.wav")
return

rem ***************************************
rem          Sprite IDs
rem ***************************************
rem 1 to 100 - snake
rem 498 - diet pill
rem 499 - apple gold
rem 500 - apple
rem 501 - title page background
rem 502 - particle image
rem 503 - backdrop to game
rem 504 - gameover screen
rem 601 - off button

rem ***************************************
rem          Image IDs
rem ***************************************
rem 1 - snake head
rem 2 - snake body
rem 3 - snake bend
rem 4 - snake tail
rem 5 - apple red
rem 8 - game background
rem 9 - game over screen
rem 10 - gold apple
rem 11 - diet pill
rem 12 - rock
rem 13 - slow down
rem ********************************************
rem dimension to hold data about entities. #of entity
rem x,1 : x position
rem x,2 : y position
rem x,3 : rotation
rem x,4 : 0 = not live, 1=live
rem x,5 : 0 = fade out, 1 = fade in, 2 = 100% displayed, 3 = 0% visible
rem x,6 : fade counter
rem x,7 : sprite ID
rem entity 1 = apple
rem entity 2 = golden apple
rem entity 3 = pill
rem entity 4 to 19 = rocks
rem entity 20 = slow down

loadimages:

t$="title.png"

loadimage(6,t$)
CreateSprite(501,6)
SetSpriteSize(501,480,320)
SetSpritePosition(501,0,0)

// Load Game Background
LoadImage(8,"background.png")
CreateSprite(503,8)
SetSpriteSize(503,480,320)
SetSpriteColorAlpha(503,200)
SetSpriteDepth(503,100)

// Load game over background
LoadImage(9,"gameover.png")
CreateSprite(504,9)
SetSpriteSize(504,480,320)

t$="click.png"

touch_i=LoadImage(t$)
touch_s=CreateSprite(touch_i)
SetSpriteSize(touch_s,300,-1)
SetSpritePosition(touch_s,475/2-(GetSpriteWidth(touch_s)/2),285)
SetSpriteVisible(touch_s,0)

rem 200911 - Power Off For AppUp
loadimage(601,"off.png")
CreateSprite(601,601)
SetSpritePosition(601,480-50,0)
SetSpriteSize(601,50,50)

if touch=1

tleft_i=loadimage("turn_left.png")
tright_i=loadimage("turn_right.png")

tleft_s=createsprite(tleft_i)
tright_s=createsprite(tright_i)


SetSpriteSize(tleft_s,scaler*2+2,-1)
SetSpriteSize(tright_s,scaler*2+2,-1)

SetSpritePosition(tleft_s,0,320-GetSpriteHeight(tleft_s))
SetSpritePosition(tright_s,480-GetSpriteWidth(tright_s),320-GetSpriteHeight(tright_s))
SetSpriteColorAlpha(tleft_s, 100 )
SetSpriteColorAlpha(tright_s, 100 )



endif



// Load Snake segments
loadimage(1,"snake-head.png")
CreateSprite ( 1, 1 )
SetSpriteSize (1,scaler,-1)
loadimage(2,"snake-body.png")
loadimage(3,"snake-bend.png")
loadimage(4,"snake-tail.png")

// Load Apple

loadimage(5,"apple.png")
CreateSprite(500,5)
SetSpriteSize(500,scaler,-1)
SetSpriteShape(500,1)
entity[1,7]=500


// Load Golden Apple

loadimage(10,"applegold.png")
CreateSprite(499,10)
SetSpriteSize(499,scaler,-1)
entity[2,7]=499


// Load Rock

loadimage(12,"rock.png")
count=1
for i=rockstart to rockend
    rocks[count]=createsprite(12)
    SetSpriteSize (rocks[count],scaler,-1)
    entity[i,7]=rocks[count] // Save sprite IDs
    count=count+1
next i

// Load Slow down images

loadimage(13,"slow.png")
slowid=CreateSprite(13)
SetSpriteSize(slowid,scaler,-1)
entity[20,7]=slowid

entity[3,7]=498

// Pill and it's animation

CreateSprite(498,0)
AddSpriteAnimationFrame ( 498, LoadImage ( "pill-00.png" ) ) : AddSpriteAnimationFrame ( 498, LoadImage ( "pill-01.png" ) )
AddSpriteAnimationFrame ( 498, LoadImage ( "pill-02.png" ) ) : AddSpriteAnimationFrame ( 498, LoadImage ( "pill-03.png" ) )
AddSpriteAnimationFrame ( 498, LoadImage ( "pill-04.png" ) ) :AddSpriteAnimationFrame ( 498, LoadImage ( "pill-05.png" ) )
AddSpriteAnimationFrame ( 498, LoadImage ( "pill-06.png" ) ) : AddSpriteAnimationFrame ( 498, LoadImage ( "pill-07.png" ) )
AddSpriteAnimationFrame ( 498, LoadImage ( "pill-08.png" ) ) : AddSpriteAnimationFrame ( 498, LoadImage ( "pill-09.png" ) )
AddSpriteAnimationFrame ( 498, LoadImage ( "pill-10.png" ) ) : AddSpriteAnimationFrame ( 498, LoadImage ( "pill-11.png" ) )
AddSpriteAnimationFrame ( 498, LoadImage ( "pill-12.png" ) ) :AddSpriteAnimationFrame ( 498, LoadImage ( "pill-13.png" ) )
AddSpriteAnimationFrame ( 498, LoadImage ( "pill-14.png" ) )

SetSpriteSize(498,scaler,-1)


//Create Snake Segment Sprites

for i=1 to 100
DeleteSprite(i)
CreateSprite(i,2)
SetSpriteSize (i,scaler,-1)
SetSpritePosition(i,-50,-50)
next i

// Load Pause icon and Pause message
pause_i=loadimage("pause.png")
pause_s=createsprite(pause_i)
setspritesize(pause_s,scaler,-1)
setspriteposition(pause_s,450,0)

pausetext_i=loadimage("pause_text.png")
pausetext_s=createsprite(pausetext_i)
rem setspritesize(pausetext_s,GetDeviceHeight()-(GetDeviceHeight()/4),-1)
setspritesize(pausetext_s,360,-1)
setspriteposition(pausetext_s,240-(GetSpriteWidth(pausetext_s)/2),320/2-(GetSpriteHeight(pausetext_s)/2))
setspritevisible(pausetext_s,0)

// Particles

LoadImage(7,"particle.png")
CreateParticles(1,-100,-100)
CreateParticles(2,-100,-100)
SetParticlesDepth(1,5)

return

game_sprites_visible:
// Show Snake
for i=1 to 100
    SetSpriteVisible(i,1)
next i
// Show Rocks
count=1
for i=rockstart to rockend
    SetSpriteVisible(rocks[count],1)
inc count
next i
// Show Apple
SetSpriteVisible(500,1)

// Show Left and Right
if touch=1
    SetSpriteVisible(tright_s,1)
    SetSpriteVisible(tleft_s,1)
endif
//Show Pause
SetSpriteVisible(pause_s,1)
return

game_sprites_dimmed:
for i=1 to 100
    SetSpriteColorAlpha(i,100)
next i
count=1
for i=rockstart to rockend
        SetSpriteColorAlpha(rocks[count],100)
inc count
next i
alph498=GetSpriteColorAlpha(498)
alph499=GetSpriteColorAlpha(499)
alph500=GetSpriteColorAlpha(500)

for i=498 to 500
    if GetSpriteColorAlpha(i)-100 < 0
        alph=GetSpriteColorAlpha(i)
    else
        alph=GetSpriteColorAlpha(i)-100
    endif
SetSpriteColorAlpha(i,alph)
next i
SetSpriteColorAlpha(503,100)
SetSpriteColorAlpha(slowid,100)
if touch=1
SetSpriteColorAlpha(tleft_s,100)
SetSpriteColorAlpha(tright_s,100)
endif
return

game_sprites_undimmed:
for i=1 to 100
    SetSpriteColorAlpha(i,255)
next i
count=1
for i=rockstart to rockend
        SetSpriteColorAlpha(rocks[count],255)
inc count
next i
SetSpriteColorAlpha(498,alph498)
SetSpriteColorAlpha(slowid,255)
SetSpriteColorAlpha(499,alph499)
SetSpriteColorAlpha(500,alph500)
SetSpriteColorAlpha(503,200)
if touch=1
SetSpriteColorAlpha(tleft_s,150)
SetSpriteColorAlpha(tright_s,150)
endif
return

game_sprites_invisible:

// Hide Snake
for i=1 to 100
    SetSpriteVisible(i,0)
next i
// Hide Rocks
count=1
for i=rockstart to rockend
    SetSpriteVisible(rocks[count],0)
    inc count
next i
// Hide Diet Pill
SetSpriteVisible(498,0)
// Hide Slow down
SetSpriteVisible(slowid,0)
// Hide Gold Apple
SetSpriteVisible(499,0)
// Hide Apple
SetSpriteVisible(500,0)
// Hide Game Backdrop
SetSpriteVisible(503,0) : SetSpritePosition ( 503, 0, -9999 )
// Hide Left and Right
if touch=1
    SetSpriteVisible(tright_s,0)
    SetSpriteVisible(tleft_s,0)
endif
//Hide Pause
SetSpriteVisible(pause_s,0)
return


Show_Game:
SetTextColor(2,255,255,255,255)
SetTextColor(3,255,255,255,255)
SetTextColor(4,255,255,255,255)
SetTextColor(5,255,255,255,255)
SetTextPosition(2,20,0)
SetTextPosition(3,86,0)
SetTextPosition(4,315,0)
SetTextPosition(5,415,0)
SetTextVisible(2,1)
SetTextVisible(3,1)
SetTextVisible(4,1)
SetTextVisible(5,1)
SetSpriteVisible(501,0) : SetSpritePosition ( 501, 0, -9999 )
SetSpriteVisible(601,0) : SetSpritePosition ( 601, 480-50, -9999 )
SetSpriteVisible(504,0) : SetSpritePosition ( 504, 480-50, -9999 )
SetSpriteVisible(503,1) : SetSpritePosition ( 503, 0, 0 )
stopmusicogg(1)
stopmusicogg(3)
playmusicogg(2,1)
return

Show_Menu:
SetTextVisible(2,0)
SetTextVisible(3,0)
SetTextVisible(4,0)
SetTextVisible(5,0)
SetSpriteVisible(touch_s,0)
SetSpriteVisible(501,1) : SetSpritePosition ( 501, 0, 0 )
SetSpriteVisible(601,1) : SetSpritePosition ( 601, 480-50, 0 )
SetSpriteVisible(504,0) : SetSpritePosition ( 504, 0, -9999 )
SetSpriteVisible(503,0) : SetSpritePosition ( 503, 0, -9999 )
stopmusicogg(2)
stopmusicogg(3)
playmusicogg(1,1)
return

Show_GameOver:
SetSpriteVisible(501,0) : SetSpritePosition ( 501, 0, -9999 )
SetSpriteVisible(601,0) : SetSpritePosition ( 601, 480-50, -9999 )
SetSpriteVisible(504,1) : SetSpritePosition ( 504, 0, 0 )
SetSpriteVisible(503,0) : SetSpritePosition ( 503, 0, -9999 )
SetSpriteVisible(touch_s,1)
stopmusicogg(1)
stopmusicogg(2)
playmusicogg(3)
SetTextPosition(2,120,250)
SetTextPosition(3,175,250)
SetTextPosition(4,240,250)
SetTextPosition(5,340,250)
return

textjob:
red=random(1,255)
blue=random(1,255)
green=random(1,255)
SetTextColor(2,red,green,blue,255)
SetTextColor(3,red,green,blue,255)
SetTextColor(4,red,green,blue,255)
SetTextColor(5,red,green,blue,255)
return

makegametext:
CreateText(2,"SCORE:")
CreateText(3,"0")
CreateText(4,"HIGH SCORE:")
CreateText(5,"0")
SetTextSize(2,14) :  SetTextSize(3,14) : SetTextSize(4,14) : SetTextSize(5,14)
SetTextDepth( 2, 0 ) : SetTextDepth( 3, 0 ) : SetTextDepth( 4, 0 ) : SetTextDepth( 5, 0 )
return

