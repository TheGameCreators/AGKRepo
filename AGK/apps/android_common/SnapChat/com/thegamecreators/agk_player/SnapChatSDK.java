package com.thegamecreators.agk_player;

import android.app.Activity;
import android.util.Log;

import com.snap.creativekit.SnapCreative;
import com.snap.creativekit.api.SnapCreativeKitApi;
import com.snap.creativekit.exceptions.SnapMediaSizeException;
import com.snap.creativekit.exceptions.SnapStickerSizeException;
import com.snap.creativekit.media.SnapMediaFactory;
import com.snap.creativekit.media.SnapPhotoFile;
import com.snap.creativekit.media.SnapSticker;
import com.snap.creativekit.models.SnapPhotoContent;

import java.io.File;

public class SnapChatSDK
{
    private static float m_fStickerX = 0.5f; // between 0 and 1
    private static float m_fStickerY = 0.5f;
    private static int m_iStickerWidth = 250;
    private static int m_iStickerHeight = 250;
    private static float m_fStickerAngle = 0;
    private static String m_sCaption = "";
    private static String m_sURL = "";

    public static void ExternalCommand(Activity act, String cmd, String str1, String str2 )
    {
        if( cmd.equals("setposition") )
        {
            m_fStickerX = Float.parseFloat(str1);
            m_fStickerY = Float.parseFloat(str2);
        }
        else if( cmd.equals("setsize") )
        {
            m_iStickerWidth = Integer.parseInt(str1);
            m_iStickerHeight = Integer.parseInt(str2);
        }
        else if ( cmd.equals("setangle") )
        {
            m_fStickerAngle = Float.parseFloat(str2);
        }
        else if ( cmd.equals("captionandurl") )
        {
            m_sCaption = str1;
            m_sURL = str2;
        }
        else if ( cmd.equals("share") )
        {
            String image = str1;
            String sticker = str2;

            SnapCreativeKitApi snapCreativeKitApi = SnapCreative.getApi( act );
            SnapMediaFactory snapMediaFactory = SnapCreative.getMediaFactory( act );

            SnapPhotoFile photoFile;
            try {
                photoFile = snapMediaFactory.getSnapPhotoFromFile( new File(image) );
            } catch (SnapMediaSizeException e) {
                Log.e("SnapChat API", "Photo file is too large");
                return;
            }

            SnapPhotoContent snapPhotoContent = new SnapPhotoContent(photoFile);

            if ( sticker != null && !sticker.equals("") ) {
                SnapSticker snapSticker = null;
                try {
                    snapSticker = snapMediaFactory.getSnapStickerFromFile(new File(sticker));
                    snapSticker.setWidth(m_iStickerWidth);
                    snapSticker.setHeight(m_iStickerHeight);
                    snapSticker.setPosX(m_fStickerX); // between 0 and 1
                    snapSticker.setPosY(m_fStickerY);
                    snapSticker.setRotationDegreesClockwise(m_fStickerAngle); // degrees clockwise
                    snapPhotoContent.setSnapSticker(snapSticker);
                } catch (SnapStickerSizeException e) {
                    Log.e("SnapChat API", "Sticker file is too large");
                }
            }

            if ( m_sCaption != null && !m_sCaption.equals("") ) snapPhotoContent.setCaptionText( m_sCaption );
            if ( m_sURL != null && !m_sURL.equals("") ) snapPhotoContent.setAttachmentUrl( m_sURL );

            snapCreativeKitApi.send(snapPhotoContent);
        }
    }
}