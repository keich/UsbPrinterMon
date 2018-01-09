/*
 MIT License

Copyright (c) 2018 Alexander Zazhiginas
as.zazhigin@gmail.com


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>
#include <Winspool.h>
#include <SetupAPI.h>
#include "data.h"


//Write PJL
int MyWritePrinter(HANDLE hPrinter,const char* strBuffer ){
	LOG_INFO("ENTER MyWritePrinter");
    DWORD bytesWritten = 0;
    OVERLAPPED OverlappedWrite;
    ZeroMemory(&OverlappedWrite, sizeof(OVERLAPPED));
    OverlappedWrite.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
    int signal =  WriteFile(hPrinter, strBuffer, (DWORD)strlen(strBuffer), &bytesWritten, &OverlappedWrite);
    //printf("DEBUG signal %i\n",signal);
    CloseHandle(OverlappedWrite.hEvent);
    if(!signal) {
    	LOG_INFO("EXIT MyWritePrinter");
    	return OK;
    }
    LOG_INFO("EXIT MyWritePrinter");
    return ERROR_WRITEPJL;
}

//Read PJL
int MyReadPrinter(HANDLE hPrinter,char* strBuffer){
	LOG_INFO("ENTER MyReadPrinter");
    DWORD bytesRead = 0;
    OVERLAPPED OverlappedRead;
    ZeroMemory(&OverlappedRead, sizeof(OVERLAPPED));
    OverlappedRead.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
    GetLastError();
	ReadFile(hPrinter, strBuffer, BUFF_SIZE, &bytesRead, &OverlappedRead);
	int signal = WaitForSingleObject(OverlappedRead.hEvent, 1000);
    CloseHandle(OverlappedRead.hEvent);
    if(!signal) {
    	LOG_INFO("EXIT MyReadPrinter");
    	return OK;
    }
	//DWORD dwLastError = GetLastError();
	//printf("DEBUG dwLastError %ld\n",dwLastError);
    LOG_INFO("EXIT MyReadPrinter");
    return ERROR_READPJL;
}

//Run PJL and get result
int getDataForPJL(HANDLE _hPrinter,const char * PJL_cmd,char * readBuffer){
	LOG_INFO("ENTER getDataForPJL");
	if(MyWritePrinter(_hPrinter,PJL_cmd)){
		LOG_ERROR("MyWritePrinter return error");
		LOG_INFO("EXIT getDataForPJL");
		return ERROR_GETPJL;
	}
	FlushFileBuffers(_hPrinter);
	ZeroMemory(readBuffer, sizeof(BUFF_SIZE));
	if(MyReadPrinter(_hPrinter,readBuffer)){
		LOG_ERROR("MyReadPrinter return error");
		LOG_INFO("EXIT getDataForPJL");
		return ERROR_GETPJL;
	}
	LOG_INFO("EXIT getDataForPJL");
	return OK;
}



//Print PJL
int PJL_debug(HANDLE _hPrinter,char * PJL_cmd){
	LOG_INFO("ENTER PJL_debug");
	char * readBuffer = calloc(1, BUFF_SIZE);
	if(getDataForPJL(_hPrinter,PJL_cmd,readBuffer)){
		free(readBuffer);
		LOG_INFO("EXIT PJL_debug");
		return ERROR_PJL;
	}
	printf("DEBUG: %s\n",readBuffer);
	free(readBuffer);
	LOG_INFO("EXIT PJL_debug");
	return OK;
}

//Get pagecount from printer
int PJL_pagecount(PRINTER_DATA_ARRAY * Data_Printer_Array,HANDLE _hPrinter,char * PJL_cmd,const char * Name){
	LOG_INFO("ENTER PJL_pagecount");
	char * readBuffer = calloc(1, BUFF_SIZE);
	if(getDataForPJL(_hPrinter,PJL_cmd,readBuffer)){
		free(readBuffer);
		LOG_INFO("EXIT PJL_pagecount");
		return ERROR_PJL;
	}
	char * count = strstr(readBuffer,"PAGECOUNT");
	char * newline = strstr(&count[10],"\r\n");
	newline[0] = '\0';
	Set_PJL_Pagecount(Name,Data_Printer_Array,&count[11]);
	free(readBuffer);
	LOG_INFO("EXIT PJL_pagecount");
	return OK;
}

//Get status from printer
int PJL_status(PRINTER_DATA_ARRAY * Data_Printer_Array,HANDLE _hPrinter,char * PJL_cmd, const char * Name){
	LOG_INFO("ENTER PJL_status");
	char * readBuffer = calloc(1, BUFF_SIZE);
	if(getDataForPJL(_hPrinter,PJL_cmd,readBuffer)){
		free(readBuffer);
		LOG_INFO("EXIT PJL_status");
		return ERROR_PJL;
	}

	char * code = strstr(readBuffer,"CODE=");
	char * online1 = strstr(readBuffer,"ONLINE=TRUE");

	if(code){
		code[10]='\0';
	}
	char * online = "FALSE";
	if(online1!=NULL){
		online = "TRUE";
	}

	Set_PJL_Online(Name,Data_Printer_Array,online);
	Set_WINAPI_Status_Code(Name,Data_Printer_Array,&code[5]);
	//printf("Printer status code: %s\n",&code[5]);
	//printf("Printer online: %i\n",online);
	free(readBuffer);
	LOG_INFO("EXIT PJL_status");
	return OK;
}


/*char * PJL_getname(HANDLE _hPrinter,const char * PJL_cmd){
	LOG_INFO("ENTER PJL_getname");
	char * readBuffer = calloc(1, BUFF_SIZE);
	if(getDataForPJL(_hPrinter,PJL_cmd,readBuffer)){
		return NULL;
	}
	char * name = strstr(&readBuffer[12],"\r\n");
	if(name !=NULL){
		char * end = strstr(&name[2],"\r\n");
		if(end!=NULL){
			end[0] = '\0';
			LOG_INFO("EXIT PJL_getname");
			return &name[2];
		}
	}
	LOG_INFO("EXIT PJL_getname");
	return NULL;
}*/

