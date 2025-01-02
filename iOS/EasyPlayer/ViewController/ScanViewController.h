//
//  ScanViewController.h
//  EasyPlayerRTMP
//
//  Created by leo on 2019/4/26.
//  Copyright © 2019年 cs. All rights reserved.
//

#import "BaseViewController.h"
#import <ReactiveCocoa/ReactiveCocoa.h>

NS_ASSUME_NONNULL_BEGIN

/**
 二维码扫描
 */
@interface ScanViewController : BaseViewController

- (instancetype) initWithStoryboard;

@property (nonatomic, strong) RACSubject *subject;

@end

NS_ASSUME_NONNULL_END
