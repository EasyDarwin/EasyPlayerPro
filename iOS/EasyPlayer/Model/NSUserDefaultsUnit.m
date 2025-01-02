//
//  NSUserDefaultsUnit.m
//  EasyPlayer
//
//  Created by leo on 2017/12/30.
//  Copyright © 2017年 cs. All rights reserved.
//

#import "NSUserDefaultsUnit.h"

static NSString *UDPKEY = @"UDPKEY";
static NSString *activeDay = @"activeDay";

@implementation NSUserDefaultsUnit

#pragma mark - UDP模式观看视频(默认TCP模式)

+ (void) setUDP:(BOOL)isUDP {
    [[NSUserDefaults standardUserDefaults] setBool:isUDP forKey:UDPKEY];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

+ (BOOL) isUDP {
    return [[NSUserDefaults standardUserDefaults] boolForKey:UDPKEY];
}

+ (NSString *) UDPDesc {
    if ([self isUDP]) {
        return @"udp";
    } else {
        return @"tcp";
    }
}

#pragma mark - key有效期

+ (void) setActiveDay:(int)value {
    [[NSUserDefaults standardUserDefaults] setInteger:value forKey:activeDay];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

+ (int) activeDay {
    return (int)[[NSUserDefaults standardUserDefaults] integerForKey:activeDay];
}

@end
