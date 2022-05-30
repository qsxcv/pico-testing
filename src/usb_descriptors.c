// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2017-2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include <stdalign.h>
#include "usb_descriptors.h"

/*- Variables ---------------------------------------------------------------*/
const alignas(4) usb_device_descriptor_t usb_device_descriptor =
{
  .bLength            = sizeof(usb_device_descriptor_t),
  .bDescriptorType    = USB_DEVICE_DESCRIPTOR,
  .bcdUSB             = USB_BCD_VERSION,
  .bDeviceClass       = 0,
  .bDeviceSubClass    = 0,
  .bDeviceProtocol    = 0,
  .bMaxPacketSize0    = USB_CTRL_EP_SIZE,
  .idVendor           = 0x6666,
  .idProduct          = 0x9930,
  .bcdDevice          = 0x0100,
  .iManufacturer      = USB_STR_MANUFACTURER,
  .iProduct           = USB_STR_PRODUCT,
  .iSerialNumber      = 0,
  .bNumConfigurations = 1,
};

const alignas(4) usb_configuration_hierarchy_t usb_configuration_hierarchy =
{
  .configuration =
  {
    .bLength             = sizeof(usb_configuration_descriptor_t),
    .bDescriptorType     = USB_CONFIGURATION_DESCRIPTOR,
    .wTotalLength        = sizeof(usb_configuration_hierarchy_t),
    .bNumInterfaces      = USB_INTF_COUNT,
    .bConfigurationValue = 1,
    .iConfiguration      = 0,
    .bmAttributes        = USB_ATTRIBUTE_BUS_POWERED,
    .bMaxPower           = USB_MAX_POWER(100),
  },

  .hid_interface =
  {
    .bLength             = sizeof(usb_interface_descriptor_t),
    .bDescriptorType     = USB_INTERFACE_DESCRIPTOR,
    .bInterfaceNumber    = USB_INTF_HID,
    .bAlternateSetting   = 0,
    .bNumEndpoints       = 1,
    .bInterfaceClass     = USB_HID_DEVICE_CLASS,
    .bInterfaceSubClass  = 1, // 1 = Boot
    .bInterfaceProtocol  = 2, // 2 = Mouse
    .iInterface          = 0,
  },

  .hid =
  {
    .bLength             = sizeof(usb_hid_descriptor_t),
    .bDescriptorType     = USB_HID_DESCRIPTOR,
    .bcdHID              = 0x0111,
    .bCountryCode        = 0,
    .bNumDescriptors     = 1,
    .bDescriptorType1    = USB_HID_REPORT_DESCRIPTOR,
    .wDescriptorLength   = sizeof(usb_hid_report_descriptor),
  },

  .hid_ep_in =
  {
    .bLength             = sizeof(usb_endpoint_descriptor_t),
    .bDescriptorType     = USB_ENDPOINT_DESCRIPTOR,
    .bEndpointAddress    = USB_IN_ENDPOINT | USB_HID_EP_SEND,
    .bmAttributes        = USB_INTERRUPT_ENDPOINT,
    .wMaxPacketSize      = 6,
    .bInterval           = 1,
  },
};

const alignas(4) uint8_t usb_hid_report_descriptor[69] =
{
  0x05, 0x01,             // Usage Page (Generic Desktop)
  0x09, 0x02,             // Usage (Mouse)
  0xA1, 0x01,             // Collection (Application)
  0x05, 0x09,             //   Usage Page (Button)
  0x19, 0x01,             //   Usage Minimum (Button #1)
  0x29, 0x05,             //   Usage Maximum (Button #5)
  0x15, 0x00,             //   Logical Minimum (0)
  0x25, 0x01,             //   Logical Maximum (1)
  0x95, 0x05,             //   Report Count (5)
  0x75, 0x01,             //   Report Size (1)
  0x81, 0x02,             //   Input (Data, Variable, Absolute)
  0x95, 0x01,             //   Report Count (1)
  0x75, 0x03,             //   Report Size (3)
  0x81, 0x03,             //   Input (Constant) // Byte 1

  0x05, 0x01,             //   Usage Page (Generic Desktop)

  0x09, 0x38,             //   Usage (Wheel)
  0x15, 0x81,             //   Logical Minimum (-127)
  0x25, 0x7F,             //   Logical Maximum (127)
  0x35, 0x81,             //   Physical Minimum (-127)
  0x45, 0x7F,             //   Physical Maximum (127)
  0x75, 0x08,             //   Report Size (8)
  0x95, 0x01,             //   Report Count (1)
  0x81, 0x06,             //   Input (Data, Variable, Relative) // Byte 2

  0x09, 0x30,             //   Usage (X)
  0x09, 0x31,             //   Usage (Y)
  0x16, 0x01, 0x80,       //   Logical Minimum (-32,767)
  0x26, 0xFF, 0x7F,       //   Logical Maximum (32,767)
  0x36, 0x01, 0x80,       //   Physical Minimum (-32,767)
  0x46, 0xFF, 0x7F,       //   Physical Maximum (32,767)
  0x75, 0x10,             //   Report Size (16),
  0x95, 0x02,             //   Report Count (2),
  0x81, 0x06,             //   Input (Data, Variable, Relative) // Byte 3-6

  0xC0                    // End Collection
};

const alignas(4) usb_string_descriptor_zero_t usb_string_descriptor_zero =
{
  .bLength         = sizeof(usb_string_descriptor_zero_t),
  .bDescriptorType = USB_STRING_DESCRIPTOR,
  .wLANGID         = USB_LANGID_ENGLISH,
};

const char *usb_strings[] =
{
  [USB_STR_MANUFACTURER]  = "qs",
  [USB_STR_PRODUCT]       = "xcv",
};

const usb_class_handler_t usb_class_handlers[1] =
{
  usb_hid_handle_request
};
