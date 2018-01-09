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
#include <string.h>
#include <windows.h>
#include "data.h"

int log_info = 0;

//Check if some printer is used
int TestForUsePrinter(PRINTER_DATA_ARRAY * dataprinterarray){
	LOG_INFO("ENTER TestForUsePrinter");
	for(int i=0;i<DATAARRAY_MAX_LEN;i++){
		if(dataprinterarray->Array[i]!=NULL){
			if(dataprinterarray->Array[i]->WINAPI_Status_Int!=0){
				LOG_INFO("EXIT TestForUsePrinter");
				return 1;
			}
		}
	}
	LOG_INFO("EXIT TestForUsePrinter");
	return OK;
}

//Array for result data
PRINTER_DATA_ARRAY * CreateDataPrinterArray(){
	LOG_INFO("ENTER CreateDataPrinterArray");
	PRINTER_DATA_ARRAY * res_array;
	res_array = malloc(sizeof(PRINTER_DATA_ARRAY));
	if(res_array!=NULL){
		res_array->lenght = 0;
		for(int i=0;i<DATAARRAY_MAX_LEN;i++){
			res_array->Array[i] = NULL;
		}
	}
	LOG_INFO("EXIT CreateDataPrinterArray");
	return res_array;
}

//Result data for one printer
PRINTER_DATA * CreateDataPrinter(){
	LOG_INFO("ENTER CreateDataPrinter");
	PRINTER_DATA * res;
	res = malloc(sizeof(PRINTER_DATA));
	if(res!=NULL){
		ZeroMemory(res, sizeof(PRINTER_DATA));
	}
	LOG_INFO("EXIT CreateDataPrinter");
	return res;
}


void DestroyDataPrinterArray(PRINTER_DATA_ARRAY * dataprinterarray){
	LOG_INFO("ENTER DestroyDataPrinterArray");
	for(int i=0;i<DATAARRAY_MAX_LEN;i++){
		if(dataprinterarray->Array[i]!=NULL){
			DestroyDataPrinter(dataprinterarray->Array[i]);
		}

	}
	LOG_INFO("EXIT DestroyDataPrinterArray");
}

void DestroyDataPrinter(PRINTER_DATA * dataprinter){
	LOG_INFO("ENTER DestroyDataPrinter");
	if(dataprinter->Name!=NULL){
		free(dataprinter->Name);
	}
	if(dataprinter->PJL_Code_Str!=NULL){
		free(dataprinter->PJL_Code_Str);
	}
	if(dataprinter->PJL_Name!=NULL){
		free(dataprinter->PJL_Name);
	}
	if(dataprinter->PJL_Online!=NULL){
		free(dataprinter->PJL_Online);
	}
	if(dataprinter->WINAPI_Portname!=NULL){
		free(dataprinter->WINAPI_Portname);
	}
	if(dataprinter->WINAPI_PrinterName!=NULL){
		free(dataprinter->WINAPI_PrinterName);
	}
	if(dataprinter->WINAPI_Status_Str!=NULL){
		free(dataprinter->WINAPI_Status_Str);
	}
	if(dataprinter->PJL_Pagecount!=NULL){
		free(dataprinter->PJL_Pagecount);
	}
	if(dataprinter->PJL_StatusCode!=NULL){
		free(dataprinter->PJL_StatusCode);
	}
	LOG_INFO("EXIT DestroyDataPrinter");
}

char *mycreatecopy(const char * in){
	LOG_INFO("ENTER mycreatecopy");
	if(in == NULL){
		return NULL;
	}
	int str_len = strlen(in);
	if(str_len>PARAM_MAX_SIZE){
		str_len = 	PARAM_MAX_SIZE-1;
	}
	char * out = malloc(str_len+1);
	if(out==NULL){
		LOG_INFO("EXIT mycreatecopy");
		return NULL;
	}
	memcpy(out,in,str_len+1);
	out[str_len] = '\0';
	LOG_INFO("EXIT mycreatecopy");
	return out;
}

