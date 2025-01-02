//
//  PathUnit.m
//  EasyPlayer
//
//  Created by leo on 2017/12/30.
//  Copyright © 2017年 cs. All rights reserved.
//

#import "PathUnit.h"

@implementation PathUnit

#pragma mark - record path

+ (void) deleteBaseRecordPathWithURL:(NSString *)url {
    NSString *path = [self baseRecordPathWithURL:url];
    
    if([[NSFileManager defaultManager] fileExistsAtPath:path]){
        [[NSFileManager defaultManager] removeItemAtPath:path error:nil];
    }
}

// 删除录像
+ (void) deleteBaseRecordName:(NSString *)name url:(NSString *)url {
    NSString* _dir = [self baseShotPathWithURL:url];
    
    if(![[NSFileManager defaultManager] fileExistsAtPath:_dir]) {
        return;
    }
    
    NSString *path = [_dir stringByAppendingPathComponent:name];
    
    if([[NSFileManager defaultManager] fileExistsAtPath:path]) {
        [[NSFileManager defaultManager] removeItemAtPath:path error:nil];
    }
}

// 摄像头下的文件
+ (NSArray *) recordListWithURL:(NSString *)url {
    NSArray *fileNameList = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:[self baseRecordPathWithURL:url] error:nil];
    
    return fileNameList;
}

/**
 摄像头的录像地址
 
 @param url 摄像头的url
 @return 录像地址
 */
+ (NSString *) recordWithURL:(NSString *)url {
    NSString* _dir = [self baseRecordPathWithURL:url];
    
    if(![[NSFileManager defaultManager] fileExistsAtPath:_dir]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:_dir withIntermediateDirectories:YES attributes:nil error:nil];
    }
    
    NSDate *date = [NSDate date];
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"YYYYMMddhhmmss"];
    
    NSString *path = [_dir stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.mp4", [formatter stringFromDate:date]]];
    
    return path;
}

// 录像时 临时生成的h264、aac
+ (NSString *) recordH264 {
    NSString *dir = [[self documentsDirectory] stringByAppendingPathComponent:@"record"];
    if(![[NSFileManager defaultManager] fileExistsAtPath:dir]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:dir withIntermediateDirectories:YES attributes:nil error:nil];
    }
    
    NSString *path = [dir stringByAppendingPathComponent:@"video.h264"];
    return path;
}

+ (NSString *) recordAAC {
    NSString *dir = [[self documentsDirectory] stringByAppendingPathComponent:@"record"];
    if(![[NSFileManager defaultManager] fileExistsAtPath:dir]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:dir withIntermediateDirectories:YES attributes:nil error:nil];
    }
    
    NSString *path = [dir stringByAppendingPathComponent:@"audio.aac"];
    return path;
}

#pragma mark - image path

+ (void) deleteBaseShotPathWithURL:(NSString *)url {
    NSString *path = [self baseShotPathWithURL:url];
    
    if([[NSFileManager defaultManager] fileExistsAtPath:path]){
        [[NSFileManager defaultManager] removeItemAtPath:path error:nil];
    }
}

// 删除录像
+ (void) deleteBaseShotName:(NSString *)name url:(NSString *)url {
    NSString* _dir = [self baseShotPathWithURL:url];
    
    if(![[NSFileManager defaultManager] fileExistsAtPath:_dir]) {
        return;
    }
    
    NSString *path = [_dir stringByAppendingPathComponent:name];
    
    if([[NSFileManager defaultManager] fileExistsAtPath:path]) {
        [[NSFileManager defaultManager] removeItemAtPath:path error:nil];
    }
}

// 摄像头下的文件
+ (NSArray *) screenShotListWithURL:(NSString *)url {
    NSArray *fileNameList = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:[self baseShotPathWithURL:url] error:nil];
    
    return fileNameList;
}

/**
 摄像头的截屏地址
 
 @param url 摄像头的url
 @return 截屏地址
 */
+ (NSString *) screenShotWithURL:(NSString *)url {
    NSString* _dir = [self baseShotPathWithURL:url];
    
    if(![[NSFileManager defaultManager] fileExistsAtPath:_dir]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:_dir withIntermediateDirectories:YES attributes:nil error:nil];
    }
    
    NSDate *date = [NSDate date];
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"YYYYMMddhhmmss"];
    
    NSString *path = [_dir stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.png", [formatter stringFromDate:date]]];
    
    return path;
}

/**
 摄像头自动截屏的地址

 @param url 摄像头的url
 @return 截屏地址
 */
+ (NSString *) snapshotWithURL:(NSString *)url {
    NSString* _dir = [self baseShotPathWithURL:url];
    
    if(![[NSFileManager defaultManager] fileExistsAtPath:_dir]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:_dir withIntermediateDirectories:YES attributes:nil error:nil];
    }
    
    NSString *path = [_dir stringByAppendingPathComponent:@"snapshot.png"];
    
    return path;
}

#pragma mark - base path

+ (NSString *) baseRecordPathWithURL:(NSString *)url {
    NSString *name = [NSString stringWithFormat:@"record/%@", [self baseURLWithURL:url]];
    
    NSString *dir = [[self documentsDirectory] stringByAppendingPathComponent:name];
    
    return dir;
}

+ (NSString *) baseShotPathWithURL:(NSString *)url {
    NSString *name = [NSString stringWithFormat:@"image/%@", [self baseURLWithURL:url]];
    
    NSString *dir = [[self documentsDirectory] stringByAppendingPathComponent:name];
    
    return dir;
}

+ (NSString *) baseURLWithURL:(NSString *)url {
    NSString *addr = [url stringByReplacingOccurrencesOfString:@"/" withString:@""];
    addr = [addr stringByReplacingOccurrencesOfString:@":" withString:@""];
    addr = [addr stringByReplacingOccurrencesOfString:@"." withString:@""];
    
    if (addr.length >= 128) {
        return [addr substringToIndex:128];
    }
    
    return addr;
}

+ (NSString *) documentsDirectory {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    
    return documentsDirectory;
}

@end