//Find parameter  and return link. Set \0 for in data.
char * parseParam(char * buffer,const char * param){
	char * name = strstr(buffer,param);
	if(name !=NULL){
		char * end = strstr(name,"\r\n");
		if(end!=NULL){
			end[0] = '\0';
			return &name[strlen(param)];
		}
	}
	return NULL;
}

//Detailed information from printer. Used calloc. Don't forget use free.
char * PJL_ProdInfo(PRINTER_DATA_ARRAY * Data_Printer_Array,HANDLE _hPrinter,char * PJL_cmd){
	LOG_INFO("ENTER PJL_ProdInfo");
	char * readBuffer = calloc(1, BUFF_SIZE);
	if(getDataForPJL(_hPrinter,PJL_cmd,readBuffer)){
		free(readBuffer);
		LOG_INFO("EXIT PJL_ProdInfo");
		return ERROR_PJL;
	}
	char * name = parseParam(readBuffer,"ProductName = ");
	if(name ==NULL){
		free(readBuffer);
		LOG_ERROR("Can't get Name printer. Format data error");
		LOG_INFO("EXIT PJL_ProdInfo");
		return NULL;
	}
	char * returnName = mycreatecopy(name);
	//char * formatternumber = parseParam(readBuffer,"FormatterNumber = ");
	//char * printernumber = parseParam(readBuffer,"FormatterNumber = ");
	char * productserialnumber = parseParam(&name[strlen(name)+1],"ProductSerialNumber = ");
	Set_PJL_SerialNumber(returnName,Data_Printer_Array,productserialnumber);
	free(readBuffer);
	LOG_INFO("EXIT PJL_ProdInfo");
	return returnName;
}

