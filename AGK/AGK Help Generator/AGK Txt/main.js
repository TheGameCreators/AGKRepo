var maxCodeBlocks = 100;
var cppVisible = 0;
var basicVisible = 1;

function ToggleCPP()
{
    if ( document.getElementById('comments') )
    {
        if ( cppVisible ) document.getElementById('comments').contentWindow.postMessage("tier1","*");
        else document.getElementById('comments').contentWindow.postMessage("tier2","*");
    }
    
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

function checkFrameSize(f)
{
    if (window.name.length == 0) 
    {
        setTimeout(checkFrameSize(f),200);
        return;
    }

    var hashValue = window.name;
    alert(hashValue);

    if ((hashValue == null) || (hashValue.length == 0)) 
    {
        setTimeout(checkFrameSize(f),200);
        return;
    }

    var pair = hashValue.split('=');
    if ((pair != null) && (pair.length > 0)) {
    
        if (pair[0] == 'height') {
        
            var height = parseInt(pair[1]);

            if (!isNaN(height)) 
            {
                f.style.height = height+15;
            }
        }
    }
}