package com.thegamecreators.agk_player;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.AlarmManager;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.ActivityNotFoundException;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.SharedPreferences;
import android.content.pm.ResolveInfo;
import android.content.res.Configuration;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.PendingIntent;
import android.content.Intent;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.Runnable;
import java.lang.String;
import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Collections;
import java.util.List;
import java.util.Locale;
import java.util.UUID;

import android.os.Build;
import android.os.Environment;
import android.os.StatFs;
import android.os.Vibrator;

import androidx.core.content.FileProvider;
import android.util.DisplayMetrics;
import android.util.Log;
import android.provider.MediaStore;
import android.provider.Settings.Secure;
import android.os.Looper;

import android.view.DisplayCutout;
import android.webkit.MimeTypeMap;
import android.view.InputDevice;
import android.view.View;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.media.MediaScannerConnection;
import android.content.DialogInterface;

// Used for agk::Message()
class RunnableMessage implements Runnable
{
	public Activity act;
	public String msg;
	
	public void run() {
        //Toast.makeText(act, msg, Toast.LENGTH_LONG).show();
		AlertDialog alertDialog;
		alertDialog = new AlertDialog.Builder(act).create();
		alertDialog.setTitle("Message");
		alertDialog.setMessage(msg);
		alertDialog.setButton( DialogInterface.BUTTON_POSITIVE, "OK", new DialogInterface.OnClickListener(){public void onClick(DialogInterface dialog, int which) {}});
		alertDialog.show();
    }
}

// Entry point for AGK Helper calls
public class AGKHelper
{
	public static Activity g_pAct = null;
	static int isVisible = 0;
	public static String g_sLastURI = null;

	public static void SetClipboardText( Activity act, String text )
	{
		Looper.prepare();

		ClipboardManager clipboard = (ClipboardManager) act.getSystemService( Context.CLIPBOARD_SERVICE );
		ClipData clip = ClipData.newPlainText( "Text", text );
		clipboard.setPrimaryClip( clip );
	}

	public static String GetClipboardText( Activity act )
	{
		Looper.prepare();

		ClipboardManager clipboard = (ClipboardManager) act.getSystemService(Context.CLIPBOARD_SERVICE);
		String pasteData;

		if (!(clipboard.hasPrimaryClip())) return "";
		//if (!(clipboard.getPrimaryClipDescription().hasMimeType(MIMETYPE_TEXT_PLAIN))) return "";

		ClipData.Item item = clipboard.getPrimaryClip().getItemAt(0);
		pasteData = item.getText().toString();
		if (pasteData != null) return pasteData;

		pasteData = item.coerceToText( act ).toString();
		if (pasteData != null) return pasteData;

		return "";
	}

	public static int IsDarkTheme( Activity act )
	{
		int mode = act.getResources().getConfiguration().uiMode & Configuration.UI_MODE_NIGHT_MASK;
		return (mode == Configuration.UI_MODE_NIGHT_YES) ? 1 : 0;
	}

	public static void MinimizeApp( Activity act )
	{
		act.moveTaskToBack(true);
	}

	public static int IsPinAppAvailable( Activity act )
	{
		if ( Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP ) return 1;
		else return 0;
	}

	public static void PinApp( Activity act, int enable )
	{
		if ( Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP )
		{
			if ( enable != 0 ) act.startLockTask();
			else act.stopLockTask();
		}
	}

	public static void QuitApp( Activity act )
	{
		act.finishAffinity();
	}

	public static void onStart( Activity act )
	{
		g_pAct = act;
		isVisible = 1;
	}

	public static void onStop( Activity act )
	{
		isVisible = 0;
	}

	public static String GetLastURIText()
	{
		return (g_sLastURI == null) ? "" : g_sLastURI;
	}

	public static void ClearLastURIText()
	{
		g_sLastURI = null;
	}

	public static String GetIP(Activity act)
	{
		//WifiManager wm = (WifiManager) act.getSystemService(Context.WIFI_SERVICE);
		//return Formatter.formatIpAddress(wm.getConnectionInfo().getIpAddress());

		try {
			List<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());
			for (NetworkInterface intf : interfaces) {
				if ( intf.isUp() )
				{
					List<InetAddress> addrs = Collections.list(intf.getInetAddresses());
					for (InetAddress addr : addrs) {
						if (!addr.isLoopbackAddress() && !addr.isLinkLocalAddress() && !addr.isMulticastAddress()) {
							if (addr instanceof Inet4Address)
							{
								String sAddr = addr.getHostAddress().toUpperCase();
								return sAddr;
							}
						}
					}
				}
			}
		}
		catch (Exception ex)
		{
			Log.e( "GetIP", "Failed to get IPv4 address " + ex.toString() );
		}

