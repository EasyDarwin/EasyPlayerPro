//
//  UICustomAlertView.m
//  UIOT-SmartHome-Mobile
//
//  Created by apple-mini on 16/4/13.
//  Copyright © 2016年 UIOT_YF. All rights reserved.
//

#import "CustomAlertView.h"

@interface CustomAlertView ()
@property(nonatomic, retain)UIAlertView * alertView;
@end

static CustomAlertView *customView = nil;
@implementation CustomAlertView
+(CustomAlertView *)shareCustimView{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        customView = [[CustomAlertView alloc] init];
        [customView initViewCustrom];
    });
    return customView;
}
- (void)initViewCustrom{
    self.alertView = [[UIAlertView alloc] init];
    [self.alertView addButtonWithTitle:NSLocalizedString(@"确定",@"")];
}
- (void)showWithCustomWithTitle:(NSString *)title andMessage:(NSString *)message{
    self.alertView.title = title;
    self.alertView.message = message;
    [self.alertView show];
}
- (void)dissCustomView{
    [self.alertView dismissWithClickedButtonIndex:0 animated:YES];
}
/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

@end
