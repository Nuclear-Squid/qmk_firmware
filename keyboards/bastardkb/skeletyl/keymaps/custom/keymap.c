#include QMK_KEYBOARD_H
#include "features/mouse_turbo_click.h"

enum ferris_layers {
    _base,
    _nav,
    _prog,
    _num,
};

#define KC_BE LT(_base, KC_E)
#define KC_BI LT(_base, KC_I)
#define KC_SS LSFT_T(KC_S)
#define KC_SL LSFT_T(KC_L)
#define KC_GD LGUI_T(KC_D)
#define KC_GK LGUI_T(KC_K)
#define KC_AF LALT_T(KC_F)
#define KC_AJ LALT_T(KC_J)

#define KC_S2 LSFT_T(KC_2)
#define KC_S9 LSFT_T(KC_9)
#define KC_G3 LGUI_T(KC_3)
#define KC_G8 LGUI_T(KC_8)
#define KC_A4 LALT_T(KC_4)
#define KC_A7 LALT_T(KC_7)

// Alias for a custom LT key, use it in a keymap like any other KeyCode.
#define CTRL_OTS LCTL_T(KC_NO)

enum custom_keycodes {
    PARENT_PATH = SAFE_RANGE,
    TURBO_CLICK,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_base] = LAYOUT_split_3x5_3(
        KC_Q, KC_W,  KC_BE, KC_R,  KC_T,       KC_Y, KC_U,  KC_BI,   KC_O,   KC_P,
        KC_A, KC_SS, KC_GD, KC_AF, KC_G ,      KC_H, KC_AJ, KC_GK,   KC_SL,  KC_SCLN,
        KC_Z, KC_X,  KC_C,  KC_V,  KC_B,       KC_N, KC_M,  KC_COMM, KC_DOT, KC_SLSH,
        LT(_num, KC_TAB), CTRL_OTS, KC_ESC,     KC_ENTER, LT(_prog, KC_SPC), LT(_nav, KC_BSPC)
    ),

    [_num] = LAYOUT_split_3x5_3(
        KC_NO, S(KC_2), S(KC_3), KC_NO,       KC_NO,      KC_NO, KC_NO,  KC_NO, KC_NO, KC_NO,
        KC_1,  KC_S2,   KC_G3,   KC_A4,       KC_5,       KC_6,  KC_A7,  KC_G8, KC_S9, KC_0,
        KC_NO, KC_NO,   KC_NO,   PARENT_PATH, KC_NO,      KC_NO, KC_TAB, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_TRNS, KC_NO,      KC_NO, S(KC_SPC), KC_NO
    ),

    [_nav] = LAYOUT_split_3x5_3(
        KC_NO,   S(KC_TAB), KC_MS_U, KC_TAB,  KC_NO,      KC_HOME, KC_PGDN, KC_PGUP, KC_END,  KC_NO,
        C(KC_A), KC_MS_L,   KC_MS_D, KC_MS_R, KC_NO,      KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_NO,
        C(KC_Z), C(KC_X),   C(KC_W), C(KC_V), KC_NO,      KC_WH_L, KC_WH_D, KC_WH_U, KC_WH_R, KC_NO,
        KC_ENTER, KC_BTN1, TURBO_CLICK,     KC_NO, KC_NO, KC_NO
    ),

    [_prog] = LAYOUT_split_3x5_3(
        KC_CIRC, KC_NUBS,    S(KC_NUBS), KC_DLR,  KC_PERC,      S(KC_0), KC_AMPR, KC_ASTR,   KC_QUOT, KC_GRV,
        KC_LCBR, RALT(KC_S), RALT(KC_D), KC_RCBR, KC_EQL,       KC_BSLS, KC_PLUS, KC_PMNS,   KC_PSLS, KC_DQUO,
        KC_TILD, KC_LBRC,    KC_RBRC,    KC_UNDS, S(KC_9),      KC_PIPE, S(KC_O), S(KC_DOT), S(KC_N), S(KC_C),
        LCTL_T(KC_ENTER), LT(_num, KC_SPC), KC_NO,     KC_NO, KC_NO, KC_AT
    ),
};

#define with_mod_on_hold(kc, mod) ((record->tap.count > 0) ? kc : mod(kc))
volatile bool scroll_lock_state = false;
uint32_t last_key_press_timer = 0;

// This callback is called before the keycode is generally sent
// returning false cancels any furnther treatment.
// for instance, calling `tap_code(KC_B)` if KC_A is pressed but true is
// returned, "ba" is sent, but if `false` is returned, it’s just "b"
bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    if (!process_mouse_turbo_click(keycode, record, TURBO_CLICK)) { return false; }

    if (keycode == LT(_num, KC_TAB) && record->tap.count == 0) {
        tap_code(KC_NUM_LOCK);
        return true;
    }

    if (!record->event.pressed) return true;

    last_key_press_timer = timer_read32();

    switch (keycode) {
        case KC_BE: tap_code16(with_mod_on_hold(KC_E, LGUI)); return false;
        case KC_BI: tap_code16(with_mod_on_hold(KC_I, LGUI)); return false;
        case PARENT_PATH: SEND_STRING("nn-"); return false;

        case CTRL_OTS:
            if (scroll_lock_state) {
                tap_code16(KC_BTN1);
                return false;
            }
            if (record->tap.count == 1) {
                set_oneshot_mods(MOD_LSFT);
                return false;
            }
            if (record->tap.count == 2) {
                caps_word_on();
                return false;
            }
            return true;

        case LT(_nav, KC_BSPC):
            if (record->tap.count == 1 && get_oneshot_mods() & MOD_BIT(KC_LSFT)) {
                tap_code16(LCTL(KC_BSPC));
                return false;
            }
            return true;
    }

    return true;
}


layer_state_t layer_state_set_user(layer_state_t state) {
    del_oneshot_mods(MOD_BIT(KC_LSFT));
    return state;
}

bool led_update_user(led_t led_state) {
    scroll_lock_state = led_state.scroll_lock;
    return true;
}

enum combos {
    COMBO_CTRL_BS,
    COMBO_MOUSE_1,
};

const uint16_t PROGMEM sd_combo[] = { KC_SS, KC_GD, COMBO_END };
const uint16_t PROGMEM cv_combo[] = { KC_C,  KC_V,  COMBO_END };

combo_t key_combos[] = {
    [COMBO_CTRL_BS] = COMBO(sd_combo, LCTL(KC_BSPC)),
    [COMBO_MOUSE_1] = COMBO(cv_combo, KC_BTN1),
};

bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    return timer_elapsed32(last_key_press_timer) > TAP_INTERVAL;
}
