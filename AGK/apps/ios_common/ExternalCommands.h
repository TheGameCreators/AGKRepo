
class ExternalSDK
{
public:
    virtual void ExternalCommand( const char* command, const char* str1, const char* str2 ) {}
    virtual int ExternalCommandInt( const char* command, const char* str1, const char* str2 ) { return 0; }
    virtual float ExternalCommandFloat( const char* command, const char* str1, const char* str2 ) { return 0; }
    virtual char* ExternalCommandString( const char* command, const char* str1, const char* str2 )
    {
        char* str = new char[ 1 ];
        *str = 0;
        return str;
    }
};
