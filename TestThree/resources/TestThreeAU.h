
#include <TargetConditionals.h>
#if TARGET_OS_IOS == 1
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

#define IPLUG_AUVIEWCONTROLLER IPlugAUViewController_vTestThree
#define IPLUG_AUAUDIOUNIT IPlugAUAudioUnit_vTestThree
#import <TestThreeAU/IPlugAUViewController.h>
#import <TestThreeAU/IPlugAUAudioUnit.h>

//! Project version number for TestThreeAU.
FOUNDATION_EXPORT double TestThreeAUVersionNumber;

//! Project version string for TestThreeAU.
FOUNDATION_EXPORT const unsigned char TestThreeAUVersionString[];

@class IPlugAUViewController_vTestThree;
