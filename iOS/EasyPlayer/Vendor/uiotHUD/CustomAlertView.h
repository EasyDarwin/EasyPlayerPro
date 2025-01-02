#import <UIKit/UIKit.h>

@interface CustomAlertView : NSObject
+(CustomAlertView *)shareCustimView;
- (void)showWithCustomWithTitle:(NSString *)title andMessage:(NSString *)message;
- (void)dissCustomView;
@end
