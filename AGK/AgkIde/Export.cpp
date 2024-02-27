
/*
Keystore Details
A keystore is a container file for a private key that is used to sign your app so that it can be submitted to the Google Play or Amazon store. If you are distributing outside either of these app stores, or you just want to test on your device, then you do not need this.

Once you have submitted your signed app, all updates to it must be signed with the same keystore. Keystores are protected with a password, if you lose your keystore file or forget its password then &lt;b&gt;you will not be able to submit updates to any apps that were signed with it&lt;/b&gt;.

Each of your apps could use a separate keystore, but for simplicity it is easier to have a single keystore for all your apps. For those of you that want to use this keystore with other programs the alias name will be "mykeystore" and the alias password will be the same as the one you type below.

To generate a keystore file you need to submit some information about you or your company that will tell people who signed it. You may leave these blank (except the password) but anyone inspecting the key used to sign an app may be less likely to trust it if all the fields are blank.

A-Z and space only.

MAX 30 each.



<object class="GtkEntry" id="keystore_full_name_entry">
<property name="width_request">200</property>

<object class="GtkEntry" id="keystore_company_name_entry">
<property name="width_request">200</property>
<property name="tooltip_text" translatable="yes">A-Z and spaces only</property>

<object class="GtkEntry" id="keystore_city_entry">
<property name="width_request">200</property>

<object class="GtkEntry" id="keystore_country_entry">
<property name="width_request">50</property>


<object class="GtkEntry" id="keystore_password1_entry">
<property name="width_request">200</property>

<object class="GtkEntry" id="keystore_password2_entry">
<property name="width_request">200</property>


Output File Location: [release.keystore] ...


*/

void project_generate_keystore()
{


	if (ui_widgets.keystore_dialog == NULL)
	{
		ui_widgets.keystore_dialog = create_keystore_dialog();
		gtk_widget_set_name(ui_widgets.keystore_dialog, _("Generate Keystore"));
		gtk_window_set_transient_for(GTK_WINDOW(ui_widgets.keystore_dialog), GTK_WINDOW(main_widgets.window));

		g_signal_connect(ui_widgets.keystore_dialog, "response", G_CALLBACK(on_keystore_dialog_response), NULL);
		g_signal_connect(ui_widgets.keystore_dialog, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);

		ui_setup_open_button_callback_keystore(ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_output_file_path"), NULL,
			GTK_FILE_CHOOSER_ACTION_SAVE, GTK_ENTRY(ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_output_file_entry")));
	}

	GtkWidget *widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_output_file_entry");
	const gchar *output_file = gtk_entry_get_text(GTK_ENTRY(widget));
	if (!output_file || !*output_file)
	{
		gchar* out_path = g_build_filename(global_project_prefs.project_file_path, "release.keystore", NULL);
		gtk_entry_set_text(GTK_ENTRY(ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_output_file_entry")), out_path);
		g_free(out_path);
	}

	gtk_window_present(GTK_WINDOW(ui_widgets.keystore_dialog));
}
static void on_keystore_dialog_response(GtkDialog *dialog, gint response, gpointer user_data)
{
	static int running = 0;
	if (running) return;

	running = 1;

	if (response != 1)
	{
		gtk_widget_hide(GTK_WIDGET(dialog));
	}
	else
	{
		int i;
		GtkWidget *widget;

		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.keystore_dialog, "button9"), FALSE);
		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.keystore_dialog, "button8"), FALSE);

		// keystore details
		widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_full_name_entry");
		gchar *full_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_company_name_entry");
		gchar *company_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_city_entry");
		gchar *city = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_country_entry");
		gchar *country = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_password1_entry");
		gchar *password1 = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_password2_entry");
		gchar *password2 = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		// output
		widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_output_file_entry");
		gchar *output_file = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		// START CHECKS

		if (!output_file || !*output_file) { SHOW_ERR(_("You must choose an output location to save your keystore file")); goto keystore_dialog_clean_up; }

		if (g_file_test(output_file, G_FILE_TEST_EXISTS))
		{
			if (!dialogs_show_question(_("\"%s\" already exists. Do you want to overwrite it?"), output_file))
			{
				goto keystore_dialog_clean_up;
			}
		}

		// check full name
		if (strlen(full_name) > 30) { SHOW_ERR(_("Full name must be less than 30 characters")); goto keystore_dialog_clean_up; }
		for (i = 0; i < strlen(full_name); i++)
		{
			if ((full_name[i] < 97 || full_name[i] > 122)
				&& (full_name[i] < 65 || full_name[i] > 90)
				&& full_name[i] != 32)
			{
				SHOW_ERR(_("Full name contains invalid characters, must be A-Z and spaces only"));
				goto keystore_dialog_clean_up;
			}
		}
		if (!*full_name)
		{
			g_free(full_name);
			full_name = g_strdup("Unknown");
		}

		// check company name
		if (strlen(company_name) > 30) { SHOW_ERR(_("Company name must be less than 30 characters")); goto keystore_dialog_clean_up; }
		for (i = 0; i < strlen(company_name); i++)
		{
			if ((company_name[i] < 97 || company_name[i] > 122)
				&& (company_name[i] < 65 || company_name[i] > 90)
				&& company_name[i] != 32)
			{
				SHOW_ERR(_("Company name contains invalid characters, must be A-Z and spaces only"));
				goto keystore_dialog_clean_up;
			}
		}
		if (!*company_name)
		{
			g_free(company_name);
			company_name = g_strdup("Unknown");
		}

		// city
		if (strlen(city) > 30) { SHOW_ERR(_("City must be less than 30 characters")); goto keystore_dialog_clean_up; }
		for (i = 0; i < strlen(city); i++)
		{
			if ((city[i] < 97 || city[i] > 122)
				&& (city[i] < 65 || city[i] > 90)
				&& city[i] != 32)
			{
				SHOW_ERR(_("City contains invalid characters, must be A-Z and spaces only"));
				goto keystore_dialog_clean_up;
			}
		}
		if (!*city)
		{
			g_free(city);
			city = g_strdup("Unknown");
		}

		// country
		if (strlen(country) > 0 && strlen(country) != 2) { SHOW_ERR(_("Country code must be 2 characters")); goto keystore_dialog_clean_up; }
		for (i = 0; i < strlen(country); i++)
		{
			if ((country[i] < 97 || country[i] > 122)
				&& (country[i] < 65 || country[i] > 90))
			{
				SHOW_ERR(_("Country code contains invalid characters, must be A-Z only"));
				goto keystore_dialog_clean_up;
			}
		}
		if (!*country)
		{
			g_free(country);
			country = g_strdup("Unknown");
		}

		// check passwords
		if (!password1 || !*password1) { SHOW_ERR(_("Password cannot be blank")); goto keystore_dialog_clean_up; }
		if (strlen(password1) < 6) { SHOW_ERR(_("Password must be at least 6 characters long")); goto keystore_dialog_clean_up; }
		if (strchr(password1, '"')) { SHOW_ERR(_("Password cannot contain double quotes")); goto keystore_dialog_clean_up; }
		if (strcmp(password1, password2) != 0) { SHOW_ERR(_("Passwords do not match")); goto keystore_dialog_clean_up; }

		goto keystore_dialog_continue;

	keystore_dialog_clean_up:
		if (full_name) g_free(full_name);
		if (company_name) g_free(company_name);
		if (city) g_free(city);
		if (country) g_free(country);
		if (password1) g_free(password1);
		if (password2) g_free(password2);

		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.keystore_dialog, "button8"), TRUE);
		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.keystore_dialog, "button9"), TRUE);
		running = 0;
		return;

	keystore_dialog_continue:

		;

		// CHECKS COMPLETE, START KEY GENERATION

#ifdef G_OS_WIN32
		gchar* path_to_keytool = g_build_path("/", app->datadir, "android", "jre", "bin", "keytool.exe", NULL);
#else
		//gchar* path_to_jarsigner = g_build_path( "/", "/usr", "bin", "keytool", NULL );
		gchar* path_to_keytool = g_build_path("/", app->datadir, "android", "jre", "bin", "keytool", NULL);
#endif

		// decalrations
		gchar **argv = NULL;
		gchar *dname = NULL;
		int status = 0;
		GError *error = 0;
		gchar *keystore_name = NULL;
		gchar* str_out = 0;

		utils_str_replace_char(output_file, '\\', '/');
		gchar* slash = strrchr(output_file, '/');
		if (slash)
		{
			keystore_name = g_strdup(slash + 1);
			*slash = 0;
		}
		else
		{
			keystore_name = g_strdup(output_file);
			g_free(output_file);
			output_file = global_project_prefs.project_file_path;
		}

		if (!g_file_test(path_to_keytool, G_FILE_TEST_EXISTS))
		{
			SHOW_ERR1(_("Could not find keytool program, the path \"%s\" is incorrect"), path_to_keytool);
			goto keystore_dialog_cleanup2;
		}

		dname = g_strdup_printf("CN=%s, O=%s, L=%s, C=%s", full_name, company_name, city, country);

		// package manifest and resources
		argv = g_new0(gchar*, 19);
		argv[0] = g_strdup(path_to_keytool);
		argv[1] = g_strdup("-genkey");
		argv[2] = g_strdup("-keystore");
		argv[3] = g_strdup(keystore_name);
		argv[4] = g_strdup("-alias");
		argv[5] = g_strdup("mykeystore");
		argv[6] = g_strdup("-keyalg");
		argv[7] = g_strdup("RSA");
		argv[8] = g_strdup("-keysize");
		argv[9] = g_strdup("2048");
		argv[10] = g_strdup("-validity");
		argv[11] = g_strdup("20000");
		argv[12] = g_strdup("-storepass");
		argv[13] = g_strdup(password1);
		argv[14] = g_strdup("-keypass");
		argv[15] = g_strdup(password1);
		argv[16] = g_strdup("-dname");
		argv[17] = g_strdup(dname);
		argv[18] = NULL;

		if (!utils_spawn_sync(output_file, argv, NULL, 0, NULL, NULL, &str_out, NULL, &status, &error))
		{
			SHOW_ERR1(_("Failed to run keytool program: %s"), error->message);
			g_error_free(error);
			error = NULL;
			goto keystore_dialog_cleanup2;
		}

		if (status != 0 && str_out && *str_out)
		{
			SHOW_ERR1(_("keytool program returned error: %s"), str_out);
			goto keystore_dialog_cleanup2;
		}

		gtk_widget_hide(GTK_WIDGET(dialog));

	keystore_dialog_cleanup2:

		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.keystore_dialog, "button8"), TRUE);
		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.keystore_dialog, "button9"), TRUE);

		if (error) g_error_free(error);

		if (path_to_keytool) g_free(path_to_keytool);
		if (argv) g_strfreev(argv);
		if (dname) g_free(dname);
		if (keystore_name) g_free(keystore_name);
		if (str_out) g_free(str_out);

		if (full_name) g_free(full_name);
		if (company_name) g_free(company_name);
		if (city) g_free(city);
		if (country) g_free(country);
		if (password1) g_free(password1);
		if (password2) g_free(password2);
	}

	running = 0;
}


