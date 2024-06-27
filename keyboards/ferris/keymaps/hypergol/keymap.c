#include QMK_KEYBOARD_H

enum ferris_layers {
    _base,
    _nav,
    _prog,
    _num,
};

const uint16_t KC_BE = LT(_base, KC_E);
const uint16_t KC_BI = LT(_base, KC_I);
const uint16_t KC_SS = LSFT_T(KC_S);
const uint16_t KC_SL = LSFT_T(KC_L);
const uint16_t KC_GD = LGUI_T(KC_D);
const uint16_t KC_GK = LGUI_T(KC_K);
const uint16_t KC_AF = LALT_T(KC_F);
const uint16_t KC_AJ = LALT_T(KC_J);

const uint16_t KC_S2 = LSFT_T(KC_2);
const uint16_t KC_S9 = LSFT_T(KC_9);
const uint16_t KC_G3 = LGUI_T(KC_3);
const uint16_t KC_G8 = LGUI_T(KC_8);
const uint16_t KC_A4 = LALT_T(KC_4);
const uint16_t KC_A7 = LALT_T(KC_7);

// Alias for a custom LT key, use it in a keymap like any other KeyCode.
const uint16_t LT_NUM_OTS = LT(_num, KC_NO);
const uint16_t MAIN_SPACE = LT(_prog, KC_SPC);
const uint16_t E_OR_CTRL = LCTL_T(KC_NUBS);
const uint16_t MAGIC_BACKSPACE = LT(_nav, KC_BSPC);

enum custom_keycodes {
    PARENT_PATH = SAFE_RANGE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_base] = LAYOUT_split_3x5_2(
        KC_Q, KC_W,  KC_BE, KC_R,  KC_T,       KC_Y, KC_U,  KC_BI,   KC_O,   KC_P,
        KC_A, KC_SS, KC_GD, KC_AF, KC_G ,      KC_H, KC_AJ, KC_GK,   KC_SL,  KC_SCLN,
        KC_Z, KC_X,  KC_C,  KC_V,  KC_B,       KC_N, KC_M,  KC_COMM, KC_DOT, KC_SLSH,
        LT_NUM_OTS, E_OR_CTRL,     MAIN_SPACE, MAGIC_BACKSPACE
    ),

    [_num] = LAYOUT_split_3x5_2(
        KC_NO,   S(KC_2),    S(KC_3), KC_NO,     KC_NO,      KC_NO, KC_NO,  KC_NO, KC_NO, KC_NO,
        KC_1,    KC_S2,      KC_G3,   KC_A4,     KC_5,       KC_6,  KC_A7,  KC_G8, KC_S9, KC_0,
        C(KC_B), C(KC_SLSH), C(KC_I), C(KC_DOT), KC_NO,      KC_NO, KC_TAB, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_TRNS,      S(KC_SPC), KC_NO
    ),

    [_nav] = LAYOUT_split_3x5_2(
        KC_NO,   S(KC_TAB), KC_MS_U, KC_TAB,  KC_NO,      KC_HOME, KC_PGDN, KC_PGUP, KC_END,  KC_NO,
        C(KC_A), KC_MS_L,   KC_MS_D, KC_MS_R, KC_NO,      KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_NO,
        C(KC_Z), C(KC_X),   C(KC_W), C(KC_V), KC_NO,      KC_WH_L, KC_WH_D, KC_WH_U, KC_WH_R, KC_NO,
        KC_ENTER, KC_BTN1,     KC_NO, KC_NO
    ),

    [_prog] = LAYOUT_split_3x5_2(
        KC_CIRC, RALT(KC_W), RALT(KC_E), KC_DLR,  KC_PERC,      S(KC_0), KC_AMPR, KC_ASTR, KC_QUOT, KC_GRV,
        KC_LCBR, RALT(KC_S), RALT(KC_D), KC_RCBR, KC_EQL,       KC_BSLS, KC_PLUS, KC_PMNS, KC_PSLS, KC_DQUO,
        KC_TILD, KC_LBRC,    KC_RBRC,    KC_UNDS, S(KC_9),      KC_PIPE, S(KC_X), S(KC_Q), S(KC_N), S(KC_P),
        LCTL_T(KC_ENTER), LT(_num, KC_SPC),     KC_NO, KC_AT
    ),
};

