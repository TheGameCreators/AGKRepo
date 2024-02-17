rem
rem Drawing lines with sprites!
rem

global ratio# : ratio#=2.0
SetDisplayAspect(ratio#)

rem Create Line resource
global drawlineimg : drawlineimg=LoadImage("drawline.png")
dim drawlinespr[100]
for t=0 to 100
 drawlinespr[t]=CreateSprite(drawlineimg)
 SetSpriteVisible(drawlinespr[t],0)
next t
global drawlinei : drawlinei=0

do
 DrawALine(10,10,GetPointerX(),GetPointerY(),255,255,0)
 DrawALine(80,80,10,80,255,255,0)
 DrawALineFinish()
 Sync()
loop

function DrawALine(x1 as float, y1 as float, x2 as float, y2 as float, r as float, g as float, b as float)
 t=drawlinei : inc drawlinei
 x1=x1*ratio#
 x2=x2*ratio#
 mix#=(x2-x1) : miy#=(y2-y1)
 mihx#=mix#/2 : mihy#=miy#/2
 mia#=atanfull(-miy#,mix#)
 mil#=sqrt(abs(mix#*mix#)+abs(miy#*miy#))/ratio#
 SetSpriteOffset(drawlinespr[t],mil#/2,0.25)
 SetSpritePositionByOffset(drawlinespr[t],(x1+mihx#)/ratio#,y1+mihy#)
 SetSpriteSize(drawlinespr[t],mil#,0.5)
 SetSpriteAngle(drawlinespr[t],mia#)
 SetSpriteColor(drawlinespr[t],r,g,b,255)
 SetSpriteVisible(drawlinespr[t],1)
endfunction

function DrawALineFinish()
 for t=drawlinei to 100
  SetSpriteVisible(drawlinespr[t],0)
 next t
 drawlinei=0
endfunction
