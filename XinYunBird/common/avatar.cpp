//
//  avatardisplay.cpp
//  avatarpreview
//
//  Created by buffer on 13-5-23.
//  Copyright (c) 2013年 com.Intel.Avatar. All rights reserved.
//

#include "avatar.h"

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <QuartzCore/QuartzCore.h>

//--------------------------------------------------------------------------------
Avatar::Avatar()
{
    pAnimator_ = NULL;
    pTracker_ = NULL;
    
    pEvent_ = NULL;
    for (int i = 0; i < 2; ++i)
    {
        pQueues_[i] = NULL;
    }
    
    pModel_ = NULL;
    pNextModel_ = NULL;
    iModelIndex_ = 0;
}

Avatar::~Avatar()
{
    Uninitialize();
}

bool Avatar::Initialize(const char *pPath, const char *pModel, const char *sdmpath)
{
    pAnimator_ = paAnimatorCreate();
    if (pAnimator_ == NULL)
        return false;
    
    pTracker_ = paTrackerCreate();
    if (pTracker_ == NULL)
    {
        paAnimatorRelease(&pAnimator_);
        pAnimator_ = NULL;
        return false;
    }
    
    pEvent_ = paEventCreate();
    for (int i = 0; i < 2; ++i)
    {
        pQueues_[i] = paEventQueueCreate(4);
    }
    
    if (pModel == NULL) return true;
    
    vctstrModels_ = ListDir(pModel);
    std::string filename = vctstrModels_[iModelIndex_];
    pModel_ = paAvatarModelCreate(filename.c_str());
    if (pModel_ == NULL)
    {
        for (int i = 0; i < 2; ++i)
        {
            paEventQueueRelease(&pQueues_[i]);
            pQueues_[i] = NULL;
        }
        paEventRelease(&pEvent_);
        pEvent_ = NULL;
        paTrackerRelease(&pTracker_);
        pTracker_ = NULL;
        paAnimatorRelease(&pAnimator_);
        pAnimator_ = NULL;
        return false;
    }
    
    return true;
}

void Avatar::Uninitialize()
{
    if (pAnimator_ != NULL)
    {
        paAnimatorRelease(&pAnimator_);
        pAnimator_ = NULL;
    }
    if (pTracker_ != NULL)
    {
        paTrackerRelease(&pTracker_);
        pTracker_ = NULL;
    }
    
    if (pEvent_ != NULL)
    {
        paEventRelease(&pEvent_);
        pEvent_ = NULL;
    }
    for (int i = 0; i < 2; ++i)
    {
        paEventQueueRelease(&pQueues_[i]);
        pQueues_[i] = NULL;
    }
    
    if (pModel_ != NULL)
    {
        paAvatarModelRelease(&pModel_);
        pModel_ = NULL;
    }
    if (pNextModel_ != NULL)
    {
        paAvatarModelRelease(&pNextModel_);
        pNextModel_ = NULL;
    }
    iModelIndex_ = 0;
}

void Avatar::Reset()
{
    paAvatarReset(pTracker_, NULL, 0);
}

//---------------------------------------------------------------------------------
//add by buffer
#define RECIPROCAL_UCHAR (1/255.0)
static float ucharTofloat(unsigned char value) { return (float)((float)value*RECIPROCAL_UCHAR);}
static float uint16Tofloat_t(unsigned short value) { return (float)((float)value / 65535.0);}
typedef struct {
    unsigned int type;
    unsigned int version;
    unsigned short size;
    unsigned char subversion;
    unsigned char reserveda;
    
    // action element
    unsigned short head_rx;
    unsigned short head_ry;
    unsigned short head_rz;
    unsigned short head_tx;
    unsigned short head_ty;
    unsigned short head_scale;
    
    unsigned char blendshape[36];
    unsigned char reservedb[28];
} miniEvent;

int Avatar::OnTrack(void* pImageBuffer, int bufsize, int iWidth, int iHeight, int iWidStp)
{
    paImage_.width = iWidth;
    paImage_.height = iHeight;
    paImage_.widthstep = iWidStp;
    if (paImage_.data == NULL) {
        paImage_.data = new unsigned char[bufsize];
    }

    memcpy(paImage_.data, pImageBuffer, bufsize);
    paImage_.rotatemode = PA_ROTATION_90_MIRROR;
    paImage_.channel = 1;
   
    PaRect pa_rect;
    PaStatus trackResult = paTrackerDetect(pTracker_, &paImage_, pEvent_, &pa_rect);
    if (trackResult == PA_STATUS_OK)
    {
        miniEvent aEvent;
        memcpy((void*)&aEvent, pEvent_, 88);
        static float value = 0;
        float temp = uint16Tofloat_t(aEvent.head_rx > 0 ? aEvent.head_rx : 0);
        //float delta = value - temp;
        value = temp;
        //printf("head: %f, delta: %f.\n", temp, delta);
        printf("mouth: %f.\n", ucharTofloat(aEvent.blendshape[12]));
        if (ucharTofloat(aEvent.blendshape[12]) > 0.3)
            return 1;
    }
    
    return 0;
}

void Avatar::OnDisplay(int iWidth, int iHeight)
{
    if (pNextModel_ == NULL)
    {
        paEventQueuePushBack(pQueues_[0], pEvent_);
        paAnimatorAnimate(pAnimator_, pQueues_[0], pModel_);
        
        PaVector3f p3v; PaVector2f p2v;
        p3v.x = 0; p3v.y = 0; p3v.z = -50;
        p2v.x = iWidth; p2v.y = iHeight;
        paAvatarModelDraw(pModel_, p3v, p2v);
        //paTrackerMeshDraw(pTracker_);
    }
    else
    {
        std::swap(pModel_, pNextModel_);
        paAvatarModelRelease(&pNextModel_);
        pNextModel_ = NULL;
    }
}

