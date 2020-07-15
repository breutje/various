//
// winscard 
//
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <exception>
#include <typeinfo>
#include <stdexcept>

#include <Windows.h>
#include <winscard.h>
#include <stdio.h>

#define MAX_APDU_SIZE 255

#define NameAndID(id) #id, id

struct attribute
{
   std::string name;
   DWORD id;
   const enum ATYPE : DWORD
   {
      ATYPE_UNDEFINED,
      ATYPE_DWORD,
      ATYPE_DWORD_DEC,
      ATYPE_DWORD_HEX,
      ATYPE_BYTE_HEX,
      ATYPE_BOOL,
      ATYPE_STRING
   } type;
};

std::vector <attribute> attributes
{
   { NameAndID(SCARD_ATTR_ATR_STRING),            attribute::ATYPE_UNDEFINED },
   { NameAndID(SCARD_ATTR_CHANNEL_ID),            attribute::ATYPE_DWORD_HEX },
   { NameAndID(SCARD_ATTR_CHARACTERISTICS),       attribute::ATYPE_DWORD_HEX },
   { NameAndID(SCARD_ATTR_CURRENT_BWT),           attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_CURRENT_CLK),           attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_CURRENT_CWT),           attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_CURRENT_D),             attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_CURRENT_EBC_ENCODING),  attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_CURRENT_F),             attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_CURRENT_IFSC),          attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_CURRENT_IFSD),          attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_CURRENT_N),             attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_CURRENT_PROTOCOL_TYPE), attribute::ATYPE_DWORD_HEX },
   { NameAndID(SCARD_ATTR_CURRENT_W),             attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_DEFAULT_CLK),           attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_DEFAULT_DATA_RATE),     attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_DEVICE_FRIENDLY_NAME),  attribute::ATYPE_STRING    },
   { NameAndID(SCARD_ATTR_DEVICE_IN_USE),         attribute::ATYPE_UNDEFINED },
   { NameAndID(SCARD_ATTR_DEVICE_SYSTEM_NAME),    attribute::ATYPE_STRING    },
   { NameAndID(SCARD_ATTR_DEVICE_UNIT),           attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_ICC_INTERFACE_STATUS),  attribute::ATYPE_BYTE_HEX  },
   { NameAndID(SCARD_ATTR_ICC_PRESENCE),          attribute::ATYPE_BYTE_HEX  },
   { NameAndID(SCARD_ATTR_ICC_TYPE_PER_ATR),      attribute::ATYPE_BYTE_HEX  },
   { NameAndID(SCARD_ATTR_MAX_CLK),               attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_MAX_DATA_RATE),         attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_MAX_IFSD),              attribute::ATYPE_DWORD_DEC },
   { NameAndID(SCARD_ATTR_POWER_MGMT_SUPPORT),    attribute::ATYPE_BOOL      },
   { NameAndID(SCARD_ATTR_PROTOCOL_TYPES),        attribute::ATYPE_DWORD_HEX },
   { NameAndID(SCARD_ATTR_VENDOR_IFD_SERIAL_NO),  attribute::ATYPE_STRING    },
   { NameAndID(SCARD_ATTR_VENDOR_IFD_TYPE),       attribute::ATYPE_STRING    },
   { NameAndID(SCARD_ATTR_VENDOR_IFD_VERSION),    attribute::ATYPE_DWORD_HEX },
   { NameAndID(SCARD_ATTR_VENDOR_NAME),           attribute::ATYPE_STRING    },
};

int terminate(int reply)
{
   std::cout << std::endl << "So long, and thanks for all the fish @0x" << std::hex << reply << std::endl;
   std::cout << "Press [Enter]" << std::endl;
   std::cin.get();

   exit(reply);
}