static void on_android_dialog_response(GtkDialog *dialog, gint response, gpointer user_data)
{
	static int running = 0;
	if (running) return;

	running = 1;

	// save default settings
	if (app->project && user_data == 0)
	{
		GtkWidget *widget;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_app_name_entry");
		AGK_CLEAR_STR(app->project->apk_settings.app_name) = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_package_name_entry");
		AGK_CLEAR_STR(app->project->apk_settings.package_name) = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_app_icon_entry");
		AGK_CLEAR_STR(app->project->apk_settings.app_icon_path) = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_notif_icon_entry");
		AGK_CLEAR_STR(app->project->apk_settings.notif_icon_path) = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_ouya_icon_entry");
		AGK_CLEAR_STR(app->project->apk_settings.ouya_icon_path) = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_firebase_config_entry");
		AGK_CLEAR_STR(app->project->apk_settings.firebase_config_path) = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_orientation_combo");
		app->project->apk_settings.orientation = gtk_combo_box_get_active(GTK_COMBO_BOX_TEXT(widget));;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_arcore_combo");
		app->project->apk_settings.arcore = gtk_combo_box_get_active(GTK_COMBO_BOX_TEXT(widget));;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_sdk_combo");
		gchar *app_sdk = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
		app->project->apk_settings.sdk_version = 1; // 4.0.3
		if (strncmp(app_sdk, "4.1", 3) == 0) app->project->apk_settings.sdk_version = 2;
		if (strncmp(app_sdk, "4.2", 3) == 0) app->project->apk_settings.sdk_version = 3;
		if (strncmp(app_sdk, "4.3", 3) == 0) app->project->apk_settings.sdk_version = 4;
		if (strncmp(app_sdk, "4.4", 3) == 0) app->project->apk_settings.sdk_version = 5;
		if (strncmp(app_sdk, "5.0", 3) == 0) app->project->apk_settings.sdk_version = 6;
		if (strncmp(app_sdk, "5.1", 3) == 0) app->project->apk_settings.sdk_version = 7;
		if (strncmp(app_sdk, "6.0", 3) == 0) app->project->apk_settings.sdk_version = 8;
		if (strncmp(app_sdk, "7.0", 3) == 0) app->project->apk_settings.sdk_version = 9;
		if (strncmp(app_sdk, "7.1", 3) == 0) app->project->apk_settings.sdk_version = 10;
		if (strncmp(app_sdk, "8.0", 3) == 0) app->project->apk_settings.sdk_version = 11;
		g_free(app_sdk);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_url_scheme");
		AGK_CLEAR_STR(app->project->apk_settings.url_scheme) = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_deep_link");
		AGK_CLEAR_STR(app->project->apk_settings.deep_link) = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_google_play_app_id");
		AGK_CLEAR_STR(app->project->apk_settings.play_app_id) = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		// permissions
		app->project->apk_settings.permission_flags = 0;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_external_storage");
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) app->project->apk_settings.permission_flags |= AGK_ANDROID_PERMISSION_WRITE;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_location_fine");
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) app->project->apk_settings.permission_flags |= AGK_ANDROID_PERMISSION_GPS;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_location_coarse");
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) app->project->apk_settings.permission_flags |= AGK_ANDROID_PERMISSION_LOCATION;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_internet");
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) app->project->apk_settings.permission_flags |= AGK_ANDROID_PERMISSION_INTERNET;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_wake");
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) app->project->apk_settings.permission_flags |= AGK_ANDROID_PERMISSION_WAKE;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_billing");
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) app->project->apk_settings.permission_flags |= AGK_ANDROID_PERMISSION_IAP;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_push_notifications");
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) app->project->apk_settings.permission_flags |= AGK_ANDROID_PERMISSION_PUSH;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_camera");
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) app->project->apk_settings.permission_flags |= AGK_ANDROID_PERMISSION_CAMERA;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_expansion");
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) app->project->apk_settings.permission_flags |= AGK_ANDROID_PERMISSION_EXPANSION;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_vibrate");
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) app->project->apk_settings.permission_flags |= AGK_ANDROID_PERMISSION_VIBRATE;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_record_audio");
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) app->project->apk_settings.permission_flags |= AGK_ANDROID_PERMISSION_RECORD_AUDIO;

		widget = ui_lookup_widget ( ui_widgets.android_dialog, "android_permission_adverts" );
		if ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( widget ) ) ) app->project->apk_settings.permission_flags |= AGK_ANDROID_PERMISSION_ADVERTS;

		// signing
		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_keystore_file_entry");
		AGK_CLEAR_STR(app->project->apk_settings.keystore_path) = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_version_number_entry");
		AGK_CLEAR_STR(app->project->apk_settings.version_name) = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_build_number_entry");
		int build_number = atoi(gtk_entry_get_text(GTK_ENTRY(widget)));
		app->project->apk_settings.version_number = build_number;

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_alias_entry");
		AGK_CLEAR_STR(app->project->apk_settings.alias) = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		// output
		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_output_file_entry");
		AGK_CLEAR_STR(app->project->apk_settings.output_path) = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_output_type_combo");
		app->project->apk_settings.app_type = gtk_combo_box_get_active(GTK_COMBO_BOX_TEXT(widget));
	}

	if (response != 1)
	{
		if (dialog) gtk_widget_hide(GTK_WIDGET(dialog));
	}
	else
	{
		int i;
		GtkWidget *widget;

		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.android_dialog, "android_export1"), FALSE);
		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.android_dialog, "button7"), FALSE);

		while (gtk_events_pending())
			gtk_main_iteration();

		// app details
		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_app_name_entry");
		gchar *app_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_package_name_entry");
		gchar *package_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_app_icon_entry");
		gchar *app_icon = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_notif_icon_entry");
		gchar *notif_icon = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_ouya_icon_entry");
		gchar *ouya_icon = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_firebase_config_entry");
		gchar *firebase_config = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_orientation_combo");
		int app_orientation_int = gtk_combo_box_get_active(GTK_COMBO_BOX_TEXT(widget));
		int orientation = 10;
		if (app_orientation_int == 0) orientation = 6;
		else if (app_orientation_int == 1) orientation = 7;
		gchar szOrientation[20];
		sprintf(szOrientation, "%d", orientation);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_arcore_combo");
		int arcore_mode = gtk_combo_box_get_active(GTK_COMBO_BOX_TEXT(widget));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_sdk_combo");
		gchar *app_sdk = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
		int sdk = 10;
		if (strncmp(app_sdk, "4.0.3", 5) == 0) sdk = 15;
		if (strncmp(app_sdk, "4.1", 3) == 0) sdk = 16;
		if (strncmp(app_sdk, "4.2", 3) == 0) sdk = 17;
		if (strncmp(app_sdk, "4.3", 3) == 0) sdk = 18;
		if (strncmp(app_sdk, "4.4", 3) == 0) sdk = 19;
		if (strncmp(app_sdk, "5.0", 3) == 0) sdk = 21;
		if (strncmp(app_sdk, "5.1", 3) == 0) sdk = 22;
		if (strncmp(app_sdk, "6.0", 3) == 0) sdk = 23;
		if (strncmp(app_sdk, "7.0", 3) == 0) sdk = 24;
		if (strncmp(app_sdk, "7.1", 3) == 0) sdk = 25;
		if (strncmp(app_sdk, "8.0", 3) == 0) sdk = 26;
		g_free(app_sdk);
		gchar szSDK[20];
		sprintf(szSDK, "%d", sdk);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_url_scheme");
		gchar *url_scheme = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_deep_link");
		gchar *deep_link = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_google_play_app_id");
		gchar *google_play_app_id = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		// permissions
		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_external_storage");
		int permission_external_storage = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_location_fine");
		int permission_location_fine = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_location_coarse");
		int permission_location_coarse = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_internet");
		int permission_internet = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_wake");
		int permission_wake = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_billing");
		int permission_billing = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_push_notifications");
		int permission_push = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_camera");
		int permission_camera = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_expansion");
		int permission_expansion = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_vibrate");
		int permission_vibrate = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_record_audio");
		int permission_record_audio = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

		// signing
		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_keystore_file_entry");
		gchar *keystore_file = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_keystore_password_entry");
		gchar *keystore_password = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_version_number_entry");
		gchar *version_number = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));
		if (!*version_number) SETPTR(version_number, g_strdup("1.0.0"));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_build_number_entry");
		int build_number = atoi(gtk_entry_get_text(GTK_ENTRY(widget)));
		if (build_number == 0) build_number = 1;
		gchar szBuildNum[20];
		sprintf(szBuildNum, "%d", build_number);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_alias_entry");
		gchar *alias_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_alias_password_entry");
		gchar *alias_password = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		// output
		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_output_file_entry");
		gchar *output_file = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_output_type_combo");
		gchar *output_type = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
		int app_type = gtk_combo_box_get_active(GTK_COMBO_BOX_TEXT(widget));

		gchar *percent = 0;
		while ((percent = strchr(output_file, '%')) != 0)
		{
			if (strncmp(percent + 1, "[version]", strlen("[version]")) == 0)
			{
				*percent = 0;
				percent += strlen("[version]") + 1;
				gchar *new_output = g_strconcat(output_file, szBuildNum, percent, NULL);
				g_free(output_file);
				output_file = new_output;
				continue;
			}

			if (strncmp(percent + 1, "[type]", strlen("[type]")) == 0)
			{
				*percent = 0;
				percent += strlen("[type]") + 1;
				gchar *new_output = g_strconcat(output_file, output_type, percent, NULL);
				g_free(output_file);
				output_file = new_output;
				continue;
			}

			break;
		}

		g_free(output_type);

		// START CHECKS

		if (!output_file || !*output_file) { SHOW_ERR(_("You must choose an output location to save your APK")); goto android_dialog_clean_up; }
		if (strchr(output_file, '.') == 0) { SHOW_ERR(_("The output location must be a file not a directory")); goto android_dialog_clean_up; }

		// check app name
		if (!app_name || !*app_name) { SHOW_ERR(_("You must enter an app name")); goto android_dialog_clean_up; }
		if (strlen(app_name) > 30) { SHOW_ERR(_("App name must be less than 30 characters")); goto android_dialog_clean_up; }
		for (i = 0; i < strlen(app_name); i++)
		{
			/*
			if ( (app_name[i] < 97 || app_name[i] > 122)
			&& (app_name[i] < 65 || app_name[i] > 90)
			&& (app_name[i] < 48 || app_name[i] > 57)
			&& app_name[i] != 32
			&& app_name[i] != 45
			&& app_name[i] != 95 )
			{
			SHOW_ERR(_("App name contains invalid characters, must be A-Z, 0-9, dash, spaces, and undersore only"));
			goto android_dialog_clean_up;
			}
			*/
			//switch to black list
			if (app_name[i] == 34 || app_name[i] == 60 || app_name[i] == 62 || app_name[i] == 39)
			{
				SHOW_ERR(_("App name contains invalid characters, it must not contain quotes or < > characters."));
				goto android_dialog_clean_up;
			}
		}

		// check package name
		if (!package_name || !*package_name) { SHOW_ERR(_("You must enter a package name")); goto android_dialog_clean_up; }
		if (strlen(package_name) > 100) { SHOW_ERR(_("Package name must be less than 100 characters")); goto android_dialog_clean_up; }
		if (strchr(package_name, '.') == NULL) { SHOW_ERR(_("Package name must contain at least one dot character")); goto android_dialog_clean_up; }
		if ((package_name[0] < 65 || package_name[0] > 90) && (package_name[0] < 97 || package_name[0] > 122)) { SHOW_ERR(_("Package name must begin with a letter")); goto android_dialog_clean_up; }
		if (package_name[strlen(package_name) - 1] == '.') { SHOW_ERR(_("Package name must not end with a dot")); goto android_dialog_clean_up; }

		gchar last = 0;
		for (i = 0; i < strlen(package_name); i++)
		{
			if (last == '.' && (package_name[i] < 65 || package_name[i] > 90) && (package_name[i] < 97 || package_name[i] > 122))
			{
				SHOW_ERR(_("Package name invalid, a dot must be followed by a letter"));
				goto android_dialog_clean_up;
			}

			if ((package_name[i] < 97 || package_name[i] > 122) // a-z
				&& (package_name[i] < 65 || package_name[i] > 90) // A-Z
				&& (package_name[i] < 48 || package_name[i] > 57) //0-9
				&& package_name[i] != 46 // .
				&& package_name[i] != 95) // _
			{
				SHOW_ERR(_("Package name contains invalid characters, must be A-Z 0-9 . and undersore only"));
				goto android_dialog_clean_up;
			}

			last = package_name[i];
		}

		if (url_scheme && *url_scheme)
		{
			if (strchr(url_scheme, ':') || strchr(url_scheme, '/'))
			{
				SHOW_ERR(_("URL scheme must not contain : or /"));
				goto android_dialog_clean_up;
			}
		}

		if (deep_link && *deep_link)
		{
			if (strncmp(deep_link, "https://", strlen("https://")) != 0 && strncmp(deep_link, "http://", strlen("http://")) != 0)
			{
				SHOW_ERR(_("Deep link must start with http:// or https://"));
				goto android_dialog_clean_up;
			}

			if (strcmp(deep_link, "https://") == 0 || strcmp(deep_link, "http://") == 0)
			{
				SHOW_ERR(_("Deep link must have a domain after http:// or https://"));
				goto android_dialog_clean_up;
			}
		}

		// check icon
		//if ( !app_icon || !*app_icon ) { SHOW_ERR(_("You must select an app icon")); goto android_dialog_clean_up; }
		if (app_icon && *app_icon)
		{
			if (!strrchr(app_icon, '.') || utils_str_casecmp(strrchr(app_icon, '.'), ".png") != 0) { SHOW_ERR(_("App icon must be a PNG file")); goto android_dialog_clean_up; }
			if (!g_file_test(app_icon, G_FILE_TEST_EXISTS)) { SHOW_ERR(_("Could not find app icon location")); goto android_dialog_clean_up; }
		}

		if (notif_icon && *notif_icon)
		{
			if (!strrchr(notif_icon, '.') || utils_str_casecmp(strrchr(notif_icon, '.'), ".png") != 0) { SHOW_ERR(_("Notification icon must be a PNG file")); goto android_dialog_clean_up; }
			if (!g_file_test(notif_icon, G_FILE_TEST_EXISTS)) { SHOW_ERR(_("Could not find notification icon location")); goto android_dialog_clean_up; }
		}

		if (app_type == 2)
		{
			if (!ouya_icon || !*ouya_icon) { SHOW_ERR(_("You must select an Ouya large icon")); goto android_dialog_clean_up; }
			if (!strrchr(ouya_icon, '.') || utils_str_casecmp(strrchr(ouya_icon, '.'), ".png") != 0) { SHOW_ERR(_("Ouya large icon must be a PNG file")); goto android_dialog_clean_up; }
			if (!g_file_test(ouya_icon, G_FILE_TEST_EXISTS)) { SHOW_ERR(_("Could not find ouya large icon location")); goto android_dialog_clean_up; }
		}

		// check firebase config file
		if (firebase_config && *firebase_config)
		{
			if (!strrchr(firebase_config, '.') || utils_str_casecmp(strrchr(firebase_config, '.'), ".json") != 0) { SHOW_ERR(_("Google services config file must be a .json file")); goto android_dialog_clean_up; }
			if (!g_file_test(firebase_config, G_FILE_TEST_EXISTS)) { SHOW_ERR(_("Could not find Google services config file")); goto android_dialog_clean_up; }
		}

		// check version
		if (version_number && *version_number)
		{
			for (i = 0; i < strlen(version_number); i++)
			{
				if ((version_number[i] < 48 || version_number[i] > 57) && version_number[i] != 46)
				{
					SHOW_ERR(_("Version name contains invalid characters, must be 0-9 and . only"));
					goto android_dialog_clean_up;
				}
			}
		}

		// check keystore
		if (keystore_file && *keystore_file)
		{
			if (!g_file_test(keystore_file, G_FILE_TEST_EXISTS)) { SHOW_ERR(_("Could not find keystore file location")); goto android_dialog_clean_up; }
		}

		// check passwords
		if (keystore_password && strchr(keystore_password, '"')) { SHOW_ERR(_("Keystore password cannot contain double quotes")); goto android_dialog_clean_up; }
		if (alias_password && strchr(alias_password, '"')) { SHOW_ERR(_("Alias password cannot contain double quotes")); goto android_dialog_clean_up; }

		if (keystore_file && *keystore_file)
		{
			if (!keystore_password || !*keystore_password) { SHOW_ERR(_("You must enter your keystore password when using your own keystore")); goto android_dialog_clean_up; }
		}

		if (alias_name && *alias_name)
		{
			if (!alias_password || !*alias_password) { SHOW_ERR(_("You must enter your alias password when using a custom alias")); goto android_dialog_clean_up; }
		}

		int includeFirebase = (firebase_config && *firebase_config && (app_type == 0 || app_type == 1)) ? 1 : 0;
		int includePushNotify = (permission_push && app_type == 0) ? 1 : 0;
		int includeGooglePlay = (google_play_app_id && *google_play_app_id && app_type == 0) ? 1 : 0;

		if (includePushNotify && !includeFirebase)
		{
			SHOW_ERR(_("Push Notifications on Android now use Firebase, so you must include a Firebase config file to use them"));
			goto android_dialog_clean_up;
		}

		goto android_dialog_continue;

	android_dialog_clean_up:
		if (app_name) g_free(app_name);
		if (package_name) g_free(package_name);
		if (app_icon) g_free(app_icon);
		if (ouya_icon) g_free(ouya_icon);
		if (notif_icon) g_free(notif_icon);
		if (firebase_config) g_free(firebase_config);
		if (url_scheme) g_free(url_scheme);
		if (deep_link) g_free(deep_link);
		if (google_play_app_id) g_free(google_play_app_id);

		if (keystore_file) g_free(keystore_file);
		if (keystore_password) g_free(keystore_password);
		if (version_number) g_free(version_number);
		if (alias_name) g_free(alias_name);
		if (alias_password) g_free(alias_password);

		if (output_file) g_free(output_file);

		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.android_dialog, "android_export1"), TRUE);
		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.android_dialog, "button7"), TRUE);
		running = 0;
		return;

	android_dialog_continue:

		while (gtk_events_pending())
			gtk_main_iteration();

		// CHECKS COMPLETE, START EXPORT

		const char* androidJar = "android26.jar";
		//if ( app_type == 0 ) androidJar = "android21.jar";

