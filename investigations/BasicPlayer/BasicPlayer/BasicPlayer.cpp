// BasicPlayer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

static void Trim(char *buf)
{
	size_t l = strlen(buf);
	while(l > 0 && buf[l - 1] < 32)
		buf[--l] = 0;
}

static void SetStdinEcho(bool enable = true)
{
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hStdin, &mode);

	if (enable)
	{
		mode |= ENABLE_ECHO_INPUT;
	}
	else
	{
		mode &= ~ENABLE_ECHO_INPUT;
	}

	SetConsoleMode(hStdin, mode);
}

static sp_session_callbacks callbacks;

static void SP_CALLCONV logged_in(sp_session *session, sp_error error)
{
	sp_user *me;
	const char *display_name, *username;
	int cc;

	if (SP_ERROR_OK != error) {
		fprintf(stderr, "failed to log in to Spotify: %s\n",
		                sp_error_message(error));
		sp_session_release(session);
		exit(4);
	}

	// Let us print the nice message...
	me = sp_session_user(session);
	display_name = (sp_user_is_loaded(me) ? sp_user_display_name(me) : sp_user_canonical_name(me));
	username = sp_session_user_name (session);
	cc = sp_session_user_country(session);
	fprintf(stderr, "Logged in to Spotify as user %s [%s] (registered in country: %c%c)\n", username, display_name, cc >> 8, cc & 0xff);
}

int _tmain(int argc, _TCHAR* argv[])
{
	char username_buf[256];
	char password_buf[256];

	extern const uint8_t g_appkey[];
	extern const size_t g_appkey_size;

	cout << "Basic spotify player." << endl;

	// Grab a username and password from the user.
	cout << "Username: " << flush;
	fgets(username_buf, sizeof(username_buf), stdin);
	Trim(username_buf);
	cout << "Password: " << flush;
	SetStdinEcho(false);
	fgets(password_buf, sizeof(password_buf), stdin);
	Trim(password_buf);
	SetStdinEcho(true);
	cout << endl;

	// Login and initialize stuff
	sp_session_config config;
	sp_error error;
	sp_session *session;
	memset(&config, 0, sizeof(config));

	config.api_version = SPOTIFY_API_VERSION;
	config.cache_location = "tmp";
	config.settings_location = "tmp";
	config.application_key = g_appkey;
	config.application_key_size = g_appkey_size;
	config.user_agent = "node-spotify";
	config.callbacks = &callbacks;

	callbacks.logged_in = logged_in;

	error = sp_session_create(&config, &session);
	if (SP_ERROR_OK != error) {
		fprintf(stderr, "failed to create session: %s\n",
		                sp_error_message(error));
		return 2;
	}

	error = sp_session_login(session, username_buf, password_buf, true, nullptr);

	int next_timeout = 0;
	do {
		sp_session_process_events(session, &next_timeout);
	} while (true);

	// TODO: Stream the data to the console window

	cin.get();
	return 0;
}

