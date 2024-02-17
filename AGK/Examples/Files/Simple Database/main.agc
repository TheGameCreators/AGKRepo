
// Project: My Simple Database!
// Created: 2017-02-12


// set window properties
SetWindowTitle( "Simple Database" )
SetWindowSize( 1024, 768, 0 )

// set display properties
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 0, 0 )
SetScissor(0,0,0,0)
SetSyncRate( 30, 0 ) // 30fps instead of 60 to save battery
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts

//Load a backdrop
loadimage(1,"agk-demo-back-3-black.jpg")
CreateSprite(1,1)
SetSpriteSize(1,GetScreenBoundsRight()-GetScreenBoundsLeft(),GetScreenBoundsBottom()-GetScreenBoundsTop())
SetSpritePosition(1,GetScreenBoundsLeft(), GetScreenBoundsTop())

#constant max_records = 100
current_record=1
total_records=1

global exit_button as integer
global dw# as float
dw# = GetDeviceWidth()
sync()

#include "exit_button.agc"

// Create a Type container to deal with the fields of records
type record
 firstname as string
 lastname as string
 age as integer
 score as float
endtype

// Create an array using the record Type
database as record[max_records]

// Set text and edit boxes up
gosub clear_all_records
gosub create_text
gosub Position_Text
gosub Create_Buttons
gosub Create_editboxes

database[current_record].firstname=""
gosub FillData

display_exit()

// The main Loop
// Check if the user has clicked any of the virtual buttons
do

	// Add a new record
  	If GetVirtualButtonPressed(1) 
		Gosub SaveData
		if total_records<>max_records
			inc total_records
			current_record=total_records
		endif
		database[current_record].firstname=""
		Gosub FillData
	endif
	
	// Delete the current record
	If GetVirtualButtonPressed(2)
		// We need to move all the records ahead of this one down one
		// First check there's more than one record
		if current_record<total_records
			for i=current_record to total_records
				database[i].firstname=database[i+1].firstname
				database[i].lastname=database[i+1].lastname
				database[i].age=database[i+1].age
				database[i].score=database[i+1].score
			next i
			dec total_records
		else
		// Don't delete the final record
			if current_record<>1
				database[current_record].firstname=""
				database[current_record].lastname=""
				database[current_record].age=0
				database[current_record].score=0
		//Shift back one record
				dec current_record
				dec total_records
			endif
		endif
	Gosub FillData
	endif
	//***********************************************

	// Sort the database based on first name
	If GetVirtualButtonPressed(3) 
		Gosub SaveData
		Gosub SortRecords
		Gosub FillData
	endif
	
	//View the Previous Record
	If GetVirtualButtonPressed(4) 
		Gosub SaveData
		dec current_record
		if current_record<1
			current_record=1
		endif
		Gosub FillData
	endif

	// View the Next Record
	If GetVirtualButtonPressed(5) 
		Gosub SaveData
		if current_record<>total_records
			inc current_record
			if current_record>max_records
				current_record=max_records
			endif
		Gosub FillData
		endif
	endif
	//***********************************************
	// Save the database
	If GetVirtualButtonPressed(6) 
		Gosub SaveData 
		Gosub SaveOutData
	endif
	
	// Load the database
	If GetVirtualButtonPressed(7) 
		Gosub LoadInData
	endif
	
	//Show which record we are viewing and how many there are in total
	Print("Viewing "+str(current_record)+" of "+str(total_records))

	// Check if the exit button was pressed
	check_exit()

	// Check if screen orientation has changed
	if dw# <> GetDeviceWidth() 
		gosub Position_Text
		display_exit()
	endif
    Sync()
loop

// Clear all the records and set the name field to zzzz so that the sort works
clear_all_records:
for i=1 to max_records
	database[i].firstname="z"
	database[i].lastname=""
	database[i].age=0
	database[i].score=0
next i
return

// Create the text for the display
Create_Text:
CreateText(1,"My Simple Database")
SetTextAlignment(1,1)
CreateText(2,"First Name")
CreateText(3,"Last Name")
CreateText(4,"Age")
CreateText(5,"Score")
for i=1 to 5
	SetTextSize(i,48)
next i
Return

