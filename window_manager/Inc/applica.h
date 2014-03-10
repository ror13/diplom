#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

#ifdef DRIVER_XXX
#	ifndef UNICODE
#		define UNICODE
#		define _UNICODE
#	endif
#endif

#ifdef DBG
#define BPX __asm int 3;
#else
#define BPX 
#endif

// {ABCCCED3-A852-4beb-9D49-0D0E76F6E64D}
//static const GUID g_guidApplicaRegistryInformation = 
//{ 0xabccced3, 0xa852, 0x4beb, { 0x9d, 0x49, 0xd, 0xe, 0x76, 0xf6, 0xe6, 0x4d } };

#define APPLICA_GUID_A    "{ABCCCED3-A852-4beb-9D49-0D0E76F6E64D}"
#define APPLICA_GUID_T _T(APPLICA_GUID_A)

// ***** Base registry key: *************************************************************

#define HKLM_CONTROL_REG_KEY_	"SYSTEM\\CurrentControlSet\\Control\\"

// SYSTEM\CurrentControlSet\Control\{ABCCCED3-A852-4beb-9D49-0D0E76F6E64D}
#define HKLM_INFO_REG_KEY_T _T(HKLM_CONTROL_REG_KEY_) APPLICA_GUID_T
// In the param SYSTEM\CurrentControlSet\Control\{ABCCCED3-A852-4beb-9D49-0D0E76F6E64D}
// DoHookAffinity (DWORD) we save 1 if the necessary to hook set affinity functions
#define APPLICA_APPLICA_REG_INFO_REG_KEY_T  _T("\\Registry\\Machine\\") HKLM_INFO_REG_KEY_T
#define HKLM_APPLICA_REG_INFO_REG_KEY_T  HKLM_INFO_REG_KEY_T

// Indicate if need to Hook affinity mask 
#define APPLICA_PARAM_DOHOOKAFFINITY_T _T("DoHookAffinity")
//#define REG_INFO_REG_KEY_W  _W("\\Registry\\Machine\\") _W(HKLM_INFO_REG_KEY_BASE) APPLICA_GUID_W

#define REG_BASE_REG_VALUE_T	_T("ProductBaseRegKey")

#define REG_BASE_FOLDER_VALUE_T	_T("ProductBaseFolder")

#define REG_PRODUCT_NAME_VALUE_T	_T("ProductName")
	
// Indicate if no need to run Host
#define REG_DISABLE_SERVICE_VALUE_T	_T("DisableService")

	// Indicate if need to run Host in MultiScreen Compatible mode
#define REG_MULTISCREEN_COMPATIBLE_DRIVER_VALUE_T	_T("MultiScreenCompatibleDriver")

// ***** Drivers: *************************************************************

// SYSTEM\CurrentControlSet\Control\{ABCCCED3-A852-4beb-9D49-0D0E76F6E64D}\Drivers\Host
#define HKLM_DRIVER_HOST_REG_KEY_T   HKLM_INFO_REG_KEY_T _T("\\Drivers\\Host")
//#define REG_DRIVER_HOST_REG_KEY_W    REG_INFO_REG_KEY_W  _W("\\Drivers\\Host")

#define REG_DRIVER_NAME_VALUE_T	_T("Name")

// ***** Stations: *************************************************************


// ***** DLLs: *************************************************************


// ***** Services: *************************************************************

// SYSTEM\CurrentControlSet\Control\{ABCCCED3-A852-4beb-9D49-0D0E76F6E64D}\Services\MainService
#define HKLM_SERVICES_MAIN_SERVICE_T   HKLM_INFO_REG_KEY_T _T("\\Services\\MainService")

#define REG_SERVICE_NAME_VALUE_T		_T("ServiceName")
#define REG_SERVICE_DELAY_NAME_VALUE_T	_T("DelayBeforeServiceStart")

// ***** KernelOnjects: *************************************************************

// SYSTEM\CurrentControlSet\Control\{ABCCCED3-A852-4beb-9D49-0D0E76F6E64D}\KernelObjects\Mailslots
#define HKLM_KERNEL_OBJECTS_MAILSLOTS_T   HKLM_INFO_REG_KEY_T _T("\\KernelObjects\\Mailslots")

#define REG_STATION_MAILSLOT_FMT_T	_T("StationMailslotFmt")

/////////////////////////////////////////////////////////////////

#define APPLICA_STATION_INFORMATION_FMT			_T("%s\\Setup\\Station%d")

#define APPLICA_STATION_AUTOLOGON_VALUE			_T("AutoLogon")
#define APPLICA_STATION_DOMAIN_VALUE			_T("Default Domain")
#define APPLICA_STATION_LOGIN_VALUE				_T("Default User")
#define APPLICA_STATION_PASSWORD_VALUE			_T("Default Password")
#define APPLICA_STATION_COMMAND_VALUE			_T("InitialCommand")

#define APPLICA_STATION_LAST_DOMAIN_VALUE		_T("Last Logged Domain")
#define APPLICA_STATION_LAST_LOGIN_VALUE		_T("Last Logged User")

/////////////////////////////////////////////////////////////////

#define APPLICA_VERSION_SZ "5.2"

#define SIZE_OF_BIOS_SAVE_AREA	0x600

// Extra stations cost, so keep this number reasonable

#define HOST_SESSION_ID         0x00000000
#define APPLICA_SESSION_ID      0x80000000

//{{AXPDEL
/*
#ifdef APPLICA_LITE
#define APPLICA_MAX_NUM_STATIONS		APPLICA_LITE
#endif
*/
//}}AXPDEL

#ifndef APPLICA_MAX_NUM_STATIONS
#define APPLICA_MAX_NUM_STATIONS		16
#endif

#define MAX_NAME_LEN		256
#define SIZEOF_MAX_NAME		(MAX_NAME_LEN*sizeof(WCHAR))

// The following is a (very short) list of supported hardware types
// and their corresponding drivers

typedef enum _APPLICA_HARDWARE_ID
{
	APPLICA_HARDWARE_U2,
	APPLICA_HARDWARE_SGX,
	APPLICA_HARDWARE_CN,
	APPLICA_HARDWARE_SW,
	APPLICA_HARDWARE_SVX,
	APPLICA_HARDWARE_SV,
	APPLICA_HARDWARE_PM99,
	APPLICA_HARDWARE_PGX,
	APPLICA_HARDWARE_MSI,
	APPLICA_HARDWARE_PGX2000,
	APPLICA_HARDWARE_PONE2000,
	APPLICA_HARDWARE_SIS,
}
APPLICA_HARDWARE_ID, *PAPPLICA_HARDWARE_ID;

typedef enum _APPLICA_DISPLAY_ID
{
	APPLICA_DISPLAY_CIRRUS,
	APPLICA_DISPLAY_S3,
	APPLICA_DISPLAY_SAVAGE,
	APPLICA_DISPLAY_VGA,
	APPLICA_DISPLAY_BLADE3D,
	APPLICA_DISPLAY_3DIMAGE,
	APPLICA_DISPLAY_RAGEIIC,
	APPLICA_DISPLAY_SIS,
}
APPLICA_DISPLAY_ID, *PAPPLICA_DISPLAY_ID;

