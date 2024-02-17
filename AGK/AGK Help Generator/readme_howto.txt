Update Jan 2023
----------------
Use the tools/AGKBuildSystem/Windows tool and run the "Build help files" step

From Mike
---------
The Generator project creates everything aside from the reference documentation. 
Edit Generator1.cpp and modify the first call to fopen and ensure the path is relative. 
Scroll down a bit and look for the first call to SetCurrentDirectory and update this. 
There's a few more calls just below doing the same thing that need to be updated.
The Reference project builds all of the reference documentation. 
Edit Generator1.cpp and look for the function named ParseSourceFile. 
There's two function calls to ParseFile - these paths need to be adjusted.
Once both are built run the generator executable. 
It will create html and output into the AGK Txt folder. Next run the reference executable. 
Once finished copy the contents of the output folder into AGK Txt / Reference (this needs to be automated at some point). 
Keywordlinks.txt gets exported into this folder. Now everything is ready to be uploaded.

From Paul
---------
It turns out there are two generator projects for the help files, one in the "AGK Help Generator\Generator" folder that creates the guides, 
and one in the "AGK Help Generator\Reference" folder that generates the command listings. Both must be run to generate the complete help files 
and then the contents of "AGK Help Generator\Reference\Output" must be copied into "AGK Help Generator\AGK Txt\Reference" to complete the process. 
The help files are then all in the "AGK Txt" folder.

From Lee
--------
Created a script to automatically perform the above. 
Noticed that I had to be in the Output folder when I ran Reference.exe as it required base.htm which was only in this folder. 
Seemed to work. I have also replicated this build script in the master script too, this local one is just to test that the help system generated the correct HTMLs.
