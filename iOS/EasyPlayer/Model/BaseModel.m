//
//  BaseModel.m
//  Easy
//
//  Created by leo on 2018/11/19.
//  Copyright © 2018 leo. All rights reserved.
//

#import "BaseModel.h"

@implementation BaseModel

+ (instancetype) convertFromDict:(NSDictionary *)dict {
    return [BaseModel modelWithDictionary:dict];
}

+ (NSMutableArray *) convertFromArray:(NSArray *)array {
    NSMutableArray *result = [[NSMutableArray array] init];
    
    if (!array || array.count == 0) {
        return result;
    }
    
    for (NSDictionary *dict in array) {
        [result addObject: [self convertFromDict:dict]];
    }
    
    return result;
}

- (void)encodeWithCoder:(NSCoder *)aCoder {
    [self modelEncodeWithCoder:aCoder];
}

- (id)initWithCoder:(NSCoder *)aDecoder {
    self = [super init];
    return [self modelInitWithCoder:aDecoder];
}

- (id)copyWithZone:(NSZone *)zone {
    return [self modelCopy];
}

- (NSUInteger)hash {
    return [self modelHash];
}

- (BOOL)isEqual:(id)object {
    return [self modelIsEqual:object];
}

- (NSString *)description {
    return [self modelDescription];
}

@end
