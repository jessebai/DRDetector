#ifndef PUTTEXT_H
#define PUTTEXT_H
#include <Windows.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <QDebug>

using namespace cv;

char* GB18030ToUTF_8(const char *pText,int pLen);
char* UTF_8ToGB18030(const char *pText,int pLen);
void GB18030ToUnicode(wchar_t* pOut,const char *gbBuffer);
void UnicodeToUTF_8(char* pOut,WCHAR* pText);
void UnicodeToGB18030(char* pOut,unsigned short* uData) ;
void UTF_8ToUnicode(wchar_t* pOut,const char *pText);

void GetStringSize(HDC hDC, const char* str, int* w, int* h);
void putTextZH(Mat &dst, const char* str, Point org, Scalar color, int fontSize,
    const char *fn = "Arial", bool italic = false, bool underline = false);

#endif // PUTTEXT_H
