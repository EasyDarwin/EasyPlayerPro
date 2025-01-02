#import "MBTextHud.h"
#import "MBProgressHUD.h"
#import "UiotHUD.h"

@interface MBTextHud  ()
@property(nonatomic, retain)MBProgressHUD * hud;
@property(nonatomic, retain)UiotHUD * uiotHud;
@property(nonatomic, retain)MBProgressHUD * textHud;
@end

@implementation MBTextHud
+(id)shareMBTextHud{
    static MBTextHud * mbtextHud = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        mbtextHud = [[MBTextHud alloc] init];
    });
    return mbtextHud;
}
- (MBProgressHUD *)hud{
    if (_hud == nil) {
        self.hud = [[MBProgressHUD alloc] initWithView:[UIApplication sharedApplication].keyWindow];
        _hud.mode = MBProgressHUDModeText;
        _hud.removeFromSuperViewOnHide = YES;
        //[[UIApplication sharedApplication].keyWindow addSubview:_hud];
    }
    return _hud;
}
- (UiotHUD *)uiotHud{
    if (_uiotHud == nil) {
        _uiotHud = [[UiotHUD alloc] initWithUiotView:[UIApplication sharedApplication].keyWindow];
        //_uiotHud.removeFromSuperViewOnHide = YES;
        //[[UIApplication sharedApplication].keyWindow addSubview:_uiotHud];
    }
    return _uiotHud;
}
//- (MBProgressHUD *)textHud{
//    if (_textHud == nil) {
//        self.textHud = [MBProgressHUD showHUDAddedTo:[UIApplication sharedApplication].keyWindow animated:YES];
//        _textHud.mode = MBProgressHUDModeText;
//        _textHud.removeFromSuperViewOnHide = YES;
//        _textHud.label.numberOfLines = 0;
//        _textHud.offset = CGPointMake(0.f, MBProgressMaxOffset);
//    }
//    return _textHud;
//}
- (void)mbtextShowText:(NSString *)textStr{
    [self.hud showAnimated:YES];
    self.hud.label.text = textStr;
    _hud.label.numberOfLines = 0;
    [[UIApplication sharedApplication].keyWindow addSubview:self.hud];
    [self.hud hideAnimated:YES afterDelay:1.5];
}
- (void)mbUiotAnimationShow{
    [[UIApplication sharedApplication].keyWindow addSubview:_uiotHud];
    [self.uiotHud show:YES];
    [self.uiotHud hide:YES afterDelay:15];
}
- (void)mbUiotAnimationHidden:(NSInteger)time{
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(time * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self.uiotHud removeFromSuperview];
    });
    [self.uiotHud hide:YES afterDelay:time];
}
+(void)MBTextHudShowWithText:(NSString *)textStr{
    [[MBTextHud shareMBTextHud] mbtextShowText:textStr];
}
+ (void)uiotHudShow{
    [[MBTextHud shareMBTextHud] mbUiotAnimationShow];
}
+(void)uiotHudHideAferDelay:(NSInteger)time{
     [[MBTextHud shareMBTextHud] mbUiotAnimationHidden:time];
}
- (void)MBTextShowStayWaytext:(NSString*)str
{
    if (_textHud == nil) {
        self.textHud = [MBProgressHUD showHUDAddedTo:[UIApplication sharedApplication].keyWindow animated:YES];
        _textHud.mode = MBProgressHUDModeText;
        _textHud.label.text = @"网络不给力,请重试";
        _textHud.label.numberOfLines = 0;
        _textHud.offset = CGPointMake(0.f, MBProgressMaxOffset);
        _textHud.hidden = YES;
    }
    else if(self.textHud.hidden) {
        _textHud.hidden = NO;
        [_textHud showAnimated:YES];
    }
    
}
- (void)MBTextHiddenStayWaytext:(NSString*)str
{
        _textHud.hidden = NO;
        [_textHud hideAnimated:YES];
    
}
+(void)MBTextShowStayWay:(NSString*)str{
    [[MBTextHud shareMBTextHud] MBTextShowStayWaytext:str];
}
//+(void)MBTextHudShowWithText:(NSString *)textStr{
//    MBProgressHUD * viewhud = [[MBProgressHUD alloc] initWithView:[UIApplication sharedApplication].keyWindow];
//    viewhud.mode = MBProgressHUDModeText;
//    viewhud.removeFromSuperViewOnHide = YES;
//    viewhud.labelText = textStr;
//    if (textStr.length>12) {
//         viewhud.labelFont = [UIFont systemFontOfSize:13];
//    }
//    [[UIApplication sharedApplication].keyWindow addSubview:viewhud];
//    [viewhud show:YES];
//    [viewhud hide:YES afterDelay:1.5];
//}
//+ (void)uiotHudShow{
//    [self uiotHudHideAferDelay:0];
//    UiotHUD * hud = [[UiotHUD alloc] initWithUiotView:[UIApplication sharedApplication].keyWindow];
//    [[UIApplication sharedApplication].keyWindow addSubview:hud];
//    hud.removeFromSuperViewOnHide = YES;
//     [hud hide:YES afterDelay:15];
//    hud.tag = 12121;
//    [hud show:YES];
//}
//+(void)uiotHudHideAferDelay:(NSInteger)time{
//    UiotHUD * hud = [[UIApplication sharedApplication].keyWindow viewWithTag:12121];
//    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(time * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
//        [hud removeFromSuperview];
//    });
//    
//    [hud hide:YES afterDelay:time];
//}
/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

@end