#ifdef G_OS_WIN32
		gchar* path_to_aapt2 = g_build_path("\\", app->datadir, "android", "aapt2.exe", NULL);
		gchar* path_to_android_jar = g_build_path("\\", app->datadir, "android", androidJar, NULL);
		gchar* path_to_jarsigner = g_build_path("\\", app->datadir, "android", "jre", "bin", "jarsigner.exe", NULL);
		gchar* path_to_zipalign = g_build_path("\\", app->datadir, "android", "zipalign.exe", NULL);

		// convert forward slashes to backward slashes for parameters that will be passed to aapt2
		gchar *pathPtr = path_to_android_jar;
		while (*pathPtr) { if (*pathPtr == '/') *pathPtr = '\\'; pathPtr++; }

		pathPtr = output_file;
		while (*pathPtr) { if (*pathPtr == '/') *pathPtr = '\\'; pathPtr++; }
#else
		gchar* path_to_aapt2 = g_build_path("/", app->datadir, "android", "aapt2", NULL);
		gchar* path_to_android_jar = g_build_path("/", app->datadir, "android", androidJar, NULL);
		gchar* path_to_jarsigner = g_build_path("/", app->datadir, "android", "jre", "bin", "jarsigner", NULL);
		gchar* path_to_zipalign = g_build_path("/", app->datadir, "android", "zipalign", NULL);
#endif

		// make temporary folder
		gchar* android_folder = g_build_filename(app->datadir, "android", NULL);
		gchar* tmp_folder = g_build_filename(app->project->base_path, "build_tmp", NULL);

		utils_str_replace_char(android_folder, '\\', '/');
		utils_str_replace_char(tmp_folder, '\\', '/');

		gchar* src_folder;
		if (app_type == 2) src_folder = g_build_path("/", app->datadir, "android", "sourceOuya", NULL);
		else if (app_type == 1) src_folder = g_build_path("/", app->datadir, "android", "sourceAmazon", NULL);
		else src_folder = g_build_path("/", app->datadir, "android", "sourceGoogle", NULL);
		utils_str_replace_char(src_folder, '\\', '/');

		gchar *output_file_zip = g_strdup(output_file);
		gchar *ext = strrchr(output_file_zip, '.');
		if (ext) *ext = 0;
		SETPTR(output_file_zip, g_strconcat(output_file_zip, ".zip", NULL));

		if (!keystore_file || !*keystore_file)
		{
			if (keystore_file) g_free(keystore_file);
			if (keystore_password) g_free(keystore_password);

			keystore_file = g_build_path("/", app->datadir, "android", "debug.keystore", NULL);
			keystore_password = g_strdup("android");

			if (alias_name) g_free(alias_name);
			if (alias_password) g_free(alias_password);

			alias_name = g_strdup("androiddebugkey");
			alias_password = g_strdup("android");
		}
		else
		{
			if (!alias_name || !*alias_name)
			{
				if (alias_name) g_free(alias_name);
				if (alias_password) g_free(alias_password);

				alias_name = g_strdup("mykeystore");
				alias_password = g_strdup(keystore_password);
			}
		}