// Position the text objects
Position_Text:
sync()
//SetTextPosition(1,GetDeviceWidth()/2,0)
SetTextPosition(1,512,0)
SetTextPosition(2,20,150)
SetTextPosition(3,20,250)
SetTextPosition(4,20,350)
SetTextPosition(5,20,450)
// Position background image
SetSpriteSize(1,GetScreenBoundsRight()-GetScreenBoundsLeft(),GetScreenBoundsBottom()-GetScreenBoundsTop())
SetSpritePosition(1,GetScreenBoundsLeft(), GetScreenBoundsTop())
dw# = GetDeviceWidth()
Return

// Make 5 virtual buttons
Create_Buttons:
AddVirtualButton( 1, 100, 600, 120 ) 
SetVirtualButtontext(1,"Add")

AddVirtualButton( 2, 300, 600, 120 ) 
SetVirtualButtontext(2,"Delete")

AddVirtualButton( 3, 500, 600, 120 ) 
SetVirtualButtontext(3,"Sort")

AddVirtualButton( 4, 700, 600, 120 ) 
SetVirtualButtontext(4,"Prev")

AddVirtualButton( 5, 900, 600, 120 ) 
SetVirtualButtontext(5,"Next")

//***********************************************

AddVirtualButton( 6, 400, 720, 80 ) 
SetVirtualButtontext(6,"Save")

AddVirtualButton( 7, 600, 720, 80 ) 
SetVirtualButtontext(7,"Load")

Return

// Make 4 edit boxes
Create_editboxes:
CreateEditBox(1)
SetEditBoxPosition(1,250,150) 
SetEditBoxSize(1,400,50)
SetEditBoxTextSize(1,48) 
SetEditBoxMaxChars(1,15) 

CreateEditBox(2)
SetEditBoxPosition(2,250,250) 
SetEditBoxSize(2,400,50)
SetEditBoxTextSize(2,48) 
SetEditBoxMaxChars(2,15) 

CreateEditBox(3)
SetEditBoxPosition(3,250,350) 
SetEditBoxSize(3,100,50)
SetEditBoxTextSize(3,48) 
SetEditBoxMaxChars(3,3)

CreateEditBox(4)
SetEditBoxPosition(4,250,450) 
SetEditBoxSize(4,200,50)
SetEditBoxTextSize(4,48) 
SetEditBoxMaxChars(4,5) 

return

// Save the text entered into the edit boxes and save it into the Type Array
SaveData:
database[current_record].firstname=GetEditBoxText(1)
database[current_record].lastname=GetEditBoxText(2)
database[current_record].age=val(GetEditBoxText(3)) 
database[current_record].score=valfloat(GetEditBoxText(4))

Return

//Copy the data from the Type array into the edit boxes.
FillData:
SetEditBoxText(1,database[current_record].firstname)
SetEditBoxText(2,database[current_record].lastname)
SetEditBoxText(3,str(database[current_record].age))
SetEditBoxText(4,str(database[current_record].score))

Return

//***********************************************

SortRecords:
//Just a simple, slow bubble sort!
	//Check we have at least 2 recods to sort!
	if total_records>1
		//Two loops to bubble sort each record
		for count=1 to total_records-1
			for i=1 to total_records-1
				//Lowercase the strings and compare them
				sort1$=lower(database[i].firstname)
				sort2$=lower(database[i+1].firstname)
				//Compare the two records
				if sort1$>sort2$
					//OK these need to be swapped over, store one copy
					store$=database[i+1].firstname
					//Swap over the values
					database[i+1].firstname=database[i].firstname
					database[i].firstname=store$
					
					store$=database[i+1].lastname
					//Swap over the values
					database[i+1].lastname=database[i].lastname
					database[i].lastname=store$
					
					store=database[i+1].age
					database[i+1].age=database[i].age
					database[i].age=store

					store#=database[i+1].score
					database[i+1].score=database[i].score
					database[i].score=store#
				endif	
			next i
		next count
	endif
Return

//***********************************************

SaveOutData:
	OpenToWrite(1,"simpledb.dat",0)
	WriteInteger(1,total_records)
	for i=1 to max_records
		WriteString(1,database[i].firstname)
		WriteString(1,database[i].lastname)
		WriteInteger(1,database[i].age)
		WriteFloat(1,database[i].score)
	next i
	CloseFile(1)
Return

LoadInData:
	if GetFileExists("simpledb.dat")
		OpenToRead(1,"simpledb.dat")
		total_records=ReadInteger(1)
		for i=1 to max_records
			database[i].firstname=ReadString(1)
			database[i].lastname=ReadString(1)
			database[i].age=ReadInteger(1)
			database[i].score=ReadFloat(1)
		next i
		CloseFile(1)
		current_record=1
		gosub FillData
	endif
Return

