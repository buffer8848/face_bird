/*
 *
 * Copyright (C) 1985-2015 Intel Corporation.  All rights reserved.
 *
 * The information and source code contained herein is the exclusive
 * property of Intel Corporation and may not be disclosed, examined
 * or reproduced in whole or in part without explicit written authorization
 * from the company.
 *
 * Embedded Application Lab, Intel Labs China.
 *
 * @brief  One sentence description
 * @date   2012-12-10
 * @author Green Wang
 *
 */

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#undef PRODUCER_HAS_VIDEO_CAPTURE
#define PRODUCER_HAS_VIDEO_CAPTURE (__IPHONE_OS_VERSION_MIN_REQUIRED >= 40000 && TARGET_OS_EMBEDDED)

// Re-implement this protocol if you need capture data
@protocol CameraHelperDelegate

-(void)VideoDataOutputBuffer:(CMSampleBufferRef)sampleBuffer;

@end

// CameraHelper
@interface CameraHelper : NSObject
#if PRODUCER_HAS_VIDEO_CAPTURE
<AVCaptureVideoDataOutputSampleBufferDelegate>
#endif
{
@private
	int mFps;
	BOOL mFrontCamera;
	BOOL mFirstFrame;
	BOOL mStarted;
    NSDictionary* mVideoSettings;
    
	id<CameraHelperDelegate> outDelegate;
#if PRODUCER_HAS_VIDEO_CAPTURE
	AVCaptureSession* mCaptureSession;
	AVCaptureDevice *mCaptureDevice;
    AVCaptureVideoPreviewLayer* mPreviewLayer;
#endif
}

// Single object
+ (CameraHelper*)shareCameraHelper;
+ (void)closeCamera;

- (BOOL)startVideoCapture;
- (BOOL)startVideoCapture:(BOOL) bFront;
- (void)stopVideoCapture;

- (BOOL)changeToFront;
- (BOOL)changeToBack;

- (AVCaptureVideoPreviewLayer*)getAVCaptureVideoPreviewLayer;

// Set capture data output
- (void)setVideoDataOutputBuffer:(id<CameraHelperDelegate>)delegate;

// Set capture param
- (void)prepareVideoCapture:(NSDictionary*) videoSettings andFps: (int) fps andFrontCamera:(BOOL) bfront;

@end
