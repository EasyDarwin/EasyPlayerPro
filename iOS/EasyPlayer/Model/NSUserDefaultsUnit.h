//
//  NSUserDefaultsUnit.h
//  EasyPlayer
//
//  Created by leo on 2017/12/30.
//  Copyright © 2017年 cs. All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 设置的管理
 */
@interface NSUserDefaultsUnit : NSObject

#pragma mark - UDP模式观看视频(默认TCP模式)

+ (void) setUDP:(BOOL)isUDP;

+ (BOOL) isUDP;

+ (NSString *) UDPDesc;

#pragma mark - key有效期

+ (void) setActiveDay:(int)value;

+ (int) activeDay;

@end
