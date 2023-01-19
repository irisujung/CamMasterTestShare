#pragma once

// const COLORREF ///////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* 일반적인 색상 선언                                                   */
/************************************************************************/
const COLORREF	COLOR_WHITE = RGB(255, 255, 255);			// White
const COLORREF	COLOR_WHITE_HIGH = RGB(250, 250, 255);			// Deep White
const COLORREF	COLOR_BLACK = RGB(0, 0, 0);			// Black
const COLORREF	COLOR_SILVER = RGB(192, 192, 192);			// Silver
const COLORREF	COLOR_SILVER_LOW = RGB(220, 220, 220);			// Silver Low
const COLORREF	COLOR_SILVER_LIGHT = RGB(220, 220, 220);			// Silver Light
const COLORREF	COLOR_SILVER_HLIGHT = RGB(240, 240, 240);			// Silver Heavy Light
const COLORREF	COLOR_SLATEGRAY = RGB(112, 128, 144);			// SlateGray
const COLORREF	COLOR_SLATEGRAY_LOW = RGB(119, 136, 153);			// SlateGray Low
const COLORREF	COLOR_SLATEGRAY_LIGHT = RGB(119, 136, 153);			// LightSlateGray
const COLORREF	COLOR_RED = RGB(255, 0, 0);			// Red
const COLORREF	COLOR_RED_MID = RGB(255, 128, 128);			// Red Mid
const COLORREF	COLOR_RED_LOW = RGB(255, 200, 200);			// LowLevel-red
const COLORREF	COLOR_GREEN = RGB(0, 255, 0);			// Greed
const COLORREF	COLOR_GREEN_MID = RGB(128, 255, 128);			// Greed Mid
const COLORREF	COLOR_GREEN_LOW = RGB(245, 255, 245);			// Greed Low
const COLORREF	COLOR_GREEN_DARK = RGB(64, 128, 64);			// Greed Dark
const COLORREF	COLOR_BLUE = RGB(0, 0, 255);			// Blue
const COLORREF	COLOR_BLUE_HEADER = RGB(169, 196, 233);			// Blue
const COLORREF	COLOR_BLUE_HEADER_LT = RGB(211, 220, 233);			// Blue
const COLORREF	COLOR_BLUE_WEEK = RGB(233, 237, 241);			// Week Blue240, 240, 250
const COLORREF	COLOR_BLUE_LIGHT = RGB(192, 255, 255);			// LightBlue
const COLORREF	COLOR_BLUE_MID = RGB(128, 128, 255);			// Mid Blue
const COLORREF	COLOR_BLUE_LOW = RGB(100, 100, 150);			// Low Blue
const COLORREF	COLOR_BLUE_HALF = RGB(128, 128, 125);			// Blue Half
const COLORREF	COLOR_BLUE_DARK = RGB(0, 0, 64);			// Blue Half
const COLORREF	COLOR_BLUE_LIGHT2 = RGB(230, 250, 230);			// Blue Half
const COLORREF	COLOR_CYAN = RGB(0, 255, 255);			// Cyan
const COLORREF	COLOR_CYAN_LOW = RGB(128, 255, 255);			// Cyan Medium
const COLORREF	COLOR_CYAN_MID = RGB(0, 128, 255);			// Cyan Medium
const COLORREF	COLOR_MAGENTA = RGB(255, 0, 255);			// Magenta
const COLORREF	COLOR_MAGENTA_LOW = RGB(255, 128, 255);			// Magenta Medium
const COLORREF	COLOR_YELLOW = RGB(255, 255, 0);			// Yellows
const COLORREF	COLOR_YELLOW_LIGHT = RGB(255, 255, 220);			// Light Yellow
const COLORREF	COLOR_YELLOW_LOW = RGB(196, 196, 0);			// Yellow Low
const COLORREF	COLOR_YELLOW_MID = RGB(255, 255, 128);			// Yellow Mid
const COLORREF	COLOR_ORANGE = RGB(255, 128, 0);			// Orange
const COLORREF	COLOR_ORANGE_DARK = RGB(255, 192, 0);			// Orange
const COLORREF	COLOR_LEMON = RGB(255, 250, 230);			// LemonChiffon
const COLORREF	COLOR_LABEL = RGB(205, 205, 235);			// LemonChiffon
const COLORREF	COLOR_LABEL_LT = RGB(225, 225, 255);			// LemonChiffon
const COLORREF	COLOR_EMPTY = RGB(240, 242, 244);			// LemonChiffon
/************************************************************************/
/* Dialog Background                                                    */
/************************************************************************/
const COLORREF	COLOR_BACK_GROUND = RGB(233, 237, 241);			// Background Color
/************************************************************************/
/* Tower Lamp 색상	                                                    */
/************************************************************************/
const COLORREF	COLOR_LAMP_R = RGB(255, 180, 180);			// Tower Lamp Red
const COLORREF	COLOR_LAMP_Y = RGB(255, 255, 180);			// Tower Lamp Yellow
const COLORREF	COLOR_LAMP_G = RGB(180, 255, 180);			// Tower Lamp Green
/************************************************************************/
/* Glass 타입별 색상	                                                */
/************************************************************************/
const COLORREF	COLOR_GLASS_ENCAP = RGB(255, 255, 200);			// Encap Glass
const COLORREF	COLOR_GLASS_TFT = RGB(200, 255, 200);			// TFT Glass
const COLORREF	COLOR_GLASS_ASSEMBLY = RGB(255, 200, 255);			// Assembly Glass
const COLORREF  COLOR_GLASS_BARE = RGB(255, 230, 210);			// Bare Glass
const COLORREF  COLOR_GLASS_NONE = RGB(255, 255, 255);			// None Glass
/************************************************************************/
/* CHAMBER MODE COLOR                                                   */
/************************************************************************/
const COLORREF	COLOR_CHMODE_NONE = RGB(255, 0, 0);
const COLORREF	COLOR_CHMODE_MANUAL = RGB(0, 0, 0);
const COLORREF	COLOR_CHMODE_SEMI = RGB(255, 0, 255);
const COLORREF	COLOR_CHMODE_AUTO = RGB(0, 0, 255);
/************************************************************************/
/* CHAMBER STATUS COLOR                                                 */
/************************************************************************/
const COLORREF	COLOR_STATUS_NONE = RGB(255, 0, 0);
const COLORREF	COLOR_STATUS_STOP = RGB(0, 0, 0);
const COLORREF	COLOR_STATUS_START = RGB(0, 0, 255);
const COLORREF	COLOR_STATUS_PAUSE = RGB(255, 0, 255);
const COLORREF	COLOR_STATUS_CONTINUE = RGB(0, 0, 255);
/************************************************************************/
/* VACUUM STATUS COLOR                                                  */
/************************************************************************/
const COLORREF	COLOR_VACSTS_NONE = RGB(255, 0, 0);
const COLORREF	COLOR_VACSTS_ATM = RGB(0, 0, 0);
const COLORREF	COLOR_VACSTS_LOW = RGB(0, 255, 255);
const COLORREF	COLOR_VACSTS_HIGH = RGB(0, 0, 255);
/************************************************************************/
/* PROCESS STATUS COLOR                                                 */
/************************************************************************/
const COLORREF	COLOR_PRCSTS_NONE = RGB(255, 0, 0);
const COLORREF	COLOR_PRCSTS_READY = RGB(0, 0, 0);
const COLORREF	COLOR_PRCSTS_PREOCESSING = RGB(0, 0, 255);
const COLORREF	COLOR_PRCSTS_COMPLETE = RGB(255, 0, 255);
const COLORREF	COLOR_PRCSTS_ALARM = RGB(255, 0, 0);
/************************************************************************/
/* REMOTE/LOCAL MODE STAUTS COLOR                                       */
/************************************************************************/
const COLORREF COLOR_LOCAL = RGB(0, 0, 0);
const COLORREF COLOR_REMOTE = RGB(255, 0, 0);
/************************************************************************/
/* BUTTON ENABLE/DISABLE COLOR (BackColor)                              */
/************************************************************************/
const COLORREF	COLOR_BUTTON_ENABLE = RGB(0, 0, 255);
const COLORREF	COLOR_BUTTON_DISABLE = RGB(64, 64, 64);
/************************************************************************/
/* BUTTON ON/OFF COLOR (BackColorInterior)				                */
/************************************************************************/
const COLORREF	COLOR_BUTTON_ON = RGB(128, 255, 255);
const COLORREF	COLOR_BUTTON_OFF = COLOR_SILVER_LOW;
/************************************************************************/
/* BUTTON RUN On/Off COLOR (BackColorInterior)				            */
/************************************************************************/
const COLORREF	COLOR_BUTTON_RUN_ON = COLOR_GREEN_MID;
const COLORREF	COLOR_BUTTON_RUN_OFF = COLOR_SILVER_LOW;
/************************************************************************/
/* BUTTON STOP On/Off COLOR (BackColorInterior)				            */
/************************************************************************/
const COLORREF	COLOR_BUTTON_STOP_ON = COLOR_RED_MID;
const COLORREF	COLOR_BUTTON_STOP_OFF = COLOR_SILVER_LOW;
/************************************************************************/
/* STATUS ON/OFF COLOR (BackColorInterior) - 3 State                    */
/************************************************************************/
const COLORREF	COLOR_MS_ON = COLOR_BLUE_MID;
const COLORREF	COLOR_MS_OFF = COLOR_RED_MID;
const COLORREF	COLOR_MS_NONE = COLOR_WHITE;
/************************************************************************/
/* SUBSTRATE COLOR (BackColorInterior)				                    */
/************************************************************************/
const COLORREF	COLOR_SUB_GLASS = COLOR_CYAN;
const COLORREF	COLOR_SUB_FILM_BOTTOM = COLOR_YELLOW_MID;
const COLORREF	COLOR_SUB_FILM_TOP = COLOR_MAGENTA_LOW;
const COLORREF	COLOR_SUB_TRAY = COLOR_SILVER;

/************************************************************************/
/* PUMP STATUS COLOR                                                    */
/************************************************************************/
const COLORREF	COLOR_PUMP_ALARM = COLOR_RED;
const COLORREF	COLOR_PUMP_WARNING = COLOR_MAGENTA_LOW;
const COLORREF	COLOR_PUMP_RUN2 = RGB(128, 255, 128);
const COLORREF	COLOR_PUMP_NONE = COLOR_WHITE;
