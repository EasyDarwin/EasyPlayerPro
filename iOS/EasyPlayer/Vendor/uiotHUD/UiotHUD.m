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
  
    self = [super initWithView:view];
    if (self)
    {
        self.mode = MBProgressHUDModeIndeterminate;
        self.margin = 10.0f;
        self.square = YES;
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
}

- (void)hide:(BOOL)animated
{
    [super hideAnimated:animated];
    [self hideAnimated:YES];

}

- (void)hide:(BOOL)animated afterDelay:(NSTimeInterval)delay
{
    [super hideAnimated:animated afterDelay:delay];
    [self hideAnimated:animated afterDelay:delay];
}

//私用方法
- (void)removeAnmiation
{
    [ringImageView.layer removeAnimationForKey:@"transform.rotation.z"];
    [smallImageView.layer removeAnimationForKey:@"transform.rotation.z2"];
    
    [timer invalidate];
}




@end
