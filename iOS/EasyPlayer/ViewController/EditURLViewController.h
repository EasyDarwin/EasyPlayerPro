//
//  EditURLViewController.h
//  EasyPlayerRTMP
//
//  Created by leo on 2019/4/25.
//  Copyright © 2019年 cs. All rights reserved.
//

#import "BaseViewController.h"
#import "URLModel.h"
#import <ReactiveCocoa/ReactiveCocoa.h>

NS_ASSUME_NONNULL_BEGIN

/**
 编译/添加流地址
 */
@interface EditURLViewController : BaseViewController

@property (nonatomic, strong) URLModel *urlModel;

@property (nonatomic, strong) RACSubject *subject;

- (instancetype) initWithStoryboard;

@end

NS_ASSUME_NONNULL_END