int PJL(PRINTER_DATA_ARRAY * Data_Printer_Array,char * usbPath){
	LOG_INFO("ENTER PJL");
	LOG_INFO(usbPath);
	HANDLE _hPrinter = CreateFile(usbPath,GENERIC_READ | GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED ,NULL);
	if (_hPrinter == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR("CreateFile return INVALID_HANDLE_VALUE");
		LOG_INFO("EXIT PJL");
		return ERROR_CREATEFILE;
	}
		//const char*  strBuffer = "\e%-12345X@PJL ECHO Testing 68394 \r\n\e%-12345X";
	PJL_debug(_hPrinter,"\e%-12345X@PJL INFO PRODINFO\r\n\e%-12345X\r\n");
	//char *name =  PJL_ProdInfo(Data_Printer_Array,_hPrinter,"\e%-12345X@PJL INFO PRODINFO\r\n\e%-12345X");
	char *name =  PJL_ProdInfo(Data_Printer_Array,_hPrinter,"\e%-12345X@PJL INFO PRODINFO\r\n\e%-12345X");
	//char *name = PJL_getname(_hPrinter,"\e%-12345X@PJL INFO ID\r\n\e%-12345X");

	if(name == NULL){
		CloseHandle(_hPrinter);
		LOG_ERROR("PJL_getname return NULL");
		LOG_INFO("EXIT PJL");
		return ERROR_PJL;
	}

	PJL_status( Data_Printer_Array,_hPrinter,"\e%-12345X@PJL INFO STATUS\r\n\e%-12345X",name);
	PJL_pagecount(Data_Printer_Array,_hPrinter,"\e%-12345X@PJL INFO PAGECOUNT\r\n\e%-12345X",name);

	//PJL_debug(_hPrinter, "\e%-12345X@PJL INFO ID\r\n\e%-12345X");
		//PJL_debug(_hPrinter,"\e%-12345X@PJL INFO MEMORY\r\n\e%-12345X");
		//PJL_debug(_hPrinter,"\e%-12345X@PJL USTATUS DEVICE \r\n\e%-12345X");





	CloseHandle(_hPrinter);
	LOG_INFO("EXIT PJL");
	return OK;
}