//-1 means pre
//1 means next
bool Avatar::SwitchModel()
{
    int iSize = vctstrModels_.size();
    if (pNextModel_ != NULL || iSize == 0)
    {
        return false;
    }
    
    if (iModelIndex_ > iSize - 1)
        iModelIndex_ = 0;
    if (iModelIndex_ < 0)
        iModelIndex_ = iSize -1;
    pNextModel_ = paAvatarModelCreate(vctstrModels_[iModelIndex_].c_str());
    return true;
}

bool Avatar::snapshot(int &width, int &heith, int &size, unsigned char* &data)
{
    paModelGetProperty(pModel_, PA_MODEL_SNAPSHOT_WIDTH, &width);
    paModelGetProperty(pModel_, PA_MODEL_SNAPSHOT_HEIGHT, &heith);
    paModelGetProperty(pModel_, PA_MODEL_SNAPSHOT_SIZE, &size);
    data = (unsigned char*)malloc(size);
    paModelGetProperty(pModel_, PA_MODEL_SNAPSHOT_DATA, data);
    return true;
}


bool Avatar::touch(int index)
{
    switch (index) {
        case 1:
            //paAvatarTouch(PA_TOUCH_CANNED1, 20, NULL);
            break;
        case 2:
            //paAvatarTouch(PA_TOUCH_CANNED2, 20, NULL);
            break;
        case 3:
            paAvatarTouch(PA_TOUCH_TONGUE_OUT, 15, NULL);
            break;
        case 4:
            paAvatarTouch(PA_TOUCH_KISS, 15, NULL);
            break;
        default:
            break;
    }
    
    return true;
}

//--------------------------------------------------------------------------------
void Avatar::OnDisplay(const char *event, int width, int height)
{
    memcpy(pEvent_, event, paEventGetSize(pEvent_));
    OnDisplay(width, height);
}

void Avatar::OnTrack(void *image, int size, int width, int height,
                     int step, char* out)
{
    OnTrack(image, size, width, height, step);
    memcpy(out, pEvent_, paEventGetSize(pEvent_));
}

//--------------------------------------------------------------------------------
std::vector<std::string> ListDir(const char *path, int level)
{
    struct dirent* ent = NULL;
    DIR *pDir;
    std::vector<std::string> vctstrModels;
    
    pDir = opendir(path);
    if (pDir == NULL)
    {
        std::cout << "error path: " << path << std::endl;
        return vctstrModels;
    }
    while (NULL != (ent = readdir(pDir)))
    {
        if (ent->d_type == 8)
        {
            if (memcmp(ent->d_name + strlen(ent->d_name) - 4, ".atm", 4) == 0)
            {
                char subfilename[256] = {0};
                memset(subfilename, 0x0, 256);
                sprintf(subfilename, "%s/%s", path, ent->d_name);
                printf("found file: %s\n", subfilename);
                vctstrModels.push_back(subfilename);
            }
        }
    }
    return vctstrModels;
}

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)
const int bytes_per_pixel = 2;
void color_convert_common(const unsigned char *pY, const unsigned char *pUV, int width, int height, unsigned char *buffer, int grey)
{
    
    int i, j;
    int nR, nG, nB;
    int nY, nU, nV;
    unsigned char *out = buffer;
    int offset = 0;
    
    if (grey)
    {
        memcpy(out,pY,width*height*sizeof(unsigned char));
    }
    else
        // YUV 4:2:0
        for (i = 0; i < height; i++)
        {
            for (j = 0; j < width; j++)
            {
                nY = *(pY + i * width + j);
                nV = *(pUV + (i / 2) * width + bytes_per_pixel * (j / 2));
                nU = *(pUV + (i / 2) * width + bytes_per_pixel * (j / 2) + 1);
                
                // Yuv Convert
                nY -= 16;
                nU -= 128;
                nV -= 128;
                
                if (nY < 0)
                    nY = 0;
                
                nB = (int)(1192 * nY + 2066 * nU);
                nG = (int)(1192 * nY - 833 * nV - 400 * nU);
                nR = (int)(1192 * nY + 1634 * nV);
                
                nR = min(262143, max(0, nR));
                nG = min(262143, max(0, nG));
                nB = min(262143, max(0, nB));
                
                nR >>= 10;
                nR &= 0xff;
                nG >>= 10;
                nG &= 0xff;
                nB >>= 10;
                nB &= 0xff;
                
                out[offset++] = (unsigned char)nR;
                out[offset++] = (unsigned char)nG;
                out[offset++] = (unsigned char)nB;
            }
        }
}

// input: image *src, type: -1, anti-clockwise rotation, 1, clockwise rotation
// output: *dst: rotated image
int cxImgRotate (PaGrayImage *src, PaGrayImage *dst, int type)
{
    if ((!src) || (!dst) || (src->width != dst->height) || (src->height != dst->width))
    {
        return -1;
    }
    
    int n = 0;
    //    int linesize = src->widthstep;
    int width = src->width;
    int height = src->height;
    
    unsigned char *ptrSrc = (unsigned char *)(src->data);
    unsigned char *ptrDst = (unsigned char *)(dst->data);
    
    
    if (-1 == type)  // anti closewise 90 degree
    {
        for (int j=width;j>0;j--)
        {
            for (int i=0;i<height;i++)
            {
                *(ptrDst+n) = *(ptrSrc+width*i+j-1);
                n++;
            }
        }
    }
    else if (type == 1) // clockwise 90 degree
    {
        for (int j=1;j<=width;j++)
        {
            for (int i=height;i>0;i--)
            {
                *(ptrDst+n) = *(ptrSrc+width*(i-1)+j-1);
                n++;
            }
        }
    }
    
    return 0;
}