uint32_t last_key_press_timer = 0;
bool scroll_lock_state = false;

bool remember_last_key_user(uint16_t kc, keyrecord_t* record, uint8_t* remembered_mods) {
    return kc != MAGIC_BACKSPACE;
}

#define with_mod_on_hold(kc, mod) ((record->tap.count > 0) ? kc : mod(kc))

// This callback is called before the keycode is generally sent
// returning false cancels any furnther treatment.
// for instance, calling `tap_code(KC_B)` if KC_A is pressed but true is
// returned, "ba" is sent, but if `false` is returned, it’s just "b"
bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    static bool letter_q_pressed = false;

    if (!record->event.pressed) return true;

    if (keycode != MAGIC_BACKSPACE && !letter_q_pressed)
        last_key_press_timer = timer_read32();

    if (letter_q_pressed
        && timer_elapsed32(last_key_press_timer) < TAP_INTERVAL
    ) switch (keycode) {
        case KC_SS:
        case KC_GD:
        case KC_BE:
        case KC_P:
        case KC_C:
        case E_OR_CTRL:
            letter_q_pressed = false;
            last_key_press_timer = timer_read32();
            tap_code(KC_A);
            return true;
    }

    // letter_q_pressed = false;

    switch (keycode) {
        case KC_H: letter_q_pressed = true; return true;
        case KC_BE: tap_code16(with_mod_on_hold(KC_E, LGUI)); return false;
        case KC_BI: tap_code16(with_mod_on_hold(KC_I, LGUI)); return false;
        case PARENT_PATH: SEND_STRING("nn-"); return false;

        case MAGIC_BACKSPACE:
            if (record->tap.count == 0) return true;
            if (get_mods() != 0) return true;
            if (timer_elapsed32(last_key_press_timer) > TAP_INTERVAL) return true;

            last_key_press_timer = timer_read32();
            uint16_t last_keycode = get_last_keycode();
            if (last_keycode == KC_SS) {
                tap_code(KC_X);
                tap_code(KC_NUBS);
            }

            else
                tap_code16(last_keycode);
            return false;

        case MAIN_SPACE:
            if (record->tap.count == 0) return true;
            if (get_oneshot_mods() & MOD_BIT(KC_LSFT))
                tap_code16(LSFT(KC_NUBS));
            else
                tap_code(KC_SPC);
            return false;

        case LT_NUM_OTS:
            if (record->tap.count == 1) {
                set_oneshot_mods(MOD_LSFT);
                return false;
            }
            if (record->tap.count == 2) {
                tap_code(KC_CAPS_LOCK);
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
    COMBO_ENTER,
    COMBO_ESC,
    COMBO_TAB,
};

const uint16_t PROGMEM sd_combo[] = { KC_SS, KC_GD, COMBO_END };
const uint16_t PROGMEM df_combo[] = { KC_GD, KC_AF, COMBO_END };
const uint16_t PROGMEM kj_combo[] = { KC_GK, KC_AJ, COMBO_END };
const uint16_t PROGMEM lk_combo[] = { KC_SL, KC_GK, COMBO_END };

combo_t key_combos[] = {
    [COMBO_CTRL_BS] = COMBO(sd_combo, LCTL(KC_BSPC)),
    [COMBO_ENTER]   = COMBO(df_combo, KC_ENTER),
    [COMBO_ESC]     = COMBO(kj_combo, KC_ESC),
    [COMBO_TAB]     = COMBO(lk_combo, KC_TAB),
};

bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    return timer_elapsed32(last_key_press_timer) > TAP_INTERVAL;
}