//Get PATH for printer in WinOS
int GetUsbPrinterDevicePath(PRINTER_DATA_ARRAY * Data_Printer_Array)
{
	LOG_INFO("ENTER GetUsbPrinterDevicePath");
	//strUsbPath[0] = '\0';
    DWORD dwLastError = 0;
    HDEVINFO devs = INVALID_HANDLE_VALUE;
    SP_DEVINFO_DATA devinfo = { 0 };
    SP_DEVICE_INTERFACE_DATA devinterface;
    GUID intfce;
    PSP_DEVICE_INTERFACE_DETAIL_DATA interface_detail = NULL;
    DWORD index = 0;
    DWORD requiredLength = 0;
    DWORD dwLocInfoSize = 2048;
    WCHAR strLocationInfo[2048];
    //DWORD dwRegType = 0;

    //Prepare
    memset(strLocationInfo,0, sizeof(WCHAR)* dwLocInfoSize);
    ZeroMemory(&devinterface, sizeof(SP_DEVICE_INTERFACE_DATA));
    devinterface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    //GUID class for usb printer
    const GUID GUID_DEVINTERFACE_USBPRINT =    {0x28d78fad,0x5a12,0x11D1,0xae,0x5b,0x00,0x00,0xf8,0x03,0xa8,0xc2};


    intfce = GUID_DEVINTERFACE_USBPRINT;

    devs = SetupDiGetClassDevs(&intfce, NULL, NULL, DIGCF_PRESENT |DIGCF_DEVICEINTERFACE  );

    if (devs == INVALID_HANDLE_VALUE){
    	LOG_ERROR("SetupDiGetClassDevs return INVALID_HANDLE_VALUE");
    	LOG_INFO("EXIT GetUsbPrinterDevicePath");
        return ERROR_SetupDiGetClassDevs;
    }

    for (index = 0;SetupDiEnumDeviceInterfaces(devs,0,&intfce,index,&devinterface); index++)
    {
        // Clear out error list
        GetLastError();
        requiredLength = 0;
        //Get size for data
        if (!SetupDiGetDeviceInterfaceDetail(devs,&devinterface,NULL,0,&requiredLength,NULL))
        {
            dwLastError = GetLastError();
            if (dwLastError != ERROR_INSUFFICIENT_BUFFER)        // If not wrong size
            {
                SetupDiDestroyDeviceInfoList(devs);
                LOG_ERROR("SetupDiGetDeviceInterfaceDetail return ERROR_INSUFFICIENT_BUFFER");
                LOG_INFO("EXIT GetUsbPrinterDevicePath");
                return dwLastError;
            }
        }
        //Get mem
        interface_detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA) calloc(1, requiredLength);
        ZeroMemory(interface_detail, sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA));
        interface_detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        devinfo.cbSize = sizeof(SP_DEVINFO_DATA);


        if (!SetupDiGetDeviceInterfaceDetail(devs,&devinterface,interface_detail,requiredLength,&requiredLength,&devinfo))
        {
            dwLastError = GetLastError();
            SetupDiDestroyDeviceInfoList(devs);
            free(interface_detail);
            LOG_ERROR("SetupDiGetDeviceInterfaceDetail return error");
            LOG_INFO("EXIT GetUsbPrinterDevicePath");
            return dwLastError;
        }
        //printf("DEBUG1 %s \n",interface_detail->DevicePath);
        //printf("DEBUG %s \n",interface_detail[0]->);SPDRP_LOCATION_INFORMATION
        // SPDRP_ENUMERATOR_NAME = USB
        //SPDRP_HARDWAREID  = USB\VID_03F0&PID_4017&REV_0100
        //SPDRP_LOCATION_INFORMATION Port_#0004.Hub_#0003
        //SPDRP_PHYSICAL_DEVICE_OBJECT_NAME \Device\USBPDO-8


        //Run PJL
        PJL(Data_Printer_Array,interface_detail->DevicePath);


        //strcpy(strUsbPath,interface_detail->DevicePath);
        //ree(interface_detail);
        //SetupDiDestroyDeviceInfoList(devs);
        //LOG_INFO("EXIT GetUsbPrinterDevicePath");
        //return OK;




        /*requiredLength = 0;
        if (!SetupDiGetDeviceRegistryProperty(devs,&devinfo, SPDRP_HARDWAREID       ,NULL,(PBYTE)strLocationInfo,dwLocInfoSize,&requiredLength))
        {
            dwLastError = GetLastError();
            printf("DEBUG10 %ld \n",dwLastError);
        }
        else
        {
        	printf("DEBUG10 %s \n",strLocationInfo);
        }
        SetupDiDestroyDeviceInfoList(devs);*/

        free(interface_detail);
        interface_detail = NULL;
    } // for loop

    SetupDiDestroyDeviceInfoList(devs);

    if (index == 0)
    {
        dwLastError = GetLastError();
        LOG_INFO("EXIT GetUsbPrinterDevicePath");
        return (dwLastError == 0) ? (-1) : dwLastError;
    }
    LOG_INFO("EXIT GetUsbPrinterDevicePath");
    return 0;
 }






