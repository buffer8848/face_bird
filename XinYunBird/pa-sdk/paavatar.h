/**
*** Copyright (C) 1985-2013 Intel Corporation.  All rights reserved.
***
*** The information and source code contained herein is the exclusive
*** property of Intel Corporation and may not be disclosed, examined
*** or reproduced in whole or in part without explicit written authorization
*** from the company.
***
*** # high-level apis definition
*** Embedded Application Lab, Intel Labs China.
**/

#ifndef PA_AVATAR_H_
#define PA_AVATAR_H_

#ifdef PA_SHARED
#define AVATAREXPORT __declspec(dllexport)
#else
#define AVATAREXPORT
#endif 


#ifdef __cplusplus
extern "C" {
#endif 

////////////////////////////////////////////////////////////////////////////
// avatar algorithm version
//
////////////////////////////////////////////////////////////////////////////
#define PA_MAJOR_VERSION    4
#define PA_MINOR_VERSION    3
#define PA_SUBMINOR_VERSION 164


#define PA_TRACK_VERSION 2
#define PA_TRACK_SUBVERSION 1

#ifndef NULL
#define NULL 0
#endif
////////////////////////////////////////////////////////////////////////////
// avatar basic data structure
//
////////////////////////////////////////////////////////////////////////////

//pocket avatar API returned status
typedef enum PaStatus
{
    PA_STATUS_OK = 0,               //everything is ok
    PA_STATUS_ERROR,                //unknown /unspecified error 
    PA_STATUS_NO_MEM,               //insufficient memory

    PA_STATUS_BAD_ARG,              //function arg/param is bad
    PA_STATUS_BAD_FUNC,             //unsupported function
    PA_STATUS_BAD_IMAGE,            //image is invalid

    PA_STATUS_NULL_POINTER,         //null pointer
    PA_STATUS_DIVIDE_ZERO,          //division by zero

    PA_STATUS_UNKNOWN_EVENT,        //event type error 
    PA_STATUS_VERSION_INCOMPATIBLE, //
    PA_STATUS_UNKNOWN_PROPERTY,
    PA_STATUS_UNKNOWN_STAT,
    PA_STATUS_UNKNOWN_TOUCH,
    PA_STATUS_UNKNOWN_MODEL_PROPERTY,
    PA_STATUS_UNKNOWN_TRACK_PROPERTY,

    PA_STATUS_TRACK_FAILED,         //track failed

} PaStatus;

typedef enum PaRotation
{
    PA_ROTATION_0 = 0, // rotation [0~270] no mirror
    PA_ROTATION_90, 
    PA_ROTATION_180, 
    PA_ROTATION_270, 

    PA_ROTATION_0_MIRROR,        // rotation [0~270] & mirror
    PA_ROTATION_90_MIRROR,
    PA_ROTATION_180_MIRROR, 
    PA_ROTATION_270_MIRROR   
}PaRotation;

typedef enum PaProperty
{
    PA_PROPERTY_DISPLAY_MESH = 0,       // draw mesh or not
    PA_PROPERTY_MOVING_AVATAR,          // model can change position
    PA_PROPERTY_SKELETON,
    PA_PROPERTY_RENDER_FLIP_Y,
    PA_PROPERTY_RENDER_FUR
}PaProperty;

typedef enum PaTrackerProperty
{
    PA_PROPERTY_DISPLAY_MASK = 0,
    PA_PROPERTY_MASK_SIZE,
    PA_PROPERTY_MASK_LAYOUT,
    
    // message recording and conversion
    PA_PROPERTY_RECORDING,
    PA_PROPERTY_CONV_MESSAGE,
    
    // message property
    PA_PROPERTY_MESSAGE_FPS,
    PA_PROPERTY_MESSAGE_SIZE,
    PA_PROPERTY_MESSAGE_MIRROR
}PaTrackerProperty;

typedef enum PaModelProperty
{
    PA_MODEL_SUPPORT_BS_TONGUEOUT = 0,  // whether support tongue out
    PA_MODEL_SUPPORT_BS_WINK,           // whether support wink
    PA_MODEL_SUPPORT_BS_KISS,           // whether support KISS

    // model image
    PA_MDDEL_BACKGROUND,
    PA_MDDEL_PREVIEWIMAGE,
    PA_MDDEL_ICON,
    // model meta data
    PA_MODEL_VERSION_S,        
    PA_MODEL_TYPE,             
    PA_MODEL_NAME_EN_S,        
    PA_MODEL_NAME_CH_S,        
    PA_MODEL_DESCRIPTION_ENGLISH_S,
    PA_MODEL_DESCRIPTION_CHINESE_S,
    PA_MODEL_BS_NUMBER,        
    PA_MDDEL_ENABLE_WINK,      
    PA_MDDEL_ENABLE_TONGUE,    
    PA_MDDEL_ENABLE_KISS, 
    PA_MDDEL_ENABLE_SKELETON,
    PA_MDDEL_CATEGORY_0_S,     
    PA_MDDEL_CATEGORY_1_S,     
    PA_MDDEL_CATEGORY_2_S,     
    PA_MDDEL_DEMOGRAPHICS_S,   
    PA_MDDEL_MMNO_S,           
    PA_MDDEL_GOOGLE_ID_S,      
    PA_MDDEL_APPLE_ID_S,       
    PA_MDDEL_POLITICAL_FIGURE, 
    PA_MDDEL_GOLIVE_DATE_S,    
    PA_MDDEL_EXPIRE_DATE_S,    
    PA_MDDEL_PRICE,            
    PA_MDDEL_PRICE_P2P,        
    PA_MDDEL_PRICE_VIDEO,      
    PA_MDDEL_AWARD_P2P,        
    PA_MDDEL_AWARD_VIDEO,      
    PA_MDDEL_STOREFRONT_NAME_S,
    PA_MDDEL_PA_LOGO,          
    PA_MDDEL_BANNER_URL_S,     
    PA_MDDEL_REFERRAL_URL_S,   
    PA_MDDEL_DISPLAY_BANNER,   
    PA_MDDEL_DISPLAY_REFERRAL, 
    PA_MDDEL_CHANGE_BACK,      
    PA_MDDEL_AUTHOR_S,  
    PA_MDDEL_ALLOW_SHARING,    
    PA_MDDEL_SAMPLE_URL_S,     
    PA_MDDEL_ZOOMING,          
    PA_MDDEL_SCREEN_LOCATION_S,

    PA_MODEL_OFFSET,           
    PA_MODEL_SIZE,             
    PA_MDDEL_BS_OFFSET,        
    PA_MDDEL_BS_OFFSET_SIZE,   
    PA_MDDEL_BACKGROUND_OFFSET,
    PA_MDDEL_BACKGROUND_SIZE,  
    PA_MDDEL_PREVIEW_OFFSET,   
    PA_MDDEL_PREIVEW_SIZE,     
    PA_MDDEL_ICON_OFFSET,      
    PA_MDDEL_ICON_SIZE,

    PA_MDDEL_SUPPORT_SKELETON,
    PA_MDDEL_SKELETON_OFFSET,
    PA_MDDEL_SKELETON_OFFSET_SIZE,

    PA_MODEL_SNAPSHOT_WIDTH,
    PA_MODEL_SNAPSHOT_HEIGHT,
    PA_MODEL_SNAPSHOT_SIZE,
    PA_MODEL_SNAPSHOT_DATA,
}PaModelProperty;

typedef enum PaTrackerStat
{
    PA_TRACKER_STAT_FPS_VALUE = 0,  //get the track FPS value
    PA_TRACKER_STAT_FPS_VALID,      //check if FPS is enough for tracker working well		
    PA_TRACKER_STAT_IMAGE_QUALITY,  //check if image quality is good for tracking
    PA_TRACKER_STAT_FAIL_RATE,      //get tracking fail rate
    PA_TRACKER_STAT_RESET_RATE,     //get tracking reset rate
    PA_TRACKER_STAT_MASK_NODE       //get mask node
}PaTrackerStat;

typedef enum PaModelStat
{
    PA_MODEL_STAT_DRAW_FPS_VALUE = 0,   //get the model output FPS value
    PA_MODEL_STAT_DRAW_FPS_VALID,       //check if FPS is enough for renderer working well
}PaModelStat;

typedef enum PaTouchType
{
    PA_TOUCH_UNTOUCH = 0,
    PA_TOUCH_TONGUE_OUT,
    PA_TOUCH_RIGHT_WINK,
    PA_TOUCH_LEFT_WINK,
    PA_TOUCH_KISS,
    PA_TOUCH_WINK,
}PaTouchType;

//pocket avatar image 
typedef struct PaGrayImage
{
    int width;
    int height;
    int widthstep;       //bytes in each line
    int channel;
    unsigned char* data; //gray image data
    PaRotation rotatemode;
} PaGrayImage, PaImage;

//pocket avatar face rect 
typedef struct PaRect
{
    int x;
    int y;
    int width;
    int height;
} PaRect;

typedef struct PaVector3f
{
    float x;
    float y;
    float z;
} PaVector3f;

typedef struct PaVector2f
{
    float x;
    float y;
} PaVector2f;

//void type as virtual class
typedef struct PaAudioData   PaAudioData;
typedef struct PaAvatarModel PaAvatarModel;

typedef struct PaEvent       PaEvent;
typedef struct PaEventQueue  PaEventQueue;

typedef struct PaTracker     PaTracker;
typedef struct PaSpeecher    PaSpeecher;
typedef struct PaAnimator    PaAnimator;
typedef struct PaRender      PaRender;
////////////////////////////////////////////////////////////////////////////
// avatar high-level APIs
////////////////////////////////////////////////////////////////////////////
//avatar face tracker APIs
AVATAREXPORT PaTracker*     paTrackerCreate();                       //output created PaTracker                     
AVATAREXPORT PaStatus       paTrackerDetect(PaTracker* pa_tracker, PaImage* pa_image, PaEvent* pa_event, PaRect *pa_face_rect); //input pa_gray_image, output detected pa_event     
AVATAREXPORT void           paTrackerRelease(PaTracker** pa_tracker);        //release pa_tracker    
AVATAREXPORT PaStatus       paTrackerGetStat(PaTracker* pa_tracker, PaTrackerStat pastat, void* pvalue); //get tracker runtime statistical
AVATAREXPORT PaStatus       paTrackerSetProperty(PaTracker* pa_tracker, PaTrackerProperty property, void* pvalue);
AVATAREXPORT PaStatus       paTrackerGetProperty(PaTracker* pa_tracker, PaTrackerProperty property, void* pvalue);

// draw mesh use opengl
AVATAREXPORT PaStatus       paTrackerMeshCreate(PaTracker* pa_tracker);
AVATAREXPORT PaStatus       paTrackerMeshDraw(PaTracker* pa_tracker);
AVATAREXPORT void           paTrackerMeshRelease(PaTracker* pa_tracker);

//avatar touch APIs
AVATAREXPORT PaStatus       paAvatarTouch(PaTouchType touch_type, int duration_frame, PaEvent* pa_event);

//avatar event create API
AVATAREXPORT PaEvent*       paEventCreate();
AVATAREXPORT int            paEventGetSize(PaEvent *pa_event);
AVATAREXPORT PaStatus       paEventGetMirror(PaEvent *pa_event, PaEvent *pa_event_mirror);
AVATAREXPORT void           paEventRelease(PaEvent** pa_event);

//avatar event management of track event, touch event, speech event and symbol event
AVATAREXPORT PaEventQueue*  paEventQueueCreate(int max_size);            //create PaEventQueue with max_size buffer
AVATAREXPORT PaStatus       paEventQueuePushBack(PaEventQueue* pa_event_queue, PaEvent* pa_event);  //push an event into pa_event_queue
AVATAREXPORT PaStatus       paEventQueuePopUp(PaEventQueue* pa_event_queue, PaEvent** pa_event);     //pop up an event from pa_event_queue
AVATAREXPORT PaStatus       paEventQueueClear(PaEventQueue* pa_event_queue);                         //clear pa_event_queue
AVATAREXPORT void           paEventQueueRelease(PaEventQueue** pa_event_queue);

//avatar model I/O APIs
AVATAREXPORT PaAvatarModel* paAvatarModelCreate(const char* model_file);    //load avatar graphics model from model_file.
AVATAREXPORT PaStatus       paAvatarModelDraw(PaAvatarModel* pa_avatar_model, PaVector3f pa_position, PaVector2f pa_size);  //draw animated pa_avatar_model to view_port(pa_position, pa_imagesize)
AVATAREXPORT void           paAvatarModelRelease(PaAvatarModel** pa_avatar_model);
AVATAREXPORT PaStatus       paModelGetStat(PaAvatarModel* pa_model, PaModelStat pastat, void* pvalue); 
AVATAREXPORT PaStatus       paModelGetProperty(PaAvatarModel* pa_model, PaModelProperty property, void* pvalue);

//avatar animator APIs
AVATAREXPORT PaAnimator*    paAnimatorCreate();          //input animator's init path and output created PaAnimator
AVATAREXPORT PaStatus       paAnimatorAnimate(PaAnimator* pa_animator, PaEventQueue* pa_event_queue, PaAvatarModel* pa_avatar_model); //input pa_event_queue and pa_avatar_model, output animated pa_avatar_model
AVATAREXPORT void           paAnimatorRelease(PaAnimator** pa_animator);

//avatar render APIs
AVATAREXPORT PaRender*      paRendererCreate(PaAvatarModel* pa_avatar_model);          //input renderer's init model object and output created PaRenderer
AVATAREXPORT void           paRendererRelease(PaRender** PaAvatarRender);

// set avatar property
AVATAREXPORT PaStatus       paGetVersion(int *major, int *minor, int *subminor);
AVATAREXPORT PaStatus       paAvatarEnable(PaProperty property);
AVATAREXPORT PaStatus       paAvatarDisable(PaProperty property);
AVATAREXPORT PaStatus       paAvatarReset(PaTracker* pa_tracker, PaAnimator* pa_animator,  int delay_count);
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Avatar log configure to output debug info
typedef AVATAREXPORT  void (*pLogCallBack)(int /*iLevel*/, const char* /*pTag*/, const char* /*pMsg*/);
AVATAREXPORT void RegisterLogCallBack(pLogCallBack pCallback);
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AVATAREXPORT void paCallTrackerdata(void* data, PaTracker* pa_tracker,  int flag);
//AVATAREXPORT void paGetTrackerWorkload(PaTracker* pa_tracker, float msec[], int count);

#ifdef __cplusplus
}
#endif

#endif
