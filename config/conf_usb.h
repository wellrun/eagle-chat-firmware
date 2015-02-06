#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"

/**
 * USB Device Configuration
 */
//@{
//! Device definition (mandatory)
#define  USB_DEVICE_VENDOR_ID               USB_VID_ATMEL
#define  USB_DEVICE_PRODUCT_ID              USB_PID_ATMEL_XPLAINED
#define  USB_DEVICE_MAJOR_VERSION           1
#define  USB_DEVICE_MINOR_VERSION           0
#define  USB_DEVICE_POWER                   100 // Consumption on Vbus (mA)
#define  USB_DEVICE_ATTR                    \
	(USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_BUS_POWERED)
// (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions (Optional)
#define  USB_DEVICE_MANUFACTURE_NAME        "ATMEL ASF"
#define  USB_DEVICE_PRODUCT_NAME            "CDC Virtual Com"
#define  USB_DEVICE_SERIAL_NAME
#define  USB_DEVICE_GET_SERIAL_NAME_POINTER cdc_serial_number
#define  USB_DEVICE_GET_SERIAL_NAME_LENGTH  12


/**
 * Device speeds support
 * Low speed not supported by CDC
 */
//@{
//! To authorize the High speed
#if (UC3A3||UC3A4)
#define  USB_DEVICE_HS_SUPPORT
#endif
//@}


/**
 * USB Device Callbacks definitions (Optional)
 */
//@{
// #define  UDC_VBUS_EVENT(b_vbus_high)
// #define  UDC_SOF_EVENT()
// #define  UDC_SUSPEND_EVENT()
// #define  UDC_RESUME_EVENT()
//! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature
// #define  UDC_REMOTEWAKEUP_ENABLE()
// #define  UDC_REMOTEWAKEUP_DISABLE()
//! When a extra string descriptor must be supported
//! other than manufacturer, product and serial string
// #define  UDC_GET_EXTRA_STRING()
//@}

//@}


/**
 * USB Interface Configuration
 */
//@{
/**
 * Configuration of CDC interface
 */
//@{
//! Interface callback definition
#define  UDI_CDC_ENABLE_EXT(port)             true
#define  UDI_CDC_DISABLE_EXT(port)
#define  UDI_CDC_RX_NOTIFY(port)
#define  UDI_CDC_SET_CODING_EXT(port,cfg)
#define  UDI_CDC_SET_DTR_EXT(port,set)         cdc_set_dtr(set)
#define  UDI_CDC_SET_RTS_EXT(port,set)

/**
 * Define it when the transfer CDC Device to Host is a low rate (<512000 bauds)
 * to reduce CDC buffers size.
 */
//#define  UDI_CDC_LOW_RATE

//! Default configuration of communication port
#define  UDI_CDC_DEFAULT_RATE             115200
#define  UDI_CDC_DEFAULT_STOPBITS         CDC_STOP_BITS_1
#define  UDI_CDC_DEFAULT_PARITY           CDC_PAR_NONE
#define  UDI_CDC_DEFAULT_DATABITS         8
//@}
//@}


/**
 * USB Device Driver Configuration
 */
//@{
//@}

/*
 * The includes of classes and other headers must be done at the end of this
 * file to avoid compile error.
 */
#include "udi_cdc_conf.h"
//#include "cdc.h"

#endif // _CONF_USB_H_