//lookup table for printer status
char * print_status(DWORD status){
	if(status & PRINTER_STATUS_PAUSED) return "PRINTER_STATUS_PAUSED";
	if(status & PRINTER_STATUS_ERROR) return "PRINTER_STATUS_ERROR";
	if(status & PRINTER_STATUS_PENDING_DELETION) return "PRINTER_STATUS_PENDING_DELETION";
	if(status & PRINTER_STATUS_PAPER_JAM) return "PRINTER_STATUS_PAPER_JAM";
	if(status & PRINTER_STATUS_PAPER_OUT) return "PRINTER_STATUS_PAPER_OUT";
	if(status & PRINTER_STATUS_MANUAL_FEED) return "PRINTER_STATUS_MANUAL_FEED";
	if(status & PRINTER_STATUS_PAPER_PROBLEM) return "PRINTER_STATUS_PAPER_PROBLEM";
	if(status & PRINTER_STATUS_OFFLINE) return "PRINTER_STATUS_OFFLINE";
	if(status & PRINTER_STATUS_IO_ACTIVE) return "PRINTER_STATUS_IO_ACTIVE";
	if(status & PRINTER_STATUS_BUSY) return "PRINTER_STATUS_BUSY";
	if(status & PRINTER_STATUS_PRINTING) return "PRINTER_STATUS_PRINTING";
	if(status & PRINTER_STATUS_OUTPUT_BIN_FULL) return "PRINTER_STATUS_OUTPUT_BIN_FULL";
	if(status & PRINTER_STATUS_NOT_AVAILABLE) return "PRINTER_STATUS_NOT_AVAILABLE";
	if(status & PRINTER_STATUS_WAITING) return "PRINTER_STATUS_WAITING";
	if(status & PRINTER_STATUS_PROCESSING) return "PRINTER_STATUS_PROCESSING";
	if(status & PRINTER_STATUS_INITIALIZING) return "PRINTER_STATUS_INITIALIZING";
	if(status & PRINTER_STATUS_WARMING_UP) return "PRINTER_STATUS_WARMING_UP";
	if(status & PRINTER_STATUS_TONER_LOW) return "PRINTER_STATUS_TONER_LOW";
	if(status & PRINTER_STATUS_NO_TONER) return "PRINTER_STATUS_NO_TONER";
	if(status & PRINTER_STATUS_PAGE_PUNT) return "PRINTER_STATUS_PAGE_PUNT";
	if(status & PRINTER_STATUS_USER_INTERVENTION) return "PRINTER_STATUS_USER_INTERVENTION";
	if(status & PRINTER_STATUS_OUT_OF_MEMORY) return "PRINTER_STATUS_OUT_OF_MEMORY";
	if(status & PRINTER_STATUS_DOOR_OPEN) return "PRINTER_STATUS_DOOR_OPEN";
	if(status & PRINTER_STATUS_SERVER_UNKNOWN) return "PRINTER_STATUS_SERVER_UNKNOWN";
	if(status & PRINTER_STATUS_POWER_SAVE) return "PRINTER_STATUS_POWER_SAVE";
	return "PRINTER_STATUS_OK";
}


int WinApi_Parse(PRINTER_DATA_ARRAY * Data_Printer_Array,DWORD pcReturned,PRINTER_INFO_2* info){
	LOG_INFO("ENTER WinApi_Parse");
	for(int i=0;i<(int)pcReturned;i++){
		if(strstr(info[i].pPortName,"USB") !=NULL){
			int res = AddDataPrinter(info[i].pPrinterName,Data_Printer_Array);
			if(res){
				continue;
			}
			Set_WINAPI_PrinterName(info[i].pPrinterName,Data_Printer_Array,info[i].pPrinterName);
			Set_WINAPI_PortName(info[i].pPrinterName,Data_Printer_Array,info[i].pPortName);
			char * status_str = print_status(info[i].Status);
			Set_WINAPI_Status_Str(info[i].pPrinterName,Data_Printer_Array,status_str);
			Set_WINAPI_Status_Int(info[i].pPrinterName,Data_Printer_Array,info[i].Status);
			Set_WINAPI_Jobs(info[i].pPrinterName,Data_Printer_Array,info[i].cJobs);
		}
	}
	LOG_INFO("EXIT WinApi_Parse");
	return OK;
}

