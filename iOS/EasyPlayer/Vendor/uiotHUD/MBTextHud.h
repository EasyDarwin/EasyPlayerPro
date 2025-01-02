#import <UIKit/UIKit.h>

@interface MBTextHud : UIView
+(void)MBTextHudShowWithText:(NSString *)textStr;
+ (void)uiotHudShow;
+(void)uiotHudHideAferDelay:(NSInteger)time;
@end
