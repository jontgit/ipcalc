#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;

typedef struct IP_ADDRESS {
  u_int8_t octet[4];
} IP_ADDRESS;

typedef struct Result {
    IP_ADDRESS ip_address;
    IP_ADDRESS network_address;
    IP_ADDRESS first_address;
    IP_ADDRESS last_address;
    IP_ADDRESS broadcast_address;
    IP_ADDRESS octet_mask;
    char type[30];
    unsigned char mask;
    unsigned char host_bits;
    int total_hosts;
    int available_hosts;
    int error;
} Result;

void returnIPString(IP_ADDRESS inputAddress, char stringArray[]) {
  // Returns the IP address provided as a string
  char intChar[4];
  for (int i = 0; i < 4; i++) {
    sprintf(intChar, "%d", inputAddress.octet[i]);
    strcat(stringArray, intChar);
    if (i != 3) {
      strcat(stringArray, ".");
    }
  }
}

void printIPAddress(IP_ADDRESS inputAddress) {
  char printAddress[15] = "";
  returnIPString(inputAddress, printAddress);
  printf("%s\n", printAddress);
}

void printResult(Result Input) {

  if (Input.mask > 30) {
    printf("\nAddress count:    %d \n", Input.available_hosts);
  }
  else {
    printf("\nAddress count:    %d (%d usable)\n", Input.available_hosts, Input.total_hosts);
  }

  printf("IP Address:       ");
  printIPAddress(Input.ip_address);
  printf("Type:             %s\n", Input.type);
  printf("CIDR Mask:        %d\n", Input.mask);
  printf("Octet Mask:       ");
  printIPAddress(Input.octet_mask);


  if (Input.mask < 31) {

    printf("\nNetwork Address:  ");
    printIPAddress(Input.network_address);
    printf("Bcast Address:    ");
    printIPAddress(Input.broadcast_address);

    printf("\nFirst Address:    ");
    printIPAddress(Input.first_address);

    printf("Last Address:     ");
    printIPAddress(Input.last_address);
  }

  printf("\n");
}

IP_ADDRESS returnOctetMask(int cidrMask) {
  IP_ADDRESS octetMask;
  char maskString[32] = "00000000000000000000000000000000";

  for (int i = 0; i < cidrMask; i++) {
    maskString[i] = '1';
  }



  char tempOctets[4][8];
  for (int octetCount = 0; octetCount < 4; octetCount++) {
    strcpy(tempOctets[octetCount], "00000000");
    for (int bitCount = 0; bitCount < 8; bitCount++) {
      int maskCount = bitCount+octetCount*8;
      tempOctets[octetCount][bitCount] = maskString[maskCount];
      //printf("%d  |  %d  |  %d  | %c\n", bitCount+1, octetCount+1, maskCount, maskString[maskCount]);
    }
    //printf("%s\n", tempOctets[octetCount]);
    int octetValue = strtol(tempOctets[octetCount], NULL, 2);
    octetMask.octet[octetCount] = octetValue;
    //tempOctets[0][] = '\0';
  }
  return octetMask;
}

void initIPAddress(Result inputAddress) {
  // sets all octets in an IP address to 0 (ends up being 0.0.0.0)
  for (int i = 0; i < 4; i++) {
      inputAddress.ip_address.octet[i] = '0';
  }
  inputAddress.mask, inputAddress.error = 0;
}

void clearString(char inputString[]) {
  int inputStringLength = strlen(inputString);
  memset(inputString, 0, inputStringLength);
}

int isInArray(char inputChar, char valueArray[], int valueArrayLength) {
  //int valueArrayLength = sizeof(valueArray) / sizeof(valueArray[0]);
  for (int i = 0; i < valueArrayLength; i++)
  {
    if (valueArray[i] == inputChar)
      return 1;
  }
  return 0;
}

int checkOctet(char inputString[], int tmpOctetCount) {
  // Checks need to equal the length of the input string
  int checks = 0;
  for (int i = 0; i < tmpOctetCount; i++) {
    // if char is not a number, return 0 and raise an error
    if (inputString[i] >= '0' && inputString[i] <= '9') {
      int octetValue = atoi(inputString);
      // if string is bigger than 255, or negative, raise an error
      if (octetValue >= 0 && octetValue <= 255) {
        checks++;
      }
    }
    else {
      return 0;
    }
  }
  // If all checks pass, return 1 for success
  if (checks == tmpOctetCount) {
    return 1;
  }
}