int Set_PJL_SerialNumber(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray, const char * serial){
	LOG_INFO("ENTER Set_PJL_SerialNumber");
	for(int i=0;i<DATAARRAY_MAX_LEN;i++){
		if(PrinterDataArray->Array[i]!=NULL){
			if(!strcmp(PrinterDataArray->Array[i]->Name,Name)){
				if(PrinterDataArray->Array[i]->PJL_SerialNumber==NULL){
					PrinterDataArray->Array[i]->PJL_SerialNumber = mycreatecopy(serial);
				}else{
					free(PrinterDataArray->Array[i]->PJL_SerialNumber);
					PrinterDataArray->Array[i]->PJL_SerialNumber = mycreatecopy(serial);
				}
			}
		}

	}
	LOG_INFO("EXIT Set_PJL_SerialNumber");
	return OK;
}
int Set_PJL_Pagecount(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray, const char * pagecount){
	LOG_INFO("ENTER Set_PJL_Pagecount");
	for(int i=0;i<DATAARRAY_MAX_LEN;i++){
		if(PrinterDataArray->Array[i]!=NULL){
			if(!strcmp(PrinterDataArray->Array[i]->Name,Name)){
				if(PrinterDataArray->Array[i]->PJL_Pagecount==NULL){
					PrinterDataArray->Array[i]->PJL_Pagecount = mycreatecopy(pagecount);
				}else{
					free(PrinterDataArray->Array[i]->PJL_Pagecount);
					PrinterDataArray->Array[i]->PJL_Pagecount = mycreatecopy(pagecount);
				}
			}
		}

	}
	LOG_INFO("EXIT Set_PJL_Pagecount");
	return OK;
}
int Set_PJL_Online(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const char * Online){
	LOG_INFO("ENTER Set_PJL_Online");
	for(int i=0;i<DATAARRAY_MAX_LEN;i++){
		if(PrinterDataArray->Array[i]!=NULL){
			if(!strcmp(PrinterDataArray->Array[i]->Name,Name)){
				if(PrinterDataArray->Array[i]->PJL_Online==NULL){
					PrinterDataArray->Array[i]->PJL_Online = mycreatecopy(Online);
				}else{
					free(PrinterDataArray->Array[i]->PJL_Online);
					PrinterDataArray->Array[i]->PJL_Online = mycreatecopy(Online);
				}
			}
		}

	}
	LOG_INFO("EXIT Set_PJL_Online");
	return OK;
}

int Set_WINAPI_PrinterName(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const char * PrinterName){
	LOG_INFO("ENTER Set_WINAPI_PrinterName");
	for(int i=0;i<DATAARRAY_MAX_LEN;i++){
		if(PrinterDataArray->Array[i]!=NULL){
			if(!strcmp(PrinterDataArray->Array[i]->Name,Name)){
				if(PrinterDataArray->Array[i]->WINAPI_PrinterName==NULL){
					PrinterDataArray->Array[i]->WINAPI_PrinterName = mycreatecopy(PrinterName);
				}else{
					free(PrinterDataArray->Array[i]->WINAPI_PrinterName);
					PrinterDataArray->Array[i]->WINAPI_PrinterName = mycreatecopy(PrinterName);
				}
			}
		}

	}
	LOG_INFO("EXIT Set_WINAPI_PrinterName");
	return OK;
}
int Set_WINAPI_PortName(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const char * PortName){
	LOG_INFO("ENTER Set_WINAPI_PortName");
	for(int i=0;i<DATAARRAY_MAX_LEN;i++){
		if(PrinterDataArray->Array[i]!=NULL){
			if(!strcmp(PrinterDataArray->Array[i]->Name,Name)){
				if(PrinterDataArray->Array[i]->WINAPI_Portname==NULL){
					PrinterDataArray->Array[i]->WINAPI_Portname = mycreatecopy(PortName);
				}else{
					free(PrinterDataArray->Array[i]->WINAPI_Portname);
					PrinterDataArray->Array[i]->WINAPI_Portname = mycreatecopy(PortName);
				}
			}
		}

	}
	LOG_INFO("EXIT Set_WINAPI_PortName");
	return OK;
}
int Set_WINAPI_Status_Str(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const char * Status){
	LOG_INFO("ENTER Set_WINAPI_Status_Str");
	for(int i=0;i<DATAARRAY_MAX_LEN;i++){
		if(PrinterDataArray->Array[i]!=NULL){
			if(!strcmp(PrinterDataArray->Array[i]->Name,Name)){
				if(PrinterDataArray->Array[i]->WINAPI_Status_Str==NULL){
					PrinterDataArray->Array[i]->WINAPI_Status_Str = mycreatecopy(Status);
				}else{
					free(PrinterDataArray->Array[i]->WINAPI_Status_Str);
					PrinterDataArray->Array[i]->WINAPI_Status_Str = mycreatecopy(Status);
				}
			}
		}

	}
	LOG_INFO("EXIT Set_WINAPI_Status_Str");
	return OK;
}