typedef enum _APPLICA_KEYBOARD_ID
{
	APPLICA_KEYBOARD_NONE,
	APPLICA_KEYBOARD_XT,
	APPLICA_KEYBOARD_AT
}
APPLICA_KEYBOARD_ID, *PAPPLICA_KEYBOARD_ID;

typedef enum _APPLICA_POINTER_ID
{
	APPLICA_POINTER_NONE,
	APPLICA_POINTER_PS2,
	APPLICA_POINTER_SERIAL_2B,
	APPLICA_POINTER_SERIAL_3B,
	APPLICA_POINTER_SERIAL_WHEEL,
	APPLICA_POINTER_SERIAL_BALLPOINT
}
APPLICA_POINTER_ID, *PAPPLICA_POINTER_ID;


#define	DEFAULT_PROCESSOR_AFFINITY	0x00000000
#define	SINGLE_PROCESSOR_AFFINITY	0x00000001
#define	MULTIPLE_PROCESSOR_AFFINITY	0xFFFFFFFF

//
// Define the public device name strings.
//

#ifndef NO_SPECIALIZED_PROTECTION
#include "Product.h"
#endif

#if APPLICA_PRODUCT==PRODUCT_APPLIDEV
#	define DEVICE_PREFIX	L"Applica"
//#	define FILE_PREFIX		L"Applica"
//#	define FILE_PREFIX2		L"ApplicaXP"
//#	define FILE_PREFIX3		L"Ap"
#else
#	define DEVICE_PREFIX	L"Multixp"
//#	define FILE_PREFIX		L"Multixp"
//#	define FILE_PREFIX2		L"Multixp"
//#	define FILE_PREFIX3		L"Multixp"
#endif

#define APPLICA_HOST_DEVICE_NAME			L"\\Device\\" DEVICE_PREFIX L"Host"
#define APPLICA_STATION_DEVICE_BASE_NAME	L"\\Device\\" DEVICE_PREFIX L"Station"
#define APPLICA_VIDEO_DEVICE_BASE_NAME		L"\\Device\\ApVid"

#define APPLICA_KEYBOARD_DEVICE_BASE_NAME	L"\\Device\\KeyboardUSBClass"
#define APPLICA_POINTER_DEVICE_BASE_NAME	L"\\Device\\PointerUSBClass"

#define APPLICA_PNP_DEVICE_BASE_NAME		L"\\Device\\" DEVICE_PREFIX L"PnP#"

#define APPLICA_BEEP_DEVICE_BASE_NAME		APPLICA_STATION_DEVICE_BASE_NAME
#define APPLICA_COMMAND_DEVICE_BASE_NAME	APPLICA_STATION_DEVICE_BASE_NAME
#define APPLICA_THINWIRE_DEVICE_BASE_NAME	APPLICA_STATION_DEVICE_BASE_NAME

#define APPLICA_STATION_BASE_NAME			DEVICE_PREFIX L"#"
#define APPLICA_STATION_NAME_LEN			16
#define APPLICA_DISPLAY_NAME_LEN			16

#define APPLICA_KEYBOARD_PORT				DEVICE_PREFIX L"KeyboardPort"
#define APPLICA_POINTER_PORT				DEVICE_PREFIX L"PointerPort"

// Definitions of Applica USB to PS/2 converters
// We use it for protection
#define APPLICA_USB_PS2_DEVICE_ENUM TEXT("USB\\VID_04B4&PID_3672")
#define APPLICA_USB_PS2_DEVICE2_ENUM TEXT("USB\\VID_0403&PID_8371")
#define APPLICA_USB_SECURE_DEVICE_ENUM TEXT("USB\\VID_08E2&PID_0006")

// Time interval for recheck USB keys
#define APPLICA_USB_RECHECK_INTERVAL		3000

// Applica DS-100 USB Docking Station
// Uses for automatically map keyboard, mouse and audio in box to one station per one click
#define APPLICA_USB_PS2_DS100_DEVICE_ENUM TEXT("USB\\VID_05E3&PID_0606")

//
// This routine does a video hardware context switch
//
typedef
VOID
(*FN_SWAP_CONTEXT)
(
	VOID
);

//
// This routine is called during a context switch to enable/disable the video hardware
//
typedef
NTSTATUS
(*FN_VIDEO)
(
	PVOID Context,
	BOOLEAN Enable
);


//
// This routine is called when an Ke386CallBios Int 10 is detected
//

typedef struct _CALL_BIOS_REG
{
	ULONG	unknown1[0x27];
	ULONG	V86EDI;
	ULONG	V86ESI;
	ULONG	V86EBX;
	ULONG	V86EDX;
	ULONG	V86ECX;
	ULONG	V86EAX;
	ULONG	V86EBP;
	ULONG	unknown2[5];
} CALL_BIOS_REG, *PCALL_BIOS_REG;


typedef
NTSTATUS
(*FN_CALL_BIOS)
(
	PVOID Context,
	PCALL_BIOS_REG Registers
);


//
// This structure registers a station command device with the host device
//
typedef struct _STATION_REGISTRATION {
	ULONG Version;					//+00
	ULONG StationId;				//+04
	APPLICA_HARDWARE_ID HardwareId;	//+08
	APPLICA_DISPLAY_ID DisplayId;	//+0C
	APPLICA_KEYBOARD_ID KeyboardId;	//+10
	APPLICA_POINTER_ID PointerId;	//+14
	ULONG Board;					//+18
	ULONG SessionId;				//+1C
	HANDLE EnableThreadId;			//+20
	HANDLE DisplayThreadId;			//+24
	HANDLE VideoThreadId;			//+28
	HANDLE PollThreadId;			//+2C	
	FN_SWAP_CONTEXT SwapContext;	//+30	
	FN_VIDEO VideoSelect;			//+34
	PVOID VideoSelectContext;		//+38
	FN_CALL_BIOS CallBios;			//+3C
	PVOID CallBiosContext;			//+40
	FN_CALL_BIOS OldCallBios;		//+44
	PVOID OldCallBiosContext;		//+48
	ULONG ProcessorAffinity;		//+4C	
	ULONG Reserved;					//+50
	UCHAR pWinstationInfo21[0x60];
} STATION_REGISTRATION, *PSTATION_REGISTRATION;


//
// This routine is called with input data
//
typedef
NTSTATUS
(*FN_INPUT)
(
	PVOID Context,
	ULONG Data
);

//
// This structure is used to register an input function
//
typedef
struct
_REGISTER_FN_INPUT
{
	FN_INPUT Function;
	PVOID Context;
	PVOID Interrupt;
	ULONG Unit;
} REGISTER_FN_INPUT, *PREGISTER_FN_INPUT;

//
// This routine is called to enable/disable the host video
//
typedef
NTSTATUS
(*FN_HOST_VIDEO)
(
	PVOID Context,
	BOOLEAN Enable
);

