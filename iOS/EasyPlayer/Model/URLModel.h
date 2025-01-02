//
//  URLModel.h
//  EasyPlayerRTMP
//
//  Created by leo on 2019/4/27.
//  Copyright © 2019年 cs. All rights reserved.
//

#import "BaseModel.h"

NS_ASSUME_NONNULL_BEGIN

@interface URLModel : BaseModel

@property (nonatomic, copy) NSString *url;  // 流地址

@property (nonatomic, retain) NSMutableArray *images;
@property (nonatomic, retain) NSMutableArray *records;

- (instancetype) initDefault;

@end

NS_ASSUME_NONNULL_END
