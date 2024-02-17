
#import "ExternalCommands.h"

#import <AppTrackingTransparency/AppTrackingTransparency.h>

class IDFATrackingSDK : public ExternalSDK
{
public:
    int m_iTrackingAllowed = -1;
    
    void ExternalCommand( const char* command, const char* str1, const char* str2 ) override
    {
        if ( strcmp( command, "requestconsent" ) == 0 )
        {
            m_iTrackingAllowed = -1;
            if (@available(iOS 14, *))
            {
                [ ATTrackingManager requestTrackingAuthorizationWithCompletionHandler:^(ATTrackingManagerAuthorizationStatus status)
                    {
                        switch( status )
                        {
                            case ATTrackingManagerAuthorizationStatusAuthorized: m_iTrackingAllowed = 1; break;
                            case ATTrackingManagerAuthorizationStatusDenied: m_iTrackingAllowed = 0; break;
                            case ATTrackingManagerAuthorizationStatusRestricted: m_iTrackingAllowed = 0; break;
                            case ATTrackingManagerAuthorizationStatusNotDetermined: m_iTrackingAllowed = 0; break;
                            default: m_iTrackingAllowed = 0; break;
                        }
                    }
                ];
            }
            else
            {
                m_iTrackingAllowed = 1;
            }
        }
    }
    
    int ExternalCommandInt( const char* command, const char* str1, const char* str2 ) override
    {
        if ( strcmp( command, "trackingallowed" ) == 0 ) return m_iTrackingAllowed;
        
        return 0;
    }
};

IDFATrackingSDK cIDFATrackingSDK;
ExternalSDK* g_pIDFATrackingSDK = &cIDFATrackingSDK;