int main()
{
	LONG         lRet = 0;
	SCARDCONTEXT hContext = 0;
	SCARDHANDLE  hCard = 0;
   LPTSTR       pmszCards = nullptr;
   LPTSTR       pCard;
   DWORD        cch = SCARD_AUTOALLOCATE;
   DWORD        pcbAttrLen = SCARD_AUTOALLOCATE;
   LPBYTE       pbAttr = nullptr;

	DWORD        dwAP = 0;
	BYTE         pbSend[MAX_APDU_SIZE];
	BYTE         pbRecv[MAX_APDU_SIZE];
	DWORD        cbSend = 0;
	DWORD        cbRecv = 0;

   // Establish context
   if ((lRet = SCardEstablishContext(SCARD_SCOPE_USER, 0, 0, &hContext)) != SCARD_S_SUCCESS)
   {
      std::cout << "SCardEstablishContext returns error: " << lRet << std::endl;
      terminate(lRet);
   }
   std::cout << "SCardEstablishContext == SCARD_S_SUCCESS" << std::endl;

   // Connect to reader
   if ((lRet = SCardConnect(hContext, L"HID Global OMNIKEY 5422CL Smartcard Reader 0", SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, &hCard, &dwAP)) != SCARD_S_SUCCESS)
	{
      std::cout << "Error connecting to the card: 0x" << std::hex << lRet;
      terminate(lRet);
	}
   std::cout << "SCardConnect == SCARD_S_SUCCESS" << std::endl;

   // get attributes
   for (auto const & value : attributes)
   {
      std::cout << value.name << " : ";
      if (value.type == attribute::ATYPE_UNDEFINED)
      {
         std::cout << "UNDEFINED (not queried)" << std::endl; // Rule 178: no continue!!!
      }
      else
      {
         pcbAttrLen = SCARD_AUTOALLOCATE;
         if ((lRet = SCardGetAttrib(hCard, value.id, (LPBYTE)&pbAttr, &pcbAttrLen)) == SCARD_S_SUCCESS)
         {
            switch (value.type)
            {
            case attribute::ATYPE_STRING:
               for (DWORD i = 0; i < pcbAttrLen; i++)
               {
                  std::cout << static_cast<char>(*(pbAttr + i));
               }
               break;
            case attribute::ATYPE_BYTE_HEX:
            case attribute::ATYPE_DWORD_HEX:
               std::cout << "0x" << std::hex << static_cast<DWORD>(*pbAttr);
               break;
            case attribute::ATYPE_BOOL:
            case attribute::ATYPE_DWORD_DEC:
               std::cout << std::dec << static_cast<DWORD>(*pbAttr);
               break;
            default:
               std::cout << "UNHANDLED TYPE";
               break;
            }
         }
         else
            std::cout << "ERROR";
         SCardFreeMemory(hContext, pbAttr);
         std::cout << std::endl;
      }
   }

   // list cards
   if ((lRet = SCardListCards(NULL, NULL, NULL, NULL, (LPTSTR) &pmszCards, &cch)) != SCARD_S_SUCCESS)
   {
      std::cout << "SCardListCards returns error: " << lRet << std::endl;
      terminate(lRet);
   }
   pCard = pmszCards;
   while ('\0' != *pCard)
   {
      // Display the value.
      std::cout << "[" << pCard << "]" << std::endl;
      pCard = pCard + wcslen(pCard) + 1;
   }
   SCardFreeMemory(hContext, pmszCards);

	// Get Data - Read card serial number
	memcpy(pbSend, "\xFF\xCA\x00\x00\x00", 5);
	cbSend = 5;
	cbRecv = MAX_APDU_SIZE;
   if ((lRet = SCardTransmit(hCard, SCARD_PCI_T1, pbSend, cbSend, NULL, pbRecv, &cbRecv)) != SCARD_S_SUCCESS) {
      std::cout << "SCardTransmit returns error: " << lRet << std::endl;
      terminate(lRet);
   }
	printf("Card UID: ");
	for (DWORD i = 0; i < cbRecv - 2; i++)
	{
		printf("%02X", pbRecv[i]);
		if (i == cbRecv - 3) printf("\n");
	}

   // load key


   if ((lRet = SCardDisconnect(hCard, SCARD_LEAVE_CARD)) != SCARD_S_SUCCESS)
      terminate(lRet);
   if ((lRet = SCardReleaseContext(hContext)) != SCARD_S_SUCCESS)
      terminate(lRet);

   terminate(0);

#ifdef DEMO
	// Load Key - Default Mifare FF key, P1=0x20 means in non volatile reader memory, P2-key number
	memcpy(pbSend, "\xFF\x82\x20\x00\x06\xFF\xFF\xFF\xFF\xFF\xFF", 11);
   //              CLA  INS P1  P2  
	cbSend = 11;
	cbRecv = MAX_APDU_SIZE;
	if ((lRet = SCardTransmit(hCard, SCARD_PCI_T1, pbSend, cbSend, NULL, pbRecv, &cbRecv)) != SCARD_S_SUCCESS)
		return lRet;

	// General Authenticate - Read the documents mentioned in the README :)
	memcpy(pbSend, "\xFF\x86\x00\x00\x05\x01\x00\x01\x60\x00", 10);
	cbSend = 10;
	cbRecv = MAX_APDU_SIZE;
	if ((lRet = SCardTransmit(hCard, SCARD_PCI_T1, pbSend, cbSend, NULL, pbRecv, &cbRecv)) != SCARD_S_SUCCESS)
		return lRet;

	// Update Binary
	memcpy(pbSend, "\xFF\xD6\x00\x01\x10\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F", 21);
	cbSend = 21;
	cbRecv = MAX_APDU_SIZE;
	if ((lRet = SCardTransmit(hCard, SCARD_PCI_T1, pbSend, cbSend, NULL, pbRecv, &cbRecv)) != SCARD_S_SUCCESS)
		return lRet;

	// Read Binary
	memcpy(pbSend, "\xFF\xB0\x00\x01\x00", 5);
	cbSend = 5;
	cbRecv = MAX_APDU_SIZE;
	if ((lRet = SCardTransmit(hCard, SCARD_PCI_T1, pbSend, cbSend, NULL, pbRecv, &cbRecv)) != SCARD_S_SUCCESS)
		return lRet;

	printf("Block 01: ");
	for (DWORD i = 0; i < cbRecv - 2; i++)
	{
		printf("%02X", pbRecv[i]);
		if (i == cbRecv - 3) printf("\n");
	}

   if ((lRet = SCardDisconnect(hCard, SCARD_LEAVE_CARD)) != SCARD_S_SUCCESS)
      return lRet;

   if ((lRet = SCardReleaseContext(hContext)) != SCARD_S_SUCCESS)
      return lRet;

	return 0;
#endif // DEMO
}
