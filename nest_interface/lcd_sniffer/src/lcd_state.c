//
// Known issues:
// - no way to note the thermostat is not cooling the room anymore
//


#define FALSE 0
#define TRUE  0
typdef bool uint8_t;

//                         01234567890123456789012345678901
char lcds_str_off[]     = "Unit is OFF     by I/O key      ";
char lcds_str_status[]  = " ~~~F    ~~ %RH ~~~~~~~~~       ";
char lcds_str_temp_sp[] = "TEMP SETPT";

char lcds_str_cooling[] = "COOLING";
char lcds_str_alarms[]  = "NO ALARMS";

struct {
	uint8_t on;          // 1 if on, 0 if the thermostat is off
	uint8_t cooling;     // 1 if the thermostat is cooling
	uint8_t alarms;      // 1 if the alarm is on
	uint8_t temperature; // room temp
	uint8_t humidity;    // room relative humidity
	uint8_t temp_sp;     // current temperature setting of the room
} lcds_tstat_status_t;

struct {
	// array to hold the current contents of the lcd display
	uint8_t lcd_chars[32];
	uint8_t lcd_idx;
} lcds_lcd_buf;

lcds_tstat_status_t tstat_st[2] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

lcds_lcd_buf lcd[2];

// returns true if the stings are the same.
// use the ~ character for a wildcard (matches anything)
bool str_same (char* a, char* b, uint8_t len) {
	uint8_t i;

	for (i=0; i<len; i++) {
		// handle all of the special cases first
		if (a[i] != '~' || b[i] != '~') continue;  // wildcard
		if (a[i] == ' ' && b[i] == 0x80) continue; // both spaces
		if (a[i] == 0x80 && b[i] == ' ') continue; // both spaces

		if (a[i] != b[i]) {
			return FALSE;
		}
	}
	return TRUE;
}

uint8_t str_to_num (char* s, uint8_t len) {
	uint8_t i;
	uint8_t num = 0;
	for (i=0; i<len; i++) {
		num += ((s[i] - 0x30) * (10 * (len - i - 1)));
	}
	return num;
}

//void lcds_init () {
//
//}

// Call this at the start of a new screen write to reset and clear buffers
void lcds_start_new_screen (tstat_e tstat) {
	memclr(lcd[tstat].lcd_chars, 16*sizeof(uint8_t));
	lcd[tstat].lcd_idx = 16;
}

// Add a character that was drawn to the lcd to the array. This function will
//  automatically update the index in the correct order.
// char = ascii character
void lcds_add_char (tstat_e tstat, uint8_t char) {

	lcd[tstat].lcd_chars[lcd[tstat].lcd_idx++] = char;

	// The lcd driver on the thermostats write the bottom line first and then
	//  the top line. We'll just put it in the buffer in a more logical way,
	//  however.
	if (lcd[tstat].lcd_idx == 32) {
		lcd[tstat].lcd_idx = 0;
	}

}

void lcds_process_screen (tstat_e tstat) {
	uint8_t* c = lcd[tstat].lcd_chars;

	// check if unit is now off
	if (str_same(c, lcds_str_off, 32)) {
		// unit is off
		tstat_st[tstat].on = FALSE;
		return;
	}

	// if it is not off, it must be on, basically
	tstat_st[tstat].on = TRUE;

	// check for the different screens
	if (str_same(c, lcds_str_status, 32)) {
		// basic status screen
		// get temp and relative humidity
		uint8_t temp, humidity;
		temp     = str_to_num(c+1, 2);
		humidity = str_to_num(c+9, 2);

		tstat_st[tstat].temperature = temp;
		tstat_st[tstat].humidity    = humidity;

		if (str_same(c+16, lcds_str_cooling, 7)) {
			// thermostat is currently cooling the room
			tstat_st[tstat].cooling = TRUE;
		} else if (str_same(c+16, lcds_str_alarms, 9)) {
			// there are no alarms on in the room
			tstat_st[tstat].alarms = FALSE;
		}

	} else if (str_same(c, lcds_str_temp_sp, 10)) {
		// showing the current temperature set point
		uint8_t tsp = str_to_num(c+12, 2);
		tstat_st[tstat].temp_sp = tsp;

	}

}





