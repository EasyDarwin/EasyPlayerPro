
#import "BaseViewController.h"
#import <VideoToolbox/VideoToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <IJKMediaFramework/IJKMediaFramework.h>

/**
 视频播放
 */
@interface PlayViewController : BaseViewController

@property (nonatomic, strong) NSString *urlStr;// 视频源
@property (nonatomic, assign) BOOL isLocal;// 是否播放本地视频

@property (atomic, retain) id<IJKMediaPlayback> player;

@end