int checkMask(char inputString[]) {
  // Works essentially the same way as check Octet
  int stringLength = strlen(inputString);
  int checks = 0;
  for (int i = 0; i < stringLength; i++) {
    if (inputString[i] >= '0' && inputString[i] <= '9') {
      int maskValue = atoi(inputString);
      if (maskValue >= 0 && maskValue <= 32) {
        checks++;
      }
    }
    else {
      return 0;
    }
  }

  if (checks == stringLength) {
    return 1;
  }
}

Result formatIPAddress(char* charArray) {
  // Formats the input to ensure the IP address / CIDR mask is valid
  // sets the result's error to -1 if this is not the case

  const char IP_DELIMITER = '.';
  const char CIDR_DELIMITER = '/';
  int DELIMITER_COUNT = 0;
  char DELIMITERS[] = {'.', '/', '\0'};
  Result returnIPAddress;
  initIPAddress(returnIPAddress);
  int stringLength = strlen(charArray);
  int octetCount = 0;

  int tmpOctetCount = 0;
  char tmpOctet[30] = "\0";

  for (int i = 0; i < stringLength; i++)
  {
    if (octetCount < 4) {
      if (tmpOctetCount > 3) {
        printf("Error: Invalid Octet - \'%s\'\n", tmpOctet);
        return returnIPAddress;
      }
      else {
        tmpOctet[tmpOctetCount] = charArray[i];
        tmpOctetCount++;
      }

      if (isInArray(charArray[i], DELIMITERS, 3)) {
        if (isInArray(tmpOctet[0], DELIMITERS, 3)) {
          returnIPAddress.error = -1;
          printf("Error: Invalid Address - \'%s\'\n", charArray);
          return returnIPAddress;
        }

        tmpOctet[tmpOctetCount-1] = '\0';
        int test = checkOctet(tmpOctet, tmpOctetCount-1);

        if (test != 1) {
          returnIPAddress.error = -1;
          printf("Error: Invalid Octet - \'%s\'\n", tmpOctet);
          return returnIPAddress;
        }

        returnIPAddress.ip_address.octet[octetCount] = atoi(tmpOctet);
        returnIPAddress.network_address.octet[octetCount] = atoi(tmpOctet);
        returnIPAddress.first_address.octet[octetCount] = atoi(tmpOctet);
        returnIPAddress.last_address.octet[octetCount] = atoi(tmpOctet);
        octetCount++;
        tmpOctetCount = 0;
        clearString(tmpOctet);
      }

    }
    else {
      tmpOctet[tmpOctetCount] = charArray[i];
      tmpOctetCount++;
    }
    returnIPAddress.mask = atoi(tmpOctet);
  }

  int test = 0;
  test = checkMask(tmpOctet);
  if (test != 1) {
    returnIPAddress.error = -1;
    printf("Error: Invalid Mask - \'%s\'\n", tmpOctet);
    return returnIPAddress;
  }

  //printf("Mask - %s\n", tmpOctet);
  return returnIPAddress;

}

Result getTotalHosts(Result inputIPAddress) {
  int hosts;
  inputIPAddress.host_bits = 32 - inputIPAddress.mask;
  char hostBitsArray[32] = "";
  for (int i = 0; i < inputIPAddress.host_bits; i++) {
    hostBitsArray[i] = '1';
  }
  inputIPAddress.total_hosts = strtol(hostBitsArray, NULL, 2)-1;
  inputIPAddress.available_hosts = inputIPAddress.total_hosts+2;
  return inputIPAddress;
  //return strtol(hostBitsArray, NULL, 2)-1;
}