//
// This structure is used to register a HostVideo() function
//
typedef
struct
_REGISTER_FN_HOST_VIDEO
{
	FN_HOST_VIDEO VideoFunction;
	PVOID VideoContext;
	FN_CALL_BIOS BiosFunction;
	PVOID BiosContext;
	FN_CALL_BIOS OldCallBios;
	PVOID OldCallBiosContext;
} REGISTER_FN_HOST_VIDEO, *PREGISTER_FN_HOST_VIDEO;

//
// This structure is used to register a station's session
//
typedef
struct
_REGISTER_STATION_SESSION_ID
{
	ULONG StationId;
	ULONG SessionId;
} REGISTER_STATION_SESSION_ID, *PREGISTER_STATION_SESSION_ID;

//
// These structures are used to return a Stations PCI details
//
typedef
struct
_STATION_DISPLAY_PCI_INFO
{
	USHORT VendorId;
	USHORT DeviceId;
	ULONG BusNumber;
	ULONG SlotNumber;

	// Now some private data for station<->miniport communication
	// So we keep the structure size constant as we add new hardware we reserve 64 bytes
	// in the first member of the union.
    union {
		struct _Reserved
		{
			ULONG Data[16];				// space reserved for up to 16 DWORD
		} Reserved;
 
		struct _PCIConfig
		{
			ULONG BaseAddress[6];		// in most cases the PCI Config Base Addresses will
		} _PCIConfig;					// contain everything we need

		struct _FrameBuffer
		{
			ULONG Address;				// location and size of frame buffer
			ULONG Length;
		} FrameBuffer;

		struct _PciVga
		{
			ULONG BaseAddress;			// whatever manfred needs for Image 3D driver
			ULONG BaseAddressLength;
			ULONG IoBaseAddress;
			ULONG IoBaseAddressLength;
		} PciVga;

		struct _Image3D
		{
			ULONG BaseAddress;			// whatever manfred needs for Image 3D driver
			ULONG BaseAddressLength;
			ULONG IoBaseAddress;
			ULONG IoBaseAddressLength;
		} Image3D;

		struct _Rage2c
		{
			ULONG LfbBaseAddress;
			ULONG LfbBaseAddressLength;
			ULONG IoBaseAddress;
			ULONG IoBaseAddressLength;
			ULONG AuxBaseAddress;
			ULONG AuxBaseAddressLength;
			ULONG RomImage;
			ULONG RomImageLength;
		} Rage2c;
    } u;

} STATION_DISPLAY_PCI_INFO, *PSTATION_DISPLAY_PCI_INFO;

typedef
struct
_STATION_SOUND_PCI_INFO		// sizeof == 0xC2
{
 	USHORT VendorId;		// +0x00
	USHORT DeviceId;		// +0x02
	ULONG BusNumber;		// +0x04
	ULONG SlotNumber;		// +0x08 if bits.Reserved non-zero it contains USB PortIndex
	USHORT Wave[13];		// +0x0C
	USHORT WaveMapper[13];	// +0x26
	USHORT Midi[13];		// +0x40
	USHORT MidiMapper[13];	// +0x5a
	USHORT Joystick[13];	// +0x74
	USHORT Aux[13];			// +0x8e
	USHORT Mixer[13];		// +0xa8
} STATION_SOUND_PCI_INFO, *PSTATION_SOUND_PCI_INFO;

typedef
struct
_NEW_STATION_SOUND_PCI_INFO
{
	STATION_SOUND_PCI_INFO sspi;
	WCHAR String1[MAX_NAME_LEN]; // +0xC2
} NEW_STATION_SOUND_PCI_INFO, *PNEW_STATION_SOUND_PCI_INFO;

typedef
struct
_STATION_SOUND_USB_INFO
{
	ULONG StationId;
	WCHAR RegistryPath[MAX_NAME_LEN];
} STATION_SOUND_USB_INFO, *PSTATION_SOUND_USB_INFO;

typedef
struct
_STATION_HID_USB_INFO
{
	ULONG StationId;
	WCHAR RegistryPath[MAX_NAME_LEN];
} STATION_HID_USB_INFO, *PSTATION_HID_USB_INFO;


typedef
struct
_STATION_USB_PCI_INFO
{
	USHORT VendorId;
	USHORT DeviceId;
	ULONG BusNumber;
	ULONG SlotNumber;	// if bits.Reserved non-zero it contains USB PortIndex
} STATION_USB_PCI_INFO, *PSTATION_USB_PCI_INFO;

typedef
struct
_INIT_PCI_DEVICE
{
	ULONG BusNumber;
	ULONG SlotNumber;
	ULONG MemorySize;
	ULONG IOSize;
	ULONG MaxCards;
	ULONG status;
} INIT_PCI_DEVICE, *PINIT_PCI_DEVICE;

typedef
struct
_ALLOC_PCI
{
	ULONG BusNumber;
	BOOLEAN IoWanted;
	ULONG Size;
	ULONG BaseAddress;
} ALLOC_PCI, *PALLOC_PCI;

//
// Some command codes for the sync interface
//
#define ASYNC_CMD_MASK				0x0000FF00
#define ASYNC_DATA_MASK				0x000000FF
#define ASYNC_FLOW_CONTROL_MASK		0xFFFF0000
#define ASYNC_PARITY_MASK			0x03000000
#define ASYNC_PARITY_SHIFT			24
#define ASYNC_STOP_MASK				0x0C000000
#define ASYNC_STOP_SHIFT			26
#define ASYNC_WORD_MASK				0xC0000000
#define ASYNC_WORD_SHIFT			30
#define ASYNC_BAUD_MASK				0x00FF0000
#define ASYNC_BAUD_SHIFT			16
#define ASYNC_LINE_CONTROL_MASK		0x00FF0000
#define ASYNC_LINE_CONTROL_SHIFT	16
#define ASYNC_DTR					0x00010000
#define ASYNC_RTS					0x00020000
#define ASYNC_XON_MASK				0x00FF0000
#define ASYNC_XOFF_MASK				0xFF000000
#define ASYNC_BREAK_MASK			0x00010000

#define ASYNC_SEND					0x00000000
#define ASYNC_TBE					0x00000100
#define ASYNC_RDA					0x00000200
#define ASYNC_STATUS				0x00000300
#define ASYNC_INITIALIZE			0x00000400
#define ASYNC_HANDSHAKE				0x00000500
#define ASYNC_FLOW_CONTROL			0x00000600
#define ASYNC_SET_XON_XOFF			0x00000700
#define ASYNC_FLUSH					0x00000800
#define ASYNC_MOUSE_TYPE			0x00000900
#define ASYNC_SET_BAUD				0x00000A00
#define ASYNC_LINE_CONTROL			0x00000B00
#define ASYNC_BREAK					0x00000C00
#define ASYNC_BREAK_ON				0x00010C00
#define ASYNC_BREAK_OFF				0x00000C00
#define ASYNC_KICK_TBE				0x00000D00
#define ASYNC_TXFIFO				0x00000E00

