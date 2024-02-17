rem
rem Video Playback Example
rem

// nice fonts
UseNewDefaultFonts( 1 )

rem Load Video
LoadVideo("video.mp4")
SetVideoDimensions(0,0,100,93)
PlayVideo()

rem Prompt
CreateText(1,"Press screen to replay")
SetTextPosition(1,50,93)
SetTextAlignment(1,1)
SetTextSize(1,5)

rem Main loop
do
 if GetPointerPressed()=1
  rem Reset video
  StopVideo()
  rem Play from beginning
  PlayVideo()
 endif
 Sync()
loop
