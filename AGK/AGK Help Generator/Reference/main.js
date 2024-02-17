var maxCodeBlocks = 40;
var cppVisible = 0;
var basicVisible = 1;

function ToggleCPP()
{
    for ( i=0; i<maxCodeBlocks; i++ )
    {
        var name = 'cppCodeBlock' + i;
        var code = document.getElementById( name );
        if ( !code ) continue;
        
        if ( cppVisible ) code.style.display = 'none';
        else code.style.display = '';
    }

    cppVisible = 1 - cppVisible;

    for ( i=0; i<maxCodeBlocks; i++ )
    {
        var name = 'basicCodeBlock' + i;
        var code = document.getElementById( name );
        if ( !code ) continue;

        if ( basicVisible ) code.style.display = 'none';
        else code.style.display = '';
    }

    basicVisible = 1 - basicVisible;
}

function ToggleBASIC()
{
    for ( i=0; i<maxCodeBlocks; i++ )
    {
        var name = 'basicCodeBlock' + i;
        var code = document.getElementById( name );
        if ( !code ) continue;

        if ( basicVisible ) code.style.display = 'none';
        else code.style.display = '';
    }

    basicVisible = 1 - basicVisible;
}