//
// The IOCTL codes supported by the host device
//
#define IOCTL_APPLICA_VERSION					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_REGISTER_STATION			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_UNREGISTER_STATION		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0803, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_GET_STATION				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0804, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_GET_STATION_ID			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0805, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_GET_SESSION_ID			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0806, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_PATCH_WIN32K				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0807, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_HOST_VIDEO				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0808, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_REGISTER_HOST_VIDEO		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0809, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_SET_SESSION_ID			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x080A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_PATCH_VIDEO				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x080B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_INIT_PCI_DEVICE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x080C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_PRODUCT_SUITE				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x080D, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_ALLOC_PCI					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x080E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_GET_HOST_VARS				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x080F, METHOD_BUFFERED, FILE_ANY_ACCESS)


//
// Now the IOCTL definitions for the Station device
//
#define IOCTL_APPLICA_STATUS					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0811, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_VIDEO_RESET				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0812, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_SET_KEYBOARD_HANDLER		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0813, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_SET_POINTER_HANDLER		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0814, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_GET_PSTATION_REG			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0815, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_VIDEO_SELECT				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0816, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_STATION_RESET				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0817, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_SCAN_CODE					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0818, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_SET_SERIAL_HANDLER		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0819, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_SET_PARALLEL_HANDLER		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x081A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_STATION_DISPLAY_PCI_INFO	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x081B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_STATION_SOUND_PCI_INFO	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x081C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_SET_FRAMEBUF				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x081D, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_APPLICA_STATION_NEW_SOUND_INFO	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0850, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_STATION_IS_GOOD_SIS_VIDEO	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0861, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_APPLICA_STATION_USB_PCI_INFO		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0840, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_SET_KEYBOARD_DEVREF		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0841, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_SET_POINTER_DEVREF		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0842, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Now the IOCTL definitions for the USB Filter device
//
#define IOCTL_APPLICA_USB_SET_HANDLER			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0821, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_USB_SET_INDICATORS		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0822, METHOD_BUFFERED, FILE_ANY_ACCESS)


//
// Now the IOCTL definitions for the Input device
//
#define IOCTL_APPLICA_GET_KEYBOARD_HANDLER		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0831, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_GET_POINTER_HANDLER		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0832, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Now the IOCTL definitions for the Input device (new for USB and Filters)
//
#define IOCTL_APPLICA_KEYBOARD_PDO_FLASH_LIGHTS		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0882, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_KEYBOARD_FLASH_LIGHTS		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0884, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_SET_DEL_KEYBOARD_PDO		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0885, METHOD_BUFFERED, FILE_ANY_ACCESS)

enum {APPLICA_SET_KEYBOARD_PDO = 1, APPLICA_DEL_KEYBOARD_PDO = 2};

#ifdef DRIVER_XXX

typedef struct tagS_APPLICA_SET_GET_KEYBOARD_PDO
{
	ULONG			Action;
	PDEVICE_OBJECT	pdo;
} S_APPLICA_SET_GET_KEYBOARD_PDO, *PS_APPLICA_SET_GET_KEYBOARD_PDO;

#endif //DRIVER_XXX

#define IOCTL_APPLICA_REATTACH_HID_DEVICE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0890, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_APPLICA_ADDITIONAL_HID_INIT		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0880, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_ADDITIONALSTATIONINIT		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0881, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_ADDITIONAL_HID_ATTACH		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0883, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Test IOCTL_VIDEO_QUERY_NUM_AVAIL_MODES
//
#define IOCTL_APPLICA_VIDEO_QUERY_NUM_AVAIL_MODES		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0900, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Test IOCTL_VIDEO_SET_MODE
//
#define IOCTL_APPLICA_VIDEO_SET_MODE					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0901, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Get Station ParentIdPrefix. Used for automatic assigning devices
//
#define IOCTL_APPLICA_GET_PARENT_ID_PREFIX			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0910, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Get StationId for automatic assigning devices. In Device registry namer, out - StationId
//
#define IOCTL_APPLICA_GET_AUTOMATIC_STATION_ID		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0911, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Set StationId for ALL AUDIO ONLY FOR SIS DRIVER!!!automatic assigning devices. In Device registry namer, out - StationId
//
#define IOCTL_APPLICA_SET_AUTOMATIC_AUDIO_STATION_ID		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0912, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// New the IOCTL definitions for the Additional Kernel Patch rdprp
//
#define IOCTL_APPLICA_PATCH_RDPDR				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0920, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// New the IOCTL definitions for the Additional Kernel Patch rdprp
//
#define IOCTL_APPLICA_SET_WINSTATION_INFO		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0930, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_GET_WINSTATION_INFO		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0931, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// New the IOCTL definitions for the Set and Get USB Sound Info
//
#define IOCTL_APPLICA_SET_STATION_SOUND_USB_INFO		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0950, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_GET_STATION_SOUND_USB_INFO		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0951, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_APPLICA_SET_STATION_HID_USB_INFO		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0952, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_APPLICA_GET_STATION_HID_USB_INFO		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0953, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// New the IOCTL definitions for the Start HostXP PciVga
//
#define IOCTL_APPLICA_START_HOSTXP_PCIVGA				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0960, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// New the IOCTL definitions for check if there are Multiscreen compatible Host
//
#define IOCTL_APPLICA_IS_MULTISCREEN_COMPATIBLE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0970, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// New the IOCTL definitions for get original display driver name for station
//
#define IOCTL_APPLICA_GET_ORIGINAL_DISPLAY_DRIVER_NAME	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0971, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define APPLICA_WINSTATION_INFO_BUFFER_SIZE	0x60

typedef 
struct _APPLICA_WINSTATION_INFO
{
	ULONG sessionid;
	UCHAR buffer[APPLICA_WINSTATION_INFO_BUFFER_SIZE];
} APPLICA_WINSTATION_INFO, *PAPPLICA_WINSTATION_INFO;

typedef
struct
_STATION_STATUS
{
	ULONG	version;
	ULONG	flags;
} STATION_STATUS, *PSTATION_STATUS;

typedef
struct
_VIDEO_SELECT
{
	HANDLE	ThreadId;
	BOOLEAN	Enable;
} VIDEO_SELECT, *PVIDEO_SELECT;

// values returned by status call
#define	APPLICA_STATUS_PWS_OK				0x00000001
#define	APPLICA_STATUS_KEYBOARD_OK			0x00000002
#define	APPLICA_STATUS_POINTER_OK			0x00000004
#define	APPLICA_STATUS_VIDEO_OK				0x00000008
#define	APPLICA_STATUS_SERIAL_OK			0x00000010
#define	APPLICA_STATUS_PARALLEL_OK			0x00000020
#define	APPLICA_STATUS_SOUND_OK				0x00000040
#define	APPLICA_STATUS_SIS_UNK_FAILURE		0x00010000
#define	APPLICA_STATUS_KEYBOARD_FAILURE		0x00020000
#define	APPLICA_STATUS_POINTER_FAILURE		0x00040000
#define	APPLICA_STATUS_IN_RESET				0x80000000

