//
//  EditURLViewController.m
//  EasyPlayerRTMP
//
//  Created by leo on 2019/4/25.
//  Copyright © 2019年 cs. All rights reserved.
//

#import "EditURLViewController.h"
#import "ScanViewController.h"
#import "URLUnit.h"
#import <WHToast.h>

@interface EditURLViewController ()

@property (weak, nonatomic) IBOutlet UIView *contentView;
@property (weak, nonatomic) IBOutlet UITextField *textField;
@property (weak, nonatomic) IBOutlet UIButton *scanBtn;

@property (weak, nonatomic) IBOutlet NSLayoutConstraint *contentViewWidth;
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *contentViewHeight;

@property (nonatomic, strong) URLModel *model;

@end

@implementation EditURLViewController

- (instancetype) initWithStoryboard {
    return [[UIStoryboard storyboardWithName:@"Main" bundle:nil] instantiateViewControllerWithIdentifier:@"EditURLViewController"];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = UIColorFromRGBA(0x000000, 0.6);
    
    self.contentViewWidth.constant = EasyScreenWidth;
    self.contentViewHeight.constant = EasyScreenHeight;
    
    EasyViewBorderRadius(self.contentView, 4, 0, [UIColor clearColor]);
    
    [self.scanBtn setImage:[UIImage imageNamed:@"scan"] forState:UIControlStateNormal];
    [self.scanBtn setImage:[UIImage imageNamed:@"scan_click"] forState:UIControlStateHighlighted];
    
    self.model = [[URLModel alloc] initDefault];
    
    if (self.urlModel) {
        self.model.url = self.urlModel.url;
    }
    
    if (self.model.url) {
        self.textField.text = self.model.url;
    }
}

#pragma mark - click listener

// 去扫描二维码
- (IBAction)scan:(id)sender {
    ScanViewController *vc = [[ScanViewController alloc] initWithStoryboard];
    [vc.subject subscribeNext:^(NSString *url) {
        self.textField.text = url;
    }];
    [self presentViewController:vc animated:YES completion:nil];
}

// 取消
- (IBAction)cancel:(id)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}

// 确定
- (IBAction)submit:(id)sender {
    NSString *text = self.textField.text;
    
    if (text.length == 0) {
        [WHToast showMessage:@"请输入流地址" duration:2 finishHandler:nil];
        return;
    }
    
//    // RTSP/RTMP/HTTP/HLS地址
//    if (![text hasPrefix:@"rtmp://"] ||![text hasPrefix:@"rtsp://"] ||
//        ![text hasPrefix:@"http://"] || ![text hasPrefix:@"hls://"]) {
//        [WHToast showMessage:@"请输入正确的流地址" duration:2 finishHandler:nil];
//        return;
//    }
    
    self.model.url = text;
    
    if (self.urlModel) {
        [URLUnit updateURLModel:self.model oldModel:self.urlModel];
    } else {
        [URLUnit addURLModel:self.model];
    }
    
    [self.subject sendNext:nil];
    
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (RACSubject *) subject {
    if (!_subject) {
        _subject = [RACSubject subject];
    }
    
    return _subject;
}

@end
