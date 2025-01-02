//
//  URLModel.m
//  EasyPlayerRTMP
//
//  Created by leo on 2019/4/27.
//  Copyright © 2019年 cs. All rights reserved.
//

#import "URLModel.h"
#import "NSObject+YYModel.h"

@implementation URLModel

- (instancetype) initDefault {
    if (self = [super init]) {
        
    }
    
    return self;
}

+ (nullable NSDictionary<NSString *, id> *)modelCustomPropertyMapper {
    return @{ @"url" : @"url" };
}

+ (instancetype) convertFromDict:(NSDictionary *)dict {
    URLModel *model = [URLModel modelWithDictionary:dict];
    
    return model;
}

@end