#define HKLM_SERVICES_REG_BASE_NAME				L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\"

#if APPLICA_PRODUCT==PRODUCT_APPLIDEV
#	define REG_PREFIX1		L"App"
//#	define REG_PREFIX2		L"Appli"
#	define REG_PREFIX3		L"Applica"
#else
#	define REG_PREFIX1		L"Multixp"
//#	define REG_PREFIX2		L"Multixp"
#	define REG_PREFIX3		L"Multixp"
#endif

#define APPLICA_API								REG_PREFIX3 L"Api"
// The leaf name "Applica" feeds thru to a terminal server sub-key, so keep these two in step
#define APPLICA_NAME_UNKNOWN					L"ADD"
#define	APPLICA_TERMINAL_SERVER_REG_NAME		L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\Terminal Server\\Video\\ADD"
#define	APPLICA_TERMINAL_SERVER_REG_NAME_XP		L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\Terminal Server\\Video\\D"

// These two point to the same key, so keep in step
#define HKLM_STATION_REG_BASE_NAME				L"SYSTEM\\CurrentControlSet\\Services\\" REG_PREFIX1 L"Sta"
#define APPLICA_STATION_REG_BASE_NAME			L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\" REG_PREFIX1 L"Sta"

// Registry definition for our station config (and ancestors)
#define HKLM_APPLICA_REG_NAME					L"SYSTEM\\CurrentControlSet\\Control\\" REG_PREFIX3
#define APPLICA_APPLICA_REG_NAME				L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\" REG_PREFIX3
#define APPLICA_APPLICA_ERROR_REG_NAME			L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\" REG_PREFIX3 L"\\Error"
#define HKLM_SETUP_REG_NAME						L"SYSTEM\\CurrentControlSet\\Control\\" REG_PREFIX3 L"\\Setup"
#define APPLICA_SETUP_REG_NAME					L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\" REG_PREFIX3 L"\\Setup"
#define HKLM_CONFIG_REG_BASE_NAME				L"SYSTEM\\CurrentControlSet\\Control\\" REG_PREFIX3 L"\\Setup\\Station"
#define APPLICA_CONFIG_REG_BASE_NAME			L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\" REG_PREFIX3 L"\\Setup\\Station"

#define APPLICA_STATION_ID_REG_NAME				L"StationId"
//#define APPLICA_HID_STATION_ID_REG_NAME			L"Station"
#define APPLICA_HID_STATION_ID_REG_NAME			APPLICA_STATION_ID_REG_NAME

// Now some port names
#define SM_API_PORT_NAME						L"\\SmSsWinStationApiPort"
#define SM_API_SYSTEM_PORT_NAME					L"\\SmSsWinStationApiPort\\SYSTEM"

#define APPLICA_SM_API_PORT_NAME				L"\\" REG_PREFIX3 L"WinStationApiPort"
#define APPLICA_SM_API_SYSTEM_PORT_NAME			L"\\" REG_PREFIX3 L"WinStationApiPort\\SYSTEM"

#define RPC_API_PORT_NAME						L"\\RPC Control\\IcaApi"
#define APPLICA_RPC_API_PORT_NAME				L"\\RPC Control\\" APPLICA_API

#define RPC_LSM_API_PORT_NAME					L"\\RPC Control\\LSMApi"
#define APPLICA_RPC_LSM_API_PORT_NAME			L"\\RPC Control\\" APPLICA_API

#define RPC_TERMSRV_API_PORT_NAME				L"\\RPC Control\\TermSrvApi"
#define APPLICA_RPC_TERMSRV_API_PORT_NAME		L"\\RPC Control\\" APPLICA_API


#define APPLICA_API2							L"/.:/" APPLICA_API

#define APPLICA_DISPLAY_DRIVER_NAME				L"MxDD"

#define INVALID_STATION_ID			0xFFFFFFFF
#define HOST_STATION_ID				0
#define INVALID_SESSION_ID			0xFFFFFFFF
#define INVALID_PROCESS_ID			0xFFFFFFFF
#define INVALID_PROCESS_ID			0xFFFFFFFF
#define INVALID_THREAD_ID			((HANDLE) 0xFFFFFFFF)
#define INVALID_INDEX				0xFFFFFFFF
#define INIT_SESSION_ID				0xFFFFFFFE

#ifdef __cplusplus
}


//
// This macro was added to the ddk with NT 5 Beta.
// We use this in several drivers, so to aid building under NT 4.0 DDK's we define
// it here if necessary.
//
#ifndef IoCopyCurrentIrpStackLocationToNext
#define IoCopyCurrentIrpStackLocationToNext( Irp ) { \
    PIO_STACK_LOCATION irpSp; \
    PIO_STACK_LOCATION nextIrpSp; \
    irpSp = IoGetCurrentIrpStackLocation( (Irp) ); \
    nextIrpSp = IoGetNextIrpStackLocation( (Irp) ); \
    RtlCopyMemory( nextIrpSp, irpSp, FIELD_OFFSET(IO_STACK_LOCATION, CompletionRoutine)); \
    nextIrpSp->Control = 0; }
#endif

#endif

///////////////////////
// PE MODULE STRUCTS //
///////////////////////
//typedef UCHAR BYTE;
//typedef USHORT WORD;
//typedef ULONG DWORD;

#ifndef IMAGE_SIZEOF_FILE_HEADER
typedef struct _IMAGE_DOS_HEADER {      // DOS .EXE header
    WORD   e_magic;                     // Magic number
    WORD   e_cblp;                      // Bytes on last page of file
    WORD   e_cp;                        // Pages in file
    WORD   e_crlc;                      // Relocations
    WORD   e_cparhdr;                   // Size of header in paragraphs
    WORD   e_minalloc;                  // Minimum extra paragraphs needed
    WORD   e_maxalloc;                  // Maximum extra paragraphs needed
    WORD   e_ss;                        // Initial (relative) SS value
    WORD   e_sp;                        // Initial SP value
    WORD   e_csum;                      // Checksum
    WORD   e_ip;                        // Initial IP value
    WORD   e_cs;                        // Initial (relative) CS value
    WORD   e_lfarlc;                    // File address of relocation table
    WORD   e_ovno;                      // Overlay number
    WORD   e_res[4];                    // Reserved words
    WORD   e_oemid;                     // OEM identifier (for e_oeminfo)
    WORD   e_oeminfo;                   // OEM information; e_oemid specific
    WORD   e_res2[10];                  // Reserved words
    LONG   e_lfanew;                    // File address of new exe header
  } IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

//
// File header format.
//