		return "";
	}

	public static String GetIPv6(Activity act)
	{
		String sAddr = "";
		try {
			List<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());
			int count = 0;
			int currLevel = 0;
			for (NetworkInterface intf : interfaces) {
				int index = 0;
				count++;
				if (Build.VERSION.SDK_INT >= 19 ) index = intf.getIndex();
				//Log.e( "IP", "Interface: " + index + ", name: " + intf.getName() + ", count: " + count );
				if ( intf.isUp() )
				{
					List<InetAddress> addrs = Collections.list(intf.getInetAddresses());
					for (InetAddress addr : addrs) {
						//Log.e( "IP", "  Addr: " + addr.getHostAddress() + ", name: " + addr.getHostName() );
						if (addr instanceof Inet6Address)
						{
							if (!addr.isLoopbackAddress() && !addr.isMulticastAddress())
							{
								String sIP = addr.getHostAddress().toLowerCase();
								int level = 0;
								if ( addr.isLinkLocalAddress() ) level = 1;
								else if ( addr.isSiteLocalAddress() ) level = 2;
								else if ( sIP.startsWith("fd") || sIP.startsWith("fc") ) level = 3; // unique site local
								else level = 4;

								//Log.e( "IP", "IP: " + sIP + ", Level: " + level );

								if ( level > currLevel ) {
									currLevel = level;
									sAddr = sIP;
									int percent = sAddr.indexOf('%');
									if (percent >= 0) sAddr = sAddr.substring(0, percent);
									sAddr = sAddr + "%" + intf.getName();
								}
							}
						}
					}
				}
			}
		}
		catch (Exception ex)
		{
			Log.e( "GetIP", "Failed to get IPv6 address " + ex.toString() );
		}

		return sAddr;
	}

	@SuppressLint("MissingPermission")
	public static void Vibrate(Activity act, float seconds )
	{
		if ( PermissionSDK.CheckRawPermission(act, Manifest.permission.VIBRATE) == 1)
		{
			Vibrator vib = (Vibrator) act.getSystemService(Context.VIBRATOR_SERVICE);
			vib.vibrate((int) (seconds * 1000));
		}
		else
		{
			Log.w("Vibrate", "Tried to vibrate the device without the vibrate permission");
		}
	}

	public static String UpdateInputDevices()
	{
		String returnValues = "";
		int ids[] = InputDevice.getDeviceIds();

		// find all device IDs that are joysticks
		for ( int i = 0; i < ids.length; i++ )
		{
			InputDevice device = InputDevice.getDevice( ids[i] );
			if ( (device.getSources() & InputDevice.SOURCE_CLASS_JOYSTICK) != 0 )
			{
				if ( returnValues.length() > 0 ) returnValues += ":";
				returnValues += Integer.toString(ids[i]);
			}
		}
		return returnValues;
	}

	public static void RefreshMediaPath( Activity act, String path )
	{
		MediaScannerConnection.scanFile(act, new String[]{path}, null, null);
	}

	// message box
	public static void ShowMessage( Activity act, String msg )
	{
		RunnableMessage run = new RunnableMessage();
		run.act = act;
		run.msg = msg;
		act.runOnUiThread(run);
	}

	// local notifications
	static NotificationChannel mNotificationChannel = null;
	public static void SetNotification( Activity act, int id, int unixtime, String message, String deeplink )
	{
		if (mNotificationChannel == null && android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O)
		{
			NotificationManager mNotificationManager = (NotificationManager) act.getSystemService(Context.NOTIFICATION_SERVICE);
			mNotificationChannel = new NotificationChannel("notify","Notifications", NotificationManager.IMPORTANCE_DEFAULT );
			mNotificationChannel.setDescription("App Notifications");
			mNotificationManager.createNotificationChannel(mNotificationChannel);
		}

		Intent intent = new Intent(act, NotificationAlarmReceiver.class);
		intent.putExtra("title", LocalApp.GetAppName(act) );
		intent.putExtra("message", message);
		intent.putExtra("id",id);
		intent.putExtra("deeplink",deeplink);

		int flags = PendingIntent.FLAG_UPDATE_CURRENT;
		if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M) flags |= PendingIntent.FLAG_IMMUTABLE;
		PendingIntent sender = PendingIntent.getBroadcast(act, id, intent, flags);

		// Get the AlarmManager service
		AlarmManager am = (AlarmManager) act.getSystemService(Context.ALARM_SERVICE);
		am.set(AlarmManager.RTC_WAKEUP, unixtime * 1000L, sender);
	}

	public static void SetNotification( Activity act, int id, int unixtime, String message )
	{
		SetNotification( act, id, unixtime, message, "" );
	}

	public static void CancelNotification( Activity act, int id )
	{
		Intent intent = new Intent(act, NotificationAlarmReceiver.class);

		int flags = PendingIntent.FLAG_UPDATE_CURRENT;
		if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M) flags |= PendingIntent.FLAG_IMMUTABLE;
		PendingIntent sender = PendingIntent.getBroadcast(act, id, intent, flags);

		// Get the AlarmManager service
		AlarmManager am = (AlarmManager) act.getSystemService(Context.ALARM_SERVICE);
		am.cancel(sender);
	}

	public static void SetOrientation( Activity act, int orien )
	{
		act.setRequestedOrientation(orien);
	}

	public static int GetOrientation( Activity act )
	{
		return act.getWindowManager().getDefaultDisplay().getRotation();
	}

	public static String GetDeviceID(Activity nativeactivityptr)
	{
		// This ID will remain constant for this device until a factory reset is performed
		String uuid = Secure.getString(nativeactivityptr.getContentResolver(), Secure.ANDROID_ID);
		if ( uuid == null || uuid.equals("") )
		{
			SharedPreferences sharedPrefs = nativeactivityptr.getSharedPreferences( "PREF_UNIQUE_ID", Context.MODE_PRIVATE);
			uuid = sharedPrefs.getString( "PREF_UNIQUE_ID", null);

			if (uuid == null || uuid.equals("")) {
				uuid = UUID.randomUUID().toString();
				SharedPreferences.Editor editor = sharedPrefs.edit();
				editor.putString("PREF_UNIQUE_ID", uuid);
				editor.commit();
			}
		}
		return uuid;
	}

	public static int GetDeviceDPI( Activity act )
	{
		DisplayMetrics metrics = act.getResources().getDisplayMetrics();
		return (int) ((metrics.xdpi + metrics.ydpi) / 2.0);
	}

	public static String GetPackageName( Activity act )
	{
		return act.getPackageName();
	}

	public static int GetPlatform()
	{
		if ( Build.MANUFACTURER.equals("Amazon") ) return 1;
		else return 0;
	}

	public static String GetAppName(Activity act)
	{
		final PackageManager pm = act.getApplicationContext().getPackageManager();

		ApplicationInfo ai;
		try
		{
		    ai = pm.getApplicationInfo( act.getPackageName(), 0);
		}
		catch (final NameNotFoundException e)
		{
		    ai = null;
		}
		final String applicationName = (String) (ai != null ? pm.getApplicationLabel(ai) : "unknown");
		return applicationName;
	}
	
	// image chooser code
	public static int iChoosingImage = 0;
    public static String sChosenImagePath = "";

	// Function to launch Choose Image intent
	public static void StartChooseImage(Activity act, String path)
	{
		if ( iChoosingImage == 1 ) return;
		sChosenImagePath = path;

		// Ensure we can create a new activity in this static function
		Looper.prepare();

		iChoosingImage = 1;
		Intent photoPickerIntent = new Intent(Intent.ACTION_GET_CONTENT);
		photoPickerIntent.setType("image/*");
		act.startActivityForResult(photoPickerIntent, 9005);
	}

	public static int ChooseImageResult() { return iChoosingImage; }
	
	// camera
	public static int iCapturingImage = 0; // 0=no image, 1=capturing, 2=got image
	public static String sCameraSavePath = "";
	public static void CaptureImage(Activity nativeactivityptr, String path)
	{
		if ( iCapturingImage == 1 ) return;
		sCameraSavePath = path;

		// Ensure we can create a new activity in this static function
		Looper.prepare();

		iCapturingImage = 1;
		Intent cameraIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
		nativeactivityptr.startActivityForResult( cameraIntent, 9004 );
    }

    public static int CaptureImageResult() { return iCapturingImage; }
	
	public static String GetLanguage()
	{
		return Locale.getDefault().getLanguage();
	}

	@SuppressLint("MissingPermission")
	public static int isNetworkAvailable( Activity act )
	{
		if ( PermissionSDK.CheckRawPermission(act, Manifest.permission.ACCESS_NETWORK_STATE) == 1 )
		{
			ConnectivityManager connectivityManager = (ConnectivityManager) act.getSystemService(Context.CONNECTIVITY_SERVICE);
			NetworkInfo activeNetworkInfo = connectivityManager.getActiveNetworkInfo();
			if ( activeNetworkInfo != null && activeNetworkInfo.isConnected() ) return 1;
			else return 0;
		}

		return 0;
	}

	@SuppressLint("MissingPermission")
	public static int GetNetworkType( Activity act )
	{
		if ( PermissionSDK.CheckRawPermission(act, Manifest.permission.ACCESS_NETWORK_STATE) == 1 )
		{
			ConnectivityManager connectivityManager = (ConnectivityManager) act.getSystemService(Context.CONNECTIVITY_SERVICE);
			NetworkInfo activeNetworkInfo = connectivityManager.getActiveNetworkInfo();
			if ( activeNetworkInfo == null ) return -1;
			switch( activeNetworkInfo.getType() ) {
				case ConnectivityManager.TYPE_MOBILE: return 0;
				case ConnectivityManager.TYPE_WIFI: return 1;
				case ConnectivityManager.TYPE_ETHERNET: return 1;
				default: return -1;
			}
		}

		return -1;
	}

	public static int GetStorageRemaining( Activity act, String path )
	{
		StatFs pathStats = new StatFs( path );
		if ( Build.VERSION.SDK_INT >= 18 ) return (int)(pathStats.getAvailableBytes()/1048576);
		else
		{
			int blockSize = pathStats.getBlockSize();
			long size = pathStats.getAvailableBlocks() * blockSize;
			return (int)(size/1048576);
		}
	}

	public static int GetStorageTotal( Activity act, String path )
	{
		StatFs pathStats = new StatFs( path );
		if ( Build.VERSION.SDK_INT >= 18 ) return (int)(pathStats.getTotalBytes()/1048576);
		else
		{
			int blockSize = pathStats.getBlockSize();
			long size = pathStats.getBlockCount() * blockSize;
			return (int)(size/1048576);
		}
	}
	
	// expansion files
	public static void setExpansionKey( String key )
	{

	}

	public static void setExpansionNotificationDescription( String name, String desc )
	{

	}
	
	public static void SetExpansionVersion(int version)
	{

	}
	
	public static int GetExpansionState(Activity act)
	{
		return 1;
	}

	public static int GetExpansionError(Activity act)
	{
		return 0;
	}
	
	public static void DownloadExpansion(Activity act)
	{

	}
	
	public static float GetExpansionProgress(Activity act)
	{
		return 0;
	}

	public static int GetExpansionFileExists(Activity act, String filename)
	{
		return 0;
	}

	public static int ExtractExpansionFileImage(Activity act, String filename, String newPath )
	{
		return 0;
	}

	public static int GetAPIVersion() { return Build.VERSION.SDK_INT; }

	// Shared variables
	public static void SaveSharedVariable( Activity act, String varName, String varValue )
	{
		// write local value to the shared preferences, no longer support sharing across devices
		SharedPreferences sharedPref = act.getSharedPreferences("agksharedvariables", Context.MODE_PRIVATE);
		SharedPreferences.Editor edit = sharedPref.edit();
		edit.putString( varName, varValue );
		edit.apply();
	}

	public static String LoadSharedVariable( Activity act, String varName, String defaultValue )
	{
		// read from shared preferences
		SharedPreferences sharedPref = act.getSharedPreferences("agksharedvariables", Context.MODE_PRIVATE);
		return sharedPref.getString(varName, defaultValue);
	}

	public static void DeleteSharedVariable( Activity act, String varName )
	{
		// delete any local value
		SharedPreferences sharedPref = act.getSharedPreferences( "agksharedvariables", Context.MODE_PRIVATE );
		SharedPreferences.Editor edit = sharedPref.edit();
		edit.remove(varName);
		edit.apply();
	}

	public static void ViewFile( Activity  act, String sPath )
	{
		int pos = sPath.lastIndexOf('/');
		String sFileName;
		if ( pos >= 0 ) sFileName = sPath.substring(pos+1);
		else sFileName = sPath;

		// get extension
		pos = sPath.lastIndexOf('.');
		String sExt = "";
		if ( pos >= 0 ) sExt = sPath.substring(pos+1);

		File src = new File(sPath);

		String sMIME = MimeTypeMap.getSingleton().getMimeTypeFromExtension(sExt);
		Uri uri = FileProvider.getUriForFile(act, act.getApplicationContext().getPackageName() + ".fileprovider", src);

		Intent target = new Intent( Intent.ACTION_VIEW );
		target.setDataAndType( uri, sMIME );
		target.setFlags( Intent.FLAG_ACTIVITY_NO_HISTORY | Intent.FLAG_GRANT_READ_URI_PERMISSION );

		try {
			act.startActivity(target);
		} catch (ActivityNotFoundException e) {
			ShowMessage(act,"No application found for file type \"" + sExt + "\"");
		}
	}

	public static void ShareText( Activity  act, String sText )
	{
		Intent target = new Intent( Intent.ACTION_SEND );
		target.setType( "text/plain" );
		target.putExtra( Intent.EXTRA_TITLE, "" );
		target.putExtra( Intent.EXTRA_TEXT, sText );

		try {
			act.startActivity( Intent.createChooser(target,"") );
		} catch (ActivityNotFoundException e) {
			ShowMessage(act,"No application found to share text");
		}
	}

	public static void ShareImage( Activity act, String sPath )
	{
		int pos = sPath.lastIndexOf('/');
		String sFileName;
		if ( pos >= 0 ) sFileName = sPath.substring(pos+1);
		else sFileName = sPath;

		File src = new File(sPath);

		Uri uri = FileProvider.getUriForFile(act, act.getApplicationContext().getPackageName() + ".fileprovider", src);

		Intent target = new Intent( Intent.ACTION_SEND );
		target.setType( "image/*" );
		target.putExtra( Intent.EXTRA_TITLE, "" );
		target.putExtra( Intent.EXTRA_STREAM, uri );
		target.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);

		try {
			Intent chooser = Intent.createChooser(target,"");
			chooser.setFlags( Intent.FLAG_ACTIVITY_NO_HISTORY );

			List<ResolveInfo> resInfoList = act.getPackageManager().queryIntentActivities(chooser, PackageManager.MATCH_DEFAULT_ONLY);
			for (ResolveInfo resolveInfo : resInfoList) {
				String packageName = resolveInfo.activityInfo.packageName;
				act.grantUriPermission(packageName, uri, Intent.FLAG_GRANT_READ_URI_PERMISSION);
			}
			act.startActivity( chooser );
		} catch (ActivityNotFoundException e) {
			ShowMessage(act,"No application found to share images");
		}
	}

	public static void ShareImageAndText( Activity act, String sPath, String sText )
	{
		int pos = sPath.lastIndexOf('/');
		String sFileName;
		if ( pos >= 0 ) sFileName = sPath.substring(pos+1);
		else sFileName = sPath;

		File src = new File(sPath);
		Uri uri = FileProvider.getUriForFile(act, act.getApplicationContext().getPackageName() + ".fileprovider", src);

		Intent target = new Intent( Intent.ACTION_SEND );
		target.setType( "image/*" );
		target.putExtra(Intent.EXTRA_TITLE, "");
		target.putExtra( Intent.EXTRA_STREAM, uri );
		target.putExtra( Intent.EXTRA_TEXT, sText );
		target.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);

		try {
			Intent chooser = Intent.createChooser(target,"");
			chooser.setFlags( Intent.FLAG_ACTIVITY_NO_HISTORY );

			List<ResolveInfo> resInfoList = act.getPackageManager().queryIntentActivities(chooser, PackageManager.MATCH_DEFAULT_ONLY);
			for (ResolveInfo resolveInfo : resInfoList) {
				String packageName = resolveInfo.activityInfo.packageName;
				act.grantUriPermission(packageName, uri, Intent.FLAG_GRANT_READ_URI_PERMISSION);
			}
			act.startActivity( chooser );
		} catch (ActivityNotFoundException e) {
			ShowMessage(act,"No application found to share images");
		}
	}

	public static void ShareFile( Activity act, String sPath )
	{
		int pos = sPath.lastIndexOf('/');
		String sFileName;
		if ( pos >= 0 ) sFileName = sPath.substring(pos+1);
		else sFileName = sPath;

		// get extension
		pos = sPath.lastIndexOf('.');
		String sExt = "";
		if ( pos >= 0 ) sExt = sPath.substring(pos+1);

		File src = new File(sPath);

		String sMIME = MimeTypeMap.getSingleton().getMimeTypeFromExtension(sExt);
		Uri uri = FileProvider.getUriForFile(act, act.getApplicationContext().getPackageName() + ".fileprovider", src);

		Intent target = new Intent( Intent.ACTION_SEND );
		target.setType( sMIME );
		target.putExtra( Intent.EXTRA_STREAM, uri );
		target.setFlags( Intent.FLAG_ACTIVITY_NO_HISTORY | Intent.FLAG_GRANT_READ_URI_PERMISSION );

		try {
			Intent chooser = Intent.createChooser(target,"");
			chooser.setFlags( Intent.FLAG_ACTIVITY_NO_HISTORY );

			List<ResolveInfo> resInfoList = act.getPackageManager().queryIntentActivities(chooser, PackageManager.MATCH_DEFAULT_ONLY);
			for (ResolveInfo resolveInfo : resInfoList) {
				String packageName = resolveInfo.activityInfo.packageName;
				act.grantUriPermission(packageName, uri, Intent.FLAG_GRANT_READ_URI_PERMISSION);
			}
			act.startActivity( chooser );
		} catch (ActivityNotFoundException e) {
			ShowMessage(act,"No application found to share file type \"" + sExt + "\"");
		}
	}

	public static String GetExternalDir()
	{
		if ( Build.VERSION.SDK_INT >= 29 ) // Android 10 or above
		{
			return g_pAct.getExternalFilesDir(null ).getAbsolutePath();
		}
		else
		{
			return Environment.getExternalStorageDirectory().getAbsolutePath();
		}
	}

	public static int GetPackageInstalled( Activity act, String packageName )
	{
		try {
			if ( act.getPackageManager().getApplicationInfo(packageName, 0).enabled ) return 1;
			else return 0;
		} catch (PackageManager.NameNotFoundException e) {
			return 0;
		}
	}

	public static int GetDisplayNumCutouts( Activity act )
	{
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
		{
			DisplayCutout cutout =  act.getWindow().getDecorView().getRootWindowInsets().getDisplayCutout();
			if ( cutout == null ) return 0;
			return cutout.getBoundingRects().size();
		}
		else
		{
			return 0;
		}
	}

	public static float GetDisplayCutoutBound( Activity act, int index, int side )
	{
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
		{
			DisplayCutout cutout =  act.getWindow().getDecorView().getRootWindowInsets().getDisplayCutout();
			if ( cutout == null ) return 0;
			int numCutouts = cutout.getBoundingRects().size();
			if ( numCutouts == 0) return 0;
			if ( index < 0 || index >= numCutouts ) return 0;

			switch( side )
			{
				case 1: return cutout.getBoundingRects().get( index ).top;
				case 2: return cutout.getBoundingRects().get( index ).bottom;
				case 3: return cutout.getBoundingRects().get( index ).left;
				case 4: return cutout.getBoundingRects().get( index ).right;
			}
		}

		return 0;
	}

	public static float GetDisplaySafeBound( Activity act, int side )
	{
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
		{
			DisplayCutout cutout =  act.getWindow().getDecorView().getRootWindowInsets().getDisplayCutout();
			if ( cutout == null ) return -10000;

			switch( side )
			{
				case 1: return cutout.getSafeInsetTop();
				case 2: return cutout.getSafeInsetBottom();
				case 3: return cutout.getSafeInsetLeft();
				case 4: return cutout.getSafeInsetRight();
			}
		}

		return -10000;
	}
}