void print_all_driver(DRIVER_INFO_6* printerDrive){
	LOG_DEBUG("==================================================================\n");
	LOG_DEBUG("DEBUG driver cVersion %ld\n",printerDrive->cVersion);
	LOG_DEBUG("DEBUG driver dwlDriverVersion %ld\n",printerDrive->dwlDriverVersion);
	//LOG_DEBUG("DEBUG driver ftDriverDate %s\n",printerDrive->ftDriverDate);
	LOG_DEBUG("DEBUG driver pConfigFile %s\n",printerDrive->pConfigFile);
	LOG_DEBUG("DEBUG driver pDataFile %s\n",printerDrive->pDataFile);
	LOG_DEBUG("DEBUG driver pDefaultDataType %s\n",printerDrive->pDefaultDataType);
	LOG_DEBUG("DEBUG driver pDependentFiles %s\n",printerDrive->pDependentFiles);
	LOG_DEBUG("DEBUG driver pDriverPath %s\n",printerDrive->pDriverPath);
	LOG_DEBUG("DEBUG driver pEnvironment %s\n",printerDrive->pEnvironment);
	LOG_DEBUG("DEBUG driver pHelpFile %s\n",printerDrive->pHelpFile);
	LOG_DEBUG("DEBUG driver pMonitorName %s\n",printerDrive->pMonitorName);
	LOG_DEBUG("DEBUG driver pName %s\n",printerDrive->pName);
	LOG_DEBUG("DEBUG driver pszHardwareID %s\n",printerDrive->pszHardwareID);
	LOG_DEBUG("DEBUG driver pszMfgName %s\n",printerDrive->pszMfgName);
	LOG_DEBUG("DEBUG driver pszOEMUrl %s\n",printerDrive->pszOEMUrl);
	LOG_DEBUG("DEBUG driver pszProvider %s\n",printerDrive->pszProvider);
	LOG_DEBUG("DEBUG driver pszzPreviousNames %s\n",printerDrive->pszzPreviousNames);
	LOG_DEBUG("==================================================================\n");

}
//Get printers data from WinApi
int WinApiGetDriverPrinter(PRINTER_DATA_ARRAY * Data_Printer_Array){
	LOG_INFO("ENTER WinApiGetDriverPrinter");
	DRIVER_INFO_6* printerDrive;
	DWORD pcReturned = 0;
	DWORD bytesNeeded = 0;
    DWORD Level=6;

    //Get size of data
    EnumPrinterDrivers( NULL, NULL, Level, NULL, 0, &bytesNeeded, &pcReturned );
	if(bytesNeeded<1){
		//Ошибка
		LOG_ERROR("EnumPrinterDrivers return bytesNeeded<1.");
		LOG_INFO("EXIT WinApiGetDriverPrinter");
		return ERROR_ENAMPRINTDR1;
	}

	//Get mem
	printerDrive = malloc(bytesNeeded);
	if(printerDrive==NULL){
		LOG_ERROR("Malloc return NULL.");
		LOG_INFO("EXIT WinApiGetDriverPrinter");
		return ERROR_MALLOC;
	}

	//Get info
	if(!EnumPrinterDrivers(NULL, NULL, Level, (PBYTE)printerDrive, bytesNeeded, &bytesNeeded, &pcReturned )){
		free(printerDrive);
		LOG_ERROR("EnumPrinterDrivers return error.");
		LOG_INFO("EXIT WinApiGetDriverPrinter");
		return ERROR_ENAMPRINTDR1;
	}
	//Parse data
	for(int i=0;i<(int)pcReturned;i++){
		print_all_driver(&printerDrive[i]);
	}
	//WinApi_Parse(Data_Printer_Array,pcReturned,printerInfo);
	free(printerDrive);
	LOG_INFO("EXIT WinApiGetDriverPrinter");
	return OK;
}