#define AGK_NEW_CONTENTS_SIZE 1000000

		// declarations
		gchar *newcontents = g_new0(gchar, AGK_NEW_CONTENTS_SIZE);
		gchar *newcontents2 = g_new0(gchar, AGK_NEW_CONTENTS_SIZE);
		gchar* manifest_file = NULL;
		gchar *contents = NULL;
		gchar *contents2 = NULL;
		gchar *contents3 = NULL;
		gchar *contentsOther = NULL;
		gchar *contentsOther2 = NULL;
		gchar *contentsOther3 = NULL;
		gsize length = 0;
		gchar* resources_file = NULL;
		GError *error = NULL;
		GdkPixbuf *icon_image = NULL;
		gchar *image_filename = NULL;
		GdkPixbuf *icon_scaled_image = NULL;
		gchar **argv = NULL;
		gchar **argv2 = NULL;
		gchar **argv3 = NULL;
		gint status = 0;
		mz_zip_archive zip_archive;
		memset(&zip_archive, 0, sizeof(zip_archive));
		gchar *zip_add_file = 0;
		gchar *str_out = NULL;
		gsize resLength = 0;
		gint package_count = 0;
		gint package_index = 0;

		if (!utils_copy_folder(src_folder, tmp_folder, TRUE, NULL))
		{
			SHOW_ERR(_("Failed to copy source folder"));
			goto android_dialog_cleanup2;
		}

		while (gtk_events_pending())
			gtk_main_iteration();

		// edit AndroidManifest.xml
		manifest_file = g_build_path("/", tmp_folder, "AndroidManifest.xml", NULL);

		if (!g_file_get_contents(manifest_file, &contents, &length, NULL))
		{
			SHOW_ERR(_("Failed to read AndroidManifest.xml file"));
			goto android_dialog_cleanup2;
		}

		strcpy(newcontents, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\
<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"\n\
      android:versionCode=\"");
		strcat(newcontents, szBuildNum);
		strcat(newcontents, "\"\n      android:versionName=\"");
		strcat(newcontents, version_number);
		strcat(newcontents, "\" package=\"");
		strcat(newcontents, package_name);
		strcat(newcontents, "\"");
		strcat(newcontents, " android:installLocation=\"auto\">\n\
    <uses-feature android:glEsVersion=\"0x00020000\"></uses-feature>\n\
    <uses-sdk android:minSdkVersion=\"");
		if (app_type == 0 || app_type == 1)
			strcat(newcontents, szSDK);
		else
			strcat(newcontents, "15");

		strcat(newcontents, "\" android:targetSdkVersion=\"");
		if (app_type == 0)
			strcat(newcontents, "26");
		else
			strcat(newcontents, "15");
		strcat(newcontents, "\" />\n\n");

		if (permission_external_storage) strcat(newcontents, "    <uses-permission android:name=\"android.permission.WRITE_EXTERNAL_STORAGE\"></uses-permission>\n");
		if (permission_internet)
		{
			strcat(newcontents, "    <uses-permission android:name=\"android.permission.INTERNET\"></uses-permission>\n");
			strcat(newcontents, "    <uses-permission android:name=\"android.permission.ACCESS_NETWORK_STATE\"></uses-permission>\n");
			strcat(newcontents, "    <uses-permission android:name=\"android.permission.ACCESS_WIFI_STATE\"></uses-permission>\n");
		}
		if (permission_wake) strcat(newcontents, "    <uses-permission android:name=\"android.permission.WAKE_LOCK\"></uses-permission>\n");
		if (permission_location_coarse && app_type == 0) strcat(newcontents, "    <uses-permission android:name=\"android.permission.ACCESS_COARSE_LOCATION\"></uses-permission>\n");
		if (permission_location_fine && app_type == 0) strcat(newcontents, "    <uses-permission android:name=\"android.permission.ACCESS_FINE_LOCATION\"></uses-permission>\n");
		if (permission_billing && app_type == 0) strcat(newcontents, "    <uses-permission android:name=\"com.android.vending.BILLING\"></uses-permission>\n");
		if (permission_camera) strcat(newcontents, "    <uses-permission android:name=\"android.permission.CAMERA\"></uses-permission>\n");
		if (((google_play_app_id && *google_play_app_id) || permission_push) && app_type == 0) strcat(newcontents, "    <uses-permission android:name=\"com.google.android.c2dm.permission.RECEIVE\" />\n");
		if (permission_push && app_type == 0)
		{
			strcat(newcontents, "    <permission android:name=\"");
			strcat(newcontents, package_name);
			strcat(newcontents, ".permission.C2D_MESSAGE\" android:protectionLevel=\"signature\" />\n");
			strcat(newcontents, "    <uses-permission android:name=\"");
			strcat(newcontents, package_name);
			strcat(newcontents, ".permission.C2D_MESSAGE\" />\n");
		}
		if (permission_expansion && app_type == 0)
		{
			//strcat( newcontents, "    <uses-permission android:name=\"android.permission.GET_ACCOUNTS\"></uses-permission>\n" );
			strcat(newcontents, "    <uses-permission android:name=\"com.android.vending.CHECK_LICENSE\"></uses-permission>\n");
		}
		if (permission_vibrate) strcat(newcontents, "    <uses-permission android:name=\"android.permission.VIBRATE\"></uses-permission>\n");
		if (permission_record_audio) strcat(newcontents, "    <uses-permission android:name=\"android.permission.RECORD_AUDIO\"></uses-permission>\n");

		// supports FireTV
		if (0)
		{
			strcat(newcontents, "    <uses-feature android:name=\"android.hardware.touchscreen\" android:required=\"false\" />\n");
		}

		// if ARCore required
		if (arcore_mode == 2)
		{
			strcat(newcontents, "    <uses-feature android:name=\"android.hardware.camera.ar\" android:required=\"true\" />");
		}

		contents2 = contents;
		contents3 = 0;

		// the order of these relacements is important, they must occur in the same order as they occur in the file

		// replace Google Play application ID
		contents3 = strstr(contents2, "<!--GOOGLE_PLAY_APPLICATION_ID-->");
		if (contents3)
		{
			*contents3 = 0;
			contents3 += strlen("<!--GOOGLE_PLAY_APPLICATION_ID-->");

			strcat(newcontents, contents2);
			strcat(newcontents, "<meta-data android:name=\"com.google.android.gms.games.APP_ID\" android:value=\"@string/games_app_id\" />");
			contents2 = contents3;
		}

		// replace orientation
		contents3 = strstr(contents2, "screenOrientation=\"fullSensor\"");
		if (contents3)
		{
			*contents3 = 0;
			contents3 += strlen("screenOrientation=\"fullSensor");

			strcat(newcontents, contents2);

			switch (orientation)
			{
			case 6: strcat(newcontents, "screenOrientation=\"sensorLandscape"); break;
			case 7:
			{
				// all now use API 23 with correct spelling
				strcat(newcontents, "screenOrientation=\"sensorPortrait");
				break;
			}
			default: strcat(newcontents, "screenOrientation=\"fullSensor"); break;
			}

			contents2 = contents3;
		}

		// add intent filters
		contents3 = strstr(contents2, "<!--ADDITIONAL_INTENT_FILTERS-->");
		if (contents3)
		{
			*contents3 = 0;
			contents3 += strlen("<!--ADDITIONAL_INTENT_FILTERS-->");

			strcat(newcontents, contents2);

			if (url_scheme && *url_scheme)
			{
				strcat(newcontents, "<intent-filter>\n\
         <action android:name=\"android.intent.action.VIEW\" />\n\
         <category android:name=\"android.intent.category.DEFAULT\" />\n\
         <category android:name=\"android.intent.category.BROWSABLE\" />\n\
         <data android:scheme=\"");

				strcat(newcontents, url_scheme);
				strcat(newcontents, "\" />\n    </intent-filter>\n");
			}

			if (deep_link && *deep_link)
			{
				gchar *szScheme = 0;
				gchar *szHost = 0;
				gchar *szPath = 0;
				gchar *szTemp = strstr(deep_link, "://");
				if (szTemp)
				{
					*szTemp = 0;
					szScheme = g_strdup(deep_link);
					*szTemp = ':';

					szTemp += 3;
					gchar *szTemp2 = strstr(szTemp, "/");
					if (szTemp2)
					{
						szPath = g_strdup(szTemp2);
						*szTemp2 = 0;
						szHost = g_strdup(szTemp);
						*szTemp2 = '/';
					}
					else szHost = g_strdup(szTemp);
				}

				if (szScheme && *szScheme)
				{
					strcat(newcontents, "<intent-filter>\n\
         <action android:name=\"android.intent.action.VIEW\" />\n\
         <category android:name=\"android.intent.category.DEFAULT\" />\n\
         <category android:name=\"android.intent.category.BROWSABLE\" />\n\
         <data android:scheme=\"");

					strcat(newcontents, szScheme);
					if (szHost && *szHost)
					{
						strcat(newcontents, "\" android:host=\"");
						strcat(newcontents, szHost);

						if (szPath && *szPath)
						{
							strcat(newcontents, "\" android:pathPrefix=\"");
							strcat(newcontents, szPath);
						}
					}

					strcat(newcontents, "\" />\n    </intent-filter>\n");
				}


				if (szScheme) g_free(szScheme);
				if (szHost) g_free(szHost);
				if (szPath) g_free(szPath);
			}

			contents2 = contents3;
		}

		// replace package name
		contents3 = strstr(contents2, "YOUR_PACKAGE_NAME_HERE");
		if (contents3)
		{
			*contents3 = 0;
			contents3 += strlen("YOUR_PACKAGE_NAME_HERE");

			strcat(newcontents, contents2);
			strcat(newcontents, package_name);
			contents2 = contents3;
		}

		// replace application ID
		contents3 = strstr(contents2, "${applicationId}");
		while (contents3)
		{
			*contents3 = 0;
			contents3 += strlen("${applicationId}");

			strcat(newcontents, contents2);
			strcat(newcontents, package_name);
			contents2 = contents3;
			contents3 = strstr(contents2, "${applicationId}");
		}

		// write the rest of the manifest file
		strcat(newcontents, contents2);

		if (permission_expansion && app_type == 0)
		{
			strcat(newcontents, "\n\
      <service android:name=\"com.google.android.vending.expansion.downloader.impl.DownloaderService\"\n\
            android:enabled=\"true\"/>\n\
        <receiver android:name=\"com.google.android.vending.expansion.downloader.impl.DownloaderService$AlarmReceiver\"\n\
            android:enabled=\"true\"/>");
		}

		// Google sign in
		if (app_type == 0)
		{
			strcat(newcontents, "\n\
      <activity android:name=\"com.google.android.gms.auth.api.signin.internal.SignInHubActivity\"\n\
            android:excludeFromRecents=\"true\"\n\
            android:exported=\"false\"\n\
            android:theme=\"@android:style/Theme.Translucent.NoTitleBar\" />\n\
        <service android:name=\"com.google.android.gms.auth.api.signin.RevocationBoundService\"\n\
            android:exported=\"true\"\n\
            android:permission=\"com.google.android.gms.auth.api.signin.permission.REVOCATION_NOTIFICATION\" />\n");
		}

		// IAP Purchase Activity
		if (permission_billing && app_type == 0)
		{
			strcat(newcontents, "\n\
        <activity android:name=\"com.google.android.gms.ads.purchase.InAppPurchaseActivity\" \n\
                  android:theme=\"@style/Theme.IAPTheme\" />");
		}

		// Google API Activity - for Game Services
		if (includeGooglePlay)
		{
			strcat(newcontents, "\n\
        <activity android:name=\"com.google.android.gms.common.api.GoogleApiActivity\" \n\
                  android:exported=\"false\" \n\
                  android:theme=\"@android:style/Theme.Translucent.NoTitleBar\" />");
		}

		// Firebase Init Provider - for Game Services and Firebase
		if (includeGooglePlay || includeFirebase || includePushNotify)
		{
			strcat(newcontents, "\n        <provider android:authorities=\"");
			strcat(newcontents, package_name);
			strcat(newcontents, ".firebaseinitprovider\"\n\
                  android:name=\"com.google.firebase.provider.FirebaseInitProvider\"\n\
                  android:exported=\"false\"\n\
                  android:initOrder=\"100\" />\n");
		}

		// Firebase activities
		if (includeFirebase)
		{
			strcat(newcontents, "\n\
        <receiver\n\
            android:name=\"com.google.android.gms.measurement.AppMeasurementReceiver\"\n\
            android:enabled=\"true\"\n\
            android:exported=\"false\" >\n\
        </receiver>\n\
\n\
        <service android:name=\"com.google.android.gms.measurement.AppMeasurementService\"\n\
                 android:enabled=\"true\"\n\
                 android:exported=\"false\"/>\n\
        <service\n\
            android:name=\"com.google.android.gms.measurement.AppMeasurementJobService\"\n\
            android:enabled=\"true\"\n\
            android:exported=\"false\"\n\
            android:permission=\"android.permission.BIND_JOB_SERVICE\" />");
		}

		if (includeFirebase || includePushNotify)
		{
			strcat(newcontents, "\n\
        <receiver android:name=\"com.google.firebase.iid.FirebaseInstanceIdReceiver\" \n\
                  android:exported=\"true\" \n\
                  android:permission=\"com.google.android.c2dm.permission.SEND\" > \n\
            <intent-filter> \n\
                <action android:name=\"com.google.android.c2dm.intent.RECEIVE\" /> \n\
            <action android:name=\"com.google.android.c2dm.intent.REGISTRATION\" /> \n\
                <category android:name=\"");
			strcat(newcontents, package_name);
			strcat(newcontents, "\" />\n\
            </intent-filter> \n\
        </receiver>\n\
        <receiver android:name=\"com.google.firebase.iid.FirebaseInstanceIdInternalReceiver\" \n\
                  android:exported=\"false\" /> \n\
        <service android:name=\"com.google.firebase.iid.FirebaseInstanceIdService\" \n\
                 android:exported=\"true\" > \n\
            <intent-filter android:priority=\"-500\" > \n\
                <action android:name=\"com.google.firebase.INSTANCE_ID_EVENT\" /> \n\
            </intent-filter> \n\
        </service>");
		}

		if (includePushNotify)
		{
			strcat(newcontents, "\n\
      <meta-data android:name=\"com.google.firebase.messaging.default_notification_icon\"\n\
            android:resource=\"@drawable/icon_white\" />\n\
      <service android:name=\"com.google.firebase.messaging.FirebaseMessagingService\" \n\
            android:exported=\"true\" > \n\
            <intent-filter android:priority=\"-500\" > \n\
                <action android:name=\"com.google.firebase.MESSAGING_EVENT\" /> \n\
            </intent-filter> \n\
        </service>");
		}

		// arcore activity
		if (arcore_mode > 0)
		{
			strcat(newcontents, "\n\
      <meta-data android:name=\"com.google.ar.core\" android:value=\"");
			if (arcore_mode == 1) strcat(newcontents, "optional");
			else strcat(newcontents, "required");
			strcat(newcontents, "\" />\n\
      <meta-data android:name=\"com.google.ar.core.min_apk_version\" android:value=\"180129103\" />\n\
      <meta-data android:name=\"android.support.VERSION\" android:value=\"26.0.2\" />\n\
        <activity\n\
            android:name=\"com.google.ar.core.InstallActivity\"\n\
            android:configChanges=\"keyboardHidden|orientation|screenSize\"\n\
            android:excludeFromRecents=\"true\"\n\
            android:exported=\"false\"\n\
            android:launchMode=\"singleTop\"\n\
            android:theme=\"@android:style/Theme.Material.Light.Dialog.Alert\" />");
		}


		strcat(newcontents, "\n    </application>\n</manifest>\n");

		// write new Android Manifest.xml file
		if (!g_file_set_contents(manifest_file, newcontents, strlen(newcontents), &error))
		{
			SHOW_ERR1(_("Failed to write AndroidManifest.xml file: %s"), error->message);
			g_error_free(error);
			error = NULL;
			goto android_dialog_cleanup2;
		}

		if (contents) g_free(contents);
		contents = 0;

		// read resources file
		resources_file = g_build_path("/", tmp_folder, "resOrig", "values", "values.xml", NULL);
		if (!g_file_get_contents(resources_file, &contents, &resLength, &error))
		{
			SHOW_ERR1(_("Failed to read resource values.xml file: %s"), error->message);
			g_error_free(error);
			error = NULL;
			goto android_dialog_cleanup2;
		}

		contents2 = strstr(contents, "<string name=\"app_name\">");
		if (!contents2)
		{
			SHOW_ERR(_("Could not find app name entry in values.xml file"));
			goto android_dialog_cleanup2;
		}

		contents2 += strlen("<string name=\"app_name\"");
		*contents2 = 0;
		contents3 = contents2;
		contents3++;
		contents3 = strstr(contents3, "</string>");
		if (!contents3)
		{
			SHOW_ERR(_("Could not find end of app name entry in values.xml file"));
			goto android_dialog_cleanup2;
		}

		// write resources file
		strcpy(newcontents, contents);
		strcat(newcontents, ">");
		strcat(newcontents, app_name);
		strcat(newcontents, contents3);

		// repair original file
		*contents2 = '>';

		if (app_type == 0 && google_play_app_id && *google_play_app_id)
		{
			memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);
			contents2 = strstr(newcontents2, "<string name=\"games_app_id\">");
			if (!contents2)
			{
				SHOW_ERR(_("Could not find games_app_id entry in values.xml file"));
				goto android_dialog_cleanup2;
			}

			contents2 += strlen("<string name=\"games_app_id\"");
			*contents2 = 0;
			contents3 = contents2;
			contents3++;
			contents3 = strstr(contents3, "</string>");
			if (!contents3)
			{
				SHOW_ERR(_("Could not find end of games_app_id entry in values.xml file"));
				goto android_dialog_cleanup2;
			}

			// write resources file
			strcpy(newcontents, newcontents2);
			strcat(newcontents, ">");
			strcat(newcontents, google_play_app_id);
			strcat(newcontents, contents3);

			// repair original file
			*contents2 = '>';
		}

		// firebase
		if (firebase_config && *firebase_config && (app_type == 0 || app_type == 1)) // Google and Amazon only
		{
			// read json values
			if (!g_file_get_contents(firebase_config, &contentsOther, &resLength, &error))
			{
				SHOW_ERR1(_("Failed to read firebase config file: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}

			memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);

			// find project_number value
			{
				contentsOther2 = strstr(contentsOther, "\"project_number\": \"");
				if (!contentsOther2)
				{
					SHOW_ERR(_("Could not find project_number entry in Firebase config file"));
					goto android_dialog_cleanup2;
				}

				contentsOther2 += strlen("\"project_number\": \"");
				contentsOther3 = strstr(contentsOther2, "\"");
				if (!contentsOther3)
				{
					SHOW_ERR(_("Could not find end of project_number entry in Firebase config file"));
					goto android_dialog_cleanup2;
				}
				*contentsOther3 = 0;

				// find entry in newcontents2
				contents2 = strstr(newcontents2, "<string name=\"gcm_defaultSenderId\" translatable=\"false\"");
				if (!contents2)
				{
					SHOW_ERR(_("Could not find gcm_defaultSenderId entry in values.xml file"));
					goto android_dialog_cleanup2;
				}

				contents2 += strlen("<string name=\"gcm_defaultSenderId\" translatable=\"false\"");
				*contents2 = 0;
				contents3 = contents2;
				contents3++;
				contents3 = strstr(contents3, "</string>");
				if (!contents3)
				{
					SHOW_ERR(_("Could not find end of gcm_defaultSenderId entry in values.xml file"));
					goto android_dialog_cleanup2;
				}

				// write resources file
				strcpy(newcontents, newcontents2);
				strcat(newcontents, ">");
				strcat(newcontents, contentsOther2);
				strcat(newcontents, contents3);

				*contents2 = '>'; // repair file
				*contentsOther3 = '"'; // repair file
				memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);
			}

			// find firebase_url value
			{
				contentsOther2 = strstr(contentsOther, "\"firebase_url\": \"");
				if (!contentsOther2)
				{
					SHOW_ERR(_("Could not find firebase_url entry in Firebase config file"));
					goto android_dialog_cleanup2;
				}

				contentsOther2 += strlen("\"firebase_url\": \"");
				contentsOther3 = strstr(contentsOther2, "\"");
				if (!contentsOther3)
				{
					SHOW_ERR(_("Could not find end of firebase_url entry in Firebase config file"));
					goto android_dialog_cleanup2;
				}
				*contentsOther3 = 0;

				// find entry in newcontents2
				contents2 = strstr(newcontents2, "<string name=\"firebase_database_url\" translatable=\"false\"");
				if (!contents2)
				{
					SHOW_ERR(_("Could not find firebase_database_url entry in values.xml file"));
					goto android_dialog_cleanup2;
				}

				contents2 += strlen("<string name=\"firebase_database_url\" translatable=\"false\"");
				*contents2 = 0;
				contents3 = contents2;
				contents3++;
				contents3 = strstr(contents3, "</string>");
				if (!contents3)
				{
					SHOW_ERR(_("Could not find end of firebase_database_url entry in values.xml file"));
					goto android_dialog_cleanup2;
				}

				// write resources file
				strcpy(newcontents, newcontents2);
				strcat(newcontents, ">");
				strcat(newcontents, contentsOther2);
				strcat(newcontents, contents3);

				*contents2 = '>'; // repair file
				*contentsOther3 = '"'; // repair file
				memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);
			}

			// find mobilesdk_app_id value
			// if the config file contains multiple Android apps then there will be multiple mobilesdk_app_id's, and only the corect one will work
			// look for the corresponding package_name that matches this export
			{
				package_count = 0;
				contentsOther2 = contentsOther;
				while (*contentsOther2 && (contentsOther2 = strstr(contentsOther2, "\"mobilesdk_app_id\": \"")))
				{
					package_count++;
					contentsOther2 += strlen("\"mobilesdk_app_id\": \"");
					contentsOther3 = strstr(contentsOther2, "\"");
					if (!contentsOther3)
					{
						SHOW_ERR(_("Could not find end of mobilesdk_app_id entry in Firebase config file"));
						goto android_dialog_cleanup2;
					}
					*contentsOther3 = 0;

					// look for the package_name for this mobilesdk_app_id
					gchar* contentsOther4 = strstr(contentsOther3 + 1, "\"package_name\": \"");
					if (!contentsOther4)
					{
						SHOW_ERR(_("Could not find package_name for mobilesdk_app_id entry in Firebase config file"));
						goto android_dialog_cleanup2;
					}
					contentsOther4 += strlen("\"package_name\": \"");
					if (strncmp(contentsOther4, package_name, strlen(package_name)) == 0)
					{
						contentsOther4 += strlen(package_name);
						if (*contentsOther4 == '\"')
						{
							break;
						}
					}

					*contentsOther3 = '"'; // repair file
				}

				if (!contentsOther2 || !*contentsOther2)
				{
					SHOW_ERR1(_("Could not find mobilesdk_app_id for android package_name \"%s\" in the Firebase config file"), package_name);
					goto android_dialog_cleanup2;
				}

				// find entry in newcontents2
				contents2 = strstr(newcontents2, "<string name=\"google_app_id\" translatable=\"false\"");
				if (!contents2)
				{
					SHOW_ERR(_("Could not find google_app_id entry in values.xml file"));
					goto android_dialog_cleanup2;
				}

				contents2 += strlen("<string name=\"google_app_id\" translatable=\"false\"");
				*contents2 = 0;
				contents3 = contents2;
				contents3++;
				contents3 = strstr(contents3, "</string>");
				if (!contents3)
				{
					SHOW_ERR(_("Could not find end of google_app_id entry in values.xml file"));
					goto android_dialog_cleanup2;
				}

				// write resources file
				strcpy(newcontents, newcontents2);
				strcat(newcontents, ">");
				strcat(newcontents, contentsOther2);
				strcat(newcontents, contents3);

				*contents2 = '>'; // repair file
				*contentsOther3 = '"'; // repair file
				memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);
			}

			// find current_key value
			{
				contentsOther2 = strstr(contentsOther, "\"current_key\": \"");
				if (!contentsOther2)
				{
					SHOW_ERR(_("Could not find current_key entry in Firebase config file"));
					goto android_dialog_cleanup2;
				}

				contentsOther2 += strlen("\"current_key\": \"");
				contentsOther3 = strstr(contentsOther2, "\"");
				if (!contentsOther3)
				{
					SHOW_ERR(_("Could not find end of current_key entry in Firebase config file"));
					goto android_dialog_cleanup2;
				}
				*contentsOther3 = 0;

				// find entry in newcontents2
				contents2 = strstr(newcontents2, "<string name=\"google_api_key\" translatable=\"false\"");
				if (!contents2)
				{
					SHOW_ERR(_("Could not find google_api_key entry in values.xml file"));
					goto android_dialog_cleanup2;
				}

				contents2 += strlen("<string name=\"google_api_key\" translatable=\"false\"");
				*contents2 = 0;
				contents3 = contents2;
				contents3++;
				contents3 = strstr(contents3, "</string>");
				if (!contents3)
				{
					SHOW_ERR(_("Could not find end of google_api_key entry in values.xml file"));
					goto android_dialog_cleanup2;
				}

				// write resources file
				strcpy(newcontents, newcontents2);
				strcat(newcontents, ">");
				strcat(newcontents, contentsOther2);
				strcat(newcontents, contents3);

				*contents2 = '>'; // repair file
				memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);

				// also copy it to google_crash_reporting_api_key
				contents2 = strstr(newcontents2, "<string name=\"google_crash_reporting_api_key\" translatable=\"false\"");
				if (!contents2)
				{
					SHOW_ERR(_("Could not find google_crash_reporting_api_key entry in values.xml file"));
					goto android_dialog_cleanup2;
				}

				contents2 += strlen("<string name=\"google_crash_reporting_api_key\" translatable=\"false\"");
				*contents2 = 0;
				contents3 = contents2;
				contents3++;
				contents3 = strstr(contents3, "</string>");
				if (!contents3)
				{
					SHOW_ERR(_("Could not find end of google_crash_reporting_api_key entry in values.xml file"));
					goto android_dialog_cleanup2;
				}

				// write resources file
				strcpy(newcontents, newcontents2);
				strcat(newcontents, ">");
				strcat(newcontents, contentsOther2);
				strcat(newcontents, contents3);

				*contents2 = '>'; // repair file
				*contentsOther3 = '"'; // repair file
				memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);
			}

			if (contentsOther) g_free(contentsOther);
			contentsOther = 0;
		}

		if (!g_file_set_contents(resources_file, newcontents, strlen(newcontents), &error))
		{
			SHOW_ERR1(_("Failed to write resource values.xml file: %s"), error->message);
			g_error_free(error);
			error = NULL;
			goto android_dialog_cleanup2;
		}

		if (contents) g_free(contents);
		contents = 0;

		// start packaging app
		gchar *aaptcommand = g_new0(gchar*, 1000000);
		if (!g_file_test(path_to_aapt2, G_FILE_TEST_EXISTS))
		{
			SHOW_ERR(_("Failed to export project, AAPT2 program not found"));
			goto android_dialog_cleanup2;
		}

		argv = g_new0(gchar *, 3);
		argv[0] = g_strdup(path_to_aapt2);
		argv[1] = g_strdup("m"); // open for stdin commands
		argv[2] = NULL;

		GPid aapt2_pid = 0;
		GPollFD aapt2_in = { -1, G_IO_OUT | G_IO_ERR, 0 };
		if (!g_spawn_async_with_pipes(tmp_folder, argv, NULL, G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_STDERR_TO_DEV_NULL | G_SPAWN_STDOUT_TO_DEV_NULL, NULL, NULL, &aapt2_pid,
			&aapt2_in.fd, NULL, NULL, &error))
		{
			SHOW_ERR1("g_spawn_async() failed: %s", error->message);
			g_error_free(error);
			error = NULL;
			goto android_dialog_cleanup2;
		}

		if (aapt2_pid == 0)
		{
			SHOW_ERR(_("Failed to start packaging tool"));
			goto android_dialog_cleanup2;
		}

		// compile values.xml file
