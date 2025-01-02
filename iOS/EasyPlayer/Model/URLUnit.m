//
//  URLUnit.m
//  EasyPlayerRTMP
//
//  Created by leo on 2019/4/27.
//  Copyright © 2019年 cs. All rights reserved.
//

#import "URLUnit.h"
#import <YYKit/YYKit.h>

static NSString *URLUnitName = @"URLUnitName";
static NSString *URLUnitKey = @"URLUnitKey";

@implementation URLUnit

#pragma mark - 播放url的存储

// 获取所有url
+ (NSMutableArray *) urlModels {
    YYCache *cache = [YYCache cacheWithName:URLUnitName];
    NSMutableArray *arr = (NSMutableArray *)[cache objectForKey:URLUnitKey];
    
    return arr;
}

// 添加rl
+ (void) addURLModel:(URLModel *)model {
    NSMutableArray *arr = [self urlModels];
    if (!arr) {
        arr = [[NSMutableArray alloc] init];
    }
    
    BOOL isRepeat = NO;
    for (int i = 0; i < arr.count; i++) {
        URLModel *m = arr[i];
        
        if ([m.url isEqualToString:model.url]) {
            isRepeat = YES;
            [arr replaceObjectAtIndex:i withObject:model];
            break;
        }
    }
    
    if (!isRepeat) {
        [arr insertObject:model atIndex:0];
    }
    
    YYCache *cache = [YYCache cacheWithName:URLUnitName];
    [cache setObject:arr forKey:URLUnitKey];
}

+ (void) updateURLModel:(URLModel *)model oldModel:(URLModel *)m {
    NSMutableArray *arr = [self urlModels];
    if (!arr) {
        arr = [[NSMutableArray alloc] init];
    }
    
    for (int i = 0; i < arr.count; i++) {
        URLModel *temp = arr[i];
        
        if ([temp.url isEqualToString:m.url]) {
            [arr replaceObjectAtIndex:i withObject:model];
            break;
        }
    }
    
    YYCache *cache = [YYCache cacheWithName:URLUnitName];
    [cache setObject:arr forKey:URLUnitKey];
}

// 删除url
+ (void) removeURLModel:(URLModel *)model {
    NSMutableArray *arr = [self urlModels];
    [arr addObject:model];
    
    [arr removeObject:model];
    
    YYCache *cache = [YYCache cacheWithName:URLUnitName];
    [cache setObject:arr forKey:URLUnitKey];
}

@end
