//
//  UiotHUD.m
//  UIOT-SmartHome-Mobile
//
//  Created by apple on 14-10-17.
//  Copyright (c) 2014年 UIOT_YF. All rights reserved.
//

#import "UiotHUD.h"

@implementation UiotHUD
{
    UIImageView *ringImageView;
    UIImageView *smallImageView;
    
    NSTimer *timer;
    
    NSTimeInterval outTime;
}

- (id)initWithUiotView:(UIView *)view
{
//    UIView *uiotView = [[UIView alloc]initWithFrame:CGRectMake(0, 0, 50, 50)];
//    uiotView.backgroundColor = [UIColor clearColor];
//    
//    ringImageView = [[UIImageView alloc]initWithImage:[UIImage imageNamed:@"uiot_bigRing"]];
//    [ringImageView setFrame:CGRectMake(0, 0, 50, 50)];
//    ringImageView.center = uiotView.center;
//    
//    smallImageView = [[UIImageView alloc]initWithImage:[UIImage imageNamed:@"uiot_smallRing"]];
//    [smallImageView setFrame:CGRectMake(0, 0, 50, 50)];
//    smallImageView.center = uiotView.center;
//    
//    UIImageView *uiotImageView = nil;
//    if([SHServer instance].currentIpaType == IPAType_FormalSH || [SHServer instance].currentIpaType == IPAType_BateSH)
//    {
//        uiotImageView = [[UIImageView alloc]initWithImage:[UIImage imageNamed:@"uiot_uiot"]];
//    }
//    else
//    {
//        uiotImageView.alpha = 0.0f;
//    }
//    
//    [uiotImageView setFrame:CGRectMake(0, 0, 30, 20)];
//    uiotImageView.center = uiotView.center;
//    
//    [uiotView addSubview:ringImageView];
//    [uiotView addSubview:smallImageView];
//    [uiotView addSubview:uiotImageView];
    
    self = [super initWithView:view];
    if (self)
    {
        self.mode = MBProgressHUDModeIndeterminate;
        //self.customView = uiotView;
        //self.animationType = MBProgressHUDAnimationZoom;
        //self.color = [UIColor colorWithWhite:1 alpha:0.2];
        self.margin = 10.0f;
        self.square = YES;
        //self.removeFromSuperViewOnHide = YES;
        [self.button setTitle:NSLocalizedString(@"取消", @"HUD cancel button title") forState:UIControlStateNormal];
        [self.button addTarget:self action:@selector(cancelWork:) forControlEvents:UIControlEventTouchUpInside];
    }
    
    return self;
}
- (void)cancelWork:(id)sender {
    [self hideAnimated:YES];
}
- (void)show:(BOOL)animated
{
    [super showAnimated:animated];
    [self showAnimated:animated];
//    CABasicAnimation *anmiation = [CABasicAnimation animationWithKeyPath:@"transform.rotation.z"];
//    anmiation.fromValue = [NSNumber numberWithFloat:0];
//    anmiation.toValue = [NSNumber numberWithFloat:3.14*2];
//    anmiation.duration = 1.5;
//    anmiation.repeatCount = HUGE_VALF;
//    
//    [ringImageView.layer addAnimation:anmiation forKey:@"transform.rotation.z"];
//    
//    CABasicAnimation *anmiation2 = [CABasicAnimation animationWithKeyPath:@"transform.rotation.z"];
//    anmiation2.fromValue = [NSNumber numberWithFloat:0];
//    anmiation2.toValue = [NSNumber numberWithFloat:-3.14*2];
//    anmiation2.duration = 1.5;
//    anmiation2.repeatCount = HUGE_VALF;
//    
//    [smallImageView.layer addAnimation:anmiation2 forKey:@"transform.rotation.z2"];
}

- (void)hide:(BOOL)animated
{
    [super hideAnimated:animated];
    [self hideAnimated:YES];
//    [ringImageView.layer removeAnimationForKey:@"transform.rotation.z"];
//    [smallImageView.layer removeAnimationForKey:@"transform.rotation.z2"];

}

- (void)hide:(BOOL)animated afterDelay:(NSTimeInterval)delay
{
    [super hideAnimated:animated afterDelay:delay];
    [self hideAnimated:animated afterDelay:delay];
    //timer = [NSTimer scheduledTimerWithTimeInterval:delay target:self selector:@selector(removeAnmiation) userInfo:nil repeats:NO];
}

//私用方法
- (void)removeAnmiation
{
    [ringImageView.layer removeAnimationForKey:@"transform.rotation.z"];
    [smallImageView.layer removeAnimationForKey:@"transform.rotation.z2"];
    
    [timer invalidate];
}




@end