#ifdef G_OS_WIN32
		strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\values\\values.xml\n\n");
#else
		strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/values/values.xml\n\n");
#endif
		write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

		if (error)
		{
			g_error_free(error);
			error = NULL;
		}

		// load icon file
		if (app_icon && *app_icon)
		{
			if (icon_image) gdk_pixbuf_unref(icon_image);
			icon_image = gdk_pixbuf_new_from_file(app_icon, &error);
			if (!icon_image || error)
			{
				SHOW_ERR1(_("Failed to load image icon: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}

			// scale it and save it
			if (app_type == 0 || app_type == 1)
			{
				// 192x192
				image_filename = g_build_path("/", tmp_folder, "resOrig", "drawable-xxxhdpi", "icon.png", NULL);
				icon_scaled_image = gdk_pixbuf_scale_simple(icon_image, 192, 192, GDK_INTERP_HYPER);
				if (!gdk_pixbuf_save(icon_scaled_image, image_filename, "png", &error, "compression", "9", NULL))
				{
					SHOW_ERR1(_("Failed to save xxxhdpi icon: %s"), error->message);
					g_error_free(error);
					error = NULL;
					goto android_dialog_cleanup2;
				}
				gdk_pixbuf_unref(icon_scaled_image);
				g_free(image_filename);

#ifdef G_OS_WIN32
				strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\drawable-xxxhdpi\\icon.png\n\n");
#else
				strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/drawable-xxxhdpi/icon.png\n\n");
#endif
				write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

				// 144x144
				image_filename = g_build_path("/", tmp_folder, "resOrig", "drawable-xxhdpi", "icon.png", NULL);
				icon_scaled_image = gdk_pixbuf_scale_simple(icon_image, 144, 144, GDK_INTERP_HYPER);
				if (!gdk_pixbuf_save(icon_scaled_image, image_filename, "png", &error, "compression", "9", NULL))
				{
					SHOW_ERR1(_("Failed to save xxhdpi icon: %s"), error->message);
					g_error_free(error);
					error = NULL;
					goto android_dialog_cleanup2;
				}
				gdk_pixbuf_unref(icon_scaled_image);
				g_free(image_filename);

#ifdef G_OS_WIN32
				strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\drawable-xxhdpi\\icon.png\n\n");
#else
				strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/drawable-xxhdpi/icon.png\n\n");
#endif
				write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));
			}

			const gchar* szDrawable_xhdpi = (app_type == 2) ? "drawable-xhdpi-v4" : "drawable-xhdpi";
			const gchar* szDrawable_hdpi = (app_type == 2) ? "drawable-hdpi-v4" : "drawable-hdpi";
			const gchar* szDrawable_mdpi = (app_type == 2) ? "drawable-mdpi-v4" : "drawable-mdpi";
			const gchar* szDrawable_ldpi = (app_type == 2) ? "drawable-ldpi-v4" : "drawable-ldpi";

			const gchar* szMainIcon = (app_type == 2) ? "app_icon.png" : "icon.png";

			// 96x96
			image_filename = g_build_path("/", tmp_folder, "resOrig", szDrawable_xhdpi, szMainIcon, NULL);
			icon_scaled_image = gdk_pixbuf_scale_simple(icon_image, 96, 96, GDK_INTERP_HYPER);
			if (!gdk_pixbuf_save(icon_scaled_image, image_filename, "png", &error, "compression", "9", NULL))
			{
				SHOW_ERR1(_("Failed to save xhdpi icon: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}
			gdk_pixbuf_unref(icon_scaled_image);
			g_free(image_filename);

#ifdef G_OS_WIN32
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_xhdpi); strcat(aaptcommand, "\\");
			strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#else
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_xhdpi); strcat(aaptcommand, "/");
			strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#endif
			write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

			// 72x72
			image_filename = g_build_path("/", tmp_folder, "resOrig", szDrawable_hdpi, szMainIcon, NULL);
			icon_scaled_image = gdk_pixbuf_scale_simple(icon_image, 72, 72, GDK_INTERP_HYPER);
			if (!gdk_pixbuf_save(icon_scaled_image, image_filename, "png", &error, "compression", "9", NULL))
			{
				SHOW_ERR1(_("Failed to save hdpi icon: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}
			gdk_pixbuf_unref(icon_scaled_image);
			g_free(image_filename);

#ifdef G_OS_WIN32
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_hdpi); strcat(aaptcommand, "\\");
			strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#else
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_hdpi); strcat(aaptcommand, "/");
			strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#endif
			write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

			// 48x48
			image_filename = g_build_path("/", tmp_folder, "resOrig", szDrawable_mdpi, szMainIcon, NULL);
			icon_scaled_image = gdk_pixbuf_scale_simple(icon_image, 48, 48, GDK_INTERP_HYPER);
			if (!gdk_pixbuf_save(icon_scaled_image, image_filename, "png", &error, "compression", "9", NULL))
			{
				SHOW_ERR1(_("Failed to save mdpi icon: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}
			gdk_pixbuf_unref(icon_scaled_image);
			g_free(image_filename);

#ifdef G_OS_WIN32
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_mdpi); strcat(aaptcommand, "\\");
			strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#else
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_mdpi); strcat(aaptcommand, "/");
			strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#endif
			write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

			// 36x36
			image_filename = g_build_path("/", tmp_folder, "resOrig", szDrawable_ldpi, szMainIcon, NULL);
			icon_scaled_image = gdk_pixbuf_scale_simple(icon_image, 36, 36, GDK_INTERP_HYPER);
			if (!gdk_pixbuf_save(icon_scaled_image, image_filename, "png", &error, "compression", "9", NULL))
			{
				SHOW_ERR1(_("Failed to save ldpi icon: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}

			gdk_pixbuf_unref(icon_scaled_image);
			icon_scaled_image = NULL;

#ifdef G_OS_WIN32
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_ldpi); strcat(aaptcommand, "\\");
			strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#else
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_ldpi); strcat(aaptcommand, "/");
			strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#endif
			write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

			g_free(image_filename);
			image_filename = NULL;
		}

		// load notification icon file
		if (notif_icon && *notif_icon && (app_type == 0 || app_type == 1))
		{
			if (icon_image) gdk_pixbuf_unref(icon_image);
			icon_image = gdk_pixbuf_new_from_file(notif_icon, &error);
			if (!icon_image || error)
			{
				SHOW_ERR1(_("Failed to load notification icon: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}

			// scale it and save it
			// 96x96
			image_filename = g_build_path("/", tmp_folder, "resOrig", "drawable-xxxhdpi", "icon_white.png", NULL);
			icon_scaled_image = gdk_pixbuf_scale_simple(icon_image, 96, 96, GDK_INTERP_HYPER);
			if (!gdk_pixbuf_save(icon_scaled_image, image_filename, "png", &error, "compression", "9", NULL))
			{
				SHOW_ERR1(_("Failed to save xxxhdpi icon: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}
			gdk_pixbuf_unref(icon_scaled_image);
			g_free(image_filename);

#ifdef G_OS_WIN32
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\drawable-xxxhdpi\\icon_white.png\n\n");
#else
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/drawable-xxxhdpi/icon_white.png\n\n");
#endif
			write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

			// 72x72
			image_filename = g_build_path("/", tmp_folder, "resOrig", "drawable-xxhdpi", "icon_white.png", NULL);
			icon_scaled_image = gdk_pixbuf_scale_simple(icon_image, 72, 72, GDK_INTERP_HYPER);
			if (!gdk_pixbuf_save(icon_scaled_image, image_filename, "png", &error, "compression", "9", NULL))
			{
				SHOW_ERR1(_("Failed to save xxhdpi icon: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}
			gdk_pixbuf_unref(icon_scaled_image);
			g_free(image_filename);

#ifdef G_OS_WIN32
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\drawable-xxhdpi\\icon_white.png\n\n");
#else
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/drawable-xxhdpi/icon_white.png\n\n");
#endif
			write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

			const gchar* szDrawable_xhdpi = (app_type == 2) ? "drawable-xhdpi-v4" : "drawable-xhdpi";
			const gchar* szDrawable_hdpi = (app_type == 2) ? "drawable-hdpi-v4" : "drawable-hdpi";
			const gchar* szDrawable_mdpi = (app_type == 2) ? "drawable-mdpi-v4" : "drawable-mdpi";
			const gchar* szDrawable_ldpi = (app_type == 2) ? "drawable-ldpi-v4" : "drawable-ldpi";

			// 48x48
			image_filename = g_build_path("/", tmp_folder, "resOrig", szDrawable_xhdpi, "icon_white.png", NULL);
			icon_scaled_image = gdk_pixbuf_scale_simple(icon_image, 48, 48, GDK_INTERP_HYPER);
			if (!gdk_pixbuf_save(icon_scaled_image, image_filename, "png", &error, "compression", "9", NULL))
			{
				SHOW_ERR1(_("Failed to save xhdpi icon: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}
			gdk_pixbuf_unref(icon_scaled_image);
			g_free(image_filename);

#ifdef G_OS_WIN32
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_xhdpi);
			strcat(aaptcommand, "\\icon_white.png\n\n");
#else
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_xhdpi);
			strcat(aaptcommand, "/icon_white.png\n\n");
#endif
			write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

			// 36x36
			image_filename = g_build_path("/", tmp_folder, "resOrig", szDrawable_hdpi, "icon_white.png", NULL);
			icon_scaled_image = gdk_pixbuf_scale_simple(icon_image, 36, 36, GDK_INTERP_HYPER);
			if (!gdk_pixbuf_save(icon_scaled_image, image_filename, "png", &error, "compression", "9", NULL))
			{
				SHOW_ERR1(_("Failed to save hdpi icon: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}
			gdk_pixbuf_unref(icon_scaled_image);
			g_free(image_filename);

#ifdef G_OS_WIN32
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_hdpi);
			strcat(aaptcommand, "\\icon_white.png\n\n");
#else
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_hdpi);
			strcat(aaptcommand, "/icon_white.png\n\n");
#endif
			write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

			// 24x24
			image_filename = g_build_path("/", tmp_folder, "resOrig", szDrawable_mdpi, "icon_white.png", NULL);
			icon_scaled_image = gdk_pixbuf_scale_simple(icon_image, 24, 24, GDK_INTERP_HYPER);
			if (!gdk_pixbuf_save(icon_scaled_image, image_filename, "png", &error, "compression", "9", NULL))
			{
				SHOW_ERR1(_("Failed to save mdpi icon: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}
			gdk_pixbuf_unref(icon_scaled_image);
			g_free(image_filename);

#ifdef G_OS_WIN32
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_mdpi);
			strcat(aaptcommand, "\\icon_white.png\n\n");
#else
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_mdpi);
			strcat(aaptcommand, "/icon_white.png\n\n");
#endif
			write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

			// 24x24
			image_filename = g_build_path("/", tmp_folder, "resOrig", szDrawable_ldpi, "icon_white.png", NULL);
			icon_scaled_image = gdk_pixbuf_scale_simple(icon_image, 24, 24, GDK_INTERP_HYPER);
			if (!gdk_pixbuf_save(icon_scaled_image, image_filename, "png", &error, "compression", "9", NULL))
			{
				SHOW_ERR1(_("Failed to save ldpi icon: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}

			gdk_pixbuf_unref(icon_scaled_image);
			icon_scaled_image = NULL;

#ifdef G_OS_WIN32
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_ldpi);
			strcat(aaptcommand, "\\icon_white.png\n\n");
#else
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_ldpi);
			strcat(aaptcommand, "/icon_white.png\n\n");
#endif
			write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

			g_free(image_filename);
			image_filename = NULL;
		}

		// load ouya icon and check size
		if (app_type == 2)
		{
			if (icon_image) gdk_pixbuf_unref(icon_image);
			icon_image = gdk_pixbuf_new_from_file(ouya_icon, &error);
			if (!icon_image || error)
			{
				SHOW_ERR1(_("Failed to load Ouya large icon: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}

			if (gdk_pixbuf_get_width(icon_image) != 732 || gdk_pixbuf_get_height(icon_image) != 412)
			{
				SHOW_ERR(_("Ouya large icon must be 732x412 pixels"));
				goto android_dialog_cleanup2;
			}

			// copy it to the res folder
			image_filename = g_build_path("/", tmp_folder, "resOrig", "drawable-xhdpi-v4", "ouya_icon.png", NULL);
			utils_copy_file(ouya_icon, image_filename, TRUE, NULL);
			g_free(image_filename);

#ifdef G_OS_WIN32
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\drawable-xhdpi-v4\\ouya_icon.png\n\n");
#else
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/drawable-xhdpi-v4/ouya_icon.png\n\n");
#endif
			write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

			// 320x180
			image_filename = g_build_path("/", tmp_folder, "resOrig", "drawable", "icon.png", NULL);
			icon_scaled_image = gdk_pixbuf_scale_simple(icon_image, 320, 180, GDK_INTERP_HYPER);
			if (!gdk_pixbuf_save(icon_scaled_image, image_filename, "png", &error, "compression", "9", NULL))
			{
				SHOW_ERR1(_("Failed to save lean back icon: %s"), error->message);
				g_error_free(error);
				error = NULL;
				goto android_dialog_cleanup2;
			}

			gdk_pixbuf_unref(icon_scaled_image);
			icon_scaled_image = NULL;

#ifdef G_OS_WIN32
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\drawable\\icon.png\n\n");
#else
			strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/drawable/icon.png\n\n");
#endif
			write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

			g_free(image_filename);
			image_filename = NULL;
		}

		while (gtk_events_pending())
			gtk_main_iteration();

		strcpy(aaptcommand, "l\n-I\n");
		strcat(aaptcommand, path_to_android_jar);
		strcat(aaptcommand, "\n--manifest\n");
		strcat(aaptcommand, tmp_folder);
		strcat(aaptcommand, "/AndroidManifest.xml\n-o\n");
		strcat(aaptcommand, output_file);
		strcat(aaptcommand, "\n--auto-add-overlay\n--no-version-vectors\n");

		gchar* resMergedPath = g_build_filename(tmp_folder, "resMerged", NULL);
		GDir *dir = g_dir_open(resMergedPath, 0, NULL);

		const gchar *filename;
		foreach_dir(filename, dir)
		{
			gchar* fullsrcpath = g_build_filename(tmp_folder, "resMerged", filename, NULL);

			if (g_file_test(fullsrcpath, G_FILE_TEST_IS_REGULAR))
			{
				strcat(aaptcommand, "-R\n");
				strcat(aaptcommand, fullsrcpath);
				strcat(aaptcommand, "\n");
			}

			g_free(fullsrcpath);
		}

		g_dir_close(dir);
		g_free(resMergedPath);

		/*
		gchar* fullsrcpath = g_build_filename( tmp_folder, "resMerged\\values_values.arsc.flat", NULL );
		strcat( aaptcommand, "-R\n" );
		strcat( aaptcommand, fullsrcpath );
		strcat( aaptcommand, "\n" );
		g_free(fullsrcpath);
		*/

		strcat(aaptcommand, "\nquit\n\n");

#ifdef G_OS_WIN32
		gchar *ptr = aaptcommand;
		while (*ptr)
		{
			if (*ptr == '/') *ptr = '\\';
			ptr++;
		}
#endif

		//gchar* logpath = g_build_filename( tmp_folder, "log.txt", NULL );
		//FILE *pFile = fopen( logpath, "wb" );
		//fputs( aaptcommand, pFile );
		//fclose( pFile );

		write(aapt2_in.fd, aaptcommand, strlen(aaptcommand));

#ifdef G_OS_WIN32
		WaitForProcess(aapt2_pid);
#else
		waitpid(aapt2_pid, &status, 0);
#endif
		aapt2_pid = 0;

		// if we have previously called g_spawn_async then g_spawn_sync will never return the correct exit status due to ECHILD being returned from waitpid()

		// check the file was created instead
		if (!g_file_test(output_file, G_FILE_TEST_EXISTS))
		{
			SHOW_ERR(_("Failed to write output files, check that your project directory is not in a write protected location"));
			goto android_dialog_cleanup2;
		}

		while (gtk_events_pending())
			gtk_main_iteration();

		g_rename(output_file, output_file_zip);

		// open APK as a zip file
		if (!mz_zip_reader_init_file(&zip_archive, output_file_zip, 0))
		{
			SHOW_ERR(_("Failed to initialise zip file for reading"));
			goto android_dialog_cleanup2;
		}
		if (!mz_zip_writer_init_from_reader(&zip_archive, output_file_zip))
		{
			SHOW_ERR(_("Failed to open zip file for writing"));
			goto android_dialog_cleanup2;
		}

		// copy in extra files
		zip_add_file = g_build_path("/", src_folder, "classes.dex", NULL);
		mz_zip_writer_add_file(&zip_archive, "classes.dex", zip_add_file, NULL, 0, 9);

		g_free(zip_add_file);
		zip_add_file = g_build_path("/", android_folder, "lib", "arm64-v8a", "libandroid_player.so", NULL);
		mz_zip_writer_add_file(&zip_archive, "lib/arm64-v8a/libandroid_player.so", zip_add_file, NULL, 0, 9);

		g_free(zip_add_file);
		zip_add_file = g_build_path("/", android_folder, "lib", "armeabi-v7a", "libandroid_player.so", NULL);
		mz_zip_writer_add_file(&zip_archive, "lib/armeabi-v7a/libandroid_player.so", zip_add_file, NULL, 0, 9);

		g_free(zip_add_file);
		zip_add_file = g_build_path("/", android_folder, "lib", "x86", "libandroid_player.so", NULL);
		mz_zip_writer_add_file(&zip_archive, "lib/x86/libandroid_player.so", zip_add_file, NULL, 0, 9);

		if (arcore_mode > 0)
		{
			// use real ARCore lib
			g_free(zip_add_file);
			zip_add_file = g_build_path("/", android_folder, "lib", "arm64-v8a", "libarcore_sdk.so", NULL);
			mz_zip_writer_add_file(&zip_archive, "lib/arm64-v8a/libarcore_sdk.so", zip_add_file, NULL, 0, 9);

			g_free(zip_add_file);
			zip_add_file = g_build_path("/", android_folder, "lib", "armeabi-v7a", "libarcore_sdk.so", NULL);
			mz_zip_writer_add_file(&zip_archive, "lib/armeabi-v7a/libarcore_sdk.so", zip_add_file, NULL, 0, 9);

			g_free(zip_add_file);
			zip_add_file = g_build_path("/", android_folder, "lib", "x86", "libarcore_sdk.so", NULL);
			mz_zip_writer_add_file(&zip_archive, "lib/x86/libarcore_sdk.so", zip_add_file, NULL, 0, 9);
		}

		while (gtk_events_pending())
			gtk_main_iteration();

		if (app_type != 2)
		{
			// copy assets for Google and Amazon
			g_free(zip_add_file);
			zip_add_file = g_build_path("/", android_folder, "assets", NULL);
			if (!utils_add_folder_to_zip(&zip_archive, zip_add_file, "assets", TRUE, TRUE))
			{
				SHOW_ERR(_("Failed to add media files to APK"));
				goto android_dialog_cleanup2;
			}
		}

		// copy in media files
		g_free(zip_add_file);
		zip_add_file = g_build_path("/", app->project->base_path, "media", NULL);
		if (!utils_add_folder_to_zip(&zip_archive, zip_add_file, "assets/media", TRUE, TRUE))
		{
			SHOW_ERR(_("Failed to add media files to APK"));
			goto android_dialog_cleanup2;
		}

		if (!mz_zip_writer_finalize_archive(&zip_archive))
		{
			SHOW_ERR(_("Failed to add finalize zip file"));
			goto android_dialog_cleanup2;
		}
		if (!mz_zip_writer_end(&zip_archive))
		{
			SHOW_ERR(_("Failed to end zip file"));
			goto android_dialog_cleanup2;
		}

		while (gtk_events_pending())
			gtk_main_iteration();

		// sign apk
		argv2 = g_new0(gchar*, 14);
		argv2[0] = g_strdup(path_to_jarsigner);
		argv2[1] = g_strdup("-sigalg");
		argv2[2] = g_strdup("MD5withRSA");
		argv2[3] = g_strdup("-digestalg");
		argv2[4] = g_strdup("SHA1");
		argv2[5] = g_strdup("-storepass");
#ifdef G_OS_WIN32
		argv2[6] = g_strconcat("\"", keystore_password, "\"", NULL);
#else
		argv2[6] = g_strdup(keystore_password);
#endif
		argv2[7] = g_strdup("-keystore");
		argv2[8] = g_strdup(keystore_file);
		argv2[9] = g_strdup(output_file_zip);
		argv2[10] = g_strdup(alias_name);
		argv2[11] = g_strdup("-keypass");
#ifdef G_OS_WIN32
		argv2[12] = g_strconcat("\"", alias_password, "\"", NULL);
#else
		argv2[12] = g_strdup(alias_password);
#endif
		argv2[13] = NULL;

		if (!utils_spawn_sync(tmp_folder, argv2, NULL, 0, NULL, NULL, &str_out, NULL, &status, &error))
		{
			SHOW_ERR1(_("Failed to run signing tool: %s"), error->message);
			g_error_free(error);
			error = NULL;
			goto android_dialog_cleanup2;
		}

		if (status != 0 && str_out && *str_out && strstr(str_out, "jar signed.") == 0)
		{
			SHOW_ERR1(_("Failed to sign APK, is your keystore password and alias correct? (error: %s)"), str_out);
			goto android_dialog_cleanup2;
		}

		if (str_out) g_free(str_out);
		str_out = 0;

		while (gtk_events_pending())
			gtk_main_iteration();

		// align apk
		argv3 = g_new0(gchar*, 5);
		argv3[0] = g_strdup(path_to_zipalign);
		argv3[1] = g_strdup("4");
		argv3[2] = g_strdup(output_file_zip);
		argv3[3] = g_strdup(output_file);
		argv3[4] = NULL;

		if (!utils_spawn_sync(tmp_folder, argv3, NULL, 0, NULL, NULL, &str_out, NULL, &status, &error))
		{
			SHOW_ERR1(_("Failed to run zipalign tool: %s"), error->message);
			g_error_free(error);
			error = NULL;
			goto android_dialog_cleanup2;
		}

		if (status != 0 && str_out && *str_out)
		{
			SHOW_ERR1(_("Zip align tool returned error: %s"), str_out);
			goto android_dialog_cleanup2;
		}

		while (gtk_events_pending())
			gtk_main_iteration();

		if (dialog) gtk_widget_hide(GTK_WIDGET(dialog));

	android_dialog_cleanup2:

		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.android_dialog, "android_export1"), TRUE);
		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.android_dialog, "button7"), TRUE);

		if (aapt2_pid)
		{
#ifdef G_OS_WIN32
			TerminateProcess(aapt2_pid, 0);
#else
			kill(aapt2_pid, SIGTERM);
#endif
		}

		g_unlink(output_file_zip);
		utils_remove_folder_recursive(tmp_folder);

		if (path_to_aapt2) g_free(path_to_aapt2);
		if (path_to_android_jar) g_free(path_to_android_jar);
		if (path_to_jarsigner) g_free(path_to_jarsigner);
		if (path_to_zipalign) g_free(path_to_zipalign);

		if (zip_add_file) g_free(zip_add_file);
		if (manifest_file) g_free(manifest_file);
		if (newcontents) g_free(newcontents);
		if (newcontents2) g_free(newcontents2);
		if (contents) g_free(contents);
		if (contentsOther) g_free(contentsOther);
		if (resources_file) g_free(resources_file);
		if (error) g_error_free(error);
		if (icon_image) gdk_pixbuf_unref(icon_image);
		if (image_filename) g_free(image_filename);
		if (icon_scaled_image) gdk_pixbuf_unref(icon_scaled_image);
		if (argv) g_strfreev(argv);
		if (argv2) g_strfreev(argv2);
		if (argv3) g_strfreev(argv3);
		if (aaptcommand) g_free(aaptcommand);

		if (output_file_zip) g_free(output_file_zip);
		if (tmp_folder) g_free(tmp_folder);
		if (android_folder) g_free(android_folder);
		if (src_folder) g_free(src_folder);
		if (str_out) g_free(str_out);

		if (output_file) g_free(output_file);
		if (app_name) g_free(app_name);
		if (package_name) g_free(package_name);
		if (app_icon) g_free(app_icon);
		if (ouya_icon) g_free(ouya_icon);
		if (firebase_config) g_free(firebase_config);
		if (url_scheme) g_free(url_scheme);
		if (deep_link) g_free(deep_link);
		if (google_play_app_id) g_free(google_play_app_id);

		if (keystore_file) g_free(keystore_file);
		if (keystore_password) g_free(keystore_password);
		if (version_number) g_free(version_number);
		if (alias_name) g_free(alias_name);
		if (alias_password) g_free(alias_password);
	}

	running = 0;
}

static gchar *last_proj_path_android = 0;

void project_export_apk()
{
	if (!app->project)
	{
		SHOW_ERR(_("You must have a project open to export it"));
		return;
	}

	// make sure the project is up to date
	build_compile_project(0);

	if (ui_widgets.android_dialog == NULL)
	{
		ui_widgets.android_dialog = create_android_dialog();
		gtk_widget_set_name(ui_widgets.android_dialog, "Export APK");
		gtk_window_set_transient_for(GTK_WINDOW(ui_widgets.android_dialog), GTK_WINDOW(main_widgets.window));

		g_signal_connect(ui_widgets.android_dialog, "response", G_CALLBACK(on_android_dialog_response), NULL);
		g_signal_connect(ui_widgets.android_dialog, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);

		ui_setup_open_button_callback_android(ui_lookup_widget(ui_widgets.android_dialog, "android_app_icon_path"), NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN, GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_app_icon_entry")));
		ui_setup_open_button_callback_android(ui_lookup_widget(ui_widgets.android_dialog, "android_notif_icon_path"), NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN, GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_notif_icon_entry")));
		ui_setup_open_button_callback_android(ui_lookup_widget(ui_widgets.android_dialog, "android_ouya_icon_path"), NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN, GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_ouya_icon_entry")));
		ui_setup_open_button_callback_android(ui_lookup_widget(ui_widgets.android_dialog, "android_keystore_file_path"), NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN, GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_keystore_file_entry")));
		ui_setup_open_button_callback_android(ui_lookup_widget(ui_widgets.android_dialog, "android_firebase_config_path"), NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN, GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_firebase_config_entry")));

		ui_setup_open_button_callback_android(ui_lookup_widget(ui_widgets.android_dialog, "android_output_file_path"), NULL,
			GTK_FILE_CHOOSER_ACTION_SAVE, GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_output_file_entry")));

		gtk_combo_box_set_active(GTK_COMBO_BOX(ui_lookup_widget(ui_widgets.android_dialog, "android_output_type_combo")), 0);
		gtk_combo_box_set_active(GTK_COMBO_BOX(ui_lookup_widget(ui_widgets.android_dialog, "android_orientation_combo")), 0);
		gtk_combo_box_set_active(GTK_COMBO_BOX(ui_lookup_widget(ui_widgets.android_dialog, "android_sdk_combo")), 0);
		gtk_combo_box_set_active(GTK_COMBO_BOX(ui_lookup_widget(ui_widgets.android_dialog, "android_arcore_combo")), 0);
	}

	// pointers could be the same even if the project is different, so check project path instead
	if (strcmp(FALLBACK(last_proj_path_android, ""), FALLBACK(app->project->file_name, "")) != 0)
	{
		if (last_proj_path_android) g_free(last_proj_path_android);
		last_proj_path_android = g_strdup(FALLBACK(app->project->file_name, ""));

		GtkWidget *widget;

		// set defaults for this project
		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_app_name_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.app_name, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_package_name_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.package_name, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_app_icon_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.app_icon_path, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_notif_icon_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.notif_icon_path, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_ouya_icon_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.ouya_icon_path, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_firebase_config_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.firebase_config_path, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_orientation_combo");
		gtk_combo_box_set_active(GTK_COMBO_BOX(widget), app->project->apk_settings.orientation);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_arcore_combo");
		gtk_combo_box_set_active(GTK_COMBO_BOX(widget), app->project->apk_settings.arcore);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_sdk_combo");
		int version = app->project->apk_settings.sdk_version - 1;
		if (version < 0) version = 0;
		gtk_combo_box_set_active(GTK_COMBO_BOX(widget), version);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_url_scheme");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.url_scheme, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_deep_link");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.deep_link, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_google_play_app_id");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.play_app_id, ""));

		// permissions
		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_external_storage");
		int mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_WRITE) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_location_fine");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_GPS) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_location_coarse");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_LOCATION) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_internet");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_INTERNET) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_wake");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_WAKE) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_billing");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_IAP) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_push_notifications");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_PUSH) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_camera");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_CAMERA) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_expansion");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_EXPANSION) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_vibrate");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_VIBRATE) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_record_audio");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_RECORD_AUDIO) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget ( ui_widgets.android_dialog, "android_permission_adverts" );
		mode = ( app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_ADVERTS ) ? 1 : 0;
		gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( widget ), mode );

		// signing
		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_keystore_file_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.keystore_path, ""));

		// keep old password and assume it is the same for all projects to save time
		//widget = ui_lookup_widget(ui_widgets.android_dialog, "android_keystore_password_entry");
		//gtk_entry_set_text( GTK_ENTRY(widget), "" );

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_version_number_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.version_name, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_build_number_entry");
		if (app->project->apk_settings.version_number == 0)
		{
			gtk_entry_set_text(GTK_ENTRY(widget), "");
		}
		else
		{
			char szBuildNum[20];
			sprintf(szBuildNum, "%d", app->project->apk_settings.version_number);
			gtk_entry_set_text(GTK_ENTRY(widget), szBuildNum);
		}

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_alias_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.alias, ""));

		// keep old password and assume it is the same for all projects to save time
		//widget = ui_lookup_widget(ui_widgets.android_dialog, "android_alias_password_entry");
		//gtk_entry_set_text( GTK_ENTRY(widget), "" );

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_output_type_combo");
		gtk_combo_box_set_active(GTK_COMBO_BOX(widget), app->project->apk_settings.app_type);

		if (!app->project->apk_settings.output_path || !*app->project->apk_settings.output_path)
		{
			gchar *filename = g_strconcat(app->project->name, ".apk", NULL);
			gchar* apk_path = g_build_filename(app->project->base_path, filename, NULL);
			gtk_entry_set_text(GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_output_file_entry")), apk_path);
			g_free(apk_path);
			g_free(filename);
		}
		else
		{
			widget = ui_lookup_widget(ui_widgets.android_dialog, "android_output_file_entry");
			gtk_entry_set_text(GTK_ENTRY(widget), app->project->apk_settings.output_path);
		}
	}

	gtk_window_present(GTK_WINDOW(ui_widgets.android_dialog));
}

void on_android_all_dialog_response(GtkDialog *dialog, gint response, gpointer user_data)
{
	if (response != 1)
	{
		if (dialog) gtk_widget_hide(GTK_WIDGET(dialog));
		return;
	}

	GeanyProject *orig_project = app->project;

	// export all output folder
	GtkWidget *widget = ui_lookup_widget(ui_widgets.android_all_dialog, "export_all_android_output_file_entry");
	gchar *output_file = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

	if (!*output_file)
	{
		g_free(output_file);
		SHOW_ERR(_("You must choose an output folder to save your APKs"));
		return;
	}

	// get export all options
	widget = ui_lookup_widget(ui_widgets.android_all_dialog, "export_all_android_keystore_password_entry");
	gchar *keystore_password = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

	widget = ui_lookup_widget(ui_widgets.android_all_dialog, "export_all_android_version_number_entry");
	gchar *version_number = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));
	if (!*version_number) SETPTR(version_number, g_strdup("1.0.0"));

	widget = ui_lookup_widget(ui_widgets.android_all_dialog, "export_all_android_build_number_entry");
	gchar* build_number = gtk_entry_get_text(GTK_ENTRY(widget));
	if (!*build_number) SETPTR(build_number, g_strdup("1"));

	int i;
	for (i = 0; i < projects_array->len; i++)
	{
		if (!projects[i]->is_valid) continue;

		GtkWidget *export_all_progress = ui_lookup_widget(ui_widgets.android_all_dialog, "export_all_android_progress");
		gchar *text = g_strconcat("Exporting: ", projects[i]->name, " - Google", NULL);
		gtk_label_set_text(GTK_LABEL(export_all_progress), text);
		g_free(text);

		while (gtk_events_pending()) gtk_main_iteration();

		// change current project
		app->project = projects[i];

		// set up the android export dialog
		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_app_name_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.app_name, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_package_name_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.package_name, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_app_icon_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.app_icon_path, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_notif_icon_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.notif_icon_path, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_ouya_icon_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.ouya_icon_path, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_firebase_config_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.firebase_config_path, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_orientation_combo");
		gtk_combo_box_set_active(GTK_COMBO_BOX(widget), app->project->apk_settings.orientation);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_arcore_combo");
		gtk_combo_box_set_active(GTK_COMBO_BOX(widget), app->project->apk_settings.arcore);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_sdk_combo");
		int version = app->project->apk_settings.sdk_version - 1;
		if (version < 0) version = 0;
		gtk_combo_box_set_active(GTK_COMBO_BOX(widget), version);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_url_scheme");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.url_scheme, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_deep_link");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.deep_link, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_google_play_app_id");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.play_app_id, ""));

		// permissions
		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_external_storage");
		int mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_WRITE) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_location_fine");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_GPS) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_location_coarse");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_LOCATION) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_internet");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_INTERNET) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_wake");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_WAKE) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_billing");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_IAP) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_push_notifications");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_PUSH) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_camera");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_CAMERA) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_expansion");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_EXPANSION) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_vibrate");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_VIBRATE) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_permission_record_audio");
		mode = (app->project->apk_settings.permission_flags & AGK_ANDROID_PERMISSION_RECORD_AUDIO) ? 1 : 0;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), mode);

		// signing
		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_keystore_file_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.keystore_path, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_keystore_password_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), keystore_password);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_version_number_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), version_number);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_build_number_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), build_number);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_alias_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), FALLBACK(app->project->apk_settings.alias, ""));

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_alias_password_entry");
		gtk_entry_set_text(GTK_ENTRY(widget), keystore_password);

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_output_type_combo");
		gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0); // Google

		gchar *filename = g_strconcat(app->project->name, "-Google-", version_number, ".apk", NULL);
		gchar* apk_path = g_build_filename(output_file, filename, NULL);
		gtk_entry_set_text(GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_output_file_entry")), apk_path);
		g_free(apk_path);
		g_free(filename);

		on_android_dialog_response(0, 1, 1); // no dialog, export response, don't save settings

		widget = ui_lookup_widget(ui_widgets.android_dialog, "android_output_type_combo");
		gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 1); // Amazon

		filename = g_strconcat(app->project->name, "-Amazon-", version_number, ".apk", NULL);
		apk_path = g_build_filename(output_file, filename, NULL);
		gtk_entry_set_text(GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_output_file_entry")), apk_path);
		g_free(apk_path);
		g_free(filename);

		text = g_strconcat("Exporting: ", projects[i]->name, " - Amazon", NULL);
		gtk_label_set_text(GTK_LABEL(export_all_progress), text);
		g_free(text);

		while (gtk_events_pending()) gtk_main_iteration();

		on_android_dialog_response(0, 1, 1); // no dialog, export response, don't save settings
	}

	gtk_widget_hide(GTK_WIDGET(dialog));

	// reset current project and set future exports to reload normal settings
	app->project = orig_project;
	if (last_proj_path_android) g_free(last_proj_path_android);
	last_proj_path_android = g_strdup("");
}

