#include <pebble.h>

/*************/
/* Constants */
/*************/
#define WAIT_TIME 25
#define REFRESH_INTERVAL 5


/***************/
/* Enumerators */
/***************/

// App message keys
enum CSGO_KEYS {
        CSGO_VALID_SERVER_IP        = 0,
        CSGO_TIME_SINCE_LAST_UPDATE = 1,
        CSGO_MAP_MODE               = 2,
        CSGO_ROUND_PHASE            = 3,
        CSGO_BOMB_STATUS            = 4,
        CSGO_TEAM_CT_SCORE          = 5,
        CSGO_TEAM_T_SCORE           = 6
};

// Gamemode definitions
enum GAMEMODE_KEYS {
        MODE_NULL        = 0,
        MODE_COMPETITIVE = 1,
        MODE_CASUAL      = 2,
        MODE_DEATHMATCH  = 3,
        MODE_DEMOLITION  = 4,
        MODE_HOSTAGE     = 5,
        MODE_CUSTOM      = 6
};

// Round Phase definitions
enum COMPETITIVE_ROUND_PHASE {
        RND_NULL       = 0,
        RND_LIVE       = 1,
        RND_FREEZETIME = 2,
        RND_OVER       = 3
};

// Bomb status definitions
enum COMPETITIVE_BOMB_STATUS {
        BOMB_NULL     = 0,
        BOMB_PLANTED  = 1,
        BOMB_DEFUSED  = 2,
        BOMB_EXPLODED = 3
};

/********************/
/* Global Variables */
/********************/

// Overhead variables
static bool valid_server_ip = false;  // Checks for a valid server IP address.


static bool drop_flg        = true;   // Used to force everyother call to CSGO_ROUND_PHASE is dropped.
                                      // Without this being set properly, the Watch vibrates 2 times
                                      // instead of 1.


static int waiter           = 0;      // Variable used to temporarily lock the Watch from
                                      // vibrating again.

static uint8_t last_updated = 0;      // Variable used to hold approx. how long ago the last message from
                                      // the server was recieved.

// Watch requirements
static Window *window;
static TextLayer *text_layer;
static TextLayer *t_verbose;

static AppSync s_sync;
static uint8_t s_sync_buffer[128];
