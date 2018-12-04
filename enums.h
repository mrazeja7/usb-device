#ifndef __ENUMS_H
#define __ENUMS_H

#include "stm324xg_eval_ioe.h"

/* SETUP packet request enums */
enum {
    standard = 0x0,
    class = 0x1,
    vendor = 0x2,
    reserved = 0x3
} Setup_request_type;
enum {
    device = 0x0,
    interface = 0x1,
    endpoint = 0x2,
    other = 0x3
} Setup_recipient;
enum {
    GET_STATUS = 0x0,
    CLEAR_FEATURE = 0x1,
    SET_FEATURE = 0x3,
    SET_ADDRESS = 0x5,
    GET_DESCRIPTOR = 0x6,
    SET_DESCRIPTOR = 0x7,
    GET_CONFIGURATION = 0x8,
    SET_CONFIGURATION = 0x9,
    SET_IDLE = 0xA
} Setup_bRequest;
enum {
    device_desc = 0x1,
    configuration_desc = 0x2,
    string_desc = 0x3,
    interface_desc = 0x4,
    endpoint_desc = 0x5,
    device_qualifier_desc = 0x6,
    other_speed_qualifier_desc = 0x7,
    otg_desc = 0x9,
    hid_report_descriptor = 0x22
} Setup_get_descriptor_values;

/* joystick/mouse movement enums */
enum {
    CURSOR_LEFT = JOY_LEFT,
    CURSOR_RIGHT = JOY_RIGHT,
    CURSOR_UP = JOY_UP,
    CURSOR_DOWN = JOY_DOWN,
    MOUSEBUTTON_LEFT = 0xE,
    MOUSEBUTTON_RIGHT = 0xF
} Cursor_movement;

#endif