void project_export_apk_all()
{
	if (projects_array->len <= 0)
	{
		SHOW_ERR(_("You must have at least one project open to export all"));
		return;
	}

	// make sure original android dialog exists
	if (ui_widgets.android_dialog == NULL)
	{
		ui_widgets.android_dialog = create_android_dialog();
		gtk_widget_set_name(ui_widgets.android_dialog, "Export APK");
		gtk_window_set_transient_for(GTK_WINDOW(ui_widgets.android_dialog), GTK_WINDOW(main_widgets.window));

		g_signal_connect(ui_widgets.android_dialog, "response", G_CALLBACK(on_android_dialog_response), NULL);
		g_signal_connect(ui_widgets.android_dialog, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);

		ui_setup_open_button_callback_android(ui_lookup_widget(ui_widgets.android_dialog, "android_app_icon_path"), NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN, GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_app_icon_entry")));
		ui_setup_open_button_callback_android(ui_lookup_widget(ui_widgets.android_dialog, "android_notif_icon_path"), NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN, GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_notif_icon_entry")));
		ui_setup_open_button_callback_android(ui_lookup_widget(ui_widgets.android_dialog, "android_ouya_icon_path"), NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN, GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_ouya_icon_entry")));
		ui_setup_open_button_callback_android(ui_lookup_widget(ui_widgets.android_dialog, "android_keystore_file_path"), NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN, GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_keystore_file_entry")));
		ui_setup_open_button_callback_android(ui_lookup_widget(ui_widgets.android_dialog, "android_firebase_config_path"), NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN, GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_firebase_config_entry")));

		ui_setup_open_button_callback_android(ui_lookup_widget(ui_widgets.android_dialog, "android_output_file_path"), NULL,
			GTK_FILE_CHOOSER_ACTION_SAVE, GTK_ENTRY(ui_lookup_widget(ui_widgets.android_dialog, "android_output_file_entry")));

		gtk_combo_box_set_active(GTK_COMBO_BOX(ui_lookup_widget(ui_widgets.android_dialog, "android_output_type_combo")), 0);
		gtk_combo_box_set_active(GTK_COMBO_BOX(ui_lookup_widget(ui_widgets.android_dialog, "android_orientation_combo")), 0);
		gtk_combo_box_set_active(GTK_COMBO_BOX(ui_lookup_widget(ui_widgets.android_dialog, "android_sdk_combo")), 0);
		gtk_combo_box_set_active(GTK_COMBO_BOX(ui_lookup_widget(ui_widgets.android_dialog, "android_arcore_combo")), 0);
	}

	// make sure export all dialog exists
	if (ui_widgets.android_all_dialog == NULL)
	{
		ui_widgets.android_all_dialog = create_android_all_dialog();
		gtk_widget_set_name(ui_widgets.android_all_dialog, "Export APK (All Projects)");
		gtk_window_set_transient_for(GTK_WINDOW(ui_widgets.android_all_dialog), GTK_WINDOW(main_widgets.window));

		g_signal_connect(ui_widgets.android_all_dialog, "response", G_CALLBACK(on_android_all_dialog_response), NULL);
		g_signal_connect(ui_widgets.android_all_dialog, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);

		ui_setup_open_button_callback_android(ui_lookup_widget(ui_widgets.android_all_dialog, "export_all_android_output_file_path"), NULL,
			GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_ENTRY(ui_lookup_widget(ui_widgets.android_all_dialog, "export_all_android_output_file_entry")));
	}

	gtk_window_present(GTK_WINDOW(ui_widgets.android_all_dialog));
}

static void on_keystore_dialog_response(GtkDialog *dialog, gint response, gpointer user_data)
{
	static int running = 0;
	if (running) return;

	running = 1;

	if (response != 1)
	{
		gtk_widget_hide(GTK_WIDGET(dialog));
	}
	else
	{
		int i;
		GtkWidget *widget;

		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.keystore_dialog, "button9"), FALSE);
		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.keystore_dialog, "button8"), FALSE);

		// keystore details
		widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_full_name_entry");
		gchar *full_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_company_name_entry");
		gchar *company_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_city_entry");
		gchar *city = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_country_entry");
		gchar *country = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_password1_entry");
		gchar *password1 = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_password2_entry");
		gchar *password2 = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		// output
		widget = ui_lookup_widget(ui_widgets.keystore_dialog, "keystore_output_file_entry");
		gchar *output_file = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

		// START CHECKS

		if (!output_file || !*output_file) { SHOW_ERR(_("You must choose an output location to save your keystore file")); goto keystore_dialog_clean_up; }

		if (g_file_test(output_file, G_FILE_TEST_EXISTS))
		{
			if (!dialogs_show_question(_("\"%s\" already exists. Do you want to overwrite it?"), output_file))
			{
				goto keystore_dialog_clean_up;
			}
		}

		// check full name
		if (strlen(full_name) > 30) { SHOW_ERR(_("Full name must be less than 30 characters")); goto keystore_dialog_clean_up; }
		for (i = 0; i < strlen(full_name); i++)
		{
			if ((full_name[i] < 97 || full_name[i] > 122)
				&& (full_name[i] < 65 || full_name[i] > 90)
				&& full_name[i] != 32)
			{
				SHOW_ERR(_("Full name contains invalid characters, must be A-Z and spaces only"));
				goto keystore_dialog_clean_up;
			}
		}
		if (!*full_name)
		{
			g_free(full_name);
			full_name = g_strdup("Unknown");
		}

		// check company name
		if (strlen(company_name) > 30) { SHOW_ERR(_("Company name must be less than 30 characters")); goto keystore_dialog_clean_up; }
		for (i = 0; i < strlen(company_name); i++)
		{
			if ((company_name[i] < 97 || company_name[i] > 122)
				&& (company_name[i] < 65 || company_name[i] > 90)
				&& company_name[i] != 32)
			{
				SHOW_ERR(_("Company name contains invalid characters, must be A-Z and spaces only"));
				goto keystore_dialog_clean_up;
			}
		}
		if (!*company_name)
		{
			g_free(company_name);
			company_name = g_strdup("Unknown");
		}

		// city
		if (strlen(city) > 30) { SHOW_ERR(_("City must be less than 30 characters")); goto keystore_dialog_clean_up; }
		for (i = 0; i < strlen(city); i++)
		{
			if ((city[i] < 97 || city[i] > 122)
				&& (city[i] < 65 || city[i] > 90)
				&& city[i] != 32)
			{
				SHOW_ERR(_("City contains invalid characters, must be A-Z and spaces only"));
				goto keystore_dialog_clean_up;
			}
		}
		if (!*city)
		{
			g_free(city);
			city = g_strdup("Unknown");
		}

		// country
		if (strlen(country) > 0 && strlen(country) != 2) { SHOW_ERR(_("Country code must be 2 characters")); goto keystore_dialog_clean_up; }
		for (i = 0; i < strlen(country); i++)
		{
			if ((country[i] < 97 || country[i] > 122)
				&& (country[i] < 65 || country[i] > 90))
			{
				SHOW_ERR(_("Country code contains invalid characters, must be A-Z only"));
				goto keystore_dialog_clean_up;
			}
		}
		if (!*country)
		{
			g_free(country);
			country = g_strdup("Unknown");
		}

		// check passwords
		if (!password1 || !*password1) { SHOW_ERR(_("Password cannot be blank")); goto keystore_dialog_clean_up; }
		if (strlen(password1) < 6) { SHOW_ERR(_("Password must be at least 6 characters long")); goto keystore_dialog_clean_up; }
		if (strchr(password1, '"')) { SHOW_ERR(_("Password cannot contain double quotes")); goto keystore_dialog_clean_up; }
		if (strcmp(password1, password2) != 0) { SHOW_ERR(_("Passwords do not match")); goto keystore_dialog_clean_up; }

		goto keystore_dialog_continue;

	keystore_dialog_clean_up:
		if (full_name) g_free(full_name);
		if (company_name) g_free(company_name);
		if (city) g_free(city);
		if (country) g_free(country);
		if (password1) g_free(password1);
		if (password2) g_free(password2);

		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.keystore_dialog, "button8"), TRUE);
		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.keystore_dialog, "button9"), TRUE);
		running = 0;
		return;

	keystore_dialog_continue:

		;

		// CHECKS COMPLETE, START KEY GENERATION