void print_all_info(PRINTER_INFO_2* info){
	LOG_DEBUG("==================================================================\n");
	LOG_DEBUG("DEBUG Printer pServerName %s\n", info->pServerName);
	LOG_DEBUG("DEBUG Printer pPrinterName %s\n", info->pPrinterName);
	LOG_DEBUG("DEBUG Printer pShareName %s\n",info->pShareName);
	LOG_DEBUG("DEBUG Printer pPortName %s\n",info->pPortName);
	LOG_DEBUG("DEBUG Printer pComment %s\n",info->pComment);
	LOG_DEBUG("DEBUG Printer pLocation %s\n",info->pLocation);
	LOG_DEBUG("DEBUG Printer pDatatype %s\n",info->pDatatype);
	LOG_DEBUG("DEBUG Printer pSepFile %s\n",info->pSepFile);
	LOG_DEBUG("DEBUG Printer pPrintProcessor %s\n",info->pPrintProcessor);
	LOG_DEBUG("DEBUG Printer pDatatype %s\n",info->pDatatype);
	LOG_DEBUG("DEBUG Printer pParameters %s\n",info->pParameters);
	LOG_DEBUG("DEBUG Printer Attributes %ld\n",info->Attributes);
	LOG_DEBUG("DEBUG Printer Priority %ld\n",info->Priority);
	LOG_DEBUG("DEBUG Printer DefaultPriority %ld\n",info->DefaultPriority);
	LOG_DEBUG("DEBUG Printer StartTime %ld\n",info->StartTime);
	LOG_DEBUG("DEBUG Printer UntilTime %ld\n",info->UntilTime);
	LOG_DEBUG("DEBUG Printer Status %ld\n",info->Status); print_status(info->Status);
	LOG_DEBUG("DEBUG Printer cJobs %ld\n",info->cJobs);
	LOG_DEBUG("==================================================================\n");

}
//Get printers data from WinApi
int WinApiGetPrinterStatus(PRINTER_DATA_ARRAY * Data_Printer_Array){
	LOG_INFO("ENTER WinApiGetPrinterStatus");
	PRINTER_INFO_2* printerInfo;
	DWORD pcReturned = 0;
	DWORD bytesNeeded = 0;
    DWORD Level=2;

    //Get size data
	EnumPrinters( PRINTER_ENUM_LOCAL, NULL, Level, NULL, 0, &bytesNeeded, &pcReturned );
	if(bytesNeeded<1){
		//Error
		LOG_ERROR("EnumPrinters return bytesNeeded<1.");
		LOG_INFO("EXIT WinApiGetPrinterStatus");
		return ERROR_ENAMPRINT1;
	}

	//Get mem
	printerInfo = malloc(bytesNeeded);
	if(printerInfo==NULL){
		LOG_ERROR("Malloc return NULL.");
		LOG_INFO("EXIT WinApiGetPrinterStatus");
		return ERROR_MALLOC;
	}

	//Get info
	if(!EnumPrinters(PRINTER_ENUM_LOCAL, NULL, Level, (PBYTE)printerInfo, bytesNeeded, &bytesNeeded, &pcReturned )){
		free(printerInfo);
		LOG_ERROR("numPrinters return error.");
		LOG_INFO("EXIT WinApiGetPrinterStatus");
		return ERROR_ENAMPRINT2;
	}
	//Parse data
	for(int i=0;i<(int)pcReturned;i++){
		print_all_info(&printerInfo[i]);
	}
	WinApi_Parse(Data_Printer_Array,pcReturned,printerInfo);
	free(printerInfo);
	LOG_INFO("EXIT WinApiGetPrinterStatus");
	return OK;
}




int main(int argc, char *argv[]) {

	if(argc ==2){
		if(!strcmp(argv[1],"-d")){
			log_info = 1;
		}

	}
	LOG_INFO("Start");
	//Get data from winapi
	PRINTER_DATA_ARRAY * Data_Printer_Array  = CreateDataPrinterArray();
	if(Data_Printer_Array == NULL){
		LOG_ERROR("CreateDataPrinterArray return NULL.");
		return EXIT_FAILURE;
	}
	WinApiGetDriverPrinter(Data_Printer_Array);
	int res = WinApiGetPrinterStatus(Data_Printer_Array);
	if(res){
		DestroyDataPrinterArray(Data_Printer_Array);
		LOG_ERROR("WinApiGetPrinterStatus return NULL");
		return EXIT_FAILURE;
	}




	if(!TestForUsePrinter(Data_Printer_Array)){
		GetUsbPrinterDevicePath(Data_Printer_Array);
	}

	print_all_data_array(Data_Printer_Array);
	DestroyDataPrinterArray(Data_Printer_Array);
	return EXIT_SUCCESS;
}
