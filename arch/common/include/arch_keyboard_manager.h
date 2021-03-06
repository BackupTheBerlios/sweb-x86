/**
 * @file arch_keyboard_manager.h
 *
 */
 
#ifndef _KEYBOARD_MANAGER_H
#define _KEYBOARD_MANAGER_H

#ifdef __cplusplus

extern "C"
{
#endif


#ifdef __cplusplus
}
#endif

#include "ports.h"
#include "RingBuffer.h"
#include "atkbd.h"
#include "arch_bd_io.h"

/** @class KeyboardManager
 *
 * Class that contains architecture specific parameters for AT keyboard
 * communication
 *
 */
class KeyboardManager : public bdio
{
public:

  /**
   * Static constants
   * Keyboard modifiers
   *
   */
  static uint32 const KBD_META_LALT;
  static uint32 const KBD_META_RALT;
  static uint32 const KBD_META_CTRL;
  static uint32 const KBD_META_SHIFT;
  static uint32 const KBD_META_CAPS;
  static uint32 const KBD_META_NUM;
  static uint32 const KBD_META_SCRL;

  /**
   * Keys
   *
   */
  static uint8 const KEY_F1;
  static uint8 const KEY_F2;
  static uint8 const KEY_F3;
  static uint8 const KEY_F4;
  static uint8 const KEY_F5;
  static uint8 const KEY_F6;
  static uint8 const KEY_F7;
  static uint8 const KEY_F8;
  static uint8 const KEY_F9;
  static uint8 const KEY_F10;
  static uint8 const KEY_F11;
  static uint8 const KEY_F12;

  static uint8 const KEY_INS;
  static uint8 const KEY_DEL;
  static uint8 const KEY_HOME;
  static uint8 const KEY_END;
  static uint8 const KEY_PGUP;
  static uint8 const KEY_PGDN;
  static uint8 const KEY_LFT;
  static uint8 const KEY_UP;
  static uint8 const KEY_DN;
  static uint8 const KEY_RT;
  static uint8 const KEY_PRNT;
  static uint8 const KEY_PAUSE;
  static uint8 const KEY_LWIN;
  static uint8 const KEY_RWIN;
  static uint8 const KEY_MENU;


  /**
   * Extended keys
   *
   */
  static uint8 const  E0_BASE;
  
  static uint8 const  E0_KPENTER;
  static uint8 const  E0_RCTRL;
  static uint8 const  E0_KPSLASH;
  static uint8 const  E0_PRSCR;
  static uint8 const  E0_RALT;
  static uint8 const  E0_BREAK;
  static uint8 const  E0_HOME;
  static uint8 const  E0_UP;
  static uint8 const  E0_PGUP;
  static uint8 const  E0_LEFT;
  static uint8 const  E0_RIGHT;
  static uint8 const  E0_END;
  static uint8 const  E0_DOWN;
  static uint8 const  E0_PGDN;
  static uint8 const  E0_INS;
  static uint8 const  E0_MACRO;
  static uint8 const  E0_F13;
  static uint8 const  E0_F14;
  static uint8 const  E0_HELP;
  static uint8 const  E0_DO;
  static uint8 const  E0_F17;
  static uint8 const  E0_KPMINPLUS;
  static uint8 const  E1_PAUSE;  

  /**
   * Constructor
   *
   */
  KeyboardManager();

  /**
   * Destructor
   *
   */
  ~KeyboardManager();

  /**
   * KeyboardManager is a Singleton
   *
   */
  static KeyboardManager *getInstance()
  {
    if( !instance_ )
      instance_ = new KeyboardManager();
    return instance_;
  };

  /**
   * reads in the input key
   *
   */
  bool getKeyFromKbd(uint32 &key);

  /**
   * handles the active requests
   *
   */
  void serviceIRQ( void );

  /**
   * bool functions to check the input key
   *
   */
  bool isShift();
  bool isCtrl();
  bool isAlt();
  bool isAltGr();
  bool isCaps();
  bool isNum();
  bool isScroll();

  /**
   * empties the Kbd Buffer
   *
   */
  void emptyKbdBuffer();

private:

  /**
   * function is called when the Keyboard has to wait
   *
   */
  void kb_wait();

  /**
   * writes a byte to the given IO port
   *
   */
  void send_cmd( uint8 cmd, uint8 port );

  RingBuffer<uint8> *keyboard_buffer_;

  static uint32 const KEY_MAPPING_SIZE;
  static uint32 const STANDARD_KEYMAP[];
  static uint32 const E0_KEYS[];


  /**
   * converts the scancode into a key by looking in the Standard KeyMap
   *
   */
  uint32 convertScancode( uint8 scancode );

  /**
   * function is called to handle num, caps, scroll, shift, ctrl and alt
   *
   */
  void modifyKeyboardStatus(uint8 sc );

  /**
   * setting the leds
   *
   */
  void setLEDs( void );

  uint32 extended_scancode;
  uint32 keyboard_status_;

protected:

  static KeyboardManager *instance_;
};

#endif