typedef struct _IMAGE_FILE_HEADER {
    WORD    Machine;
    WORD    NumberOfSections;
    DWORD   TimeDateStamp;
    DWORD   PointerToSymbolTable;
    DWORD   NumberOfSymbols;
    WORD    SizeOfOptionalHeader;
    WORD    Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

#define IMAGE_SIZEOF_FILE_HEADER             20


#define IMAGE_FILE_RELOCS_STRIPPED           0x0001  // Relocation info stripped from file.
#define IMAGE_FILE_EXECUTABLE_IMAGE          0x0002  // File is executable  (i.e. no unresolved externel references).
#define IMAGE_FILE_LINE_NUMS_STRIPPED        0x0004  // Line nunbers stripped from file.
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED       0x0008  // Local symbols stripped from file.
#define IMAGE_FILE_AGGRESIVE_WS_TRIM         0x0010  // Agressively trim working set
#define IMAGE_FILE_LARGE_ADDRESS_AWARE       0x0020  // App can handle >2gb addresses
#define IMAGE_FILE_BYTES_REVERSED_LO         0x0080  // Bytes of machine word are reversed.
#define IMAGE_FILE_32BIT_MACHINE             0x0100  // 32 bit word machine.
#define IMAGE_FILE_DEBUG_STRIPPED            0x0200  // Debugging info stripped from file in .DBG file
#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP   0x0400  // If Image is on removable media, copy and run from the swap file.
#define IMAGE_FILE_NET_RUN_FROM_SWAP         0x0800  // If Image is on Net, copy and run from the swap file.
#define IMAGE_FILE_SYSTEM                    0x1000  // System File.
#define IMAGE_FILE_DLL                       0x2000  // File is a DLL.
#define IMAGE_FILE_UP_SYSTEM_ONLY            0x4000  // File should only be run on a UP machine
#define IMAGE_FILE_BYTES_REVERSED_HI         0x8000  // Bytes of machine word are reversed.

#define IMAGE_FILE_MACHINE_UNKNOWN           0
#define IMAGE_FILE_MACHINE_I386              0x014c  // Intel 386.
#define IMAGE_FILE_MACHINE_R3000             0x0162  // MIPS little-endian, 0x160 big-endian
#define IMAGE_FILE_MACHINE_R4000             0x0166  // MIPS little-endian
#define IMAGE_FILE_MACHINE_R10000            0x0168  // MIPS little-endian
#define IMAGE_FILE_MACHINE_WCEMIPSV2         0x0169  // MIPS little-endian WCE v2
#define IMAGE_FILE_MACHINE_ALPHA             0x0184  // Alpha_AXP
#define IMAGE_FILE_MACHINE_POWERPC           0x01F0  // IBM PowerPC Little-Endian
#define IMAGE_FILE_MACHINE_SH3               0x01a2  // SH3 little-endian
#define IMAGE_FILE_MACHINE_SH3E              0x01a4  // SH3E little-endian
#define IMAGE_FILE_MACHINE_SH4               0x01a6  // SH4 little-endian
#define IMAGE_FILE_MACHINE_ARM               0x01c0  // ARM Little-Endian
#define IMAGE_FILE_MACHINE_THUMB             0x01c2
#define IMAGE_FILE_MACHINE_IA64              0x0200  // Intel 64
#define IMAGE_FILE_MACHINE_MIPS16            0x0266  // MIPS
#define IMAGE_FILE_MACHINE_MIPSFPU           0x0366  // MIPS
#define IMAGE_FILE_MACHINE_MIPSFPU16         0x0466  // MIPS
#define IMAGE_FILE_MACHINE_ALPHA64           0x0284  // ALPHA64
#define IMAGE_FILE_MACHINE_AXP64             IMAGE_FILE_MACHINE_ALPHA64
#define IMAGE_FILE_MACHINE_CEF               0xC0EF
//
// Directory format.
//

typedef struct _IMAGE_DATA_DIRECTORY {
    DWORD   VirtualAddress;
    DWORD   Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

//
// Optional header format.
//

typedef struct _IMAGE_OPTIONAL_HEADER {
    //
    // Standard fields.
    //

    WORD    Magic;
    BYTE    MajorLinkerVersion;
    BYTE    MinorLinkerVersion;
    DWORD   SizeOfCode;
    DWORD   SizeOfInitializedData;
    DWORD   SizeOfUninitializedData;
    DWORD   AddressOfEntryPoint;
    DWORD   BaseOfCode;
    DWORD   BaseOfData;

    //
    // NT additional fields.
    //

    DWORD   ImageBase;
    DWORD   SectionAlignment;
    DWORD   FileAlignment;
    WORD    MajorOperatingSystemVersion;
    WORD    MinorOperatingSystemVersion;
    WORD    MajorImageVersion;
    WORD    MinorImageVersion;
    WORD    MajorSubsystemVersion;
    WORD    MinorSubsystemVersion;
    DWORD   Win32VersionValue;
    DWORD   SizeOfImage;
    DWORD   SizeOfHeaders;
    DWORD   CheckSum;
    WORD    Subsystem;
    WORD    DllCharacteristics;
    DWORD   SizeOfStackReserve;
    DWORD   SizeOfStackCommit;
    DWORD   SizeOfHeapReserve;
    DWORD   SizeOfHeapCommit;
    DWORD   LoaderFlags;
    DWORD   NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_ROM_OPTIONAL_HEADER {
    WORD   Magic;
    BYTE   MajorLinkerVersion;
    BYTE   MinorLinkerVersion;
    DWORD  SizeOfCode;
    DWORD  SizeOfInitializedData;
    DWORD  SizeOfUninitializedData;
    DWORD  AddressOfEntryPoint;
    DWORD  BaseOfCode;
    DWORD  BaseOfData;
    DWORD  BaseOfBss;
    DWORD  GprMask;
    DWORD  CprMask[4];
    DWORD  GpValue;
} IMAGE_ROM_OPTIONAL_HEADER, *PIMAGE_ROM_OPTIONAL_HEADER;

typedef struct _IMAGE_OPTIONAL_HEADER64 {
    WORD        Magic;
    BYTE        MajorLinkerVersion;
    BYTE        MinorLinkerVersion;
    DWORD       SizeOfCode;
    DWORD       SizeOfInitializedData;
    DWORD       SizeOfUninitializedData;
    DWORD       AddressOfEntryPoint;
    DWORD       BaseOfCode;
    ULONGLONG   ImageBase;
    DWORD       SectionAlignment;
    DWORD       FileAlignment;
    WORD        MajorOperatingSystemVersion;
    WORD        MinorOperatingSystemVersion;
    WORD        MajorImageVersion;
    WORD        MinorImageVersion;
    WORD        MajorSubsystemVersion;
    WORD        MinorSubsystemVersion;
    DWORD       Win32VersionValue;
    DWORD       SizeOfImage;
    DWORD       SizeOfHeaders;
    DWORD       CheckSum;
    WORD        Subsystem;
    WORD        DllCharacteristics;
    ULONGLONG   SizeOfStackReserve;
    ULONGLONG   SizeOfStackCommit;
    ULONGLONG   SizeOfHeapReserve;
    ULONGLONG   SizeOfHeapCommit;
    DWORD       LoaderFlags;
    DWORD       NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;

#define IMAGE_SIZEOF_ROM_OPTIONAL_HEADER      56
#define IMAGE_SIZEOF_STD_OPTIONAL_HEADER      28
#define IMAGE_SIZEOF_NT_OPTIONAL32_HEADER    224
#define IMAGE_SIZEOF_NT_OPTIONAL64_HEADER    240

#define IMAGE_NT_OPTIONAL_HDR32_MAGIC      0x10b
#define IMAGE_NT_OPTIONAL_HDR64_MAGIC      0x20b
#define IMAGE_ROM_OPTIONAL_HDR_MAGIC       0x107

#ifdef _WIN64
typedef IMAGE_OPTIONAL_HEADER64             IMAGE_OPTIONAL_HEADER;
typedef PIMAGE_OPTIONAL_HEADER64            PIMAGE_OPTIONAL_HEADER;
#define IMAGE_SIZEOF_NT_OPTIONAL_HEADER     IMAGE_SIZEOF_NT_OPTIONAL64_HEADER
#define IMAGE_NT_OPTIONAL_HDR_MAGIC         IMAGE_NT_OPTIONAL_HDR64_MAGIC
#else
typedef IMAGE_OPTIONAL_HEADER32             IMAGE_OPTIONAL_HEADER;
typedef PIMAGE_OPTIONAL_HEADER32            PIMAGE_OPTIONAL_HEADER;
#define IMAGE_SIZEOF_NT_OPTIONAL_HEADER     IMAGE_SIZEOF_NT_OPTIONAL32_HEADER
#define IMAGE_NT_OPTIONAL_HDR_MAGIC         IMAGE_NT_OPTIONAL_HDR32_MAGIC
#endif

typedef struct _IMAGE_NT_HEADERS64 {
    DWORD Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

typedef struct _IMAGE_NT_HEADERS {
    DWORD Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;

typedef struct _IMAGE_ROM_HEADERS {
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_ROM_OPTIONAL_HEADER OptionalHeader;
} IMAGE_ROM_HEADERS, *PIMAGE_ROM_HEADERS;

#ifdef _WIN64
typedef IMAGE_NT_HEADERS64                  IMAGE_NT_HEADERS;
typedef PIMAGE_NT_HEADERS64                 PIMAGE_NT_HEADERS;
#else
typedef IMAGE_NT_HEADERS32                  IMAGE_NT_HEADERS;
typedef PIMAGE_NT_HEADERS32                 PIMAGE_NT_HEADERS;
#endif

// IMAGE_FIRST_SECTION doesn't need 32/64 versions since the file header is the same either way.

#define IMAGE_FIRST_SECTION( ntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((ULONG_PTR)ntheader +                                              \
     FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) +                 \
     ((PIMAGE_NT_HEADERS)(ntheader))->FileHeader.SizeOfOptionalHeader   \
    ))

// Subsystem Values

#define IMAGE_SUBSYSTEM_UNKNOWN              0   // Unknown subsystem.
#define IMAGE_SUBSYSTEM_NATIVE               1   // Image doesn't require a subsystem.
#define IMAGE_SUBSYSTEM_WINDOWS_GUI          2   // Image runs in the Windows GUI subsystem.
#define IMAGE_SUBSYSTEM_WINDOWS_CUI          3   // Image runs in the Windows character subsystem.
#define IMAGE_SUBSYSTEM_OS2_CUI              5   // image runs in the OS/2 character subsystem.
#define IMAGE_SUBSYSTEM_POSIX_CUI            7   // image runs in the Posix character subsystem.
#define IMAGE_SUBSYSTEM_NATIVE_WINDOWS       8   // image is a native Win9x driver.
#define IMAGE_SUBSYSTEM_WINDOWS_CE_GUI       9   // Image runs in the Windows CE subsystem.
#define IMAGE_SUBSYSTEM_EFI_APPLICATION     10   //
#define IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER  11   //
#define IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER  12   //

// DllCharacteristics Entries

//      IMAGE_LIBRARY_PROCESS_INIT           0x0001     // Reserved.
//      IMAGE_LIBRARY_PROCESS_TERM           0x0002     // Reserved.
//      IMAGE_LIBRARY_THREAD_INIT            0x0004     // Reserved.
//      IMAGE_LIBRARY_THREAD_TERM            0x0008     // Reserved.
#define IMAGE_DLLCHARACTERISTICS_NO_BIND     0x0800     // Do not bind this image.
//                                           0x1000     // Reserved.
#define IMAGE_DLLCHARACTERISTICS_WDM_DRIVER  0x2000     // Driver uses WDM model
//                                           0x4000     // Reserved.
#define IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE     0x8000

// Directory Entries

#define IMAGE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory
//      IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP
#define IMAGE_DIRECTORY_ENTRY_TLS             9   // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers
#define IMAGE_DIRECTORY_ENTRY_IAT            12   // Import Address Table
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor

//
// Non-COFF Object file header
//

typedef struct ANON_OBJECT_HEADER {
    WORD    Sig1;            // Must be IMAGE_FILE_MACHINE_UNKNOWN
    WORD    Sig2;            // Must be 0xffff
    WORD    Version;         // >= 1 (implies the CLSID field is present)
    WORD    Machine;
    DWORD   TimeDateStamp;
    CLSID   ClassID;         // Used to invoke CoCreateInstance
    DWORD   SizeOfData;      // Size of data that follows the header
} ANON_OBJECT_HEADER;

//
// Section header format.
//

#define IMAGE_SIZEOF_SHORT_NAME              8

typedef struct _IMAGE_SECTION_HEADER {
    BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
            DWORD   PhysicalAddress;
            DWORD   VirtualSize;
    } Misc;
    DWORD   VirtualAddress;
    DWORD   SizeOfRawData;
    DWORD   PointerToRawData;
    DWORD   PointerToRelocations;
    DWORD   PointerToLinenumbers;
    WORD    NumberOfRelocations;
    WORD    NumberOfLinenumbers;
    DWORD   Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

#define IMAGE_SIZEOF_SECTION_HEADER          40

//
// Section characteristics.
//
//      IMAGE_SCN_TYPE_REG                   0x00000000  // Reserved.
//      IMAGE_SCN_TYPE_DSECT                 0x00000001  // Reserved.
//      IMAGE_SCN_TYPE_NOLOAD                0x00000002  // Reserved.
//      IMAGE_SCN_TYPE_GROUP                 0x00000004  // Reserved.
#define IMAGE_SCN_TYPE_NO_PAD                0x00000008  // Reserved.
//      IMAGE_SCN_TYPE_COPY                  0x00000010  // Reserved.

#define IMAGE_SCN_CNT_CODE                   0x00000020  // Section contains code.
#define IMAGE_SCN_CNT_INITIALIZED_DATA       0x00000040  // Section contains initialized data.
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA     0x00000080  // Section contains uninitialized data.

#define IMAGE_SCN_LNK_OTHER                  0x00000100  // Reserved.
#define IMAGE_SCN_LNK_INFO                   0x00000200  // Section contains comments or some other type of information.
//      IMAGE_SCN_TYPE_OVER                  0x00000400  // Reserved.
#define IMAGE_SCN_LNK_REMOVE                 0x00000800  // Section contents will not become part of image.
#define IMAGE_SCN_LNK_COMDAT                 0x00001000  // Section contents comdat.
//                                           0x00002000  // Reserved.
//      IMAGE_SCN_MEM_PROTECTED - Obsolete   0x00004000
#define IMAGE_SCN_NO_DEFER_SPEC_EXC          0x00004000  // Reset speculative exceptions handling bits in the TLB entries for this section.
#define IMAGE_SCN_GPREL                      0x00008000  // Section content can be accessed relative to GP
#define IMAGE_SCN_MEM_FARDATA                0x00008000
//      IMAGE_SCN_MEM_SYSHEAP  - Obsolete    0x00010000
#define IMAGE_SCN_MEM_PURGEABLE              0x00020000
#define IMAGE_SCN_MEM_16BIT                  0x00020000
#define IMAGE_SCN_MEM_LOCKED                 0x00040000
#define IMAGE_SCN_MEM_PRELOAD                0x00080000

#define IMAGE_SCN_ALIGN_1BYTES               0x00100000  //
#define IMAGE_SCN_ALIGN_2BYTES               0x00200000  //
#define IMAGE_SCN_ALIGN_4BYTES               0x00300000  //
#define IMAGE_SCN_ALIGN_8BYTES               0x00400000  //
#define IMAGE_SCN_ALIGN_16BYTES              0x00500000  // Default alignment if no others are specified.
#define IMAGE_SCN_ALIGN_32BYTES              0x00600000  //
#define IMAGE_SCN_ALIGN_64BYTES              0x00700000  //
#define IMAGE_SCN_ALIGN_128BYTES             0x00800000  //
#define IMAGE_SCN_ALIGN_256BYTES             0x00900000  //
#define IMAGE_SCN_ALIGN_512BYTES             0x00A00000  //
#define IMAGE_SCN_ALIGN_1024BYTES            0x00B00000  //
#define IMAGE_SCN_ALIGN_2048BYTES            0x00C00000  //
#define IMAGE_SCN_ALIGN_4096BYTES            0x00D00000  //
#define IMAGE_SCN_ALIGN_8192BYTES            0x00E00000  //
// Unused                                    0x00F00000
#define IMAGE_SCN_ALIGN_MASK                 0x00F00000

#define IMAGE_SCN_LNK_NRELOC_OVFL            0x01000000  // Section contains extended relocations.
#define IMAGE_SCN_MEM_DISCARDABLE            0x02000000  // Section can be discarded.
#define IMAGE_SCN_MEM_NOT_CACHED             0x04000000  // Section is not cachable.
#define IMAGE_SCN_MEM_NOT_PAGED              0x08000000  // Section is not pageable.
#define IMAGE_SCN_MEM_SHARED                 0x10000000  // Section is shareable.
#define IMAGE_SCN_MEM_EXECUTE                0x20000000  // Section is executable.
#define IMAGE_SCN_MEM_READ                   0x40000000  // Section is readable.
#define IMAGE_SCN_MEM_WRITE                  0x80000000  // Section is writeable.

//
// DLL support.
//

//
// Export Format
//

typedef struct _IMAGE_EXPORT_DIRECTORY {
    DWORD   Characteristics;	//+00
    DWORD   TimeDateStamp;		//+04
    WORD    MajorVersion;		//+08
    WORD    MinorVersion;		//+0A
    DWORD   Name;				//+0C	
    DWORD   Base;				//+10
    DWORD   NumberOfFunctions;	//+14
    DWORD   NumberOfNames;		//+18
    DWORD   AddressOfFunctions;	//+1C     // RVA from base of image
    DWORD   AddressOfNames;		//+20     // RVA from base of image
    DWORD   AddressOfNameOrdinals;//+24  // RVA from base of image
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;

//
// Import Format
//

typedef struct _IMAGE_IMPORT_BY_NAME {
    WORD    Hint;
    BYTE    Name[1];
} IMAGE_IMPORT_BY_NAME, *PIMAGE_IMPORT_BY_NAME;

typedef struct _IMAGE_THUNK_DATA32 {
    union {
        DWORD ForwarderString;      // PBYTE 
        DWORD Function;             // PDWORD
        DWORD Ordinal;
        DWORD AddressOfData;        // PIMAGE_IMPORT_BY_NAME
    } u1;
} IMAGE_THUNK_DATA32;
typedef IMAGE_THUNK_DATA32 * PIMAGE_THUNK_DATA32;

typedef struct _IMAGE_IMPORT_DESCRIPTOR {
    union {
        DWORD   Characteristics;            // 0 for terminating null import descriptor
        DWORD   OriginalFirstThunk;         // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
    };
    DWORD   TimeDateStamp;                  // 0 if not bound,
                                            // -1 if bound, and real date\time stamp
                                            //     in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
                                            // O.W. date/time stamp of DLL bound to (Old BIND)

    DWORD   ForwarderChain;                 // -1 if no forwarders
    DWORD   Name;
    DWORD   FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
} IMAGE_IMPORT_DESCRIPTOR;
typedef IMAGE_IMPORT_DESCRIPTOR UNALIGNED *PIMAGE_IMPORT_DESCRIPTOR;

#endif //IMAGE_SIZEOF_FILE_HEADER

/*class ApplicaSpinLock
{
public:
	PLONG
	Lock()
	{
	}

	UnLock()
	{
	}
};*/

#define _APPLICA_SPIN_LOCK(x)		{ \
										while (InterlockedIncrement(x) > 1) \
										{ \
											InterlockedDecrement(x); \
										} \
									}

#define _APPLICA_SPIN_UNLOCK(x)		{ \
										InterlockedDecrement(x); \
									}

typedef struct _tagAPPLICA_THREAD_SPINLOCK
{
	LONG	StationSpin;
	HANDLE	Spin_OldThreadId;
} APPLICA_THREAD_SPINLOCK, *PAPPLICA_THREAD_SPINLOCK;

void APPLICA_SPIN_LOCK(PAPPLICA_THREAD_SPINLOCK x, HANDLE CurrentThread);
void APPLICA_SPIN_UNLOCK(PAPPLICA_THREAD_SPINLOCK x);

//New structure used in ApplicaXP and ApplicaHost
typedef struct _NEW_HOSTVIDEO_OLDSTATION_NEWSTATION
{
	NTSTATUS (*HostVideo)(IN BOOLEAN);
	PSTATION_REGISTRATION*		OldStation;
	PSTATION_REGISTRATION*		NewStation;
	PAPPLICA_THREAD_SPINLOCK	StationSpin;
} NEW_HOSTVIDEO_OLDSTATION_NEWSTATION, *PNEW_HOSTVIDEO_OLDSTATION_NEWSTATION;

 
//#endif // _APPLICA_H_
