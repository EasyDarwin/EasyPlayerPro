//
//  PathUnit.h
//  EasyPlayer
//
//  Created by leo on 2017/12/30.
//  Copyright © 2017年 cs. All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 录像、截图的管理
 */
@interface PathUnit : NSObject

#pragma mark - record path

// 删除摄像头的录像地址
+ (void) deleteBaseRecordPathWithURL:(NSString *)url;

// 删除录像
+ (void) deleteBaseRecordName:(NSString *)name url:(NSString *)url;

// 摄像头下的文件
+ (NSArray *) recordListWithURL:(NSString *)url;

// 摄像头的录像地址
+ (NSString *) recordWithURL:(NSString *)url;

// 录像时 临时生成的h264、aac
+ (NSString *) recordH264;
+ (NSString *) recordAAC;

#pragma mark - image path

// 删除摄像头的截屏地址
+ (void) deleteBaseShotPathWithURL:(NSString *)url;

// 删除录像
+ (void) deleteBaseShotName:(NSString *)name url:(NSString *)url;

// 摄像头下的文件
+ (NSArray *) screenShotListWithURL:(NSString *)url;

// 摄像头的截屏地址
+ (NSString *) screenShotWithURL:(NSString *)url;

// 摄像头自动截屏的地址
+ (NSString *) snapshotWithURL:(NSString *)url;

#pragma mark - base path

+ (NSString *) baseRecordPathWithURL:(NSString *)url;
+ (NSString *) baseShotPathWithURL:(NSString *)url;
    
@end