#ifdef G_OS_WIN32
		gchar* path_to_keytool = g_build_path("/", app->datadir, "android", "jre", "bin", "keytool.exe", NULL);
#else
		//gchar* path_to_jarsigner = g_build_path( "/", "/usr", "bin", "keytool", NULL );
		gchar* path_to_keytool = g_build_path("/", app->datadir, "android", "jre", "bin", "keytool", NULL);
#endif

		// decalrations
		gchar **argv = NULL;
		gchar *dname = NULL;
		int status = 0;
		GError *error = 0;
		gchar *keystore_name = NULL;
		gchar* str_out = 0;

		utils_str_replace_char(output_file, '\\', '/');
		gchar* slash = strrchr(output_file, '/');
		if (slash)
		{
			keystore_name = g_strdup(slash + 1);
			*slash = 0;
		}
		else
		{
			keystore_name = g_strdup(output_file);
			g_free(output_file);
			output_file = global_project_prefs.project_file_path;
		}

		if (!g_file_test(path_to_keytool, G_FILE_TEST_EXISTS))
		{
			SHOW_ERR1(_("Could not find keytool program, the path \"%s\" is incorrect"), path_to_keytool);
			goto keystore_dialog_cleanup2;
		}

		dname = g_strdup_printf("CN=%s, O=%s, L=%s, C=%s", full_name, company_name, city, country);

		// package manifest and resources
		argv = g_new0(gchar*, 19);
		argv[0] = g_strdup(path_to_keytool);
		argv[1] = g_strdup("-genkey");
		argv[2] = g_strdup("-keystore");
		argv[3] = g_strdup(keystore_name);
		argv[4] = g_strdup("-alias");
		argv[5] = g_strdup("mykeystore");
		argv[6] = g_strdup("-keyalg");
		argv[7] = g_strdup("RSA");
		argv[8] = g_strdup("-keysize");
		argv[9] = g_strdup("2048");
		argv[10] = g_strdup("-validity");
		argv[11] = g_strdup("20000");
		argv[12] = g_strdup("-storepass");
		argv[13] = g_strdup(password1);
		argv[14] = g_strdup("-keypass");
		argv[15] = g_strdup(password1);
		argv[16] = g_strdup("-dname");
		argv[17] = g_strdup(dname);
		argv[18] = NULL;

		if (!utils_spawn_sync(output_file, argv, NULL, 0, NULL, NULL, &str_out, NULL, &status, &error))
		{
			SHOW_ERR1(_("Failed to run keytool program: %s"), error->message);
			g_error_free(error);
			error = NULL;
			goto keystore_dialog_cleanup2;
		}

		if (status != 0 && str_out && *str_out)
		{
			SHOW_ERR1(_("keytool program returned error: %s"), str_out);
			goto keystore_dialog_cleanup2;
		}

		gtk_widget_hide(GTK_WIDGET(dialog));

	keystore_dialog_cleanup2:

		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.keystore_dialog, "button8"), TRUE);
		gtk_widget_set_sensitive(ui_lookup_widget(ui_widgets.keystore_dialog, "button9"), TRUE);

		if (error) g_error_free(error);

		if (path_to_keytool) g_free(path_to_keytool);
		if (argv) g_strfreev(argv);
		if (dname) g_free(dname);
		if (keystore_name) g_free(keystore_name);
		if (str_out) g_free(str_out);

		if (full_name) g_free(full_name);
		if (company_name) g_free(company_name);
		if (city) g_free(city);
		if (country) g_free(country);
		if (password1) g_free(password1);
		if (password2) g_free(password2);
	}

	running = 0;
}

