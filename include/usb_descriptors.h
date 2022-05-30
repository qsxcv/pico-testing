// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2022, Alex Taradov <alex@taradov.com>. All rights reserved.

#ifndef _USB_DESCRIPTORS_H_
#define _USB_DESCRIPTORS_H_

/*- Includes ----------------------------------------------------------------*/
#include "usb_std.h"
#include "usb_hid.h"

/*- Definitions -------------------------------------------------------------*/
// #define USB_ENABLE_BOS
#define USB_BCD_VERSION      0x0200

enum
{
  USB_STR_ZERO,
  USB_STR_MANUFACTURER,
  USB_STR_PRODUCT,
  USB_STR_COUNT,
};

enum
{
  USB_HID_EP_SEND  = 1,
};

enum
{
  USB_INTF_HID,
  USB_INTF_COUNT,
};

/*- Types -------------------------------------------------------------------*/
typedef struct USB_PACK
{
  usb_configuration_descriptor_t                   configuration;

  usb_interface_descriptor_t                       hid_interface;
  usb_hid_descriptor_t                             hid;
  usb_endpoint_descriptor_t                        hid_ep_in;
} usb_configuration_hierarchy_t;


//-----------------------------------------------------------------------------
extern const usb_device_descriptor_t usb_device_descriptor;
extern const usb_configuration_hierarchy_t usb_configuration_hierarchy;
extern const uint8_t usb_hid_report_descriptor[69];
extern const usb_string_descriptor_zero_t usb_string_descriptor_zero;
extern const char *usb_strings[];
extern const usb_class_handler_t usb_class_handlers[1];

#endif // _USB_DESCRIPTORS_H_