Result getAddresses(Result inputIPAddress) {
  int total_addresses = inputIPAddress.total_hosts+2;
  // This dictates the octet that the network address will change in
  // i.e. a /8 and lower will be octet 0, /8 - /16 will be octet 1, /16 - /24 octet 2 and the rest octet 3
  int firstOctet = inputIPAddress.mask/8;

  // The multiplier is the general power per 1 added to an octet.
  // so anything mainly in the 3rd octet, like a /24, this would be 256^1
  // anything in the 2nd octet would be 256^2, 65536 and so on
  int octetMultiplier = pow(256, 3 - firstOctet);
  int change = 0;

  // this is how much the octet in question will change by for the full network.
  // so a /22 would change by 3 all in all in that given octet
  // 10.255.2.1/22 = 10.255.0.1 - 10.255.3.254
  // Because 1024 / 256 = 4 (we'll minus one to get the true octet's value)
  if (inputIPAddress.host_bits > 8) {
    change = total_addresses/octetMultiplier;
  }
  else {
    change = total_addresses;
  }

  if (change == 256) {
    change = 1;
    firstOctet--;
  }

  //printf("Change: %d\n", change);
  //printf("Octet:%d Amount:%d\n", firstOctet, octetMultiplier);

  int octetCounter = 0;
  while (octetCounter <= inputIPAddress.network_address.octet[firstOctet]) {
    octetCounter+=change;
  }

  if (firstOctet != 3) {
    inputIPAddress.last_address.octet[3] = 254;
    inputIPAddress.network_address.octet[3] = 0;

    if (firstOctet == 1) {
      inputIPAddress.last_address.octet[2] = 255;
      inputIPAddress.network_address.octet[2] = 0;
    }
    if (firstOctet == 0) {
      inputIPAddress.last_address.octet[2] = 255;
      inputIPAddress.last_address.octet[1] = 255;
      inputIPAddress.network_address.octet[2] = 0;
      inputIPAddress.network_address.octet[1] = 0;
    }
  }


  for (int i = 0; i < 4; i++) {
    inputIPAddress.first_address.octet[i] = inputIPAddress.network_address.octet[i];
  }
  inputIPAddress.first_address.octet[3] = inputIPAddress.network_address.octet[3] + 1;

  octetCounter-=change;
  inputIPAddress.first_address.octet[firstOctet] = octetCounter;
  inputIPAddress.network_address.octet[firstOctet] = octetCounter;


  //inputIPAddress.last_address = inputIPAddress.network_address;
  if (firstOctet == 3) {
    inputIPAddress.first_address.octet[firstOctet] = octetCounter+1;
    inputIPAddress.last_address.octet[firstOctet] = inputIPAddress.network_address.octet[firstOctet] + change-2;
  }
  else {
    inputIPAddress.last_address.octet[firstOctet] = inputIPAddress.network_address.octet[firstOctet] + change-1;
  }

  inputIPAddress.broadcast_address = inputIPAddress.last_address;
  inputIPAddress.broadcast_address.octet[3] += 1;

  // Point to Point connection
  if ( inputIPAddress.mask == 32 ) {
    inputIPAddress.network_address = inputIPAddress.ip_address;
    inputIPAddress.first_address = inputIPAddress.ip_address;
    inputIPAddress.last_address = inputIPAddress.ip_address;
    inputIPAddress.broadcast_address = inputIPAddress.ip_address;
    inputIPAddress.total_hosts += 2;
  }

  inputIPAddress.octet_mask = returnOctetMask(inputIPAddress.mask);
  return inputIPAddress;
}

Result getNetworkType(Result inputIPAddress) {
  strcpy(inputIPAddress.type, "Unknown");

  // RFC1918 Private address
  if ( inputIPAddress.network_address.octet[0] == 10 ||
       inputIPAddress.network_address.octet[0] == 172 &&
       inputIPAddress.network_address.octet[1] <= 16 &&
       inputIPAddress.network_address.octet[1] >= 31 ||
       inputIPAddress.network_address.octet[0] == 192 &&
       inputIPAddress.network_address.octet[1] == 168 ) {
    strcpy(inputIPAddress.type, "Private Address");
       }

  // Multicast Address
  else if ( inputIPAddress.network_address.octet[0] >= 224 &&
            inputIPAddress.network_address.octet[0] <= 239 ) {
    strcpy(inputIPAddress.type, "Multicast Address");
  }

  // Limited Broadcast
  else if ( inputIPAddress.network_address.octet[0] == 255 &&
            inputIPAddress.network_address.octet[1] == 255 &&
            inputIPAddress.network_address.octet[2] == 255 &&
            inputIPAddress.network_address.octet[3] == 255 ) {
    strcpy(inputIPAddress.type, "Limited Broadcast");
  }

  // Reserved for Future use
  else if ( inputIPAddress.network_address.octet[0] >= 240 &&
            inputIPAddress.network_address.octet[0] <= 255 ) {
    strcpy(inputIPAddress.type, "Reserved");
  }

  // Link local / DHCP addresses
  else if ( inputIPAddress.network_address.octet[0] == 169 &&
            inputIPAddress.network_address.octet[1] == 254 ) {
    strcpy(inputIPAddress.type, "DHCP Link Local");
  }

  else {
    strcpy(inputIPAddress.type, "Public Internet");
  }

  return inputIPAddress;
}

int main(int argc, char* argv[]) {
  Result currentAddr;
  for (int i = 1; i < argc; i++) {
    currentAddr = formatIPAddress(argv[i]);

    if (currentAddr.error == -1) {
      goto exit;
    } 

    currentAddr = getTotalHosts(currentAddr);
    currentAddr = getAddresses(currentAddr);
    currentAddr = getNetworkType(currentAddr);
    printResult(currentAddr);

    if (argc > 2 && i != argc-1) {
      printf("-----------------------------------\n");
    }
  }

  exit:
  return 0;
}