int Set_WINAPI_Status_Code(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const char * Status_code){
	LOG_INFO("ENTER Set_WINAPI_Status_Code");
	for(int i=0;i<DATAARRAY_MAX_LEN;i++){
		if(PrinterDataArray->Array[i]!=NULL){
			if(!strcmp(PrinterDataArray->Array[i]->Name,Name)){
				if(PrinterDataArray->Array[i]->PJL_StatusCode==NULL){
					PrinterDataArray->Array[i]->PJL_StatusCode = mycreatecopy(Status_code);
				}else{
					free(PrinterDataArray->Array[i]->PJL_StatusCode);
					PrinterDataArray->Array[i]->PJL_StatusCode = mycreatecopy(Status_code);
				}
			}
		}

	}
	return OK;
	LOG_INFO("EXIT Set_WINAPI_Status_Code");
}

int Set_WINAPI_Status_Int(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const unsigned long Status){
	LOG_INFO("ENTER Set_WINAPI_Status_Int");
	for(int i=0;i<DATAARRAY_MAX_LEN;i++){
		if(PrinterDataArray->Array[i]!=NULL){
			if(!strcmp(PrinterDataArray->Array[i]->Name,Name)){
				PrinterDataArray->Array[i]->WINAPI_Status_Int = Status;
			}
		}

	}
	LOG_INFO("EXIT Set_WINAPI_Status_Int");
	return OK;
}

int Set_WINAPI_Jobs(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const unsigned long Jobs){
	LOG_INFO("ENTER Set_WINAPI_Jobs");
	for(int i=0;i<DATAARRAY_MAX_LEN;i++){
		if(PrinterDataArray->Array[i]!=NULL){
			if(!strcmp(PrinterDataArray->Array[i]->Name,Name)){
				PrinterDataArray->Array[i]->WINAPI_Jobs = Jobs;
			}
		}

	}
	LOG_INFO("EXIT Set_WINAPI_Jobs");
	return OK;
}


int AddDataPrinter(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray){
	LOG_INFO("ENTER AddDataPrinter");
	for(int i=0;i<DATAARRAY_MAX_LEN;i++){
		if(PrinterDataArray->Array[i]!=NULL){
			if(!strcmp(PrinterDataArray->Array[i]->Name,Name)){
				printf("DEBUG4 %s %s\n",PrinterDataArray->Array[i]->Name,Name);
				LOG_INFO("EXIT AddDataPrinter");
				return WARN_AddDataPrinter_Exist;
			}
		}

	}
	int add = 0;
	for(int i=0;i<DATAARRAY_MAX_LEN;i++){
		if(PrinterDataArray->Array[i]==NULL){
			PrinterDataArray->Array[i] = CreateDataPrinter();
			if(PrinterDataArray->Array[i]==NULL){
				LOG_ERROR("CreateDataPrinter return NULL.");
				LOG_INFO("EXIT AddDataPrinter");
				return ERROR_AddDataPrinter;
			}
			PrinterDataArray->Array[i]->Name = mycreatecopy(Name);
			add =1;
			break;
		}

	}
	if(!add){
		LOG_INFO("EXIT AddDataPrinter");
		LOG_ERROR("No free array items for printer");
		return ERROR_AddDataPrinter;
	}
	LOG_INFO("EXIT AddDataPrinter");
	return OK;
}

//Prin all data in array
void print_all_data_array(PRINTER_DATA_ARRAY * Data_Printer_Array){
	LOG_INFO("ENTER print_all_info");
	for(int i=0;i<DATAARRAY_MAX_LEN;i++){
		if(Data_Printer_Array->Array[i]!=NULL){
			puts("==================================================================");
			printf("Printer pPrinterName: %s\n", Data_Printer_Array->Array[i]->Name);
			printf("Printer pPortName: %s\n",Data_Printer_Array->Array[i]->WINAPI_Portname);
			printf("Printer Status: %ld\n",Data_Printer_Array->Array[i]->WINAPI_Status_Int);
			printf("Printer Status Human: %s\n",Data_Printer_Array->Array[i]->WINAPI_Status_Str);
			printf("Printer cJobs: %ld\n",Data_Printer_Array->Array[i]->WINAPI_Jobs);
			printf("Printer PJL Online: %s\n",Data_Printer_Array->Array[i]->PJL_Online);
			printf("Printer PJL Serial Number: %s\n",Data_Printer_Array->Array[i]->PJL_SerialNumber);
			printf("Printer PJL Status Code: %s\n",Data_Printer_Array->Array[i]->PJL_StatusCode);
			printf("Printer PJL PageCount: %s\n",Data_Printer_Array->Array[i]->PJL_Pagecount);
			puts("==================================================================");
		}

	}
	LOG_INFO("EXIT print_all_info");
}
