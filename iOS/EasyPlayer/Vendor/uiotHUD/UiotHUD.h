#import "MBProgressHUD.h"

@interface UiotHUD : MBProgressHUD

- (id)initWithUiotView:(UIView *)view;

- (void)show:(BOOL)animated;

- (void)hide:(BOOL)animated;
- (void)hide:(BOOL)animated afterDelay:(NSTimeInterval)delay;
@end
