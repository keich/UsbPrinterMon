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
 
 
#define BUFF_SIZE 2048
#define PARAM_MAX_SIZE 2048
#define DATAARRAY_MAX_LEN 10

int log_info;

#define OK 0
#define ERROR_ENAMPRINT1 1
#define ERROR_MALLOC 2
#define ERROR_ENAMPRINT2 3
#define ERROR_SetupDiGetClassDevs 4
#define ERROR_CREATEFILE 5
#define ERROR_WRITEPJL 6
#define ERROR_READPJL 7
#define ERROR_GETPJL 8
#define ERROR_PJL 9
#define ERROR_AddDataPrinter 10
#define WARN_AddDataPrinter_Exist 10
#define ERROR_WinApi_Parse 11
#define ERROR_ENAMPRINTDR1 12
#define ERROR_ENAMPRINTDR2 13

//#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
//#define UpTo(i, n) for((i) = 0; (i) < (n); (i)++)

#define LOG_ERROR(text) printf("ERROR: %s\n",text)
#define LOG_INFO(text) if(log_info==1) printf("INFO: %s\n", text)
#define LOG_DEBUG(text, ...)  if(log_info==1) printf(text, ##__VA_ARGS__)

typedef struct _PRINTER_DATA {
	char * Name;
    char * PJL_Online;
    char * PJL_Code_Str;
    unsigned long PJL_Code_Int;
    char * PJL_Name;
    char * PJL_Pagecount;
    char * PJL_StatusCode;
    char * PJL_SerialNumber;
    char * WINAPI_PrinterName;
    char * WINAPI_Portname;
    unsigned long WINAPI_Status_Int;
    char * WINAPI_Status_Str;
    unsigned long WINAPI_Jobs;
} PRINTER_DATA, * PPRINTER_DATA;

typedef struct _PRINTER_DATA_ARRAY {
	int lenght;
	PPRINTER_DATA Array[DATAARRAY_MAX_LEN];
} PRINTER_DATA_ARRAY;




int AddDataPrinter(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray);
int Set_WINAPI_Jobs(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const unsigned long Jobs);
int Set_WINAPI_Status_Int(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const unsigned long Status);
int Set_WINAPI_Status_Str(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const char * Status);
int Set_WINAPI_PortName(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const char * PortName);
int Set_WINAPI_PrinterName(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const char * PrinterName);
void DestroyDataPrinterArray(PRINTER_DATA_ARRAY * dataprinterarray);
void DestroyDataPrinter(PRINTER_DATA * dataprinter);
void print_all_data_array(PRINTER_DATA_ARRAY * Data_Printer_Array);
PRINTER_DATA_ARRAY * CreateDataPrinterArray();
int Set_PJL_Online(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const char * Online);
int Set_PJL_Pagecount(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const char * pagecount);
int TestForUsePrinter(PRINTER_DATA_ARRAY * dataprinterarray);
int Set_WINAPI_Status_Code(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray,const char * Status_code);
int Set_PJL_SerialNumber(const char * Name,PRINTER_DATA_ARRAY * PrinterDataArray, const char * serial);
char *mycreatecopy(const char * in);
