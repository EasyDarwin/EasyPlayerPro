//
//  AboutViewController.m
//  EasyPlayer
//
//  Created by leo on 2017/12/30.
//  Copyright © 2017年 cs. All rights reserved.
//

#import "AboutViewController.h"
#import "NSUserDefaultsUnit.h"
#import "WebViewController.h"

@interface AboutViewController ()

@property (weak, nonatomic) IBOutlet UILabel *nameLabel;
@property (weak, nonatomic) IBOutlet UILabel *descLabel;

@end

@implementation AboutViewController

- (instancetype) initWithStoryboard {
    return [[UIStoryboard storyboardWithName:@"Main" bundle:nil] instantiateViewControllerWithIdentifier:@"AboutViewController"];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.navigationItem.title = @"版本信息";
    
    NSString *name = @"EasyPlayer Pro iOS 播放器";
    NSString *content;
    UIColor *color;
    
    int activeDays = [NSUserDefaultsUnit activeDay];
    if (activeDays >= 9999) {
        content = @"激活码永久有效";
        color = UIColorFromRGB(0x2cff1c);
    } else if (activeDays > 0) {
        content = [NSString stringWithFormat:@"激活码还剩%ld天可用", (long)activeDays];
        color = UIColorFromRGB(0xeee604);
    } else {
        content = [NSString stringWithFormat:@"激活码已过期%ld天", (long)activeDays];
        color = UIColorFromRGB(0xf64a4a);
    }
    
    NSString *str = [NSString stringWithFormat:@"%@(%@)", name, content];
    NSMutableAttributedString *attr = [[NSMutableAttributedString alloc] initWithString:str];
    
    NSRange range = [str rangeOfString:content];
    NSDictionary *dict = @{ NSForegroundColorAttributeName:color };
    
    [attr setAttributes:dict range:range];
    
    self.nameLabel.attributedText = attr;
    self.nameLabel.numberOfLines = 0;

    NSString *html = @"EasyPlayerPro是由 TSINGSEE青犀开放平台 开发和维护的一款精炼、易用、高效、稳定的流媒体播放器，支持RTSP(RTP over TCP/UDP)、RTMP、HTTP、HLS、TCP、UDP等多种流媒体协议，支持各种各样编码格式的流媒体音视频直播流、点播流、文件播放！";
    NSData *data = [html dataUsingEncoding:NSUnicodeStringEncoding];
    
    NSDictionary *options = @{ NSDocumentTypeDocumentAttribute : NSHTMLTextDocumentType,
                               NSCharacterEncodingDocumentAttribute : @(NSUTF8StringEncoding) };
    
    // 设置富文本
    NSMutableAttributedString *attrStr = [[NSMutableAttributedString alloc] initWithData:data options:options documentAttributes:nil error:nil];
    
    // 设置段落格式
    NSMutableParagraphStyle *para = [[NSMutableParagraphStyle alloc] init];
    para.lineSpacing = 7;
    para.paragraphSpacing = 10;
    [attrStr addAttribute:NSParagraphStyleAttributeName value:para range:NSMakeRange(0, attrStr.length)];
    
    [attrStr addAttribute:NSFontAttributeName value:[UIFont systemFontOfSize:16] range:NSMakeRange(0, attrStr.length)];
    [attrStr addAttribute:NSForegroundColorAttributeName value:UIColorFromRGB(0x4c4c4c) range:NSMakeRange(0, attrStr.length)];
    
    self.descLabel.attributedText = attrStr;
}

- (IBAction)easyDarwin:(id)sender {
    UIButton *btn = (UIButton *)sender;
    
    WebViewController *controller = [[WebViewController alloc] init];
    controller.title = @"EasyDarwin开源流媒体服务器";
    controller.url = btn.titleLabel.text;
    [self.navigationController pushViewController:controller animated:YES];
}

- (IBAction)easyDSS:(id)sender {
    UIButton *btn = (UIButton *)sender;
    
    WebViewController *controller = [[WebViewController alloc] init];
    controller.title = @"EasyDSS商用流媒体解决方案";
    controller.url = btn.titleLabel.text;
    [self.navigationController pushViewController:controller animated:YES];
}

- (IBAction)easyNVR:(id)sender {
    UIButton *btn = (UIButton *)sender;
    
    WebViewController *controller = [[WebViewController alloc] init];
    controller.title = @"EasyNVR无插件直播方案";
    controller.url = btn.titleLabel.text;
    [self.navigationController pushViewController:controller animated:YES];
}

@end
