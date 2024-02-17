
// write data to a file
OpenToWrite ( 1, "myfile.txt", 0 )
WriteInteger ( 1, 10 )
WriteFloat ( 1, 1.23 )
WriteString ( 1, "hello" )
CloseFile ( 1 )

// reading from a file
OpenToRead ( 1, "myfile.txt" )

a = ReadInteger ( 1 )
b# = ReadFloat ( 1 )
c$ = ReadString ( 1 )

CloseFile ( 1 )

