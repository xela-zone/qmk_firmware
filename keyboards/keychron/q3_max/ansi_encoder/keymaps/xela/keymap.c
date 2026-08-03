/* Copyright 2024 ~ 2026 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "keychron_common.h"


enum layers {
    BASE,
    FN,
};

#define FN_KEY MO(FN)

// Tap dance declarations
enum tap_dance_actions {
    TD_LOCK,
};

enum custom_keycodes {
    RESET_PALETTE = QK_USER_0,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT_tkl_ansi(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_MUTE,  KC_PSCR,  TD(TD_LOCK),        LUMINO,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,   KC_HOME,            KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_DEL,   KC_END,             KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LCMD,  KC_LALT,                                KC_SPC,                                 KC_RALT,  FN_KEY,   KC_APP,   KC_RCTL,  KC_LEFT,  KC_DOWN,            KC_RGHT),

    [FN] = LAYOUT_tkl_ansi(
        QK_BOOT,  KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  _______,  _______,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  LUMINO,   _______,  _______,            LUMINO,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            DB_TOGG,
        LUMINO,   UG_NEXT,  UG_VALU,  UG_HUEU,  UG_SATU,  UG_SPDU,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  RESET_PALETTE, QK_CLEAR_EEPROM, _______,
        _______,  UG_PREV,  UG_VALD,  UG_HUED,  UG_SATD,  UG_SPDD,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  _______,  _______,  _______,  _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______,            _______),
};

// clang-format on
#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [BASE] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [FN]   = {ENCODER_CCW_CW(UG_VALD, UG_VALU)},
};
#endif // ENCODER_MAP_ENABLE

// --- Tap Dance: Lock key (hold to lock) ---

typedef enum {
    TD_LOCK_NONE,
    TD_LOCK_SINGLE_TAP,
    TD_LOCK_HOLD,
} td_lock_state_t;

static td_lock_state_t td_lock_state = TD_LOCK_NONE;

static td_lock_state_t td_lock_get_state(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_LOCK_SINGLE_TAP;
        return TD_LOCK_HOLD;
    }
    return TD_LOCK_NONE;
}

void td_lock_finished(tap_dance_state_t *state, void *user_data) {
    td_lock_state = td_lock_get_state(state);
    if (td_lock_state == TD_LOCK_HOLD) {
        lumino_sleep_soon();
        tap_code16(LCTL(LSFT(KC_ESC)));
    }
    // Single tap: reserved for future use
}

void td_lock_reset(tap_dance_state_t *state, void *user_data) {
    td_lock_state = TD_LOCK_NONE;
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_LOCK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lock_finished, td_lock_reset),
};


void keyboard_post_init_user(void) {
    debug_enable = true;
    rgb_matrix_mode(RGB_MATRIX_COMMUNITY_MODULE_PALETTEFX_FLOW);
    uint8_t palette_index = PALETTEFX_AFTERBURN;
    rgb_matrix_sethsv(RGB_MATRIX_HUE_STEP * palette_index, 255, 255);
    rgb_matrix_set_speed_noeeprom(128);
    rgb_matrix_enable_noeeprom();
}

static const uint16_t dangerous_keys[] = {
    QK_BOOT,
    QK_CLEAR_EEPROM,
    DB_TOGG,
};

static bool is_dangerous_key(uint16_t keycode) {
    for (size_t i = 0; i < ARRAY_SIZE(dangerous_keys); i++) {
        if (dangerous_keys[i] == keycode) {
            return true;
        }
    }
    return false;
}

bool rgb_matrix_indicators_user(void) {
    if (rgb_matrix_get_val() == 0) {
        return false;
    }

    if (host_keyboard_led_state().caps_lock) {
        rgb_matrix_set_color(CAPS_LOCK_INDEX, 255, 60, 0);
    }

    // If the FN layer is active
    if (IS_LAYER_ON(FN)) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            for (uint8_t col = 0; col < MATRIX_COLS; col++) {
                // Get the LED index mapped to this matrix coordinate
                uint8_t led_idx = g_led_config.matrix_co[row][col];

                // If this switch has an LED connected to it
                if (led_idx != NO_LED) {
                    // Read the keycode from the keymaps array stored in Flash memory (PROGMEM)
                    uint16_t keycode = pgm_read_word(&keymaps[FN][row][col]);

                    // Highlight dangerous keys in Red
                    if (is_dangerous_key(keycode)) {
                        rgb_matrix_set_color(led_idx, 255, 0, 0);
                    } else if (keycode != KC_TRNS) {
                        // Light up active keys in bright Cyan
                        rgb_matrix_set_color(led_idx, 0, 255, 255);
                    } else {
                        // Light up background keys in a dim blue
                        rgb_matrix_set_color(led_idx, 0, 0, 20);
                    }
                }
            }
        }
    }
    return false;
}

// --- Noctalia Dynamic Theme Sync ---
#include "palettefx.h"
#if defined(RAW_ENABLE)
#    include "raw_hid.h"
#endif

static uint16_t g_noctalia_live_palette[16];
static bool     g_has_live_palette = false;

const uint16_t* palettefx_get_palette_data(void) {
    if (g_has_live_palette) {
        return g_noctalia_live_palette;
    }
    return palettefx_get_palette_data_by_index(PALETTEFX_AFTERBURN);
}

#if defined(RAW_ENABLE)
void raw_hid_receive_user(uint8_t *data, uint8_t length) {
    if (length >= 31 && data[0] == 0x77) {
        for (uint8_t i = 0; i < 15; i++) {
            uint16_t low  = data[1 + i * 2];
            uint16_t high = data[2 + i * 2];
            g_noctalia_live_palette[i] = low | (high << 8);
        }
        g_noctalia_live_palette[15] = g_noctalia_live_palette[14];
        g_has_live_palette = true;
    }
}
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case RESET_PALETTE:
            if (record->event.pressed) {
                g_has_live_palette = false;
            }
            return false;
    }
    return true;
}

bool led_update_user(led_t led_state) {
    if (led_state.caps_lock && led_state.num_lock && led_state.scroll_lock) {
        return false;
    }
    return true;
}










