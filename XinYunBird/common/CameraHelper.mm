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
#import "CameraHelper.h"

@interface CameraHelper (Private)

#if PRODUCER_HAS_VIDEO_CAPTURE
+(AVCaptureDevice *)cameraAtPosition:(AVCaptureDevicePosition)position;
//- (BOOL)startPreview;
//- (void)stopPreview;
#endif /* NGN_PRODUCER_HAS_VIDEO_CAPTURE */

@end /* NGN_PRODUCER_HAS_VIDEO_CAPTURE */

@implementation CameraHelper (Private)

#if PRODUCER_HAS_VIDEO_CAPTURE
+ (AVCaptureDevice *)cameraAtPosition:(AVCaptureDevicePosition)position{
	NSArray *cameras = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for (AVCaptureDevice *device in cameras){
        if (device.position == position){
            return device;
        }
    }
    return [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
}
#endif /* NGN_PRODUCER_HAS_VIDEO_CAPTURE */
@end

@implementation CameraHelper

static CameraHelper* g_camera = nil;

- (id)init
{
	if(g_camera)
		return g_camera;
	else
	{
		if(self = [super init])
		{
			self->mFps = 30;
			self->mFrontCamera = YES;
			self->mStarted = NO;
#if PRODUCER_HAS_VIDEO_CAPTURE
            self->mPreviewLayer = nil;
#endif
            self->mVideoSettings = [[NSDictionary alloc] initWithObjectsAndKeys:
                                    [NSNumber numberWithUnsignedInt:kCVPixelFormatType_420YpCbCr8BiPlanarFullRange],
                                    kCVPixelBufferPixelFormatTypeKey,
                                    nil];
			g_camera = self;
			outDelegate = nil;
		}
		return g_camera;
	}
}

+ (CameraHelper*)shareCameraHelper
{
	if(!g_camera)
		g_camera = [[CameraHelper alloc] init];
	return g_camera;
}

+ (void)closeCamera
{
	if(g_camera)
	{
		g_camera = nil;
	}
}

- (void)prepareVideoCapture:(NSDictionary*) videoSettings andFps: (int) fps andFrontCamera:(BOOL) bfront
{
    if (videoSettings)
    {
        self->mVideoSettings = [[NSDictionary alloc] initWithDictionary:videoSettings copyItems:YES];
    }
	self->mFps = fps;
	self->mFrontCamera = bfront;
}

- (BOOL)startVideoCapture
{
#if PRODUCER_HAS_VIDEO_CAPTURE
	NSError *error = nil;
    
	if(self->mStarted)
	{
        [self stopVideoCapture];
	}
    
	// Open camera device
    if((mCaptureDevice = [CameraHelper cameraAtPosition:mFrontCamera? AVCaptureDevicePositionFront:AVCaptureDevicePositionBack]) == nil)
	{
		NSLog(@"Failed to get valide capture device");
        return NO;
	}
	/*We setup the input*/
	AVCaptureDeviceInput *captureInput = [AVCaptureDeviceInput
										  deviceInputWithDevice:mCaptureDevice
										  error:&error];
    if (!captureInput)
	{
		NSLog(@"Failed to get video input");
		mCaptureDevice = nil;
        return NO;
    }
    
	/*We setup the output*/
	AVCaptureVideoDataOutput *captureOutput = [[AVCaptureVideoDataOutput alloc] init];
	/*While a frame is processes in -captureOutput:didOutputSampleBuffer:fromConnection: delegate methods no other frames are added in the queue.
	 If you don't want this behaviour set the property to NO */
	captureOutput.alwaysDiscardsLateVideoFrames = YES;
	/*We specify a minimum duration for each frame (play with this settings to avoid having too many frames waiting
	 in the queue because it can cause memory issues). It is similar to the inverse of the maximum framerate.
	 In this example we set a min frame duration of 1/10 seconds so a maximum framerate of 10fps. We say that
	 we are not able to process more than 10 frames per second.*/
    //captureOutput.minFrameDuration = CMTimeMake(1, 30);
    
	[captureOutput setVideoSettings:self->mVideoSettings];
	
	/*We create a serial queue to handle the processing of our frames*/
	dispatch_queue_t queue;
	queue = dispatch_queue_create("cameraQueue", NULL);
	[captureOutput setSampleBufferDelegate:self queue:queue];
	//dispatch_release(queue);
    
    /*And we create a capture session*/
	mCaptureSession = [[AVCaptureSession alloc] init];
	/*We add input and output*/
	[mCaptureSession addInput:captureInput];
	[mCaptureSession addOutput:captureOutput];
    /*Set camera image size*/
    [mCaptureSession setSessionPreset:AVCaptureSessionPresetMedium];
	
	/*Statrt camera*/
	[mCaptureSession startRunning];
    
    self->mStarted = YES;
    
    return YES;
#else
    return NO;
#endif
}

- (BOOL)startVideoCapture:(BOOL)bFront
{
    self->mFrontCamera = bFront;
    
    return [self startVideoCapture];
}

- (void)stopVideoCapture
{
#if PRODUCER_HAS_VIDEO_CAPTURE
	if(mCaptureSession)
    {
		[mCaptureSession stopRunning];
        mCaptureSession = nil;
		NSLog(@"Video capture stopped");
	}
    mCaptureDevice = nil;
    self->mStarted = NO;
#endif
}

- (BOOL)changeToFront
{
	if(mFrontCamera)
		return YES;
    
	[self stopVideoCapture];
	mFrontCamera = YES;
	[self startVideoCapture];
    
	return YES;
}

- (BOOL)changeToBack
{
	if(!mFrontCamera)
		return YES;
    
	[self stopVideoCapture];
	mFrontCamera = NO;
	[self startVideoCapture];
    
	return YES;
}

- (void)setVideoDataOutputBuffer:(id<CameraHelperDelegate>)delegate
{
	outDelegate = delegate;
}

- (AVCaptureVideoPreviewLayer*)getAVCaptureVideoPreviewLayer
{
#if PRODUCER_HAS_VIDEO_CAPTURE
    if (self->mPreviewLayer == nil)
    {
        self->mPreviewLayer = [AVCaptureVideoPreviewLayer layerWithSession: mCaptureSession];
        self->mPreviewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
    }
	return mPreviewLayer;
#else
    return nil;
#endif
    
}

#pragma mark -
#pragma mark AVCaptureVideoDataOutputSampleBufferDelegate
#if PRODUCER_HAS_VIDEO_CAPTURE
- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    // Execute real handle.
    if(outDelegate)
        [outDelegate VideoDataOutputBuffer:sampleBuffer];
}
#endif
@end
